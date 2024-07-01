# Create your models here.
"""
This file contains the models for the room_reservation_app.

Includes:

    - User: Class to store user information
        
"""
from datetime import datetime, timedelta
from django.db import models
from django.contrib.auth.models import User

class Organization(models.Model):
    """Class Organization to store organization information"""
    
    id = models.AutoField(primary_key=True)
    name = models.CharField(max_length=50, unique=True)
    description = models.CharField(max_length=500)
    owner = models.ForeignKey(User, on_delete=models.CASCADE)
    
    def __str__(self):
        return f"Organization: {self.name}"

class Room(models.Model):
    """Class Room to store room information"""
    
    id = models.AutoField(primary_key=True)
    name = models.CharField(max_length=50, null=False)
    x = models.IntegerField()
    y = models.IntegerField()
    capacity = models.IntegerField()
    working_hours = models.CharField(max_length=50, help_text="Format: HH:MM-HH:MM")
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)
    owner = models.ForeignKey(User, on_delete=models.CASCADE)

    def __str__(self):
        return f"Room: {self.name}"
    

class Event(models.Model):
    """Event Class to store events."""

    id = models.AutoField(primary_key=True)
    title = models.CharField(max_length=100, null=False)
    description = models.CharField(max_length=500)
    category = models.CharField(max_length=50)
    capacity = models.IntegerField()
    duration = models.IntegerField(help_text="Duration in minutes")
    weekly = models.BooleanField(default=False)
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)
    owner = models.ForeignKey(User, on_delete=models.CASCADE)
    
class Reservation(models.Model):
    """Reservation Class to store reservations."""
    
    id = models.AutoField(primary_key=True)
    start_time = models.DateTimeField(null=False)
    room = models.ForeignKey(Room, on_delete=models.CASCADE)
    event = models.ForeignKey(Event, on_delete=models.CASCADE)
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)
    weekly = models.BooleanField(default=False)
    duration = models.IntegerField(help_text="Duration in minutes")
    
    def get_end_time(self) -> datetime:
        """Returns the end time of the reservation."""
        return datetime.strptime(self.start_time + timedelta(minutes=self.duration), "%Y-%m-%d %H:%M")
    
    @staticmethod
    def is_room_available(room, start_time: datetime, duration: int) -> bool:
        """Return true if the room is available at the given time.

        Args:
            room (Room): Room object
            start_time (str): Start time in the format %Y-%m-%d %H:%M
            duration (int): Duration in minutes

        Returns:
            bool : True if the room is available at the given time.
        """
        
        # Get the end time of the reservation
        requested_start = datetime.strptime(start_time, "%Y-%m-%d %H:%M")
        requested_end = requested_start + timedelta(minutes=duration)
        
        # First check if the room is working at the given time
        # Parse the working hours and combine them with the requested date
        working_hours = room.working_hours.split("-")
        working_start_time = requested_start.strftime("%Y-%m-%d") + " " + working_hours[0]
        working_end_time = requested_start.strftime("%Y-%m-%d") + " " + working_hours[1]
        working_start = datetime.strptime(working_start_time, "%Y-%m-%d %H:%M")
        working_end = datetime.strptime(working_end_time, "%Y-%m-%d %H:%M")
        
        # Check if the requested time is within the working hours
        if not (working_start <= requested_start < working_end):
            return False

        
        # Get all the reservations for the room
        reservations = Reservation.objects.filter(room=room)
        
        if not reservations:
            return True
        
        # Check if there are any reservations that overlap with the requested time
        for reservation in reservations:
            reservation_end = reservation.get_end_time()

            # Check for overlapping reservations
            if (requested_start < reservation_end) and (requested_end > reservation.start_time):
                return False

        return True
    