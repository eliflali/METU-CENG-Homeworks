from copy import deepcopy
from datetime import datetime, timedelta
from itertools import permutations, product
from math import ceil
import new_models


class SingletonMeta(type): 
    _instances = {} 
  
    def __call__(cls, *args, **kwargs): 
        if cls not in cls._instances: 
            cls._instances[cls] = super(SingletonMeta, cls).__call__(*args, **kwargs) 
        return cls._instances[cls]
    
class DatabaseProps(metaclass=SingletonMeta):
    def __init__(self):
        self.user_manager = new_models.UserManager("project.db")
        self.org_manager = new_models.OrganizationManager("project.db")
        self.room_manager = new_models.RoomManager("project.db")
        self.event_manager = new_models.EventManager("project.db")
        self.reservation_manager = new_models.ReservationManager("project.db")
        self.organization_permissions = new_models.OrganizationPermissionsManager("project.db")
        self.room_permissions = new_models.RoomPermissionsManager("project.db")
        self.event_permissions = new_models.EventPermissionsManager("project.db")
        self.notification_manager = new_models.ObservationManager("project.db")

DB_MANAGER = DatabaseProps()

def verify_token(token: str):
    user = DB_MANAGER.user_manager.find_token(token)
    if not user:
        return "Token is not valid."
    else:
        return user 
##added
def register(username, password, email, fullname):
    if DB_MANAGER.user_manager.register_user(username, password, email, fullname):
        return True
    
    return False

##added
def login(username, password):
    token = DB_MANAGER.user_manager.authenticate_user(username, password)        
    return  token if token else False

def logout(user: str):
    DB_MANAGER.user_manager.logout_user(user)
    return f"Logged out {user}"

def create_organization(user: str, org_name: str, description: str):
    """Creates an organization for the user if the user has permission to do so"""
    
    # first check if the user has permission to create an organization
    if DB_MANAGER.organization_permissions.get_add_permission(user, org_name):
        # create the organization for the user
        if DB_MANAGER.org_manager.create_organization(org_name, description, user):
            return org_name + " created with owner: " + user
        else:
            return "Organization already exists."
    
    return "You don't have permission."
    
def update_organization(user: str, org_name, field, value):
    
    if DB_MANAGER.organization_permissions.get_add_permission(user, org_name):
        DB_MANAGER.org_manager.update_organization(org_name, field, value)
        return f"Organization {org_name} updated with {field} = {value}"

    return "You don't have permission."

def list_organizations(user: str) -> list:
    """Returns a list of all the organizations to the user"""
    # first check if the user has permission to view the organizations
    organizations = DB_MANAGER.org_manager.get_all_organizations()
    return organizations if organizations else []

##deleted user parameter
def list_rooms(user: str, org_name: str):
    """Returns a list of all the rooms in the organization to the user"""    
    # first check if the user has permission to view the rooms in the organization
    if DB_MANAGER.organization_permissions.get_list_permission(user, org_name):
        # get all the rooms in the organization
        rooms = DB_MANAGER.org_manager.get_rooms(org_name)
        return rooms if rooms else []
    else:
        return "You don't have permission."

def list_events(user: str, org_name: str):
    """Returns a events of all the rooms in the organization to the user"""    
    # first check if the user has permission to view the events in the organization
    if DB_MANAGER.organization_permissions.get_list_permission(user, org_name):
        # get all the events in the organization
        events = DB_MANAGER.org_manager.get_events(org_name)
        return events if events else []
    else:
        return "You don't have permission."
    

#permission row added
def create_room(user: str, org: str, room_name: str, x: int, y: int, capacity: int, working_hours: str):
    """Creates a room for the organization if the user has permission to do so"""
    
    if DB_MANAGER.organization_permissions.get_add_permission(user, org):
        # create the room for the organization
        if DB_MANAGER.room_manager.create_room(room_name, x, y, capacity, working_hours, user, org):
            return room_name + " created for organization " + org
        
        return "Room already exists."
    
    return "You don't have permission."

def access_room(user: str, org: str, room_name: str):
    """Returns the room if the user has permission to access it"""
    # first check if the user has permission to access the room
    if DB_MANAGER.organization_permissions.get_access_permission(user, org):
        # get the room from the organization
        room_id = DB_MANAGER.room_manager.get_room_id(room_name, org)
        return DB_MANAGER.room_manager.get_room(room_id)
    
def access_event(user: str, org: str, event_title: str):
    """Returns the event if the user has permission to access it"""
    # first check if the user has permission to access the event
    if DB_MANAGER.organization_permissions.get_access_permission(user, org):
        # get the event from the organization
        event_id = DB_MANAGER.event_manager.get_event_id(event_title, org)
        return DB_MANAGER.event_manager.get_event(event_id)

#deleted user param
def delete_room(user: str, org: str, room_name: str):
    """Deletes the room if the user has permission to delete it"""
    # first check if the user has permission to delete the room
    if DB_MANAGER.org_manager.get_organization_owner(org) == user:
        # delete the room from the organization
        room_id = DB_MANAGER.room_manager.get_room_id(room_name, org)
        DB_MANAGER.room_manager.delete_room(room_id)
        return "Room successfully deleted."

    
    room_id = DB_MANAGER.room_manager.get_room_id(room_name, org)
    if DB_MANAGER.room_permissions.get_delete_permission(user, room_id):
        # delete the room from the organization
        DB_MANAGER.room_manager.delete_room(room_id)
        return "Room successfully deleted."

    return "You don't have permission."

def update_room(user: str, room_name: str, org: str, capacity: int, x: int, y: int, working_hours: str):
    """Updates the room if the user has permission to update it"""
    # first check if the user has permission to update the room
    if DB_MANAGER.org_manager.get_organization_owner(org) == user:
        # update the room in the organization
        room_id = DB_MANAGER.room_manager.get_room_id(room_name, org)
        DB_MANAGER.room_manager.update_room(room_id, room_name, x, y, capacity, working_hours, user, org)
        return "Room successfully updated."
    
    room_id = DB_MANAGER.room_manager.get_room_id(room_name, org)
    if DB_MANAGER.room_permissions.get_write_permission(user, room_id):
        # update the room in the organization
        room_id = DB_MANAGER.room_manager.get_room_id(room_name, org)
        DB_MANAGER.room_manager.update_room(room_id, room_name, x, y, capacity, working_hours, user, org)
        return "Room successfully updated."

    return "You don't have permission."

#deleted user param
def list_room_events(user: str, org: str, room_name: str):
    """Returns a list of all the events in the room to the user"""
    # first check if the user has permission to view the events in the room
    room_id = DB_MANAGER.room_manager.get_room_id(room_name, org)
    if DB_MANAGER.room_permissions.get_list_permission(user, room_id):
        # get all the events in the room
        events = DB_MANAGER.reservation_manager.get_room_reservations(room_id)
        return events if events else []
    else:
        return "You don't have permission."

#deleted user param
def create_reservation(user: str, org: str, room_name: str, event_title: str, start_time: str, duration: int, weekly: bool, description:str):
    
    room_id = DB_MANAGER.room_manager.get_room_id(room_name, org)
    if not room_id:
        return "Room does not exist."
     
    if DB_MANAGER.room_permissions.get_reserve_permission(user, room_id):
        
        event_id = DB_MANAGER.event_manager.get_event_id(event_title, org)
        if not event_id:
            return "Event does not exist."
        
        
        if DB_MANAGER.reservation_manager.is_room_available(room_id, start_time, duration):
            
            DB_MANAGER.reservation_manager.create_reservation(room_id, event_id, user, start_time, duration, weekly, description)    
            return room_name + " reserved for event " + event_title
        else:
            return "Room is already occupied for that time slot."
        
    return "You don't have permission."

#changed return  
def create_perreservation(user: str, org: str, room_name: str, event_title: str, start_time: str, duration: int, weekly: bool, description:str):
    """Creates a reservation for the room if the user has permission to do so"""
    # first check if the user has permission to create a reservation for the room
    room_id = DB_MANAGER.room_manager.get_room_id(room_name, org)
    event_id = DB_MANAGER.event_manager.get_event_id(event_title, org)
    if DB_MANAGER.room_permissions.get_reserve_permission(user, room_id):
        # create the reservation for the room
        DB_MANAGER.reservation_manager.create_reservation(room_id, event_id, user, start_time, duration, weekly, description)
        return room_name + " reserved for weekly event " + event_title
    return "You don't have permission."

#deleted user param   
def delete_reservation(user: str, org: str, room_name: str, event_title: str, start_time: str):
    """Deletes the reservation for the room if the user has permission to do so"""
    # first check if the user has permission to delete the reservation for the room
    event_id = DB_MANAGER.event_manager.get_event_id(event_title, org)
    event = DB_MANAGER.event_manager.get_event(event_id)
    room_id = DB_MANAGER.room_manager.get_room_id(room_name, org)

    if DB_MANAGER.room_permissions.get_delete_permission(user, room_id):
        # delete the reservation for the room
        reservation_id = DB_MANAGER.reservation_manager.get_reservation_id(room_id, event_id, user, start_time, event['duration'], event['weekly'])
        DB_MANAGER.reservation_manager.delete_reservation(reservation_id)
        return room_name + " reservation deleted for the event " + event_title
    return "You don't have permission."
        

#deleted user param 
def read_event( org: str, event_title: str):
    """Returns the event if the user has permission to access it"""
    # first check if the user has permission to access the event
    user = DB_MANAGER.user_manager.get_current_username()
    event_id = DB_MANAGER.event_manager.get_event_id(event_title, org)
    if DB_MANAGER.event_permissions.get_read_permission(user, event_id):
        # get the event from the organization
        return DB_MANAGER.event_manager.get_event(event_id)
    
    return "You don't have permission."

def create_event(user: str, org: str, event_title: str, capacity: int, duration: int, weekly: bool, description: str, category: str):
    """Creates an event for the organization if the user has permission to do so"""
    
    # first check if the user has permission to create an event for the organization
    if DB_MANAGER.organization_permissions.get_add_permission(user, org):
        # create the event for the organization
        if DB_MANAGER.event_manager.create_event(event_title, description, category, capacity, duration, weekly, org, user):
            return event_title + " created for organization " + org
        else:
            return "Event already exists."
        
    return "You don't have permission."

def update_event(user: str, org: str, event_title: str, capacity: int, duration: int, weekly: bool, description: str, category: str):
    """Updates an event for the organization if the user has permission to do so"""
    
    # first check if the user has permission to update an event for the organization
    event_id = DB_MANAGER.event_manager.get_event_id(event_title, org)
    if event_id is None:
        return "Event does not exist."
    if DB_MANAGER.event_permissions.get_write_permission(user, event_id):
        # update the event for the organization
        DB_MANAGER.event_manager.update_event(event_id, event_title, capacity, duration, weekly, description, category)
        return event_title + " updated for organization " + org
    
    return "You don't have permission."

def delete_event(user: str, org: str, event_title: str):
    """Deletes an event for the organization if the user has permission to do so"""
    
    # first check if the user has permission to delete an event for the organization
    event_id = DB_MANAGER.event_manager.get_event_id(event_title, org)
    if event_id is None:
        return "Event does not exist."
    if DB_MANAGER.event_permissions.get_write_permission(user, event_id):
        # delete the event for the organization
        DB_MANAGER.event_manager.delete_event(event_id)
        return event_title + " deleted for organization " + org
    
    return "You don't have permission."

"""
PERMISSIONS
"""

def create_organization_permissions(user: str, org: str, list_permission: bool, add_permission: bool, access_permission: bool, delete_permission: bool):
    """Creates the organization permissions for the user if the user has permission to do so"""
    
    
    if DB_MANAGER.organization_permissions.create_organization_permissions(user, org, list_permission, add_permission, access_permission, delete_permission):
        return "Organization permissions created for user " + user + " in organization " + org
    
    return "Unable to create organization permissions for user " + user + " in organization " + org

def create_room_permissions(user: str, org_name: str, room_name: str, list_permission: bool, reserve_permission: bool, perreserve_permission: bool, delete_permission: bool, write_permission: bool):
    """Creates the room permissions for the user if the user has permission to do so"""
    room_id = DB_MANAGER.room_manager.get_room_id(room_name, org_name)
    
    if not room_id:
        return "Room does not exist."
    
    if DB_MANAGER.room_permissions.create_room_permissions(user, room_id, list_permission, reserve_permission, perreserve_permission, delete_permission, write_permission):
        return "Room permissions created for user " + user + " in room " + str(room_name)
    
    return "Unable to create room permissions for user " + user + " in room " + str(room_name)

def create_event_permissions(user: str, org: str, event_title: str, read_permission: str, write_permission: str):
    """Creates the event permissions for the user if the user has permission to do so"""
    event_id = DB_MANAGER.event_manager.get_event_id(event_title, org)
    
    if not event_id:
        return "Event does not exist."

    if DB_MANAGER.event_permissions.create_event_permissions(user, event_id, read_permission, write_permission):
        return "Event permissions created for user " + user + " in event " + event_title
    
    return "Unable to create event permissions for user " + user + " in event " + str(event_id)


"""
CREATE EVENT AND UPDATE EVENT SHOULD BE DONE
ALSO MADE QUERY ETC.
"""


def find_free_time_slots(room_id, date, working_hours):
    room_manager = DB_MANAGER.room_manager
    reservations = room_manager.get_room_reservations(room_id)

    start_hour, end_hour = working_hours.split('-')
    opening_time = datetime.strptime(f"{date} {start_hour}", "%Y-%m-%d %H:%M")
    closing_time = datetime.strptime(f"{date} {end_hour}", "%Y-%m-%d %H:%M")

    occupied_slots = []
    for reservation in reservations:
        start = datetime.strptime(reservation['start_time'], "%Y-%m-%d %H:%M")
        end = start + timedelta(minutes=reservation['duration'])
        occupied_slots.append((start, end))

    free_slots = []
    current_time = opening_time
    while current_time < closing_time:
        # Check if the current time slot is occupied
        if not any(start <= current_time < end for start, end in occupied_slots):
            free_slots.append(current_time.strftime("%Y-%m-%d %H:%M"))
        current_time += timedelta(hours=1)  # assuming 1-hour slots

    return free_slots

def find_schedule(event_ids, org_name, room_name, date, working_hours):
    room_manager = DB_MANAGER.room_manager
    event_manager = DB_MANAGER.event_manager
    
    
    room_id = room_manager.get_room_id(room_name, org_name)
    
    free_slots = find_free_time_slots(room_id, date, working_hours)

    # Convert free slots into datetime objects for easier manipulation
    free_slots_dt = [datetime.strptime(slot, "%Y-%m-%d %H:%M") for slot in free_slots]

    # Store available schedules for each event
    available_schedules = {event_id: [] for event_id in event_ids}

    for event_id in event_ids:
        event = event_manager.get_event(event_id)
        duration_hours = ceil(event['duration'] / 60)

        for start_slot in free_slots_dt:
            end_slot = start_slot + timedelta(hours=duration_hours)
            # Check if all required slots are free and within working hours
            if all(start_slot + timedelta(hours=j) in free_slots_dt for j in range(duration_hours)) and end_slot <= free_slots_dt[-1] + timedelta(hours=1):
                available_schedules[event_id].append(start_slot.strftime("%Y-%m-%d %H:%M"))

    # Generate all possible non-overlapping schedule combinations
    schedule_combinations = []
    for schedule in product(*[available_schedules[event_id] for event_id in event_ids]):
        # Convert schedule to datetime for comparison
        schedule_dt = [datetime.strptime(time, "%Y-%m-%d %H:%M") for time in schedule]
        if all(schedule_dt[i] + timedelta(hours=ceil(event_manager.get_event(event_ids[i])['duration'] / 60)) <= schedule_dt[i+1] for i in range(len(schedule_dt)-1)):
            schedule_combinations.append(list(zip(event_ids, schedule)))

    return schedule_combinations


def roomView(user: str, org: str, start_datetime_str, end_datetime_str):
    if not DB_MANAGER.organization_permissions.get_list_permission(user, org):
        return "You don't have permission." 
    
    room_manager = DB_MANAGER.room_manager
    event_manager = DB_MANAGER.event_manager
    reservation_manager = DB_MANAGER.reservation_manager
    
    rooms = room_manager.get_all_rooms()
    room_events_report = {}

    # Convert datetime strings to datetime objects for comparison
    start_datetime = datetime.strptime(start_datetime_str, "%Y-%m-%d %H:%M")
    end_datetime = datetime.strptime(end_datetime_str, "%Y-%m-%d %H:%M")


    for room in rooms:
        reservations = reservation_manager.get_reservations_for_room(room['id'])
        events = []

        if not reservations:
            continue
        
        for reservation in reservations:
            if(reservation['description'] == 'DoggyBag'):
                continue
            try:
                
                reservation_start = datetime.strptime(reservation['start_time'], "%Y-%m-%d %H:%M")
                
            except ValueError:
                # Fallback for format without seconds
                reservation_start = datetime.strptime(reservation['start_time'], "%Y-%m-%d %H:%M:%S")
                return None
            
            reservation_end = reservation_start + timedelta(minutes=reservation['duration'])

            # Check if the reservation overlaps with the specified datetime range
            if (reservation_start <= end_datetime) and (reservation_end >= start_datetime):
                event = event_manager.get_event(reservation['event_id'])
                events.append(event)

        if events:
            room_events_report[room['name']] = events

    return room_events_report if room_events_report else "No rooms found for the specified datetime range."


def room_notify_user(room_name, organization):
    room_id = DB_MANAGER.room_manager.get_room_id(room_name, organization)
    return DB_MANAGER.notification_manager.get_users_for_room(room_id)

def event_notify_user(event_name, organization):
    event_id = DB_MANAGER.event_manager.get_event_id(event_name, organization)
    return DB_MANAGER.notification_manager.get_users_for_event(event_id)

def attach_observer(user: str, room_name, event_name, observation_type, organization):
    room_id = DB_MANAGER.room_manager.get_room_id(room_name, organization)
    event_id = DB_MANAGER.event_manager.get_event_id(event_name, organization)
    DB_MANAGER.notification_manager.create_observation(user, room_id, event_id, observation_type)
    return f"User {user} is now observing room {room_id} or event {event_id}"

def detach_observer(user: str, room_name, event_name, observation_type, organization):
    room_id = DB_MANAGER.room_manager.get_room_id(room_name, organization)
    event_id = DB_MANAGER.event_manager.get_event_id(event_name, organization)
    DB_MANAGER.notification_manager.delete_observation(user, room_id, event_id)
    return f"User {user} is now detached from room {room_id} or event {event_id}"


def fetch_reservations():
    return DB_MANAGER.reservation_manager.get_all_reservations()