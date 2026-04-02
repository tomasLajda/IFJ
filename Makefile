# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude

# Source files (dynamically find all .c files)
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

EXEC = compiler

# Default target
all: $(EXEC)

# Compile each .c file into its corresponding object file
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Link all object files into a single executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Clean up build files
clean:
	rm -f $(EXEC) $(OBJS) xcsirim00.zip

pack: clean
	zip -j xcsirim00.zip src/*.c include/*.h Makefile rozdeleni dokumentace.pdf

.PHONY: all clean build build_test

