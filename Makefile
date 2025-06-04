# Makefile for Simple Forth Interpreter
CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -O2
TARGET = forth
SRC = forth.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
