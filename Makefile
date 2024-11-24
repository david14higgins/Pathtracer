# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Idependencies

# Directories
SRC_DIR = src
CORE_DIR = Core
GEOM_DIR = Geometry
LIGHT_DIR = Lighting
MAT_DIR = Materials
IO_DIR = IO

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp $(CORE_DIR)/*.cpp $(GEOM_DIR)/*.cpp \
               $(LIGHT_DIR)/*.cpp $(MAT_DIR)/*.cpp $(IO_DIR)/*.cpp)
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = raytracer

# Build rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files, executable, and renders
clean:
	-del /Q /F src\main.o Core\*.o Geometry\*.o Lighting\*.o Materials\*.o IO\*.o raytracer.exe 2>NUL
	-rd /S /Q renders 2>NUL

.PHONY: all clean