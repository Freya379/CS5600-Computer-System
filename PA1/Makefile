# Makefile for the Unix Shell

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g

# Executable name
EXEC = shell

# Source file
SRC = shellnew.c

# Build target
all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)

clean:
	rm -f $(EXEC)

.PHONY: all clean