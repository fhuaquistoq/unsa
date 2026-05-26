CXX := g++

TARGET := result
CPP_SOURCES := main.cpp
OBJECTS := $(CPP_SOURCES:.cpp=.o)

CXXFLAGS := -std=c++17 -Wall -Wextra -O2
INCLUDES := -I. -Iinclude $(shell pkg-config --cflags glut 2>/dev/null)

LIBS := $(shell pkg-config --libs glut 2>/dev/null) -lGL -lGLU -ldl -lpthread

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all run clean
