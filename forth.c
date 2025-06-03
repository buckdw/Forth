#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STACK_SIZE 1024
#define MAX_WORD_LENGTH 32
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


/*
 *  Operators
 */

void op_add(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a + b);
}

void op_sub(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a - b);
}

void op_mul(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a * b);
}

void op_div(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    if (b == 0) {
        printf("Division by zero!\n");
        exit(1);
    }
    push(s, a / b);
}

/*
 *  Builtin keywords
 */

void op_dup(Stack *s) {
    push(s, peek(s));
}

void op_drop(Stack *s) {
    pop(s);
}

void op_swap(Stack *s) {
    int a = pop(s);
    int b = pop(s);
    push(s, a);
    push(s, b);
}

void op_print(Stack *s) {
    printf("%d\n", pop(s));
}

/*
 *  Helper functions
 */
void to_lowercase(char *str) {
    for (; *str; ++str) *str = tolower(*str);
}

int is_number(const char *token) {
    if (*token == '-' || *token == '+') token++;
    if (!*token) return 0;
    while (*token) {
        if (!isdigit(*token)) return 0;
        token++;
    }
    return 1;
}

/*
 *  lexical analysis
 */
void interpret(Stack *stack, char *line) {
    char *token = strtok(line, " \t\r\n");
    while (token != NULL) {
        to_lowercase(token);

        if (is_number(token)) {
            push(stack, atoi(token));
        } else if (strcmp(token, "+") == 0) {
            op_add(stack);
        } else if (strcmp(token, "-") == 0) {
            op_sub(stack);
        } else if (strcmp(token, "*") == 0) {
            op_mul(stack);
        } else if (strcmp(token, "/") == 0) {
            op_div(stack);
        } else if (strcmp(token, "dup") == 0) {
            op_dup(stack);
        } else if (strcmp(token, "drop") == 0) {
            op_drop(stack);
        } else if (strcmp(token, "swap") == 0) {
            op_swap(stack);
        } else if (strcmp(token, ".") == 0) {
            op_print(stack);
        } else {
            printf("Unknown word: %s\n", token);
        }

        token = strtok(NULL, " \t\r\n");
    }
}

int main() {
    Stack stack = { .top = 0 };
    char line[LINE_SIZE];

    printf("Diederick's Forth Interpreter (C) - 2025\n"
    printf("Type 'exit' to quit.\n");

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
