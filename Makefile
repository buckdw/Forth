# Makefile for Simple Forth Interpreter
CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = forth
SRC = forth.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o obj/$(TARGET) $(SRC)

clean:
	rm -f obj/$(TARGET)
