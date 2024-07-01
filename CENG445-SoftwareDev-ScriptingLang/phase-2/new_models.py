import datetime
import sqlite3
import hashlib
import uuid
from datetime import datetime, timedelta

class DBManager():
    def __init__(self, db_path: str) -> None:
        self._db_path = db_path
    
    def _connect(self):
        return sqlite3.connect(self._db_path)

    def _execute_query(self, query, params=()):
        with self._connect() as conn:
            cursor = conn.cursor()
            cursor.execute(query, params)
            conn.commit()
            return cursor
    
    def _execute_query_with_result(self, query, params=()):
        with self._connect() as conn:
            cursor = conn.cursor()
            cursor.execute(query, params)
            return cursor.fetchall()


class UserManager(DBManager):
    def __init__(self, db_path: str) -> None:
        super().__init__(db_path)
        self._create_users_table()
        self.current_user = None
        self.current_username = None

    def _create_users_table(self) -> None:
        self._execute_query("""
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT UNIQUE NOT NULL,
                password_hash TEXT NOT NULL,
                email TEXT,
                fullname TEXT,
                token TEXT
            );
        """)

    def register_user(self, username: str, password: str, email: str, fullname: str) -> None:
        password_hash = self._hash_password(password)
        try:
            self._execute_query("""
                INSERT INTO users (username, password_hash, email, fullname)
                VALUES (?, ?, ?, ?);
            """, (username, password_hash, email, fullname))
        except sqlite3.IntegrityError:
            return False
        return True

    def authenticate_user(self, username: str, password: str) :
        result = self._execute_query_with_result("""SELECT password_hash FROM users WHERE username = ?;""", (username,))
        
        if result and self._check_password(password, result[0][0]):
            self.current_user = self.get_user(username)
            self.current_username = username
            return self._generate_token(username)
        return None

    def get_current_user(self):
        return self.current_user

    def get_current_username(self):
        return self.current_username

    def _hash_password(self, password: str) -> str:
        return hashlib.sha256(password.encode()).hexdigest()

    def _check_password(self, password: str, password_hash: str) -> bool:
        return self._hash_password(password) == password_hash

    def _generate_token(self, username: str) -> str:
        token = str(uuid.uuid4())
        self._execute_query("""UPDATE users SET token = ? WHERE username = ?;""", (token, username))
        return token

    def verify_token(self, username: str, token: str) -> bool:
        result = self._execute_query_with_result("""SELECT token FROM users WHERE username = ?;""", (username,))
        return token == result[0][0] if result else False

    def logout_user(self, username: str) -> None:
        self._execute_query("""UPDATE users SET token = NULL WHERE username = ?;""", (username,))

    def get_user(self, username: str) -> dict:
        result = self._execute_query_with_result("""SELECT username, email, fullname FROM users WHERE username = ?;""", (username,))
        return {
            "username": result[0][0],
            "email": result[0][1],
            "fullname": result[0][2]
        } if result else None

    def update_user(self, username: str, email: str, fullname: str) -> None:
        self._execute_query("""UPDATE users SET email = ?, fullname = ? WHERE username = ?;""", (email, fullname, username))

    def delete_user(self, username: str) -> None:
        self._execute_query("""DELETE FROM users WHERE username = ?;""", (username,))

    def find_token(self, token: str) -> str:
        result = self._execute_query_with_result("SELECT username FROM users WHERE token = ?;", (token,))
        return result[0][0] if result else None

class OrganizationPermissionsManager(DBManager):
    def __init__(self, db_path: str) -> None:
        super().__init__(db_path)
        self._create_organization_permissions_table()
    
    def _create_organization_permissions_table(self) -> None:
        self._execute_query("""
            CREATE TABLE IF NOT EXISTS organization_permissions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user TEXT NOT NULL,
                organization TEXT NOT NULL,
                list_permission BOOLEAN,
                add_permission BOOLEAN,
                access_permission BOOLEAN,
                delete_permission BOOLEAN,
                FOREIGN KEY(user) REFERENCES users(username) ON DELETE CASCADE,
                FOREIGN KEY(organization) REFERENCES organizations(name) ON DELETE CASCADE
            );
        """)
    
    def create_organization_permissions(self, user: str, organization: str, list_permission: bool, add_permission: bool, access_permission: bool, delete_permission: bool) -> None:
        try:
            self._execute_query("""
                INSERT INTO organization_permissions (user, organization, list_permission, add_permission, access_permission, delete_permission)
                VALUES (?, ?, ?, ?, ?, ?);
            """, (user, organization, list_permission, add_permission, access_permission, delete_permission))
        except sqlite3.IntegrityError:
            return False
        return True
    
    
    def delete_organization_permissions(self, user: str, organization: str) -> None:
        self._execute_query("""DELETE FROM organization_permissions WHERE user = ? AND organization = ?;""", (user, organization))
    
    def get_organization_permissions(self, user: str, organization: str) -> dict:
        result = self._execute_query_with_result("""SELECT * FROM organization_permissions WHERE user = ? AND organization = ?;""", (user, organization))
        return {"id": result[0][0], 
                "user": result[0][1], 
                "organization": result[0][2], 
                "list_permission": result[0][3], 
                "add_permission": result[0][4], 
                "access_permission": result[0][5], 
                "delete_permission": result[0][6]
                } if result else None
    
    def get_list_permission(self, user: str, organization: str) -> bool:
        result = self._execute_query_with_result("""SELECT list_permission FROM organization_permissions WHERE user = ? AND organization = ?;""", (user, organization))
        return result[0][0] if result else None
    
    def get_add_permission(self, user: str, organization: str) -> bool:
        result = self._execute_query_with_result("""SELECT add_permission FROM organization_permissions WHERE user = ? AND organization = ?;""", (user, organization))
        return result[0][0] if result else None

    def get_access_permission(self, user: str, organization: str) -> bool:
        result = self._execute_query_with_result("""SELECT access_permission FROM organization_permissions WHERE user = ? AND organization = ?;""", (user, organization))
        return result[0][0] if result else None
    
    def get_delete_permission(self, user: str, organization: str) -> bool:
        result = self._execute_query_with_result("""SELECT delete_permission FROM organization_permissions WHERE user = ? AND organization = ?;""", (user, organization))
        return result[0][0] if result else None
    
    def update_permission(self, user: str, organization: str, field: str, value: bool) -> None:
        self._execute_query("""UPDATE organization_permissions SET ? = ? WHERE user = ? AND organization = ?;""", (field, value, user, organization))
    

class RoomPermissionsManager(DBManager):
    def __init__(self, db_path: str) -> None:
        super().__init__(db_path)
        self._create_room_permissions_table()
    
    def _create_room_permissions_table(self) -> None:
        self._execute_query("""
            CREATE TABLE IF NOT EXISTS room_permissions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user TEXT NOT NULL,
                room_id INTEGER NOT NULL,
                list_permission BOOLEAN,
                reserve_permission BOOLEAN,
                perreserve_permission BOOLEAN,
                delete_permission BOOLEAN,
                write_permission BOOLEAN,
                FOREIGN KEY(user) REFERENCES users(username) ON DELETE CASCADE,
                FOREIGN KEY(room_id) REFERENCES rooms(id) ON DELETE CASCADE
            );
        """)
    
    def create_room_permissions(self, user: str, room_id: int, list_permission: bool, reserve_permission: bool, perreserve_permission: bool, delete_permission: bool, write_permission: bool) -> None:
        try:
            self._execute_query("""
                INSERT INTO room_permissions (user, room_id, list_permission, reserve_permission, perreserve_permission, delete_permission, write_permission)
                VALUES (?, ?, ?, ?, ?, ?, ?);
            """, (user, room_id, list_permission, reserve_permission, perreserve_permission, delete_permission, write_permission))
        except sqlite3.IntegrityError:
            return False
        return True
    
    def delete_room_permissions(self, user: str, room_id: int) -> None:
        self._execute_query("""DELETE FROM room_permissions WHERE user = ? AND room_id = ?;""", (user, room_id))
    
    def get_room_permissions(self, user: str, room_id: int) -> dict:
        result = self._execute_query_with_result("""SELECT * FROM room_permissions WHERE user = ? AND room_id = ?;""", (user, room_id))
        return {"id": result[0][0], 
                "user": result[0][1], 
                "room_id": result[0][2], 
                "list_permission": result[0][3], 
                "reserve_permission": result[0][4], 
                "perreserve_permission": result[0][5], 
                "delete_permission": result[0][6],
                "write_permission": result[0][7]
                } if result else None
    
    def get_list_permission(self, user: str, room_id: int) -> bool:
        result = self._execute_query_with_result("""SELECT list_permission FROM room_permissions WHERE user = ? AND room_id = ?;""", (user, room_id))
        return result[0][0] if result else None
    
    def get_reserve_permission(self, user: str, room_id: int) -> bool:
        result = self._execute_query_with_result("""SELECT reserve_permission FROM room_permissions WHERE user = ? AND room_id = ?;""", (user, room_id))
        return result[0][0] if result else None
    
    def get_perreserve_permission(self, user: str, room_id: int) -> bool:
        result = self._execute_query_with_result("""SELECT perreserve_permission FROM room_permissions WHERE user = ? AND room_id = ?;""", (user, room_id))
        return result[0][0] if result else None
    
    def get_delete_permission(self, user: str, room_id: int) -> bool:
        result = self._execute_query_with_result("""SELECT delete_permission FROM room_permissions WHERE user = ? AND room_id = ?;""", (user, room_id))
        return result[0][0] if result else None
    
    def get_write_permission(self, user: str, room_id: int) -> bool:
        result = self._execute_query_with_result("""SELECT write_permission FROM room_permissions WHERE user = ? AND room_id = ?;""", (user, room_id))
        return result[0][0] if result else None
    
    def update_permission(self, user: str, room_id: int, field: str, value: bool) -> None:
        self._execute_query("""UPDATE room_permissions SET ? = ? WHERE user = ? AND room_id = ?;""", (field, value, user, room_id))
        
class EventPermissionsManager(DBManager):
    def __init__(self, db_path: str) -> None:
        super().__init__(db_path)
        self._create_event_permissions_table()
    
    def _create_event_permissions_table(self) -> None:
        self._execute_query("""
            CREATE TABLE IF NOT EXISTS event_permissions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user TEXT NOT NULL,
                event_id INTEGER NOT NULL,
                read_permission BOOLEAN,
                write_permission BOOLEAN,
                FOREIGN KEY(user) REFERENCES users(username) ON DELETE CASCADE,
                FOREIGN KEY(event_id) REFERENCES events(id) ON DELETE CASCADE
            );
        """)
    
    def create_event_permissions(self, user: str, event_id: int, read_permission: bool, write_permission: bool) -> None:
        try:
            self._execute_query("""
                INSERT INTO event_permissions (user, event_id, read_permission, write_permission)
                VALUES (?, ?, ?, ?);
            """, (user, event_id, read_permission, write_permission))
        except sqlite3.IntegrityError:
            return False
        return True
    
    def delete_event_permissions(self, user: str, event_id: int) -> None:
        self._execute_query("""DELETE FROM event_permissions WHERE user = ? AND event_id = ?;""", (user, event_id))
    
    def get_event_permissions(self, user: str, event_id: int) -> dict:
        result = self._execute_query_with_result("""SELECT * FROM event_permissions WHERE user = ? AND event_id = ?;""", (user, event_id))
        return {"id": result[0][0], 
                "user": result[0][1], 
                "event_id": result[0][2], 
                "read_permission": result[0][3], 
                "write_permission": result[0][4]
                } if result else None
    
    def get_read_permission(self, user: str, event_id: int) -> bool:
        result = self._execute_query_with_result("""SELECT read_permission FROM event_permissions WHERE user = ? AND event_id = ?;""", (user, event_id))
        return result[0][0] if result else None

    def get_write_permission(self, user: str, event_id: int) -> bool:
        result = self._execute_query_with_result("""SELECT write_permission FROM event_permissions WHERE user = ? AND event_id = ?;""", (user, event_id))
        return result[0][0] if result else None
        
    def update_read_permission(self, user: str, event_id: int, read_permission: bool) -> None:
        self._execute_query("""UPDATE event_permissions SET read_permission = ? WHERE user = ? AND event_id = ?;""", (read_permission, user, event_id))
    
    def update_write_permission(self, user: str, event_id: int, write_permission: bool) -> None:
        self._execute_query("""UPDATE event_permissions SET write_permission = ? WHERE user = ? AND event_id = ?;""", (write_permission, user, event_id))
    
class OrganizationManager(DBManager):
    def __init__(self, db_path: str) -> None:
        super().__init__(db_path)
        self._create_organizations_table()

    def _create_organizations_table(self) -> None:
        self._execute_query("""
            CREATE TABLE IF NOT EXISTS organizations (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT UNIQUE NOT NULL,
                description TEXT,
                owner TEXT NOT NULL,
                FOREIGN KEY(owner) REFERENCES users(username) ON DELETE CASCADE
            );
        """)

    def create_organization(self, name: str, description: str, owner: str) -> bool:
        try:
            self._execute_query("""
                INSERT INTO organizations (name, description, owner)
                VALUES (?, ?, ?);
            """, (name, description, owner))
        except sqlite3.IntegrityError:
            return False
        return True

    def delete_organization(self, name: str) -> None:
        self._execute_query("""DELETE FROM organizations WHERE name = ?;""", (name,))

    def get_all_organizations(self) -> list:
        result = self._execute_query_with_result("""SELECT * FROM organizations;""")
        return [{"id": row[0], "name": row[1], "description": row[2], "owner": row[3]} for row in result] if result else None

    def get_organization(self, name: str) -> dict:
        result = self._execute_query_with_result("""SELECT * FROM organizations WHERE name = ?;""", (name,))
        if result:
            return {
                "id": result[0][0],  # Access the first column of the first row
                "name": result[0][1],
                "description": result[0][2],
                "owner": result[0][3]
            }
        return None


    def get_organization_owner(self, name: str):
        result = self._execute_query_with_result("""SELECT owner FROM organizations WHERE name = ?;""", (name,))
        return result[0][0] if result else None

    def get_rooms(self, name: str) -> list:
        result = self._execute_query_with_result("""SELECT * FROM rooms WHERE organization = ?;""", (name,))
        return [{"id": row[0], "name": row[1], "x": row[2], "y": row[3], "capacity": row[4], "working_hours": row[5], "owner": row[6], "organization": row[7]} for row in result] if result else None
    
    def get_events(self, name: str) -> list:
        result = self._execute_query_with_result("""SELECT * FROM events WHERE organization = ?;""", (name,))
        return [{"id": row[0], "title": row[1], "description": row[2], "category": row[3], "capacity": row[4], "duration": row[5], "weekly": row[6], "organization": row[7], "owner": row[8]} for row in result] if result else None

    def update_organization(self, name: str, field: str, value: str) -> None:
        print(f"Updating organization {name} field {field} to {value}")
        allowed_fields = ["name", "description", "owner"]  # Add other valid fields as needed
        if field not in allowed_fields:
            raise ValueError("Invalid field name")

        query = f"UPDATE organizations SET {field} = ? WHERE name = ?;"
        self._execute_query(query, (value, name))
    
    
class RoomManager(DBManager):
    def __init__(self, db_path: str) -> None:
        super().__init__(db_path)
        self._create_rooms_table()
    
    def _create_rooms_table(self) -> None:
        self._execute_query("""
            CREATE TABLE IF NOT EXISTS rooms (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                x INTEGER,
                y INTEGER,
                capacity INTEGER,
                working_hours TEXT,
                owner TEXT NOT NULL,
                organization TEXT NOT NULL,
                FOREIGN KEY(owner) REFERENCES users(username) ON DELETE CASCADE,
                FOREIGN KEY(organization) REFERENCES organizations(name) ON DELETE CASCADE,
                UNIQUE(name, organization)
            );
        """)

    def create_room(self, name: str, x: int, y: int, capacity: int, working_hours: str, owner: str, organization: str) -> bool:
        try:
            self._execute_query("""
                INSERT INTO rooms (name, x, y, capacity, working_hours, owner, organization)
                VALUES (?, ?, ?, ?, ?, ?, ?);
            """, (name, x, y, capacity, working_hours, owner, organization))
        except sqlite3.IntegrityError:
            return False
        return True

    # Other methods follow the same pattern but ensure to remove references to the 'permissions' field

    # For example:
    def delete_room(self, id) -> None:
        self._execute_query("""DELETE FROM rooms WHERE id = ?;""", (id,))
        
    def update_room(self, id, name: str, x: int, y: int, capacity: int, working_hours: int, owner: str, organization: str) -> None:
        self._execute_query("""UPDATE rooms SET name = ?, x = ?, y = ?, capacity = ?, working_hours = ?, owner = ?, organization = ? WHERE id = ?;""", (name, x, y, capacity, working_hours, owner, organization, id))
    
    def get_room_id(self, name: str, organization: str) -> int:
        result = self._execute_query_with_result("""SELECT id FROM rooms WHERE name = ? AND organization = ?;""", (name, organization))
        return result[0][0] if result else None
    
    def get_all_rooms(self) -> list:
        result = self._execute_query_with_result("""SELECT * FROM rooms;""")
        return [{"id": row[0], "name": row[1], "x": row[2], "y": row[3], "capacity": row[4], "working_hours": row[5], "owner": row[6], "organization": row[7]} for row in result] if result else None
    
    def get_room(self, id: int) -> dict:
        result = self._execute_query_with_result("""SELECT * FROM rooms WHERE id = ?;""", (id,))
        return {"id": result[0][0], 
                "name": result[0][1], 
                "x": result[0][2], 
                "y": result[0][3], 
                "capacity": result[0][4], 
                "working_hours": result[0][5], 
                "owner": result[0][6], 
                "organization": result[0][7]
                } if result else None
    
    def get_room_owner(self, id: int):
        result = self._execute_query_with_result("""SELECT owner FROM rooms WHERE id = ?;""", (id,))
        return result[0][0] if result else None
    
    def get_room_organization(self, id: int):
        result = self._execute_query_with_result("""SELECT organization FROM rooms WHERE id = ?;""", (id,))
        return result[0][0] if result else None
    
    def get_room_reservations(self, room_id: int):
        result = self._execute_query_with_result("""
            SELECT start_time, duration, weekly FROM reservations WHERE room_id = ?;
        """, (room_id,))
        if result:
            return [{'start_time': row[0], 'duration': row[1], 'weekly': row[2]} for row in result]
        else:
            return []
        
class EventManager(DBManager):
    def __init__(self, db_path: str) -> None:
        super().__init__(db_path)
        self._create_events_table()
    
    def _create_events_table(self) -> None:
        self._execute_query("""
            CREATE TABLE IF NOT EXISTS events (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                title TEXT NOT NULL,
                description TEXT,
                category TEXT,
                capacity INTEGER,
                duration INTEGER,
                weekly BOOLEAN,
                organization TEXT NOT NULL,
                owner TEXT NOT NULL,
                FOREIGN KEY(organization) REFERENCES organizations(name) ON DELETE CASCADE,
                FOREIGN KEY(owner) REFERENCES users(username) ON DELETE CASCADE,
                UNIQUE(title, organization)
            );
        """)

    def create_event(self, title: str, description: str, category: str, capacity: int, duration: int, weekly: bool, organization: str, owner: str) -> bool:
        try:
            self._execute_query("""
                INSERT INTO events (title, description, category, capacity, duration, weekly, organization, owner)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?);
            """, (title, description, category, capacity, duration, weekly, organization, owner))
        except sqlite3.IntegrityError:
            return False
        return True
    
    def delete_event(self, id: int) -> None:
        self._execute_query("""DELETE FROM events WHERE id = ?;""", (id,))
        
    def update_event(self, event_id: int, event_title: str, capacity: int, duration: int, weekly: bool, description: str, category: str) -> None:
        self._execute_query("""UPDATE events SET title = ?, description = ?, category = ?, capacity = ?, duration = ?, weekly = ? WHERE id = ?;""", (event_title, description, category, capacity, duration, weekly, event_id))
        
    def get_event_id(self, title: str, organization: str) -> int:
        result = self._execute_query_with_result("""SELECT id FROM events WHERE title = ? AND organization = ?;""", (title, organization))
        return result[0][0] if result else None
    
    def get_all_events(self) -> list:
        result = self._execute_query_with_result("""SELECT * FROM events;""")
        return [{"id": row[0], "title": row[1], "description": row[2], "category": row[3], "capacity": row[4], "duration": row[5], "weekly": row[6], "organization": row[7], "owner": row[8]} for row in result] if result else None
    
    def get_event(self, id: int) -> dict:
        result = self._execute_query_with_result("""SELECT * FROM events WHERE id = ?;""", (id,))
        return {"id": result[0][0], 
                "title": result[0][1], 
                "description": result[0][2], 
                "category": result[0][3], 
                "capacity": result[0][4], 
                "duration": result[0][5], 
                "weekly": result[0][6], 
                "organization": result[0][7],
                "owner": result[0][8]
                } if result else None

    def get_event_organization(self, id: int):
        result = self._execute_query_with_result("""SELECT organization FROM events WHERE id = ?;""", (id,))
        return result[0][0] if result else None
    
    def get_event_owner(self, id: int):
        result = self._execute_query_with_result("""SELECT owner FROM events WHERE id = ?;""", (id,))
        return result[0][0] if result else None


class ReservationManager(DBManager):
    def __init__(self, db_path: str) -> None:
        super().__init__(db_path)
        self._create_reservations_table()
    
    def _create_reservations_table(self) -> None:
        self._execute_query("""
            CREATE TABLE IF NOT EXISTS reservations (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                room_id INTEGER NOT NULL,
                event_id INTEGER NOT NULL,
                user TEXT NOT NULL,
                start_time DATETIME NOT NULL,
                duration INTEGER NOT NULL,
                weekly BOOLEAN NOT NULL,
                description TEXT,
                FOREIGN KEY(room_id) REFERENCES rooms(id) ON DELETE CASCADE,
                FOREIGN KEY(event_id) REFERENCES events(id) ON DELETE CASCADE,
                FOREIGN KEY(user) REFERENCES users(username) ON DELETE CASCADE
            );
        """)
    
    def create_reservation(self, room_id: int, event_id: int, user: str, start_time: str, duration: int, weekly: bool, description: str) -> None:
        try:
            self._execute_query("""
                INSERT INTO reservations (room_id, event_id, user, start_time, duration, weekly, description)
                VALUES (?, ?, ?, ?, ?, ?, ?);
            """, (room_id, event_id, user, start_time, duration, weekly, description))
        except sqlite3.IntegrityError:
            return False
        return True
    
    def delete_reservation(self, id) -> None:
        self._execute_query("""DELETE FROM reservations WHERE id = ?;""", (id,))
    
    def get_reservation_id(self, room_id: int, event_id: int, user: str, start_time: str, duration: int, weekly: bool) -> int:
        result = self._execute_query_with_result("""SELECT id FROM reservations WHERE room_id = ? AND event_id = ? AND user = ? AND start_time = ? AND duration = ? AND weekly = ?;""", (room_id, event_id, user, start_time, duration, weekly))
        return result[0][0] if result else None
    
    def get_reservations_for_room(self, room_id: int) -> list:
        result = self._execute_query_with_result("""SELECT * FROM reservations WHERE room_id = ?;""", (room_id,))
        return [{"id": row[0], "room_id": row[1], "event_id": row[2], "user": row[3], "start_time": row[4], "duration": row[5], "weekly": row[6], "description": row[7]} for row in result] if result else None
    
    def get_reservations_for_user(self, user: str) -> list:
        result = self._execute_query_with_result("""SELECT * FROM reservations WHERE user = ?;""", (user,))
        return [{"id": row[0], "room_id": row[1], "event_id": row[2], "user": row[3], "start_time": row[4], "duration": row[5], "weekly": row[6], "description": row[7]} for row in result] if result else None
    
    def get_reservations_for_event(self, event_id: int) -> list:
        result = self._execute_query_with_result("""SELECT * FROM reservations WHERE event_id = ?;""", (event_id,))
        return [{"id": row[0], "room_id": row[1], "event_id": row[2], "user": row[3], "start_time": row[4], "duration": row[5], "weekly": row[6], "description": row[7]} for row in result] if result else None
    
    def get_reservation(self, id: int) -> dict:
        result = self._execute_query_with_result("""SELECT * FROM reservations WHERE id = ?;""", (id,))
        return {"id": result[0][0], 
                "room_id": result[0][1], 
                "event_id": result[0][2], 
                "user": result[0][3], 
                "start_time": result[0][4], 
                "duration": result[0][5], 
                "weekly": result[0][6], 
                "description": result[0][7]
                } if result else None

    
    def get_all_reservations(self) -> list:
        result = self._execute_query_with_result("""SELECT * FROM reservations;""")
        return [{"id": row[0], "room_id": row[1], "event_id": row[2], "user": row[3], "start_time": row[4], "duration": row[5], "weekly": row[6], "description": row[7]} for row in result] if result else None
    
    def is_room_available(self, room_id: int, requested_start_time: str, requested_duration: int) -> bool:
        """
        Check if a room is available for the given time and duration, considering weekly recurring reservations.

        Args:
        - room_id (int): The ID of the room to check.
        - requested_start_time (str): The start time for the requested reservation.
        - requested_duration (int): The duration of the requested reservation in minutes.

        Returns:
        - bool: True if the room is available, False otherwise.
        """
        # Convert the requested start time to a datetime object
        requested_start = datetime.strptime(requested_start_time, "%Y-%m-%d %H:%M") 

        requested_duration = timedelta(minutes=int(requested_duration))
        # Calculate the end time of the requested reservation
        requested_end = requested_start + requested_duration

        # Retrieve all reservations for the specified room
        existing_reservations = self.get_reservations_for_room(room_id)
        
        if not existing_reservations:
            return True

        # Check for time conflicts with existing reservations
        for reservation in existing_reservations:
            existing_start = datetime.strptime(reservation['start_time'], "%Y-%m-%d %H:%M")
            existing_end = existing_start + timedelta(minutes=reservation['duration'])

            # Check for overlap with non-weekly reservations
            if not reservation['weekly'] and (requested_start < existing_end) and (requested_end > existing_start):
                return False

            # Check for overlap with weekly reservations
            if reservation['weekly']:
                for week in range(0, 52):  # Check for conflicts over a year
                    weekly_existing_start = existing_start + timedelta(weeks=week)
                    weekly_existing_end = existing_end + timedelta(weeks=week)

                    if (requested_start < weekly_existing_end) and (requested_end > weekly_existing_start):
                        return False

        # No overlaps found, the room is available
        return True

            
    def get_room_reservations(self, room_id: int) -> list:
        result = self._execute_query_with_result("""SELECT * FROM reservations WHERE room_id = ?;""", (room_id,))
        return [{"id": row[0], "room_id": row[1], "event_id": row[2], "user": row[3], "start_time": row[4], "duration": row[5], "weekly": row[6], "description": row[7]} for row in result] if result else None
    
    
class ObservationManager(DBManager):
    def __init__(self, db_path: str) -> None:
        super().__init__(db_path)
        self._create_observations_table()
        
    def _create_observations_table(self) -> None:
        self._execute_query("""
            CREATE TABLE IF NOT EXISTS observations (
                username TEXT NOT NULL,
                room_id INTEGER,
                event_id INTEGER,
                observation_type TEXT NOT NULL,
                FOREIGN KEY(username) REFERENCES users(username) ON DELETE CASCADE,
                FOREIGN KEY(room_id) REFERENCES rooms(id) ON DELETE CASCADE,
                FOREIGN KEY(event_id) REFERENCES events(id) ON DELETE CASCADE
            );
        """)
    
    def create_observation(self, username: int, room_id: int, event_id: int, observation_type: str) -> None:
        """
        Args:
            username (str): The name of the user making the observation.
            room_id (int): The ID of the room being observed.
            event_id (int): The ID of the event being observed.
            observation_type (str): The type of observation being made. Must be one of "ROOM", "EVENT"
            
        Returns:
            bool: True if the observation was successfully created, False otherwise.
        """
        try:
            self._execute_query("""
                INSERT INTO observations (username, room_id, event_id, observation_type)
                VALUES (?, ?, ?, ?);
            """, (username, room_id, event_id, observation_type))
        except sqlite3.IntegrityError:
            return False
        return True
    
    def delete_observation(self, username: str, room_id: int, event_id: int) -> None:
        # If room_id and event_id are None, delete all observations for the user
        if room_id is None and event_id is None:
            self._execute_query("""DELETE FROM observations WHERE username = ?;""", (username,))
        # If room_id is None, delete all observations for the user and event
        elif room_id is None:
            self._execute_query("""DELETE FROM observations WHERE username = ? AND event_id = ?;""", (username, event_id))
        # If event_id is None, delete all observations for the user and room
        elif event_id is None:
            self._execute_query("""DELETE FROM observations WHERE username = ? AND room_id = ?;""", (username, room_id))
    
    def get_observations_for_user(self, username: int) -> list:
        result = self._execute_query_with_result("""SELECT * FROM observations WHERE username = ?;""", (username,))
        return [{"username": row[0], "room_id": row[1], "event_id": row[2], "observation_type": row[3]} for row in result] if result else None
    
    def get_users_for_room(self, room_id: int) -> list:
        result = self._execute_query_with_result("""SELECT username FROM observations WHERE room_id = ? AND observation_type = "ROOM";""", (room_id,))
        return [row[0] for row in result] if result else None
        
    
    def get_users_for_event(self, event_id: int) -> list:
        result = self._execute_query_with_result("""SELECT username FROM observations WHERE event_id = ? AND observation_type = "EVENT";""", (event_id,))
        return [row[0] for row in result] if result else None