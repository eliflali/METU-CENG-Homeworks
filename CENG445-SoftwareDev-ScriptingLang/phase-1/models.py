import datetime
from datetime import datetime, timedelta
import uuid
import hashlib
import secrets
import faker



class SingletonMeta(type):
    """
    A Singleton metaclass that creates only one instance of a class.
    """
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super().__call__(*args, **kwargs)
        return cls._instances[cls]


class UserManager(metaclass=SingletonMeta):
    def __init__(self):
        self.current_user = None
        self.users = {}  # Dictionary to store user objects

    def add_user(self, user):
        self.users[user.id] = user
        if self.current_user is None:
            self.current_user = user
            print ("Current user set to: ", self.current_user.get())

    def switch_user(self, user_id):
        if user_id in self.users:
            self.current_user = self.users[user_id]
            print ("User switched to: ", self.current_user.get())
        else:
            raise ValueError("User ID not found")

    def get_current_user(self):
        return self.current_user
    
    def get_user(self, user_id):
        return self.users.get(user_id)

    def get_users(self):
        return self.users

class CRUD:
    def __init__(self, **kwargs):
        # Create operation: Initialize the object with given arguments
        for key, value in kwargs.items():
            setattr(self, key, value)
        self.id = uuid.uuid4()

    def get(self):
        # Read operation: Return the object's data in a readable format
        return vars(self)

    def update(self, **kwargs):
        # Update operation: Update object attributes with given keyword arguments
        for key, value in kwargs.items():
            if hasattr(self, key):
                setattr(self, key, value)
            else:
                raise AttributeError(f"Attribute {key} does not exist.")

    def delete(self):
        # Delete operation: You might want to handle this by removing the object from
        # a collection or database, or setting a flag that marks it as deleted.
        del self
        return f"Object {self.id} deleted."
    

class User(CRUD):
    def __init__(self, username, email, fullname, passwd):
        super().__init__(username=username, email=email, fullname=fullname)
        self.password_hash = self._hash_password(passwd)
        self.session_token = None
        UserManager().add_user(self)

    def _hash_password(self, passwd):
        # Use hashlib or another library to hash the password
        return hashlib.sha256(passwd.encode('utf-8')).hexdigest()

    def auth(self, plainpass):
        # Check if the supplied password matches the user password hash
        return self.password_hash == self._hash_password(plainpass)

    def login(self):
        # Start a session for the user, return a random token to be used during the session
        self.session_token = secrets.token_hex()
        return self.session_token

    def checksession(self, token):
        # Check if the token is valid
        return token == self.session_token

    def logout(self):
        # End the session invalidating the token
        self.session_token = None  
    
    def get_id(self):
        return self.id
        
class Room(CRUD):
    def __init__(self, user, name, x, y, capacity, working_hours, permissions):
        super().__init__(user=user,  
                        name=name, 
                        x=x, 
                        y=y,
                        capacity=capacity, 
                        working_hours=working_hours, 
                        permissions=permissions)
        self.reservations = []  

    def get_name(self):
        return self.name

    def get_permissions(self):
        return self.permissions
    
    def get_capacity(self):
        return self.capacity
    
    def get_working_hours(self):
        return self.working_hours
    
    def get_permissions(self):
        return self.permissions
    
    def get_id(self):
        return self.id

    def is_in_rectangle(self, rect):
        #rect is defined as (x,y,w,h) 
        #rect[0] = x - min x, rect[1] = y - max y, rect[2] = w - max x, rect[3] = h - min y
        #x y are the coordinates of the bottom left corner. 
        #Top right corner will be x+w,y+h
        if(self.x > rect[0] and self.x < rect[2] and self.y > rect[3] and self.y < rect[1]):
            return True
        
        return False

    def is_available(self, start_time, duration):
        if(type(duration) is str):
            duration_parts = duration.split(', ')
            duration_delta = timedelta()

            for part in duration_parts:
                value, unit = part.split()
                if "day" in unit:
                    duration_delta += timedelta(days=int(value))
                elif "hour" in unit:
                    duration_delta += timedelta(hours=int(value))
                elif "minute" in unit:
                    duration_delta += timedelta(minutes=int(value))
        else:
            duration_delta = duration
        end_time = start_time + duration_delta

        if not self.is_working_hours(start_time, end_time):
            return False
        
        for reservation in self.reservations:
            reservation_start, reservation_end = reservation
            # Check for overlap between reservations
            if start_time < reservation_end and end_time > reservation_start:
                return False  # Room is already reserved during this time

        # If no conflicts were found, the room is available
        return True

    def is_working_hours(self, start_time, end_time):
        workhours_start_time_str, workhours_end_time_str = self.working_hours.split('-')
        workhours_start_time = datetime.strptime(workhours_start_time_str, "%H.%M").time()
        workhours_end_time = datetime.strptime(workhours_end_time_str, "%H.%M").time()
        # Check if the event's time interval falls within the room's working hours
        return workhours_start_time <= start_time.time() <= workhours_end_time and \
               workhours_start_time <= end_time.time() <= workhours_end_time
    
class Event(CRUD):
    def __init__(self, title, description, category, capacity, duration, weekly, permissions):
        super().__init__(title=title, 
                        description=description, 
                        category=category, 
                        capacity=capacity, 
                        duration=duration, 
                        weekly=weekly, 
                        permissions=permissions,
                        room_id = None,
                        start_time = None)

    def reserved_event(self, room_id, start_time):
        self.room_id = room_id
        self.start_time = start_time
    
    def get_permissions(self):
        return self.permissions
    
    def get_capacity(self):
        return self.capacity
    
    def get_duration(self):
        return self.duration
    
    def get_weekly(self):
        return self.weekly
    
    def get_category(self):
        return self.category
    
    def get_id(self):
        return self.id


class Organization(CRUD):
    objects = {}  # Dictionary to store Organization objects

    #map changed to mapOrganization since map is reserved word
    def __init__(self, owner, name, mapOrganization, backgroundImage = None):
        super().__init__(name=name, 
                        mapOrganization=mapOrganization, 
                        backgroundImage=backgroundImage)
        self.user_manager = owner #UserManager is singleton instance.
        self.owner = owner.get_current_user()
        self.rooms = {}
        self.events = {}

    @classmethod
    def listobjects(cls):
        # List all objects of this class
        return cls.objects

    def delete(self):
        for room in self.rooms:
            room.delete_room()
            
        for event in self.events:
            event.delete_event()
            
        del self

    def create_organization_room(self, name, x, y, capacity, working_hours, permissions):
        current_user = self.user_manager.get_current_user()
        if current_user is None:
            raise Exception("No current user set in UserManager.")
        new_room = Room(current_user, name, x, y, capacity, working_hours, permissions)
        id = new_room.get_id()
        self.rooms[id] = new_room
        self.objects[id] = f"room_{new_room.get_name}"
        return new_room

    def get_room(self, room_id):
        return self.rooms.get(room_id)
    
    
    #Update for room
    def update_organization_room(self, room_id, **kwargs):
        if room_id in self.rooms:
            return self.rooms[room_id].update_room(**kwargs)
        else:
            raise ValueError(f"No room found with ID {room_id}")

    #Delete for room
    def delete_organization_room(self, room_id):
        if room_id in self.rooms:
            self.rooms[room_id].delete_room()
            self.objects[room_id].delete()
            del self.rooms[room_id]
        else:
            raise ValueError(f"No room found with ID {room_id}")

    
    def create_organization_event(self, title, description, category, capacity, duration, weekly, permissions):
        current_user = self.user_manager.get_current_user()
        if current_user is None:
            raise Exception("No current user set in UserManager.")
        new_event = Event(title, description, category, capacity, duration, weekly, permissions)
        id = new_event.get_id()
        self.events[id] = new_event
        self.objects[id] = f"event_{new_event.title}"
        return new_event

    def get_event(self, event_id):
        return self.events.get(event_id)

    def update_organization_event(self, event_id, **kwargs):
        if event_id in self.events:
            return self.events[event_id].update_event(**kwargs)
        else:
            raise ValueError(f"No event found with ID {event_id}")

    def delete_organization_event(self, event_id):
        if event_id in self.events:
            self.events[event_id].delete_event()
            self.objects[event_id].delete()
            del self.events[event_id]
        else:
            raise ValueError(f"No event found with ID {event_id}")

    #taslak - no permissions included
    def reserve(self, event, room, start_time):
        # Check if the room is available for the specified time
        if not room.is_available(start_time, event.get_duration()):
            raise ValueError("Room is not available for the specified time.")

        # Check if the event is already assigned to a room
        if event.room_id is not None:
            raise ValueError("Event is already assigned to a room.")

        if(("WRITE" not in event.get_permissions() 
            or 
            "WRITE" not in room.get_permissions()) 
            and
            ("all" not in event.get_permissions()
            or
            "all" not in room.get_permissions())
            ):
            raise ValueError("User does not have permission.")
        
        if(type(event.weekly) is datetime 
            and 
            ("PERWRITE" not in room.get_permissions() or "all" not in room.get_permissions())):
            raise ValueError("User does not have permission for weekly events.")


        # Assign the room to the event and update its start time
        event.reserved_event(room.get_id(), start_time)

        # Add the reservation to the room's reservations list
        duration_parts = event.get_duration().split(', ')
        duration_delta = timedelta()

        for part in duration_parts:
            value, unit = part.split()
            if "day" in unit:
                duration_delta += timedelta(days=int(value))
            elif "hour" in unit:
                duration_delta += timedelta(hours=int(value))
            elif "minute" in unit:
                duration_delta += timedelta(minutes=int(value))

        room.reservations.append((start_time, start_time + duration_delta))

        # Return a success message or reservation details
        return f"Room {room.name} reserved for event {event.title} starting at {start_time}."

    #rect is defined as (x,y,w,h) 
    #x y are the coordinates of the bottom left corner. 
    #Top right corner will be x+w,y+h
    def find_room(self, event, rect, start, end):
        available_rooms = []

        for room_id, room in self.rooms.items():
            if room.is_in_rectangle(rect):
                if room.is_available(start, end - start):
                    "available"
                    if room.capacity >= event.get_capacity():
                        "capacity ok"
                        available_rooms.append(room)

        return available_rooms
    
    def find_schedule(self, eventlist, rect, start, end):
        schedule = {}
        for event in eventlist:
            available_rooms = self.find_room(event, rect, start, end)

            for room in available_rooms:
                current_time = start
                while current_time < end:
                    # Calculate event end time
                    duration_parts = event.get_duration().split(', ')
                    duration_delta = timedelta()
                    for part in duration_parts:
                        value, unit = part.split()
                        if "day" in unit:
                            duration_delta += timedelta(days=int(value))
                        elif "hour" in unit:
                            duration_delta += timedelta(hours=int(value))
                        elif "minute" in unit:
                            duration_delta += timedelta(minutes=int(value))

                    event_end_time = current_time + duration_delta

                    # Check if the room is available for the event duration
                    if room.is_available(current_time, event_end_time - current_time):
                        # Schedule the event
                        schedule[event.id] = {'room_id': room.id, 'start_time': current_time}
                        break

                    # Increment current time
                    current_time += timedelta(minutes=15)  # Adjust the time increment as needed

                if event.id not in schedule:
                    return None  # Unable to schedule one of the events

        return schedule

    def reassign(self, event, room, start_time):
        # Check if the room is available for the specified time
        if not room.is_available(start_time, event.get_duration()):
            raise ValueError("Room is not available for the specified time.")

        if(("WRITE" not in event.get_permissions() 
            or 
            "WRITE" not in room.get_permissions()) 
            and
            ("all" not in event.get_permissions()
            or
            "all" not in room.get_permissions())
            ):
            raise ValueError("User does not have permission.")
        
        if(type(event.weekly) is datetime 
            and 
            ("PERWRITE" not in room.get_permissions() or "all" not in room.get_permissions())):
            raise ValueError("User does not have permission for weekly events.")

        # Assign the room to the event and update its start time
        event.reserved_event(room.get_id(), start_time)

        duration_parts = event.get_duration().split(', ')
        duration_delta = timedelta()

        for part in duration_parts:
            value, unit = part.split()
            if "day" in unit:
                duration_delta += timedelta(days=int(value))
            elif "hour" in unit:
                duration_delta += timedelta(hours=int(value))
            elif "minute" in unit:
                duration_delta += timedelta(minutes=int(value))

        room.reservations.append((start_time, start_time + duration_delta))


        # Return a success message or reservation details
        return f"Room {room.name} reserved(reassigned) for event {event.title} starting at {start_time}."
    def _is_in_rect(self, room, rect):
        rect_x, rect_y, rect_w, rect_h = rect
        room_x, room_y = room.x, room.y

        # Calculate the top right coordinates of the rect
        rect_x_max = rect_x + rect_w
        rect_y_max = rect_y + rect_h

        # Check if the room's coordinates are within the rect bounds
        return (rect_x <= room_x <= rect_x_max) and (rect_y <= room_y <= rect_y_max)
    
    def query(self, rect, title, category, room):
        matches = []  # List to hold all matches
        for key in self.events:
            event = self.events[key]
            # If a specific room is specified, skip events that are not in this room
            if event.room_id != room.get_id():
                continue

            # Retrieve the associated room object for the event
            associated_room = self.rooms[event.room_id]
            # If rect is specified, use the _is_in_rect method to check if the room is within the rect
            if not associated_room or rect and not associated_room.is_in_rectangle(rect) :
                continue

            # Check if the event's title contains the title substring
            if title and title.lower() not in event.title.lower():
                continue

            # Check if the event's category matches the category substring
            if category and category.lower() not in event.category.lower():
                continue

            # If all conditions are met, add the tuple to the matches list
            matches.append((event, associated_room, event.start_time))
        return iter(matches)  # Convert the list to an iterator before returning

class View:
    def __init__(self, owner):
        self.owner = owner
        self.queries = {}
        self.next_query_id = 1

    def addquery(self, organization, **kwargs):
        query_id = self.next_query_id
        self.queries[query_id] = {'organization': organization, 'params': kwargs}
        self.next_query_id += 1
        return query_id

    def delquery(self, query_id):
        if query_id in self.queries:
            del self.queries[query_id]
        else:
            raise ValueError("Query ID not found")

    def roomView(self, start, end):
        room_based_results = {}
        for query_id, query_info in self.queries.items():
            matches = query_info['organization'].query(**query_info['params'])
            for event, room, event_start in matches:
                if start <= event_start <= end:
                    if room not in room_based_results:
                        room_based_results[room] = []
                    room_based_results[room].append((event, event_start))
        return room_based_results


    def dayView(self, start, end):
        day_based_results = {}
        current_day = start
        while current_day <= end:
            day_based_results[current_day] = []
            for query_id, query_info in self.queries.items():
                matches = query_info['organization'].query(**query_info['params'])
                for event, room, event_start in matches:
                    if event_start.date() == current_day.date():
                        day_based_results[current_day].append((event, room, event_start))
            current_day += timedelta(days=1)
        return {date: events for date, events in day_based_results.items() if events} 

def create_fake_data():
    fake = faker.Faker()
    user_manager = UserManager()
    user1 = User(username="user1", email=fake.email(), fullname=fake.name(), passwd="123")
    user2 = User(username="user2", email=fake.email(), fullname=fake.name(), passwd="123")
    user3 = User(username="user3", email=fake.email(), fullname=fake.name(), passwd="123")

    user_manager.switch_user(user1.get_id())
    
    org1 = Organization(user_manager, name="org1", mapOrganization="map1")
    time_str0 = "10-01-2023,10:01"
    time_str1 = "10-01-2023,10:01"
    time1 = datetime.strptime(time_str1, "%d-%m-%Y,%H:%M")
    time0 = datetime.strptime(time_str0, "%d-%m-%Y,%H:%M")

    start = "01-01-2023,00:00"
    end = "31-01-2023,00:00"

    start_datetime = datetime.strptime(start, "%d-%m-%Y,%H:%M")
    end_datetime = datetime.strptime(end, "%d-%m-%Y,%H:%M")

    duration = "15 minutes"

    room1 = org1.create_organization_room(name="room1", x=1, y=1, capacity=1000000, working_hours="09.00-17.00", permissions="all")
    room2 = org1.create_organization_room(name="room2", x=2, y=2, capacity=200000, working_hours="09.00-17.00", permissions="all")
    room3 = org1.create_organization_room(name="room3", x=2, y=2, capacity=200000, working_hours="09.00-17.00", permissions="all")
    event1 = org1.create_organization_event(title="event1", description="desc1", category="cat1", capacity=10, duration=duration, weekly=True, permissions="all")   
    event2 = org1.create_organization_event(title="event2", description="desc2", category="cat2", capacity=20, duration=duration, weekly=True, permissions="all")

    event3 = org1.create_organization_event(title="event2", description="desc2", category="cat2", capacity=20, duration=duration, weekly=True, permissions="all")
    view = View(user1)
    
    
    print(org1.reserve(event1, room1, time1))
    print(org1.reserve(event2, room2, time1))
    view.addquery(org1, rect = (0,100000,100000,0), title = "event2", category = "cat2", room = room2)
    print(f"view.queries: {view.queries}")
    room_view = view.roomView(start_datetime, end_datetime)
    day_view = view.dayView(start_datetime, end_datetime)

    print(f"room_view: {room_view}")
    print(f"day_view: {day_view}")
    #rect is defined as (x,y,w,h) 
    #rect[0] = x - min x, rect[1] = y - max y, rect[2] = w - max x, rect[3] = h - min y
    #x y are the coordinates of the bottom left corner. 
    #Top right corner will be x+w,y+h
    print(org1.find_room(event1, (0,100,100,0), time0, time1))
    print(org1.reassign(event1, room3, time1))
    print(org1.find_schedule([event1, event2], (0,100,100,0), time0, time1))

def test_organization():
    create_fake_data()
    
test_organization()



