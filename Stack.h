#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STACK_SIZE 1024

typedef struct {
    int data[STACK_SIZE];
    int top;
} Stack;

void push(Stack *s, int value);
int pop(Stack *s);
int peek(Stack *s);

void init_stack(Stack *s);
bool stack_has(Stack *s, int n);

#endif
