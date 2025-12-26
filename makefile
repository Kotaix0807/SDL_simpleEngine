CC      = gcc
CFLAGS_BASE = -Wall -Wextra -Wpedantic -std=c11 -Iinclude
SANITIZERS ?= -fsanitize=address,undefined
CFLAGS  = $(CFLAGS_BASE) $(SANITIZERS)
LDFLAGS = $(SANITIZERS)
LDLIBS  = -lncurses

DEPS_PATH  = include
SRC_PATH   = src

ROOT_SRC   = main.c
DEPS       = $(wildcard $(DEPS_PATH)/*.h)
SRCS_DEPS  = $(notdir $(wildcard $(SRC_PATH)/*.c))

SRCS_PREFIX = $(ROOT_SRC) $(addprefix $(SRC_PATH)/,$(SRCS_DEPS))
DEPS_PREFIX = $(addprefix $(DEPS_PATH)/,$(DEPS))

VALGRIND_FLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
VALGRIND = valgrind $(VALGRIND_FLAGS)

VALGRIND_FREE_FLAGS := valgrind --leak-check=full --show-leak-kinds=definite
VALGRIND_FREE := valgrind $(VALGRIND_FREE_FLAGS)

OBJS = $(SRCS_PREFIX:.c=.o)

.PHONY: all build run leaks clean

all: build

build: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c $(DEPS_PREFIX)
	$(CC) $(CFLAGS) -c -o $@ $<

run:
	@$(MAKE) clean
	@$(MAKE) build
	clear && ./build || stty sane; tput cnorm

leaks:
	@$(MAKE) clean
	@$(MAKE) SANITIZERS= LDFLAGS= build
	$(VALGRIND_FREE) ./build

clean:
	rm -f build $(OBJS)
