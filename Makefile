# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Source files (dynamically find all .c files)
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

EXEC = program

# Default target
all: $(EXEC)

# Compile each .c file into its corresponding object file
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Link all object files into a single executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Clean up build files
clean:
	rm -f $(EXEC) $(OBJS) xcsirim00.zip

pack: clean #todo: pridat rozsireni ak bude treba
	zip -r xcsirim00.zip *.c *.h Makefile rozdeleni dokumentace.pdf

.PHONY: all clean build build_test

