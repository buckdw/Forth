#ifndef FORTH_H_
#define FORTH_H_

/*
 * Project:      Diederick's Forth Interpreter
 * Description:  Forth interpreter main logic
 * Author:       Diederick
 * Created:      2025-06-04
 * License:      MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define STACK_SIZE 16384
#define LINE_SIZE 256
#define MEMORY_SIZE 16384

typedef struct {
    int data[STACK_SIZE];
    int top;
} Stack;

typedef enum {
    OP,     // f()
    OP_0,   // f(Stack *s)
    OP_1,   // f(Stack *s, Stack *rs)
    OP_2    // f(Stack *s, int *m)
} OpType;

typedef void (*OpFunc)();
typedef void (*OpFunc0)(Stack *s);
typedef void (*OpFunc1)(Stack *s, Stack *rs);
typedef void (*OpFunc2)(Stack *s, int *m); 

typedef struct {
    const char *word;
    OpType type;
    union {
        OpFunc  f;
        OpFunc0 f_s;
        OpFunc1 f_s_rs;
        OpFunc2 f_s_m;
    } func;
} DictEntry;

#define ABS      "ABS"
#define COUNT    "COUNT"
#define CR       "CR"
#define DEPTH    "DEPTH"
#define DIV      "/"
#define DROP     "DROP"
#define DUP      "DUP"
#define EMIT     "EMIT"
#define EQ       "="
#define EXIT     "EXIT"
#define FETCH    "@"
#define GT       ">"
#define LT       "<"
#define MIN      "-"
#define MOD      "MOD"
#define MUL      "*"
#define NEG      "0<"
#define NOT      "NOT"
#define ONE_MIN  "1-"
#define ONE_PLUS "1+"
#define OVER     "OVER"
#define PICK     "PICK"
#define PLUS     "+"
#define POS      "0>"
#define PRINT    "."
#define RFETCH   "R@"
#define RFROM    "R>"
#define ROLL     "ROLL"
#define ROT      "ROT"
#define SPACE    "SPACE"
#define SPACES   "SPACES"
#define STORE    "!"
#define SWAP     "SWAP"
#define TOR      ">R"
#define TWO_MIN  "2-"
#define TWO_PLUS "2+"
#define TYPE     "TYPE"
#define ZERO     "0="

#endif

