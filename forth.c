#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "forth.h"

#define STACK_SIZE 16384
#define MAX_WORD_LENGTH 32
#define LINE_SIZE 256
#define MEMORY_SIZE 16384

int memory[MEMORY_SIZE];

typedef enum {
    OP_0,   // zero parameters (stack only)
    OP_1,   // one parameter (stack)
    OP_2,   // two parameters (stack + return stack)
    OP_3    // hypothetical: stack + rs + extra param (not used now)
} OpType;

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

typedef void (*OpFunc0)(Stack *s);
typedef void (*OpFunc1)(Stack *s, Stack *rs);
typedef void (*OpFunc2)(Stack *s, Stack *rs, int param); 

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
void op_to_r(Stack *s, Stack *rs) {
    if (s->top == 0) {
        printf("Stack underflow for >R!\n");
        exit(1);
    }
    int value = pop(s);
    push(rs, value);
}

/* R> -> R FROM */
void op_r_from(Stack *s, Stack *rs) {
    if (rs->top == 0) {
        printf("Return stack underflow for R>!\n");
        exit(1);
    }
    int value = pop(rs);
    push(s, value);
}

/* R@ -> R FETCH */
void op_r_fetch(Stack *s, Stack *rs) {
    if (rs->top == 0) {
        printf("Return stack empty for R@!\n");
        exit(1);
    }
    int value = rs->data[rs->top - 1];
    push(s, value);
}

/* NOT */
void op_not(Stack *s) {
    int a = pop(s);
    push(s, ~a);
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

/* ZERO */
void op_zero_equal(Stack *s) {
    int a = pop(s);
    push(s, (a == 0) ? -1 : 0);
}

/* NEG */
void op_zero_less(Stack *s) {
    int a = pop(s);
    push(s, (a < 0) ? -1 : 0);
}

/* POS */
void op_zero_greater(Stack *s) {
    int a = pop(s);
    push(s, (a > 0) ? -1 : 0);
}

/* EMIT */
void op_emit(Stack *s) {
    int value = pop(s);
    if (value < 0 || value > 255) {
        printf("Invalid EMIT value: %d\n", value);
        exit(1);
    }
    putchar(value);
    fflush(stdout); 
}

/* SPACE */
void op_space(Stack *s) {
    putchar(' ');
    fflush(stdout);
}

/* CR */
void op_cr(Stack *s) {
    putchar('\n');
    fflush(stdout);
}

/* EXIT -- pseudo command */
void op_exit(Stack *s) {
    exit(1);
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

bool is_number(const char *token) {
    if (*token == '-' || *token == '+') 
        token++;
    if (!*token) 
        return false;
    while (*token) {
        if (!isdigit(*token)) 
            return false;
        token++;
    }
    return true;
}

/*
 *  Support structures and function lookup table
 */
typedef struct {
    const char *word;
    OpType type;
    union {
        OpFunc0 f0;
        OpFunc1 f1;
        OpFunc2 f2;
    } func;
} DictEntry;

DictEntry dictionary[] = {
    {  PLUS, OP_0, {.f0 = op_add            } },
    {   MIN, OP_0, {.f0 = op_sub            } },
    {   MUL, OP_0, {.f0 = op_mul            } },
    {   DIV, OP_0, {.f0 = op_div            } },
    {   DUP, OP_0, {.f0 = op_dup            } },
    {  DROP, OP_0, {.f0 = op_drop           } },
    {  SWAP, OP_0, {.f0 = op_swap           } },
    {   ROT, OP_0, {.f0 = op_rot            } },
    { FETCH, OP_0, {.f0 = op_fetch          } },
    { STORE, OP_0, {.f0 = op_store          } },
    {  OVER, OP_0, {.f0 = op_over           } },
    {  PICK, OP_0, {.f0 = op_pick           } },
    { DEPTH, OP_0, {.f0 = op_depth          } },
    {  ROLL, OP_0, {.f0 = op_roll           } },
    {   TOR, OP_1, {.f1 = op_to_r           } },
    { RFROM, OP_1, {.f1 = op_r_from         } },
    {RFETCH, OP_1, {.f1 = op_r_fetch        } },
    {   NOT, OP_0, {.f0 = op_not            } },
    {    LT, OP_0, {.f0 = op_less_than      } }, 
    {    EQ, OP_0, {.f0 = op_equal          } }, 
    {    GT, OP_0, {.f0 = op_greater_than   } }, 
    {  ZERO, OP_0, {.f0 = op_zero_equal     } },
    {   NEG, OP_0, {.f0 = op_zero_less      } },
    {   POS, OP_0, {.f0 = op_zero_greater   } },
    {  EMIT, OP_0, {.f0 = op_emit           } },
    { SPACE, OP_0, {.f0 = op_space          } },
    {    CR, OP_0, {.f0 = op_cr             } },
    {  EXIT, OP_0, {.f0 = op_exit           } },
    { PRINT, OP_0, {.f0 = op_print          } },
    {  NULL, OP_0, {NULL                    } }
};

DictEntry *find_entry(const char *word) {
    for (int i = 0; dictionary[i].word != NULL; i++) {
        if (strcmp(dictionary[i].word, word) == 0) {
            return &dictionary[i];
        }
    }
    return NULL;
}

void interpret(Stack *stack, Stack *return_stack, char *line) {
    char *token = strtok(line, " \t\r\n");
    while (token != NULL) {
        to_uppercase(token);

        DictEntry *entry = find_entry(token);
        if (entry) {
            switch (entry->type) {
                case OP_0:
                    if (entry->func.f0) 
                        entry->func.f0(stack);
                    break;
                case OP_1:
                    if (entry->func.f1) 
                        entry->func.f1(stack, return_stack);
                    break;
                case OP_2:
                    // future use: entry->func.f2(stack, return_stack, param);
                    break;
                default:
                    printf("Unknown op type\n");
                    exit(1);
            }
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
    Stack return_stack = { .top = 0};

    char line[LINE_SIZE];

    printf("Diederick's Forth Interpreter (C) - 2025\nType 'exit' to quit.\n");

    while (true) {
        printf("> ");
        if (!fgets(line, LINE_SIZE, stdin)) 
            break;
        interpret(&stack, &return_stack, line);

        printf("\nStack: ");
        for (int i = 0; i < stack.top; i++) {
            printf("%d ", stack.data[i]);
        }
        printf("\n");
    }

    return 0;
}
