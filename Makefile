CC = gcc
# Opciones de compilación para desarrollo
CFLAGS_DEV = -Wall -Wextra -g

# Opciones de compilación para producción
CFLAGS_PROD = -O3 -flto -DNDEBUG -march=native

LIBS = 
SRC_DIR = .
BUILD_DIR = build
BIN_DIR = bin

# All source files (including subdirectorios)
SRCS := $(shell find $(SRC_DIR) -name "*.c")
#$(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/*.c)

# All generated objects
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# name of program (Sysbansi for SImple BANking SYStem in reverse)
TARGET = mochiladb

all: $(BIN_DIR)/$(TARGET) clean_objs

# Rule for build executable program
# $@ es el target (target: dependencias),
# $^ es una lista  de todas las dependencias separada con espacio, n este caso OBJS
$(BIN_DIR)/$(TARGET): $(OBJS)
	@echo "Building..."
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# Rule for build each object
# $< es la primera dependencia (como si iterara de una en una) sacada de SRC_DIR/%.c
# $@ es el objetivo que va a ir de BUILD_DIR/%.o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos objeto
clean_objs:
	@echo "Cleaning build folder"
	rm -rf $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Recompilar todo desde cero
rebuild: clean all


# Tareas para compilación de desarrollo y producción
dev: CFLAGS = $(CFLAGS_DEV)
dev: rebuild

prod: CFLAGS = $(CFLAGS_PROD)
prod: rebuild
	strip $(EXEC)

.PHONY: all clean clean_objs rebuild dev prod
