CC      = gcc
CFLAGS_BASE = -Wall -Wextra -Wpedantic -std=c11 -Iinclude
SANITIZERS ?= -fsanitize=address,undefined
LSAN_SUPP  = lsan.supp
CFLAGS  = $(CFLAGS_BASE) $(SANITIZERS) `sdl2-config --cflags`
LDFLAGS = $(SANITIZERS)
LDLIBS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

SRC_DIR   = src
BUILD_DIR = build
INC_DIR   = include

SRCS = main.c $(wildcard $(SRC_DIR)/*.c)
HEADERS = $(wildcard $(INC_DIR)/*.h)

OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SRCS)))

TARGET = $(BUILD_DIR)/game

VALGRIND_FLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
VALGRIND = valgrind $(VALGRIND_FLAGS)

VALGRIND_FREE_FLAGS := valgrind --leak-check=full --show-leak-kinds=definite
VALGRIND_FREE := valgrind $(VALGRIND_FREE_FLAGS)

.PHONY: all clean run leaks test

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(BUILD_DIR)/main.o: main.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

run:
	@$(MAKE) clean
	@$(MAKE) all
	clear && LSAN_OPTIONS=suppressions=$(LSAN_SUPP) $(TARGET) || stty sane; tput cnorm

leaks:
	@$(MAKE) clean
	@$(MAKE) SANITIZERS= LDFLAGS= all
	$(VALGRIND_FREE) $(TARGET)

clean:
	rm -rf $(BUILD_DIR)

# Test individual: make test FILE=config
# Compila y ejecuta un solo archivo .c del directorio src/
test:
ifndef FILE
	@echo "Error: Debes especificar FILE=nombre_archivo"
	@echo "Uso: make test FILE=config"
	@echo ""
	@echo "Archivos disponibles:"
	@ls -1 $(SRC_DIR)/*.c 2>/dev/null | xargs -n1 basename -s .c | sed 's/^/  - /' || echo "  (ninguno)"
	@exit 1
endif
	@mkdir -p $(BUILD_DIR)
	@echo "=== Compilando $(FILE).c ==="
	$(CC) $(CFLAGS) -DTEST_MAIN $(SRC_DIR)/$(FILE).c -o $(BUILD_DIR)/test_$(FILE) $(LDLIBS)
	@echo "=== Ejecutando test_$(FILE) ==="
	@echo ""
	@$(BUILD_DIR)/test_$(FILE) || stty sane