CXX = g++
CXXFLAGS = -std=c++11 -I/usr/include/glm -I/usr/include/GL -I/usr/include/GLFW -I/usr/include/freetype2

# Linker flags for OpenGL, GLEW, and GLFW
LDFLAGS = -L/usr/lib -lGLEW -lGL -lglfw -lfreetype

# List of all .cpp files
SOURCES = main.cpp

# Name of the executable
EXECUTABLE = bunnyRun

# Default target
all: myOpenGLApp

# Compile and build the application
myOpenGLApp: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $(EXECUTABLE)
	./$(EXECUTABLE) 

# Clean up
clean:
	rm -f $(EXECUTABLE) output.txt

