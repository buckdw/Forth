# Makefile for Simple Forth Interpreter
CC = gcc
CFLAGS = -Wall -Wextra -O2
OBJ = Forth.o Stack.o
TARGET = Forth

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
