import socket
from server import Server
import sys

#controlling if the arguments are true or not
#if true, take the port number
if __name__ == "__main__":
    if len(sys.argv) != 3 or sys.argv[1] != '--port':
        print("Usage: python3 yourapp.py --port <port_number>")
        sys.exit(1)

    port = int(sys.argv[2])
    server = Server(port)
    server.start()

"""
{"action": "register", "username": "johndoe", "password": "password123", "email": "johndoe@email.com", "fullname": "John Doe"}

{"action": "login", "username": "root", "password": "root"}

{"action": "create_organization", "name": "Pera Hotel", "permissions": ["LIST", "ADD", "ACCESS", "DELETE"], "description": "nice"}

{"action": "list_organizations", "token": "bd120e25-85f3-433e-a740-59a60c00f15b"}

{"action": "add_room", "organization": "Pera Hotel", "name": "Conference Room", "x": 10, "y": 20, "capacity": 2, "working hours": "09.00-17.00", "permissions": ["LIST", "RESERVE", "PERRESERVE", "DELETE", "WRITE"]}

{"action": "update_room", "organization_name": "Pera Hotel", "room_name": "Conference Room", "x": 10, "y": 20, "capacity": 2, "working hours": "09.00-17.00", "permissions": ["LIST", "RESERVE", "PERRESERVE", "DELETE", "WRITE"]}

{"action": "delete_room", "organization_name": "Pera Hotel", "room_name": "Conference Room"}


{"action": "create_event", "organization_name": "Pera Hotel", "room_name": "Conference Room", "title": "Science Meeting", "description": "conference", "category": "formal", "capacity": "10", "duration": "15 minutes", "weekly": "", "permissions": ["READ", "WRITE"]}

{"action": "list_events", "organization_name": "Pera Hotel", "room_name": "Conference Room"}

{"action": "reserve", "organization_name": "Pera Hotel", "room_name": "Conference Room", "start_time": "10-01-2023,10:01"}

{"action": "list_rooms", "organization_name": "Pera Hotel"}

{"action": "delete_reservations", "organization_name": "Pera Hotel", "room_name": "Conference Room"}

{"action": "delete_reservation", "organization_name": "Pera Hotel", "room_name": "Conference Room", "event_name": "Science Meeting", "start_time": "10-01-2023,10:01"}

{"action": "read_event", "organization_name": "Pera Hotel", "event_name": "Science Meeting"}

{"action": "update_event", "organization_name": "Pera Hotel", "room_name": "Conference Room", "event_name": "Science Meeting"}

{"action": "delete_event", "organization_name": "Pera Hotel", "room_name": "Conference Room", "event_name": "Science Meeting"}


"""