#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "forth.h"

#define STACK_SIZE 16384
#define MAX_WORD_LENGTH 32
#define LINE_SIZE 256
#define MEMORY_SIZE 16384

int memory[MEMORY_SIZE];

typedef struct {
    int data[STACK_SIZE];
    int top;
} Stack;

Stack return_stack = { .top = 0 };

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
    int addr = pop(s);
    int value = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        printf("Memory access out of bounds at !\n");
        exit(1);
    }
    memory[addr] = value;
}

/* @ -> fetch from memory address */
void op_fetch(Stack *s) {
    int addr = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        printf("Memory access out of bounds at @\n");
        exit(1);
    }
    push(s, memory[addr]);
}

/* OVER */
void op_over(Stack *s) {
    if (s->top < 2) {
        printf("Stack underflow for OVER!\n");
        exit(1);
    }
    int x = s->data[s->top - 2];
    push(s, x);
}

/* PICK */
void op_pick(Stack *s) {
    if (s->top < 1) {
        printf("Stack underflow for PICK!\n");
        exit(1);
    }
    int n = pop(s);  // the index
    if (n < 0 || n >= s->top) {
        printf("Invalid PICK index: %d\n", n);
        exit(1);
    }
    int value = s->data[s->top - 1 - n];
    push(s, value);
}

/* DEPTH */
void op_depth(Stack *s) {
    push(s, s->top);
}

/* ROLL */
void op_roll(Stack *s) {
    if (s->top < 1) {
        printf("Stack underflow for ROLL!\n");
        exit(1);
    }
    int n = pop(s);  // depth to roll
    if (n < 0 || n >= s->top) {
        printf("Invalid ROLL index: %d\n", n);
        exit(1);
    }
    int index = s->top - 1 - n;
    int value = s->data[index];
    for (int i = index; i < s->top - 1; i++) {
        s->data[i] = s->data[i + 1];
    }
    s->data[s->top - 1] = value;
}

/* >R -> TO R */
void op_to_r(Stack *s) {
    if (s->top == 0) {
        printf("Stack underflow for >R!\n");
        exit(1);
    }
    int value = pop(s);
    push(&return_stack, value);
}

/* R> -> R FROM */
void op_r_from(Stack *s) {
    if (return_stack.top == 0) {
        printf("Return stack underflow for R>!\n");
        exit(1);
    }
    int value = pop(&return_stack);
    push(s, value);
}

/* R@ -> R FETCH */
void op_r_fetch(Stack *s) {
    if (return_stack.top == 0) {
        printf("Return stack empty for R@!\n");
        exit(1);
    }
    int value = return_stack.data[return_stack.top - 1];
    push(s, value);
}

/* LT -> LESS THAN */
void op_less_than(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, (a < b) ? -1 : 0);
}

/* EQ -> EQUAL */
void op_equal(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, (a == b) ? -1 : 0);
}

/* GT -> GREATER THAN */
void op_greater_than(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, (a > b) ? -1 : 0);
}


/* NOT */
void op_not(Stack *s) {
    int a = pop(s);
    push(s, ~a);
}

/* . -> print and remove */
void op_print(Stack *s) {
    printf("%d\n", pop(s));
}

/*
 *  Helper functions
 */
 void to_uppercase(char *str) {
    for (; *str; ++str) 
        *str = toupper(*str);
}

int is_number(const char *token) {
    if (*token == '-' || *token == '+') 
        token++;
    if (!*token) 
        return 0;
    while (*token) {
        if (!isdigit(*token)) 
            return 0;
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
    {  PLUS, op_add         },
    {   MIN, op_sub         },
    {   MUL, op_mul         },
    {   DIV, op_div         },
    {   DUP, op_dup         },
    {  DROP, op_drop        },
    {  SWAP, op_swap        },
    {   ROT, op_rot         },
    { FETCH, op_fetch       },
    { STORE, op_store       },
    {  OVER, op_over        },
    {  PICK, op_pick        },
    { DEPTH, op_depth       },
    {  ROLL, op_roll        },
    {   TOR, op_to_r        },
    { RFROM, op_r_from      },
    {RFETCH, op_r_fetch     },
    {   NOT, op_not         },
    {    LT, op_less_than   }, 
    {    EQ, op_equal       }, 
    {    GT, op_greater_than}, 
    { PRINT, op_print       },
    {  NULL, NULL           }    
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
 *  Tokenisation
 */
void interpret(Stack *stack, char *line) {
    char *token = strtok(line, " \t\r\n");
    while (token != NULL) {
        to_uppercase(token);

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
