"""
URL configuration for room_reservation project.

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/4.2/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.contrib.auth import views as auth_views

from django.urls import path
from . import views

from django.conf import settings
from django.conf.urls.static import static

urlpatterns = [
    path('admin/', admin.site.urls),
    path('', views.login_view, name='login'),
    path('login/', views.login_view, name='login'),
    path('command-center/', views.command_center, name='command_center'),
     path('logout/', auth_views.LogoutView.as_view(next_page='login'), name='logout'),
    path('execute-login/', views.execute_login, name='execute_login'),
    path('register/', views.register_view, name='register'),
    path('find-schedule/', views.find_schedule, name='find-schedule'),
    path('room/<str:organization_name>/<str:room_name>/', views.room_detail, name='room_detail'),
    path('organizations-json/', views.organizations_json, name='organizations_json'),
    path('rooms-json/<str:org_name>/<str:token>/', views.rooms_json, name='rooms-json'),
    path('events-json/<str:org_name>/<str:token>/', views.events_json, name='events-json'),
    path('room-view/', views.room_view, name='room-view'),
    path('room-view-center/', views.room_view_center, name='room-view-center'),
]  + static(settings.STATIC_URL, document_root=settings.STATIC_ROOT)
