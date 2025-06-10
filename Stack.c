#include "Stack.h"

void push(Stack *s, int value) {
    if (s->top >= STACK_SIZE) {
        fprintf(stderr, "Stack overflow!\n");
        exit(EXIT_FAILURE);
    }
    s->data[s->top++] = value;
}

int pop(Stack *s) {
    if (s->top == 0) {
        fprintf(stderr, "Stack underflow!\n");
        exit(EXIT_FAILURE);
    }
    return s->data[--s->top];
}

int peek(Stack *s) {
    if (s->top == 0) {
        fprintf(stderr, "Stack empty!\n");
        exit(EXIT_FAILURE);
    }
    return s->data[s->top - 1];
}

void init_stack(Stack *s) {
    s->top = 0;
}

bool stack_has_min_depth(Stack *s, int n) {
    return s->top >= n;
}
