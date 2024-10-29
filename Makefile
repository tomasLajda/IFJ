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
	rm -f $(EXEC) $(OBJS)

# Build specific file for production
build: $(FILE:.c=.o)
	$(CC) $(CFLAGS) -o $(EXEC) $(FILE:.c=.o) $(OBJS)

# Build specific file for testing
build_test: CFLAGS += -DTEST_MODE
build_test: $(FILE:.c=.o)
	$(CC) $(CFLAGS) -o $(EXEC) $(FILE:.c=.o) $(OBJS)

.PHONY: all clean build build_test

# make - make all executables
# make clean - remove all executables and object files
# make build FILE=yourfile.c - build a specific file for production (change yourfile.c to the file you want to build)
# make build_test FILE=yourfile.c - build a specific file for testing (change yourfile.c to the file you want to build)