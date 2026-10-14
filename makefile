# Variables
CC ?= cc
CFLAGS ?= -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -g
TARGET = p3
OBJS = p3.o list.o listNodes.o dir_proc_list.o shell.o open.o command.o command2.o command3.o

ifeq ($(shell uname -s),Linux)
LDLIBS += -ldl
endif

# Reglas
.PHONY: all clean

# Regla por defecto para compilar todo
all: $(TARGET)

# Regla para generar el ejecutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

# Generar dependencias
-include $(OBJS:.o=.d)

# Regla para compilar archivos con dependencias
%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Regla para limpiar los archivos generados
clean:
	rm -f $(OBJS) $(TARGET) $(OBJS:.o=.d)
