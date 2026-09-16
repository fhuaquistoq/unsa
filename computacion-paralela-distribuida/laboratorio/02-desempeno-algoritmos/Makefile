# Compila todos los .cpp de src (incluidas las subcarpetas) dentro de bin/ y
# permite perfilar los binarios con Valgrind (Cachegrind) y verlos en KCachegrind.

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -g -Wall -Wextra -pedantic

SRC_DIR := src
BIN_DIR := bin

SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')
SUBDIRS := $(shell find $(SRC_DIR) -mindepth 1 -type d)
NAMES   := $(notdir $(SOURCES:.cpp=))
TARGETS := $(addprefix $(BIN_DIR)/,$(NAMES))

VPATH := $(SRC_DIR) $(SUBDIRS)

BIN  ?= $(strip $(word 2,$(MAKECMDGOALS)))
ARGS ?= $(wordlist 3,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
EXTRA_GOALS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))

# --- Perfilado --------------------------------------------------------------
VALGRIND    := valgrind
KCACHEGRIND := kcachegrind

PROFILE_DIR := profiles
SIZE  ?= 256
BLOCK ?= 64

# --cache-sim=yes es obligatorio: Valgrind 3.24 trae la simulacion desactivada.
CACHEGRIND_FLAGS ?= --cache-sim=yes

PROFILES            := $(addprefix $(PROFILE_DIR)/cachegrind.out.,$(NAMES))
CACHEGRIND_TARGETS  := $(addprefix cachegrind-,$(NAMES))
KCACHEGRIND_TARGETS := $(addprefix kcachegrind-,$(NAMES))

# Nota: cachegrind-<bin> y kcachegrind-<bin> no se declaran .PHONY porque make
# omite la busqueda de reglas de patron para los targets .PHONY.
.PHONY: all list clean run clean-profiles cachegrind kcachegrind $(EXTRA_GOALS)

all: $(TARGETS)

$(BIN_DIR)/%: %.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

list:
	@printf '%s\n' $(NAMES)

clean:
	@rm -rf $(BIN_DIR)

clean-profiles:
	@rm -rf $(PROFILE_DIR)

# --- Valgrind / Cachegrind --------------------------------------------------
# make cachegrind                      perfila todos los binarios
# make cachegrind-multiply-matrix      perfila un binario
# make cachegrind SIZE=512 BLOCK=128   cambia el tamano de las matrices
# make cachegrind CACHEGRIND_FLAGS="--cache-sim=yes --branch-sim=yes"

cachegrind: $(CACHEGRIND_TARGETS)

cachegrind-%: $(BIN_DIR)/%
	@mkdir -p $(PROFILE_DIR)
	$(VALGRIND) --tool=cachegrind $(CACHEGRIND_FLAGS) \
		--cachegrind-out-file=$(PROFILE_DIR)/cachegrind.out.$* \
		./$(BIN_DIR)/$* $(if $(findstring blocked,$*),$(SIZE) $(BLOCK),$(SIZE))

# --- KCachegrind ------------------------------------------------------------
# make kcachegrind                  abre todos los perfiles en una ventana
# make kcachegrind-multiply-matrix  abre el perfil de un binario

kcachegrind:
	@command -v $(KCACHEGRIND) >/dev/null 2>&1 || { \
		echo "$(KCACHEGRIND) no esta instalado o no esta en PATH"; exit 1; }
	@test -n "$(wildcard $(PROFILES))" || { \
		echo "No hay perfiles en $(PROFILE_DIR); ejecuta: make cachegrind"; exit 1; }
	$(KCACHEGRIND) $(wildcard $(PROFILES))

kcachegrind-%:
	@command -v $(KCACHEGRIND) >/dev/null 2>&1 || { \
		echo "$(KCACHEGRIND) no esta instalado o no esta en PATH"; exit 1; }
	@test -f $(PROFILE_DIR)/cachegrind.out.$* || { \
		echo "No existe $(PROFILE_DIR)/cachegrind.out.$*; ejecuta: make cachegrind-$*"; exit 1; }
	$(KCACHEGRIND) $(PROFILE_DIR)/cachegrind.out.$*

ifneq ($(EXTRA_GOALS),)
$(EXTRA_GOALS):
	@:
endif

ifeq ($(BIN),)
run:
	@echo "Uso: make run <binario> [argumentos...]"
	@echo "     make list"
else
run: $(BIN_DIR)/$(BIN)
	@./$(BIN_DIR)/$(BIN) $(ARGS)
endif
