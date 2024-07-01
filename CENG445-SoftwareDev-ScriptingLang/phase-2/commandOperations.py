from threading import Lock
import json
import views


NOTIFY_LIST = []

class DatabaseLock:
    db_lock = Lock()


def process_token(token):
    response = views.verify_token(token)
    if not response:
        print("Invalid token.")
        return None
    else:
        print("Token is valid. And, username: " + response + " is processing the command.")
        return response
class CommandOperations:
     #in here make corresponding library calls:
    
    @staticmethod  
    def get_user(token):
        return views.verify_token(token)
    
    @staticmethod
    def send_notify(username: str):
        if username in NOTIFY_LIST:
            NOTIFY_LIST.remove(username)
            return json.dumps({"response": "You have a new notification."})
        else:
            return None
    
    @staticmethod
    def process_command(command):
        try:
            # First, parse the received command as JSON
            wrapped_command = json.loads(command)
            # Check if the 'command' key exists
            if 'command' in wrapped_command:
                # Extract the command
                actual_command_str = wrapped_command['command']
                # Parse the actual command as JSON
                actual_command = json.loads(actual_command_str)

                # Now, process the actual command
                return CommandOperations.process_actual_command(actual_command)
            else:
                return json.dumps({"response": "Invalid command structure"})

        except json.JSONDecodeError:
            return json.dumps({"response": "Invalid JSON format"})

    @staticmethod
    def process_actual_command(command):
        # Here you process the actual command logic
        #organization = Organization()
        if 'action' in command:
            if command['action'] == 'login':
                username = command['username']
                password = command['password']
                token = views.login(username, password)
                if token:
                    return json.dumps({"response": "Login successful and this is your token: ", "token": token })
                else:
                    return json.dumps({"response": "Login failed"})

            with DatabaseLock.db_lock:
                if command['action'] == 'save': #this will be implemented
                    return json.dumps({"response": "Activity saved."})
                         
                elif command['action'] == 'register':
                    username = command['username']
                    password = command['password']
                    email = command['email']
                    fullname = command['fullname']
                    
                    if views.register(username, password, email, fullname):
                        return json.dumps({"response": "Successfully registered."})
                    else: 
                        return json.dumps({"response": "Registration failed."})
                 
                #create_organization(user: str, org_name: str, description: str):
                elif command['action'] == 'create_organization':
                    token = command['token']
                    name = command['org_name']
                    description = command['description']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                

                    response = views.create_organization(user, name, description)
                    return json.dumps({"response": response})

                
                #update_organization(user: str, org_name, field, value):
                elif command['action'] == 'update_organization':
                    token = command['token']
                    org_name = command['org_name']
                    field = command['field']
                    value = command['value']

                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.update_organization(user, org_name, field, value)
                    return json.dumps({"response": response})
                
                #def list_organizations(user: str) -> list:
                elif command['action'] == 'list_organizations':
                    token = command['token']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    return json.dumps({"response": views.list_organizations(user)})
                
                #list_rooms(user: str, org_name: str):
                elif command['action'] == 'list_rooms':
                    token = command['token']
                    org_name = command['org_name']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    return json.dumps({"response": views.list_rooms(user, org_name)})
                
                # create_room(user: str, org: str, room_name: str, x: int, y: int, capacity: int, working_hours: str):
                elif command['action'] == 'create_room':
                    token = command['token']
                    org_name = command['org_name']
                    room_name = command['room_name']
                    x = command['x']
                    y = command['y']
                    capacity = command['capacity']
                    working_hours = command['working_hours']

                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.create_room(user, org_name, room_name, x, y, capacity, working_hours)
                    return json.dumps({"response": response})
                
                #access_room(user: str, org: str, room_name: str):
                elif command['action'] == 'access_room':
                    token = command['token']
                    org_name = command['org_name']
                    room_name = command['room_name']

                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.access_room(user, org_name, room_name)
                    return json.dumps({"response": response})
                
                #access_event(user: str, org: str, event_title: str):
                elif command['action'] == 'access_event':
                    token = command['token']
                    org_name = command['org_name']
                    event_title = command['event_title']

                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.access_event(user, org_name, event_title)
                    return json.dumps({"response": response})

                 
                #delete_room(user: str, org: str, room_name: str):
                elif command['action'] == 'delete_room':
                    token = command['token']
                    org_name = command['org_name']
                    room_name = command['room_name']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.delete_room(user, org_name, room_name)
                    return json.dumps({"response": response})    
                
                
                #update_room(user: str, room_name: str, org: str, capacity: int, x: int, y: int, working_hours: str):
                elif command['action'] == 'update_room':
                    token = command['token']
                    org_name = command['org_name']
                    room_name = command['room_name']
                    capacity = command['capacity']
                    x = command['x']
                    y = command['y']
                    working_hours = command['working_hours']

                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.update_room(user, room_name, org_name, capacity, x, y, working_hours)
                    return json.dumps({"response": response})
                
                
                #list_room_events(user: str, org: str, room_name: str):
                elif command['action'] == 'list_room_events':
                    token = command['token']
                    org_name = command['org_name']
                    room_name = command['room_name']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.list_room_events(user, org_name, room_name)
                    return json.dumps({"response": response})
                    
                    
                #create_reservation(user: str, org: str, room_name: str, event_title: str, start_time: str, duration: int, weekly: bool, description:str):
                elif command['action'] == 'create_reservation':
                    token = command['token']
                    org_name = command['org_name']
                    room_name = command['room_name']
                    event_title = command['event_title']
                    start_time = command['start_time']
                    duration = command['duration']
                    weekly = command['weekly']
                    description = command['description']

                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.create_reservation(user, org_name, room_name, event_title, start_time, duration, weekly, description)
                    return json.dumps({"response": response})
                
                #create_perreservation(user: str, org: str, room_name: str, event_title: str, start_time: str, duration: int, weekly: bool, description:str):
                elif command['action'] == 'create_perreservation':
                    token = command['token']
                    org_name = command['org_name']
                    room_name = command['room_name']
                    event_title = command['event_title']
                    start_time = command['start_time']
                    duration = command['duration']
                    weekly = command['weekly']
                    description = command['description']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.create_perreservation(user, org_name, room_name, event_title, start_time, duration, weekly, description)
                    return json.dumps({"response": response})
                    
                    
                #delete_reservation(user: str, org: str, room_name: str, event_title: str, start_time: str):
                elif command['action'] == 'delete_reservation':
                    token = command['token']
                    org_name = command['org_name']
                    room_name = command['room_name']
                    event_title = command['event_title']
                    start_time = command['start_time']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.delete_reservation(user, org_name, room_name, event_title, start_time)
                    return json.dumps({"response": response})
                
                #read_event( org: str, event_title: str):
                elif command['action'] == 'read_event':
                    token = command['token']
                    org_name = command['org_name']
                    event_title = command['event_title']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.read_event(org_name, event_title)
                    return json.dumps({"response": response})

                #create_event(user: str, org: str, event_title: str, capacity: int, duration: int, weekly: bool, description: str, category: str):
                elif command['action'] == 'create_event':
                    token = command['token']
                    org_name = command['org_name']
                    event_title = command['event_title']
                    capacity = command['capacity']
                    duration = command['duration']
                    weekly = command['weekly']
                    description = command['description']
                    category = command['category']

                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.create_event(user, org_name, event_title, capacity, duration, weekly, description, category)
                    return json.dumps({"response": response})
                
                #list_events(user: str, org_name: str):
                elif command['action'] == 'list_events':
                    token = command['token']
                    org_name = command['org_name']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.list_events(user, org_name)
                    return json.dumps({"response": response})

                #update_event(user: str, org: str, event_title: str, capacity: int, duration: int, weekly: bool, description: str, category: str):
                elif command['action'] == 'update_event':
                    token = command['token']
                    org_name = command['org_name']
                    event_title = command['event_title']
                    capacity = command['capacity']
                    duration = command['duration']
                    weekly = command['weekly']
                    description = command['description']
                    category = command['category']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.update_event(user, org_name, event_title, capacity, duration, weekly, description, category)
                    return json.dumps({"response": response})
                
                #create_organization_permissions(user: str, org: str, list_permission: bool, add_permission: bool, access_permission: bool, delete_permission: bool):
                elif command['action'] == 'create_organization_permissions':
                    token = command['token']
                    org_name = command['org_name']
                    list_permission = command['list_permission']
                    add_permission = command['add_permission']
                    access_permission = command['access_permission']
                    delete_permission = command['delete_permission']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.create_organization_permissions(user, org_name, list_permission, add_permission, access_permission, delete_permission)
                    return json.dumps({"response": response})
                
                #create_room_permissions(user: str, org_name: str, room_name: str, list_permission: bool, reserve_permission: bool, perreserve_permission: bool, delete_permission: bool, write_permission: bool):
                elif command['action'] == 'create_room_permissions':
                    token = command['token']
                    org_name = command['org_name']
                    room_name = command['room_name']
                    list_permission = command['list_permission']
                    reserve_permission = command['reserve_permission']
                    perreserve_permission = command['perreserve_permission']
                    delete_permission = command['delete_permission']
                    write_permission = command['write_permission']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                    
                    response = views.create_room_permissions(user, org_name, room_name, list_permission, reserve_permission, perreserve_permission, delete_permission, write_permission)
                    return json.dumps({"response": response})
                
                #create_event_permissions(user: str, , read_permission: bool, write_permission: bool):
                elif command['action'] == 'create_event_permission':
                    print(f"COMMAND IS COMMAND: {command}")
                    token = command['token']
                    org = command['org_name']
                    event_title = command['event_title']
                    read_permission = command['read_permission']
                    write_permission = command['write_permission']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                
                    response = views.create_event_permissions(user, org, event_title, read_permission, write_permission)
                    return json.dumps({"response": response})
                
                #find_schedule(event_titles, org_name, room_name, date, working_hours):
                elif command['action'] == 'find_schedule':
                    token = command['token']
                    event_ids = command.get('event_ids', '').split(',')
                    print(f"EVENT TITLES: {event_ids}")
                    org_name = command['org_name']
                    room_name = command['room_name']
                    date = command['date']
                    working_hours = command['working_hours']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                
                    response = views.find_schedule(event_ids, org_name, room_name, date, working_hours)
                    return json.dumps({"response": response})
                
                #roomView(user: str, org: str, start_datetime_str, end_datetime_str):
                elif command['action'] == 'room_view':
                    token = command['token']
                    org_name = command['org_name']
                    start_datetime_str = command['start_date']
                    end_datetime_str = command['end_date']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                
                    response = views.roomView(user, org_name, start_datetime_str, end_datetime_str)
                    return json.dumps({"response": response})
                
                #delete_event(user: str, org: str, event_title: str):
                elif command['action'] == 'delete_event':
                    token = command['token']
                    org_name = command['org_name']
                    event_title = command['event_title']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                
                    response = views.delete_event(user, org_name, event_title)
                    return json.dumps({"response": response})
                
                #def attach_observer(user: str, room_id: int, event_id: int):
                elif command['action'] == 'attach':
                    token = command['token']
                    room_id = command['room_id']
                    event_id = command['event_id']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                
                    response = views.attach_observer(user, room_id, event_id)
                    return json.dumps({"response": response}) 
                
                elif command['action'] == 'detach':
                    token = command['token']
                    room_id = command['room_id']
                    event_id = command['event_id']
                    
                    user = process_token(token)
                    if not user:
                        return json.dumps({"response": "Invalid token."})
                
                    return json.dumps({"response": "You have detached."})
                    
        else:
            return json.dumps({"response": "Invalid command"})


"""EXAMPLE USAGES"""


""" 
!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!
If you do not want to deal with token and user etc, you can use the following token as root token: 226a86d4-254a-44a8-9131-1f7c7694cf7c


# login
{"action": "login", "username": "root", "password": "root"}
    
# Register
{"action": "register", "username": "root1", "password": "root1", "email": "newuser@email.com", "fullname": "ROOT"}

# Create organization
{"action": "create_organization", "token": "1325f1f5-31d4-435a-b25a-bf0506725da8", "org_name": "NewOrg12313", "description": "New Organization Description"}

# Update Organization
{"action": "update_organization", "token": "userToken", "org_name": "ExistingOrg", "field": "description", "value": "Updated Description"}

# List Rooms
{"action": "list_rooms", "token": "userToken", "org_name": "OrgName"}

# Create Room
{"action": "create_room", "token": "userToken", "org_name": "OrgName", "room_name": "Room1", "x": 100, "y": 200, "capacity": 50, "working_hours": "09:00-17:00", "permissions": "permissionDetails"}


# Access Room
{"action": "access_room", "token": "userToken", "org_name": "OrgName", "room_name": "Room1"}

# Access Event
{"action": "access_event", "token": "userToken", "org_name": "OrgName", "event_title": "EventTitle"}

# Delete Room
{"action": "delete_room", "token": "userToken", "org_name": "OrgName", "room_name": "Room1"}

# Update Room
{"action": "update_room", "token": "userToken", "org_name": "OrgName", "room_name": "Room1", "capacity": 60, "x": 120, "y": 220, "working_hours": "08:00-18:00", "permissions": "newPermissions"}

# List Room Events
{"action": "list_room_events", "token": "userToken", "org_name": "OrgName", "room_name": "Room1"}

# Create Reservation
{"action": "create_reservation", "token": "userToken", "org_name": "OrgName", "room_name": "Room1", "event_title": "Event1", "start_time": "2021-04-01 09:00", "duration": 120, "weekly": false, "description": "Event Description"}

# Delete Reservation
{"action": "delete_reservation", "token": "userToken", "org_name": "OrgName", "room_name": "Room1", "event_title": "Event1", "start_time": "2021-04-01 09:00"}

# Read Event
{"action": "read_event", "token": "userToken", "org_name": "OrgName", "event_title": "EventTitle"}

# Create Event
{"action": "create_event", "token": "your_token", "org_name": "organization1", "event_title": "New Event", "capacity": 50, "duration": 120, "weekly": false, "description": "Event Description", "category": "Event Category"}

# Update Event
{"action": "update_event", "token": "your_token", "org_name": "organization1", "event_title": "Existing Event", "capacity": 100, "duration": 90, "weekly": true, "description": "Updated Description", "category": "Updated Category"}

# Create Organization Permissions
{"action": "create_organization_permissions", "token": "ef242de6-fdd2-4d53-9edc-1a2c8961d492", "org_name": "new12345", "list_permission": true, "add_permission": true, "access_permission": true, "delete_permission": true}

# Create Room permissions
{"action": "create_room_permissions", "token": "your_token", "org_name": "organization1", "room_name": "Room 1", "list_permission": true, "reserve_permission": true, "perreserve_permission": true, "delete_permission": true, "write_permission": true}

# Create Event Permissions
{"action": "create_event_permission", "token": "your_token", "event_id": 123, "read_permission": true, "write_permission": true}

# Find Schedule
{"action": "find_schedule", "token": "e9e19127-c438-4b94-83d3-48dcaf1aedb3", "org_name": "new12345","event_titles": ["EVENT TEST"], "room_name": 3, "date": "2021-04-01", "working_hours": "09:00-17:00"}
    
# Room View
{"action": "room_view", "token": "your_token", "org_name": "organization1", "start_date": "2021-04-01 08:00", "end_date": "2021-04-30 18:00"}
 
"""

    