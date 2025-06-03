#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STACK_SIZE 4096
#define MAX_WORD_LENGTH 32
#define LINE_SIZE 256
#define MEMORY_SIZE 4096

int memory[MEMORY_SIZE];

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

typedef void (*Operation)(Stack *);

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

/* DUP */
void op_dup(Stack *s) {
    push(s, peek(s));
}

/* DROP */
void op_drop(Stack *s) {
    pop(s);
}

/* SWAP */
void op_swap(Stack *s) {
    int a = pop(s);
    int b = pop(s);
    push(s, a);
    push(s, b);
}

/* ROT */
void op_rot(Stack *s) {
    if (s->top < 3) {
        printf("Stack underflow for ROT!\n");
        exit(1);
    }
    int c = pop(s);     // Top
    int b = pop(s);     // Second
    int a = pop(s);     // Third
    push(s, b);         // Now top is B
    push(s, c);         // Now top is C
    push(s, a);         // A goes on top
}

/* ! -> store to memory address */
void op_store(Stack *s) {
    int value = pop(s);
    int addr = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        printf("Memory access out of bounds at !\n");
        exit(1);
    }
    memory[addr] = value;
    printf("Storing %d at memory[%d]\n", value, addr);
}

/* @ -> fetch from memory address */
void op_fetch(Stack *s) {
    int addr = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        printf("Memory access out of bounds at @\n");
        exit(1);
    }
    push(s, memory[addr]);
    printf("Fetching memory[%d] = %d\n", addr, memory[addr]);
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
 *  Support structures and lookup tables 
 */
typedef struct {
    const char *word;
    Operation op;
} DictEntry;


DictEntry dictionary[] = {
    {"+", op_add},
    {"-", op_sub},
    {"*", op_mul},
    {"/", op_div},
    {"dup", op_dup},
    {"drop", op_drop},
    {"swap", op_swap},
    {"rot", op_rot},
    {"@", op_fetch},
    {"!", op_store},
    {".", op_print},
    {NULL, NULL}
};

Operation find_word(const char *word) {
    for (int i = 0; dictionary[i].word != NULL; i++) {
        if (strcmp(dictionary[i].word, word) == 0) {
            return dictionary[i].op;
        }
    }
    return NULL;
}

/*
 *  Lexical analysis
 */
void interpret(Stack *stack, char *line) {
    char *token = strtok(line, " \t\r\n");
    while (token != NULL) {
        to_lowercase(token);

        Operation op = find_word(token);
        if (op) {
            op(stack);
        } else if (is_number(token)) {
            push(stack, atoi(token));
        } else {
            printf("Unknown word: %s\n", token);
        }

        token = strtok(NULL, " \t\r\n");
    }
}

/*
 *  Main
 */
int main() {
    Stack stack = { .top = 0 };
    char line[LINE_SIZE];

    printf("Diederick's Forth Interpreter (C) - 2025\nType 'exit' to quit.\n");

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
