# Define the compiler to use
CXX = g++

# Compiler flags:
#  -O3    Optimization level 3, for production quality performance
#  -std=c++11 Use the C++11 standard
CXXFLAGS = -O3 -std=c++11

# The build target executable:
TARGET = rasterizer

all: $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) *.cpp -o $(TARGET)

clean:
	$(RM) $(TARGET)
