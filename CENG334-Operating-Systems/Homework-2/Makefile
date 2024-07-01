# Define compiler
CXX := clang++
CXXFLAGS := -std=c++11 -lpthread

# Define target
TARGET := main

# Define source files
SOURCES := main.cpp WriteOutput.c helper.c

# Default target
all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

# Clean target for cleaning up build artifacts
clean:
	rm -f $(TARGET)

.PHONY: all clean
