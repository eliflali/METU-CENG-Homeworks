cmake_minimum_required(VERSION 3.26)
project(OS_Homework2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(OS_Homework2
        helper.h
        monitor.h
        WriteOutput.c
        WriteOutput.h
        Ferry.h
        Car.h
        main.cpp
        NarrowBridge.h
        RoadConnector.h
        CrossRoad.h
        helper.c)
