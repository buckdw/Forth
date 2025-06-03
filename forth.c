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

void interpret(Stack *stack, const char *line) {
    char word[WORD_SIZE + 1];
    int pos = 0;
    
    while (sscanf(line + pos, "%s", word) == 1) {
        to_lowercase(word);
        pos += strlen(word);
        while (line[pos] == ' ') pos++;  // skip spaces

        if (is_number(word)) {
            push(stack, atoi(word));
        } else if (strcmp(word, "+") == 0) {
            int b = pop(stack);
            int a = pop(stack);
            push(stack, a + b);
        } else if (strcmp(word, "-") == 0) {
            int b = pop(stack);
            int a = pop(stack);
            push(stack, a - b);
        } else if (strcmp(word, "*") == 0) {
            int b = pop(stack);
            int a = pop(stack);
            push(stack, a * b);
        } else if (strcmp(word, "/") == 0) {
            int b = pop(stack);
            int a = pop(stack);
            if (b == 0) {
                printf("Division by zero!\n");
                exit(1);
            }
            push(stack, a / b);
        } else if (strcmp(word, "dup") == 0) {
            push(stack, peek(stack));
        } else if (strcmp(word, "drop") == 0) {
            pop(stack);
        } else if (strcmp(word, "swap") == 0) {
            int a = pop(stack);
            int b = pop(stack);
            push(stack, a);
            push(stack, b);
        } else if (strcmp(word, ".") == 0) {
            printf("%d\n", pop(stack));
        } else {
            printf("Unknown word: %s\n", word);
        }
    }
}

int main() {
    Stack stack = { .top = 0 };
    char line[LINE_SIZE + 1];

    printf("Simple Forth Interpreter (C)\nType 'exit' to quit.\n");

    while (1) {
        printf("> ");
        if (!fgets(line, LINE_SIZE, stdin)) 
            break;
        if (strncmp(line, "exit", 4) == 0) 
            break;
        interpret(&stack, line);

        // Show stack after each line
        printf("Stack: ");
        for (int i = 0; i < stack.top; i++) {
            printf("%d ", stack.data[i]);
        }
        printf("\n");
    }

    return 0;
}
