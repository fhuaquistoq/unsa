# ─── Paint OpenGL — Makefile de conveniencia ────────────────────
# No es para compilar directamente, es un wrapper alrededor de CMake.
#
# Uso:
#   make setup   — configuración inicial (genera build/)
#   make build   — compila el proyecto
#   make run     — compila y ejecuta
#   make clean   — borra el directorio de build
#   make glad    — muestra instrucciones para generar GLAD
# ────────────────────────────────────────────────────────────────

BUILD_DIR := build
TARGET    := paint

.PHONY: all setup build run clean glad

all: build

# Configuración inicial de CMake
setup:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Compilar (auto-setup si no existe build/)
build:
	@test -d $(BUILD_DIR) || $(MAKE) setup
	cmake --build $(BUILD_DIR)

# Compilar y ejecutar
run: build
	./$(BUILD_DIR)/$(TARGET)

# Limpiar
clean:
	rm -rf $(BUILD_DIR)

# Instrucciones para GLAD
glad:
	@echo ""
	@echo "╔══════════════════════════════════════════════════════════╗"
	@echo "║            GENERAR GLAD — Pasos                         ║"
	@echo "╠══════════════════════════════════════════════════════════╣"
	@echo "║ 1. Ir a: https://glad.dav1d.de/                        ║"
	@echo "║ 2. Configurar:                                          ║"
	@echo "║    - Language:  C/C++                                    ║"
	@echo "║    - Specification: OpenGL                               ║"
	@echo "║    - API: gl = 4.6 (o la que tu GPU soporte)             ║"
	@echo "║    - Profile: Core                                      ║"
	@echo "║    - Extensions: (dejar por defecto)                    ║"
	@echo "║ 3. Click 'Generate'                                     ║"
	@echo "║ 4. Descargar y descomprimir el ZIP                       ║"
	@echo "║ 5. Copiar:                                              ║"
	@echo "║    - include/glad/glad.h → glad/include/glad/glad.h     ║"
	@echo "║    - include/KHR/     → glad/include/KHR/               ║"
	@echo "║    - src/glad.c       → glad/src/glad.c                 ║"
	@echo "║ 6. Volver a ejecutar: make build                        ║"
	@echo "╚══════════════════════════════════════════════════════════╝"
	@echo ""