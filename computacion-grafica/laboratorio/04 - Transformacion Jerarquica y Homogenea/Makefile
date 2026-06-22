CXX := g++

CPP_SOURCES := $(wildcard *.cpp)
TARGETS := $(CPP_SOURCES:.cpp=)

CXXFLAGS := -std=c++17 -Wall -Wextra -O2
INCLUDES := -I. -Iinclude $(shell pkg-config --cflags glut 2>/dev/null)

LIBS := $(shell pkg-config --libs glut 2>/dev/null) -lGL -lGLU -ldl -lpthread

all: $(TARGETS)

%: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ $(LIBS)

run: $(firstword $(TARGETS))
	./$(firstword $(TARGETS))

run-%: %
	./$<

clean:
	rm -f $(TARGETS) *.o

.PHONY: all run run-% clean
