CC := gcc
CXX := g++

BUILD_DIR := build

GLAD_SRC := thirdparty/glad/src/glad.c
GLAD_OBJ := $(BUILD_DIR)/glad.o

INCLUDES := -Ithirdparty/glad/include

GLFW_CFLAGS := $(shell pkg-config --cflags glfw3 2>/dev/null)
GLFW_LIBS := $(shell pkg-config --libs glfw3 2>/dev/null || echo -lglfw)

CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic $(INCLUDES) $(GLFW_CFLAGS)
CFLAGS := -Wall -Wextra $(INCLUDES)
CFLAGS_GLAD := $(CFLAGS) -Wno-pedantic
LDFLAGS := $(GLFW_LIBS) -ldl

.PHONY: all clean

# Crear carpeta build si no existe
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compilar GLAD una sola vez
$(GLAD_OBJ): $(GLAD_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS_GLAD) -c $(GLAD_SRC) -o $(GLAD_OBJ)

# Regla principal:
# make nombre → compila nombre.cpp → genera binario nombre
%: %.cpp $(GLAD_OBJ)
	$(CXX) $(CXXFLAGS) $< $(GLAD_OBJ) -o $@ $(LDFLAGS)
	@echo "✔ Compilado: $@"

clean:
	rm -rf $(BUILD_DIR) *.o