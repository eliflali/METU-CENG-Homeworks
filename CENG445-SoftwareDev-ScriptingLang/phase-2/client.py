import socket
import json
import struct
import sys

def send_command(host, port):
    # Create a TCP/IP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # Connect to the server
        sock.connect((host, port))
        print(f"Connected to {host} on port {port}")

        while True:
            # Get command from user
            user_input = input("Enter command (or 'close connection' to exit): ")

            # Check if the user wants to close the connection
            

            # Serialize the command to a JSON string and convert to bytes
            command_json = json.dumps({"command": user_input})
            command_bytes = command_json.encode('utf-8')
            print(command_bytes)

            # Calculate the size of the command
            command_size = len(command_bytes)

            # Pack the size in a 4-byte big-endian format
            packed_size = struct.pack('!I', command_size)

            # Send the size of the command followed by the command itself
            sock.sendall(packed_size)
            sock.sendall(command_bytes)
            print(f"Command sent: {command_json}")

            if user_input.lower() == "close connection":
                print("Closing connection.")
                break
                
            # Wait for the response
            response_size_raw = sock.recv(4)
            if not response_size_raw:
                print("No response from server.")
                return

            response_size = struct.unpack('!I', response_size_raw)[0]
            response = sock.recv(response_size).decode('utf-8')
            print(f"Response received: {response}")
            

if __name__ == "__main__":
    if len(sys.argv) != 3 or sys.argv[1] != '--port':
        print("Usage: python3 client.py --port <port_number>")
        sys.exit(1)

    port = int(sys.argv[2])

    # Server details
    server_host = 'localhost'
    server_port = port

    # Send the command
    
    send_command(server_host, server_port)


"""EXAMPLE USAGES"""


""" 
!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!
If you do not want to deal with token and user etc, you can use the following token as root token: 226a86d4-254a-44a8-9131-1f7c7694cf7c


# login
{"action": "login", "username": "root", "password": "root"}
    
# Register
{"action": "register", "username": "root", "password": "root", "email": "newuser@email.com", "fullname": "ROOT"}

# Create organization
{"action": "create_organization", "token": "userToken", "org_name": "NewOrg", "description": "New Organization Description"}

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
{"action": "create_organization_permissions", "token": "your_token", "org_name": "organization1", "list_permission": true, "add_permission": true, "access_permission": true, "delete_permission": true}

# Create Room permissions
{"action": "create_room_permissions", "token": "your_token", "org_name": "organization1", "room_name": "Room 1", "list_permission": true, "reserve_permission": true, "perreserve_permission": true, "delete_permission": true, "write_permission": true}

# Create Event Permissions
{"action": "create_event_permission", "token": "your_token", "event_id": 123, "read_permission": true, "write_permission": true}

# Find Schedule
{"action": "find_schedule", "token": "your_token", "event_ids": [123, 456], "room_id": 789, "date": "2021-04-01", "working_hours": "09:00-17:00"}
    
# Room View
{"action": "room_view", "token": "your_token", "org_name": "organization1", "start_date": "2021-04-01 08:00", "end_date": "2021-04-30 18:00"}
 
"""

    