# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Source files (dynamically find all .c files)
SRCS = $(wildcard *.c)

# Executable files
EXECS = $(SRCS:.c=)

# Default target
all: $(EXECS)

# Compile source files into executables
%: %.c
    $(CC) $(CFLAGS) -o $@ $<

# Clean up build files
clean:
    rm -f $(EXECS)

# Build specific file for production
build: $(FILE)
    $(CC) $(CFLAGS) -o $(FILE:.c=) $(FILE)

# Build specific file for testing
build_test: CFLAGS += -DTEST_MODE
build_test: $(FILE)
    $(CC) $(CFLAGS) -o $(FILE:.c=) $(FILE)

.PHONY: all clean build build_test

# make - make all executables
# make clean - remove all executables
# make build FILE=yourfile.c - build a specific file for production (change yourfile.c to the file you want to build)
# make build_test FILE=yourfile.c - build a specific file for testing (change yourfile.c to the file you want to build)

