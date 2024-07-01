# Makefile for eshell project

# Compiler settings - Can choose gcc for C program
CC = gcc
CXX = g++

# Compiler flags
CFLAGS = -Wall -g
CXXFLAGS = -Wall -g

# Linker flags
LDFLAGS =

# Source files
SOURCES_C = parser.c
SOURCES_CPP = eshell.cpp
OBJECTS_C = $(SOURCES_C:.c=.o)
OBJECTS_CPP = $(SOURCES_CPP:.cpp=.o)

# Executable name
EXECUTABLE = eshell

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS_C) $(OBJECTS_CPP)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS_C) $(OBJECTS_CPP) $(EXECUTABLE)
