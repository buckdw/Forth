#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STACK_SIZE 4096
#define WORD_SIZE 32
#define LINE_SIZE 256

typedef struct {
    int data[STACK_SIZE];
    int top;
} Stack;

void push(Stack *s, int value) {
    if (s->top >= STACK_SIZE) {
        printf("Stack overflow!\n");
        exit(1);
    }
    s->data[s->top++] = value;
}

int pop(Stack *s) {
    if (s->top == 0) {
        printf("Stack underflow!\n");
        exit(1);
    }
    return s->data[--s->top];
}

int peek(Stack *s) {
    if (s->top == 0) {
        printf("Stack empty!\n");
        exit(1);
    }
    return s->data[s->top - 1];
}

void to_lowercase(char *str) {
    for (; *str; ++str) 
        *str = tolower(*str);
}

int is_number(const char *token) {
    if (*token == '-' || *token == '+') 
        token++;
    while (*token) {
        if (!isdigit(*token)) 
            return 0;
        token++;
    }
    return 1;
}

void interpret(Stack *stack, char *line) {
    char *token = strtok(line, " \t\r\n");
    while (token != NULL) {
        to_lowercase(token);

        if (is_number(token)) {
            push(stack, atoi(token));
        } else if (strcmp(token, "+") == 0) {
            int b = pop(stack);
            int a = pop(stack);
            push(stack, a + b);
        } else if (strcmp(token, "-") == 0) {
            int b = pop(stack);
            int a = pop(stack);
            push(stack, a - b);
        } else if (strcmp(token, "*") == 0) {
            int b = pop(stack);
            int a = pop(stack);
            push(stack, a * b);
        } else if (strcmp(token, "/") == 0) {
            int b = pop(stack);
            int a = pop(stack);
            if (b == 0) {
                printf("Division by zero!\n");
                exit(1);
            }
            push(stack, a / b);
        } else if (strcmp(token, "dup") == 0) {
            push(stack, peek(stack));
        } else if (strcmp(token, "drop") == 0) {
            pop(stack);
        } else if (strcmp(token, "swap") == 0) {
            int a = pop(stack);
            int b = pop(stack);
            push(stack, a);
            push(stack, b);
        } else if (strcmp(token, ".") == 0) {
            printf("%d\n", pop(stack));
        } else {
            printf("Unknown word: %s\n", token);
        }

        token = strtok(NULL, " \t\r\n");
    }
}

int main() {
    Stack stack = { .top = 0 };
    char line[LINE_SIZE + 1];

    printf("Diederick's simple Forth Interpreter (C)\nType 'exit' to quit.\n");

    while (1) {
        printf("> ");
        if (!fgets(line, LINE_SIZE, stdin)) 
            break;
        if (strncmp(line, "exit", 4) == 0) 
            break;
        interpret(&stack, line);

        printf("Stack: ");
        for (int i = 0; i < stack.top; i++) {
            printf("%d ", stack.data[i]);
        }
        printf("\n");
    }

    return 0;
}
