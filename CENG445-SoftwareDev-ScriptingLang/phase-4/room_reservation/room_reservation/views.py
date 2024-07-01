from django.shortcuts import render
import socket
import json
import struct
import asyncio
import websockets
from django.http import JsonResponse
from django.http import HttpResponse
import logging
from django.shortcuts import redirect
import requests
from datetime import timedelta
from asgiref.sync import async_to_sync
from django.contrib import messages
from django.views.decorators.csrf import csrf_exempt
from .decorators import login_required  # Import the decorator
import threading
import time

logger = logging.getLogger(__name__)
notification_buffer = []

def index(request):
    return render(request, 'index.html')

def send_notification(request):
    token = request.session['token']
    print(request.POST)
    data = {
        'action': 'get_user'
    }

    response = sync_send_command_to_webserver(data, token)

    try:
        response = json.loads(response)
        user = response.get('response', 'Invalid response for create room from server')
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Failed to decode response from server in create room'}, status=500)

    if not user:
        pass

    if(len(notification_buffer) > 0):
        for notification in notification_buffer:
            if user == notification[0]['user']:
                notif = notification_buffer.pop(0)
                messages.success(request, notif[0]['notification'])
            else:
                continue
            
            
@csrf_exempt
def send_command_to_phase2_server(command: dict, token: str = None):
    phase2_server_host = 'localhost'
    phase2_server_port = 12346

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect((phase2_server_host, phase2_server_port))
            logger.debug(f"Connected to {phase2_server_host} on port {phase2_server_port}")

            if token is not None:
                command['token'] = token

            print(f"sending command: {command}")
            command = json.dumps(command)
            command_dict = {"command": command}
            command_json = json.dumps(command_dict)
            command_bytes = command_json.encode('utf-8')

            print(f"Command sent: {command_json}")
            sock.sendall(struct.pack('!I', len(command_bytes)))
            sock.sendall(command_bytes)

            raw_response_size = sock.recv(4)
            if not raw_response_size:
                print("Error: Server did not send a response size.")
                return "Error: Server did not send a response size."

            response_size = struct.unpack('!I', raw_response_size)[0]
            response = sock.recv(response_size).decode('utf-8')
            response = '{"re' + response
            print(f"Response received: {response}")

            return response
    except ConnectionError as e:
        return f"Connection error: {e}"
    except struct.error as e:
        return f"Pack/Unpack error: {e}"
    except Exception as e:
        return f"Unknown error: {e}"

@csrf_exempt
async def send_command_to_webserver(command: dict, token: str = None):
    webserver_uri = 'ws://localhost:12345'  # WebSocket URI

    if token is not None:
        command['token'] = token

    print(f"sending command: {command}")
    command = json.dumps(command)
    command_dict = {"command": command}
    command_json = json.dumps(command_dict)
    
    async with websockets.connect(webserver_uri) as websocket:
        await websocket.send(command_json)
        response = await websocket.recv()
        return response

# Helper function to call the async function from sync code
def sync_send_command_to_webserver(command: dict, token: str = None):
    return async_to_sync(send_command_to_webserver)(command, token)


def login_view(request):
    return render(request, 'login.html')


def repeat_request(view_func):
    def wrapper(request, *args, **kwargs):
        # Initial call to the view function
        initial_response = view_func(request, *args, **kwargs)

        # Check if the initial response is JSON
        if initial_response['Content-Type'] == 'application/json':
            try:
                initial_data = json.loads(initial_response.content)
            except json.JSONDecodeError:
                return HttpResponse("Error: Response is not valid JSON", status=500)
        else:
            # If not JSON, return the response as is
            return initial_response

        # Function to call the view again after 0.5 seconds
        def delayed_call():
            time.sleep(0.5)
            followup_response = view_func(request, *args, **kwargs)
            if followup_response['Content-Type'] == 'application/json':
                try:
                    followup_data = json.loads(followup_response.content)
                    if followup_data != initial_data:
                        return JsonResponse({'refresh': True})
                except json.JSONDecodeError:
                    return HttpResponse("Error: Follow-up response is not valid JSON", status=500)
            return followup_response

        # Start the delayed call in a separate thread
        delayed_thread = threading.Thread(target=delayed_call)
        delayed_thread.start()
        delayed_thread.join()  # Wait for the thread to finish

        return initial_response

    return wrapper

@csrf_exempt
def execute_login(request):
    if request.method == 'POST':
        username = request.POST.get('username')
        password = request.POST.get('password')

        # Prepare the command to send to the phase2 server
        command = {
            "action": "login",
            "username": username,
            "password": password
        }

        # Send the command to the phase2 server
        response = sync_send_command_to_webserver(command)

        try:
            response_data = json.loads(response)
            if 'response' in response_data and 'token' in response_data:
                # Store the token in the user's session
                request.session['token'] = response_data['token']

                # Instead of redirecting, return a JSON response with the redirect URL
                return JsonResponse({'redirect': '/command-center/'})
            else:
                # Return a JSON response indicating invalid credentials
                return JsonResponse({'error': 'Invalid credentials'}, status=401)
        except json.JSONDecodeError:
            # Return a JSON response indicating failure to decode server response
            return JsonResponse({'error': 'Failed to decode response from server in login'}, status=500)

    # Return a JSON response for invalid requests
    return JsonResponse({'error': 'Invalid request'}, status=400)


@csrf_exempt  # Be mindful of CSRF in production
def register_view(request):
    # Handle GET request to serve the registration page
    if request.method == 'GET':
        return render(request, 'register.html')

    if request.method == 'POST':
        username = request.POST.get('username')
        email = request.POST.get('email')  # If you're including email in the registration process
        password = request.POST.get('password')
        fullname = request.POST.get('fullname')

        # Prepare the command to send to the phase2 server
        command = {
            "action": "register",
            "username": username,
            "email": email,  # Include this if your backend expects it
            "password": password,
            "fullname": fullname
        }

        # Send the command to the phase2 server
        response = sync_send_command_to_webserver(command)


        try:
            response_data = json.loads(response)
            # Assuming the phase2 server returns a JSON response
            return JsonResponse(response_data)
        except json.JSONDecodeError:
            # Error handling if the response is not JSON or is malformed
            return JsonResponse({'error': 'Failed to decode response from server in register view'}, status=500)

    # Handle non-POST requests or other errors
    return JsonResponse({'error': 'Invalid request'}, status=400)


def combined_view(request):
    objects = [
        {'name': 'Object1', 'x': 100, 'y': 150, 'color': 'red'},
        {'name': 'Object2', 'x': 200, 'y': 50, 'color': 'blue'},
        {'name': 'Object3', 'x': 150, 'y': 100, 'color': 'green'},
        # ... more objects ...
    ]
    response_message = ""
    token = request.COOKIES.get('token', '')

    if request.method == 'POST':
        command = request.POST.get('command')
        response_json_string = send_command_to_phase2_server(command, token)
        try:
            response_data = json.loads(response_json_string)
            response_message = response_data.get('response', 'Invalid response from server')
            token = response_data.get('token', token)
        except json.JSONDecodeError:
            response_message = 'Invalid response from server'

    context = {
        'objects': objects,
        'response_message': response_message,
        'token': token
    }
    return render(request, 'index.html', context)


# TODO: make all of the command operations.
@csrf_exempt
def create_organization(request):
    """
    This function called by command-center.
    It will create an organization.

    First it will create permissions for that organization.
    Permission options:
        'add', 'delete', 'access', 'list', 'update'
    """
    token = request.session['token']

    permissions = request.POST.getlist('permissions')
    permission_request = {'action': 'create_organization_permissions'}
    for permission in permissions:
        permission_request[permission + "_permission"] = 'true'

    permission_request['org_name'] = request.POST.get('org_name')
    permission_response = sync_send_command_to_webserver(permission_request, token)
    #permission_response = send_command_to_phase2_server(permission_request, token)


    data = {'action': 'create_organization'}
    data['org_name'] = permission_request['org_name'] 
    data['description'] = request.POST.get('description')

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)
    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response from server')

        permission_response = (json.loads(permission_response).
                               get('response', 'Invalid response from server in permissions'))

        # Prepare the context for rendering
        context = {'response_message': response_message,
                   'permission_response': permission_response,
                   'title': 'Create Organization Response'}

        # Check various conditions and modify context as needed
        if response_message == "Organization already exists.":
            context['error'] = 'The organization already exists.'
        elif response_message == "You don't have permission.":
            context['error'] = "You don't have permission to create an organization."
        elif response_message == "Invalid response from server":
            context['error'] = "Invalid response from server."

        # Render the response_template.html with the context
        print("here")
        return render(request, 'response_template.html', context)

    except json.JSONDecodeError:
        return JsonResponse({'error': 'Failed to decode response from server in create organization'}, status=500)

@csrf_exempt
def update_organization(request):
    """
    This endpoint will update the organization.
    """
    token = request.session['token']

    data = {'action': 'update_organization'}
    data['org_name'] = request.POST.get('org_name')
    data['field'] = request.POST.get('field')
    data['value'] = request.POST.get('value')

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response from')
        context = {'response_message': response_message, 'title': 'Update Organization'}

        return render(request, 'response_template.html', context)
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Invalid field name or something went wrong in phase-2 server while updating organization.'}, status=500)

@csrf_exempt
@repeat_request
def list_organizations(request):
    """
    This endpoint will list all the organizations.
    """
    token = request.session['token']

    data = {'action': 'list_organizations'}

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response from')
        context = {'organizations': response_message, 'title': 'List Organizations'}

        return render(request, 'list_organizations.html', context)
    except:
        return JsonResponse({'error': 'Something went wrong while list organizations'}, status=500)

@csrf_exempt
def organizations_json(request):
    """
    This endpoint returns all the organizations in JSON format.
    """
    token = request.session.get('token')

    data = {'action': 'list_organizations'}

    response = sync_send_command_to_webserver(data, token)
    # Alternatively, response = send_command_to_phase2_server(data, token)

    try:
        print(response)  # Debugging statement
        return HttpResponse(response, content_type="application/json")
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Something went wrong while fetching organizations'}, status=500)

@csrf_exempt
def rooms_json(request, org_name, token):
    """
    This endpoint returns all the rooms for a given organization in JSON format.
    """
    print(request)
    if request.method == 'GET':
        data = {'action': 'list_rooms', 'org_name': org_name}

        try:
            # Replace this with your actual data retrieval logic
            # Example: response = sync_send_command_to_webserver(data, token)
            response = sync_send_command_to_webserver(data, token)

            return HttpResponse(response, content_type="application/json")
        except Exception as e:
            return JsonResponse({'error': str(e)}, status=500)
    else:
        return JsonResponse({'error': 'Invalid HTTP method'}, status=405)
        
@csrf_exempt
def list_rooms(request):
    """
    This endpoint will list all the rooms belong to the given org_name.
    """
    token = request.session['token']

    data = {'action': 'list_rooms'}
    data['org_name'] = request.POST.get('org_name')

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response from phase2 server.')
        if response_message == "You don't have permission.":
            context = {'response_message': response_message, 'title': 'List Rooms', 'org_name': request.POST.get('org_name'), 'token': token}
            return render(request, 'response_template.html', context)
            pass
        context = {'rooms': response_message, 'title': 'List Rooms', 'org_name': request.POST.get('org_name'), 'token': token}

        return render(request, 'list_rooms.html', context)

    except:
        return JsonResponse({'error': 'Something went wrong while list rooms.'}, status=500)

@csrf_exempt
def create_room(request):
    """
    This endpoint will create a room object for the given org_name.

    Room:
           token = command['token']
           org_name = command['org_name']
           room_name = command['room_name']
           x = command['x']
           y = command['y']
           capacity = command['capacity']
           working_hours = command['working_hours']
    """
    token = request.session['token']

    data = {'action': 'create_room',
            'org_name': request.POST.get('org_name'),
            'room_name': request.POST.get('room_name'),
            'capacity': request.POST.get('capacity'),
            'working_hours': request.POST.get('working_hours'),
            'x': request.POST.get('x'),
            'y': request.POST.get('y')
            }

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response for create room from server')

        context = {'response_message': response_message,
                   'title': 'Create Room Response'}

        return render(request, 'response_template.html', context)
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Failed to decode response from server in create room'}, status=500)


@csrf_exempt
def create_room_permission(request):
    """
    token = command['token']
    org_name = command['org_name']
    room_name = command['room_name']
    list_permission = command['list_permission']
    reserve_permission = command['reserve_permission']
    perreserve_permission = command['perreserve_permission']
    delete_permission = command['delete_permission']
    write_permission = command['write_permission']
    """
    token = request.session['token']

    permission_request = {'action': 'create_room_permissions',
                          'org_name': request.POST.get('org_name'),
                          'room_name': request.POST.get('room_name'),
    }
    permissions = request.POST.getlist('room_permissions')
    for permission in permissions:
        permission_request[permission] = 'true'

    permission_response = sync_send_command_to_webserver(permission_request, token)

    try:
        permission_response = json.loads(permission_response)
        room_users = permission_response.get('room_users')
        permission_response = permission_response.get('response', 'Invalid response for permission from server')

        
        if(room_users is not None):
            for user in room_users:
                notification_buffer.append([{"user": user, "notification": permission_request['room_name'] + " permissions updated " }])

        context = {'response_message': permission_response,
                   'title': 'Room Permission Response'}

        send_notification(request)
        return render(request, 'response_template.html', context)
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Failed to decode response from server in create room'}, status=500)


@csrf_exempt
def get_room(request):
    """
    token = command['token']
    org_name = command['org_name']
    room_name = command['room_name']
    """
    token = request.session['token']

    data = {'action': 'access_room',
            'org_name': request.POST.get('org_name'),
            'room_name': request.POST.get('room_name')
            }

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)
    try:
        response = json.loads(response)
        response = response.get('response', 'Invalid response from phase2 server in get_room')
        context = {'rooms': response}
        return render(request, 'access_room.html', context)
    except:
        return JsonResponse({'error': 'Something went wrong while get room.'}, status=500)

@csrf_exempt
def room_detail(request, organization_name, room_name):
    #room = get_object_or_404(Room, name=room_name, organization__name=organization_name)

    token = request.session['token']

    data = {'action': 'access_room',
            'org_name': organization_name,
            'room_name': room_name
            }

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)
    try:
        response = json.loads(response)
        room = response.get('response', 'Invalid response from phase2 server in get_room')
        context = {'room': room}
        return render(request, 'clicked-room.html', context)
    except Exception as e:
        print(e)  # Log the error to the console or use Django's logging framework
        context = {'error': 'Something went wrong while getting the room.'}
        return render(request, 'clicked-room.html', context)

    
    
@csrf_exempt
def delete_room(request):
    """
    org_name = command['org_name']
    room_name = command['room_name']
    """
    token = request.session['token']
    data = {'action': 'delete_room',
            'org_name': request.POST.get('org_name'),
            'room_name': request.POST.get('room_name')}

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        room_users = response.get('room_users')
        response = response.get('response', 'Invalid response from phase2 server in delete_room')
        
        if(room_users is not None):
            for user in room_users:
                notification_buffer.append([{"user": user, "notification": data['room_name'] + " deleted " }])

        context = {'response_message': response}

        send_notification(request)
        return render(request, 'response_template.html', context)
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Invalid response from'})

@csrf_exempt
def update_room(request):
    """
    org_name = command['org_name']
    room_name = command['room_name']
    capacity = command['capacity']
    x = command['x']
    y = command['y']
    working_hours = command['working_hours']
    """
    token = request.session['token']

    data = {'action': 'update_room',
            'org_name': request.POST.get('org_name'),
            'room_name': request.POST.get('room_name'),
            'capacity': request.POST.get('capacity'),
            'working_hours': request.POST.get('working_hours'),
            'x': request.POST.get('x'),
            'y': request.POST.get('y')}

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response from phase2 server in update_room')
        print(response_message)
        if(response_message != 'Invalid response from phase2 server in update_room'):
            room_users = response.get('room_users')
            if(room_users is not None):
                for user in room_users:
                    notification_buffer.append([{"user": user, "notification": data['room_name'] + " updated " }])

        context = {'response_message': response_message, 'title': 'Update Room Response'}

        send_notification(request)
        return render(request, 'response_template.html', context)
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Something went wrong while update rooms.'}, status=500)



@csrf_exempt
def list_room_events(request):
    """
    THIS FUNCTION IS NOT TESTED YET


    org_name = command['org_name']
    room_name = command['room_name']
    """

    token = request.session['token']
    data = {
        'action': 'list_room_events',
        'org_name': request.POST.get('org_name'),
        'room_name': request.POST.get('room_name')
    }

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response from phase2 server in list_room_events')
        context = {'response_message': response_message, 'title': 'List Room Events Response'}

        return render(request, 'response_template.html', context)
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Something went wrong while list room events'}, status=500)

@csrf_exempt
def create_event(request):
    """
    org_name = command['org_name']
    event_title = command['event_title']
    capacity = command['capacity']
    duration = command['duration']
    weekly = command['weekly']
    description = command['description']
    category = command['category']
    """
    token = request.session['token']

    data = {
        'action': 'create_event',
        'org_name': request.POST.get('org_name'),
        'event_title': request.POST.get('event_title'),
        'capacity': request.POST.get('capacity'),
        'duration': request.POST.get('duration'),
        'weekly': 'true' if request.POST.get('weekly') == 'weekly' else 'false',
        'description': request.POST.get('description'),
        'category': request.POST.get('category')
    }

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response from phase2 while create event')
        context = {'response_message': response_message, 'title': 'Create Event Response'}

        return render(request, 'response_template.html', context)
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Failed to decode response from server in create event'}, status=500)

@csrf_exempt
def events_json(request, org_name, token):
    """
    This endpoint returns all the rooms for a given organization in JSON format.
    """
    if request.method == 'GET':
        data = {'action': 'list_events', 'org_name': org_name}

        try:
            # Replace this with your actual data retrieval logic
            # Example: response = sync_send_command_to_webserver(data, token)
            response = sync_send_command_to_webserver(data, token)

            return HttpResponse(response, content_type="application/json")
        except Exception as e:
            return JsonResponse({'error': str(e)}, status=500)
    else:
        return JsonResponse({'error': 'Invalid HTTP method'}, status=405)

@csrf_exempt
def list_events(request):
    """
    org_name = command['org_name']
    """
    token = request.session['token']

    data = {
        'action': 'list_events',
        'org_name': request.POST.get('org_name')
    }

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response from phase2 while list events')
        if response_message == "You don't have permission.":
            context = {'response_message': response_message, 'title': 'List Events'}
            return render(request, 'response_template.html', context)
            pass
        context = {'events': response_message, 'title': 'List Events Response', 'org_name': request.POST.get('org_name'), 'token': token}

        return render(request, 'list_events.html', context)
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Error while fetching list events.'}, status=500)

@csrf_exempt
def create_reservation(request):
    """
    org_name = command['org_name']
    room_name = command['room_name']
    event_title = command['event_title']
    start_time = command['start_time']
    duration = command['duration']
    weekly = command['weekly']
    description = command['description']
    """
    token = request.session['token']
    start_date = request.POST.get('start_date')  # e.g., '2024-01-19'
    start_time = request.POST.get('start_hour')  # e.g., '09:00'
    data = {
        'action': 'create_reservation',
        'org_name': request.POST.get('org_name'),
        'room_name': request.POST.get('room_name'),
        'event_title': request.POST.get('event_title'),
        'start_time': f'{start_date} {start_time}',
        'duration': request.POST.get('duration'),
        'weekly': request.POST.get('weekly'),
        'description': request.POST.get('description')
    }
    if data['weekly']:
        data['action'] = 'create_perreservation'

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response from phase2 in create_reservation')
        event_users = response.get('event_users')
        room_users = response.get('room_users')
        if(event_users is not None):
            for user in event_users:
                notification_buffer.append([{"user": user, "notification": data['event_title'] + " reserved " + data['room_name'] + " at time " + data['start_time'] }])
        if(room_users is not None):
            for user in room_users:
                notification_buffer.append([{"user": user, "notification": data['event_title'] + " reserved " + data['room_name'] + " at time " + data['start_time']}])

        context = {'response_message': response_message, 'title': 'Reservation Response'}

        send_notification(request)

        return render(request, 'response_template.html', context)

    except json.JSONDecodeError:
        return HttpResponse('Invalid response from phase2 in update event')



@csrf_exempt
def delete_reservation(request):
    """
    org_name = command['org_name']
    room_name = command['room_name']
    event_title = command['event_title']
    start_time = command['start_time']
    duration = command['duration']
    weekly = command['weekly']
    description = command['description']
    """
    token = request.session['token']
    start_date = request.POST.get('start_date')  # e.g., '2024-01-19'
    start_time = request.POST.get('start_hour')  # e.g., '09:00'
    data = {
        'action': 'delete_reservation',
        'org_name': request.POST.get('org_name'),
        'room_name': request.POST.get('room_name'),
        'event_title': request.POST.get('event_title'),
        'start_time': f'{start_date} {start_time}'
    }

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response from phase2 in create_reservation')
        event_users = response.get('event_users')
        room_users = response.get('room_users')
        if(event_users is not None):
            for user in event_users:
                notification_buffer.append([{"user": user, "notification": data['event_title'] + " reservation deleted " + data['room_name'] + " at time " + data['start_time'] }])
        if(room_users is not None):
            for user in room_users:
                notification_buffer.append([{"user": user, "notification": data['event_title'] + " reservation deleted " + data['room_name'] + " at time " + data['start_time']}])

        context = {'response_message': response_message, 'title': 'Reservation Response'}

        send_notification(request)

        return render(request, 'response_template.html', context)

    except json.JSONDecodeError:
        return HttpResponse('Invalid response from phase2 in update event')

@csrf_exempt
def update_event(request):
    """
    org_name = command['org_name']
    event_title = command['event_title']
    capacity = command['capacity']
    duration = command['duration']
    weekly = command['weekly']
    description = command['description']
    category = command['category']
    """
    token = request.session['token']

    data = {
        'action': 'update_event',
        'org_name': request.POST.get('org_name'),
        'event_title': request.POST.get('event_title'),
        'capacity': request.POST.get('capacity'),
        'duration': request.POST.get('duration'),
        'weekly': request.POST.get('weekly'),
        'description': request.POST.get('description'),
        'category': request.POST.get('category')
    }

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)
    send_notification(request)
    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response from phase2 in update event')
        event_users = response.get('event_users')
        print(event_users)
        if(event_users is not None):
            for user in event_users:
                notification_buffer.append([{"user": user, "notification": data['event_title'] + " updated "}])

        context = {'response_message': response_message, 'title': 'Update Event Response'}
        
        send_notification(request)

        return render(request, 'response_template.html', context)

    except json.JSONDecodeError:
        return HttpResponse('Invalid response from phase2 in update event')

@csrf_exempt
def access_event(request):
    """
    org_name = command['org_name']
    event_title = command['event_title']
    """

    token = request.session['token']

    data = {
        'action': 'access_event',
        'org_name': request.POST.get('org_name'),
        'event_title': request.POST.get('event_title')
    }

    response = sync_send_command_to_webserver(data, token)

    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response from access event')
        context = {'events': response_message, 'title': 'Access Event Response'}

        return render(request, 'access_event.html', context)
    except json.JSONDecodeError:
        return HttpResponse('Invalid response from access event in access event')

@csrf_exempt
def delete_event(request):
    """
    org_name = command['org_name']
    event_title = command['event_title']
    """
    token = request.session['token']

    data = {
        'action': 'delete_event',
        'org_name': request.POST.get('org_name'),
        'event_title': request.POST.get('event_title')
    }

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response_message = response.get('response', 'Invalid response from delete event')

        event_users = response.get('event_users')

        if(event_users is not None):
            for user in event_users:
                notification_buffer.append([{"user": user, "notification": data['event_title'] + " deleted "}])

        context = {'response_message': response_message, 'title': 'Delete Event Response'}
        send_notification(request)
        return render(request, 'response_template.html', context)

    except json.JSONDecodeError:
        return HttpResponse('Invalid response from delete event')

@csrf_exempt
def create_event_permission(request):
    """
    org = command['org_name']
    event_title = command['event_title']
    read_permission = command['read_permission']
    write_permission = command['write_permission']
    """
    token = request.session['token']

    data = {
        'action': 'create_event_permission',
        'org_name': request.POST.get('org_name'),
        'event_title': request.POST.get('event_title')
    }

    permissions = request.POST.getlist('permissions')
    for permission in permissions:
        data[permission + "_permission"] = 'true'

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        permission_response = json.loads(response)
        event_users = permission_response.get('event_users')
        permission_response = permission_response.get('response', 'Invalid response for permission from server')

        

        if(event_users is not None):
            for user in event_users:
                notification_buffer.append([{"user": user, "notification": data['event_title'] + " permissions updated "}])

        context = {'response_message': permission_response,
                   'title': 'Event Permission Response'}

        send_notification(request)
        return render(request, 'response_template.html', context)
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Failed to decode response from server in event permission'}, status=500)


@csrf_exempt
def room_view(request):
    """
    org_name = command['org_name']
    start_datetime_str = command['start_date']
    end_datetime_str = command['end_date']
    """
    token = request.session['token']
    start_date = request.POST.get('start_date')  # e.g., '2024-01-19'
    start_time = request.POST.get('start_hour')  # e.g., '09:00'
    end_date = request.POST.get('end_date')  # e.g., '2024-01-19'
    end_time = request.POST.get('end_hour')  # e.g., '09:00'
    data = {
        'action': 'room_view',
        'org_name': request.POST.get('org_name'),
        'start_date': f'{start_date} {start_time}',
        'end_date': f'{end_date} {end_time}',
    }


    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)
    try:
        response = json.loads(response)
        response = response.get('response', 'Invalid response for permission from server')
        context = {'response_message': response,
                   'title': 'Room View'}
        return render(request, 'response_template.html', context)
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Failed to decode response from server in event permission'}, status=500)

@csrf_exempt
def attach(request):
    """
    org_name = command['org_name']
    start_datetime_str = command['start_date']
    end_datetime_str = command['end_date']
    """
    token = request.session['token']
    data = {
        'action': 'attach',
        'org_name': request.POST.get('org_name'),
        'event_name': request.POST.get('event_title'),
        'room_name': request.POST.get('room_name'),
        'observation_type': request.POST.get('observation_type')
    }


    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response = response.get('response', 'Invalid response for permission from server')

        context = {'response_message': response,
                   'title': 'Attach'}

        return render(request, 'response_template.html', context)
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Failed to decode response from server in event permission'}, status=500)


@csrf_exempt
def detach(request):
    """
    org_name = command['org_name']
    start_datetime_str = command['start_date']
    end_datetime_str = command['end_date']
    """
    token = request.session['token']
    print(request.POST)
    data = {
        'action': 'detach',
        'org_name': request.POST.get('org_name'),
        'event_name': request.POST.get('event_title'),
        'room_name': request.POST.get('room_name'),
        'observation_type': request.POST.get('observation_type')
    }


    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response = response.get('response', 'Invalid response for permission from server')

        context = {'response_message': response,
                   'title': 'Detach'}

        return render(request, 'response_template.html', context)
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Failed to decode response from server in event permission'}, status=500)


@csrf_exempt
@login_required
def find_schedule(request):
    """
    event_ids = command['event_ids']
    org_name = command['org_name']
    room_name = command['room_name']
    date = command['date']
    working_hours = command['working_hours']
    """
    if request.method == 'GET':
        return render(request, 'get_schedule.html')

    token = request.session['token']

    data = {
        'action': 'find_schedule',
        'event_ids': request.POST.get('schedule_event_titles'),
        'org_name': request.POST.get('schedule_org_name'),
        'room_name': request.POST.get('schedule_room_name'),
        'date': request.POST.get('schedule_date'),
        'working_hours': request.POST.get('schedule_working_hours')
    }

    response = sync_send_command_to_webserver(data, token)
    #response = send_command_to_phase2_server(data, token)

    try:
        response = json.loads(response)
        response = response.get('response', 'Invalid response for find_schedule')
        context = {'response': response}

        return render(request, "schedule.html", context)
    except json.JSONDecodeError:
        return HttpResponse("Invalid response from server in schedule")







@csrf_exempt
def process_request(request):
    """
    Process the request and returns back from the deep backend server.

    Returns:
        response (json): Response from the deep backend server
    """

    if(notification_buffer is not None):
        print(notification_buffer)

    command = request.POST.get('command')

    if command == 'create_organization':
        return create_organization(request)
    elif command == 'update_organization':
        return update_organization(request)
    elif command == 'list_organizations':
        return list_organizations(request)
    elif command == 'list_rooms':
        return list_rooms(request)
    elif command == 'create_room':
        return create_room(request)
    elif command == 'create_room_permissions':
        return create_room_permission(request)
    elif command == 'access_room':
        return get_room(request)
    elif command == 'delete_room':
        return delete_room(request)
    elif command == 'update_room':
        return update_room(request)
    elif command == 'list_room_events':
        return list_room_events(request)
    elif command == 'create_event':
        return create_event(request)
    elif command == 'list_events':
        return list_events(request)
    elif command == 'update_event':
        return update_event(request)
    elif command == 'access_event':
        return access_event(request)
    elif command == 'delete_event':
        return delete_event(request)
    elif command == 'create_event_permission':
        return create_event_permission(request)
    elif command == 'create_reservation':
        return create_reservation(request)
    elif command == 'room_view':
        return room_view(request)
    elif command == 'attach':
        return attach(request)
    elif command == 'detach':
        return detach(request)
    elif command == 'delete_reservation':
        return delete_reservation(request)
    elif command == 'find_schedule':
        print("inside find_schedule")
        return redirect("find-schedule")
    else:
        return JsonResponse({'error': 'Command not implemented yet'})


@csrf_exempt  # To bypass CSRF token verification for demonstration purposes
@login_required  # Apply the decorator to the view
def command_center(request):
    if request.method == 'POST':
        print(f"REQUEST: {request.POST}")
        return process_request(request)

    # For GET requests, or if the form is not submitted
    return render(request, 'command_center.html')


def room_view_center(request):
    return render(request, 'room_view.html')