CC = gcc

# Opciones de compilación para desarrollo
CFLAGS_DEV = -Wall -Wextra -g

# Opciones de compilación para producción
CFLAGS_PROD = -O3 -flto -DNDEBUG -march=native

HEADERS = 
SRCS = main.c

# Archivos objeto generados
OBJS = $(SRCS:.c=.o)

# Nombre del ejecutable
EXEC = mochiladb


# Reglas de compilación
all: $(EXEC) clean_objs

# Regla para compilar el ejecutable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Regla para compilar archivos .c a archivos .o
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos objeto
clean_objs:
	rm -f $(OBJS)

# Limpiar archivos generados
clean:
	rm -f $(OBJS) $(EXEC)

# Recompilar todo desde cero
rebuild: clean all

# Tareas para compilación de desarrollo y producción
dev: CFLAGS = $(CFLAGS_DEV)
dev: rebuild

prod: CFLAGS = $(CFLAGS_PROD)
prod: rebuild
	strip $(EXEC)

.PHONY: all clean clean_objs rebuild dev prod
