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
        OpFunc  fp;
        OpFunc0 fp_s;
        OpFunc1 fp_s_rs;
        OpFunc2 fp_s_m;
    } func;
} DictEntry;

#define ABS         "ABS"
#define ADD         "+"
#define AND         "AND"
#define CFETCH      "C@"
#define COUNT       "COUNT"
#define CR          "CR"
#define CSTORE      "C!"
#define DEPTH       "DEPTH"
#define DIV         "/"
#define DNEGATE     "DNEGATE"
#define DPLUS       "D+"
#define DROP        "DROP"
#define DUP         "DUP"
#define EMIT        "EMIT"
#define EQ          "="
#define EXIT        "EXIT"
#define FETCH       "@"
#define GT          ">"
#define LT          "<"
#define MAX         "MAX"
#define MIN         "MIN"
#define MOD         "MOD"
#define MUL         "*"
#define NEG         "0<"
#define NEGATE      "NEGATE"
#define NOT         "NOT"
#define ONE_MIN     "1-"
#define ONE_PLUS    "1+"
#define OR          "OR"
#define OVER        "OVER"
#define PICK        "PICK"
#define POS         "0>"
#define PRINT        "."
#define RFETCH      "R@"
#define RFROM       "R>"
#define ROLL        "ROLL"
#define ROT         "ROT"
#define SPACE       "SPACE"
#define SPACES      "SPACES"
#define STORE       "!"
#define SUB         "-"
#define SWAP        "SWAP"
#define TOR         ">R"
#define TWO_MIN     "2-"
#define TWO_PLUS    "2+"
#define TYPE        "TYPE"
#define XOR         "XOR"
#define ZERO        "0="


/* internal */
void push(Stack *s, int value);
int pop(Stack *s); 
int peek(Stack *s); 

/* operations */
void op_add(Stack *s);
void op_sub(Stack *s);
void op_mul(Stack *s);
void op_div(Stack *s);
void op_mod(Stack *s);
void op_one_plus(Stack *s);
void op_one_minus(Stack *s);
void op_two_plus(Stack *s);
void op_two_minus(Stack *s);
void op_d_plus(Stack *s);
void op_negate(Stack *s);
void op_dnegate(Stack *s);
void op_abs(Stack *s);
void op_dup(Stack *s);
void op_drop(Stack *s);
void op_swap(Stack *s);
void op_rot(Stack *s);
void op_store(Stack *s, int *m);
void op_fetch(Stack *s, int *m);
void op_cstore(Stack *s, int *m);
void op_cfetch(Stack *s, int *m);
void op_over(Stack *s);
void op_pick(Stack *s);
void op_depth(Stack *s);
void op_roll(Stack *s);
void op_to_r(Stack *s, Stack *rs);
void op_r_from(Stack *s, Stack *rs);
void op_r_fetch(Stack *s, Stack *rs);
void op_not(Stack *s);
void op_less_than(Stack *s);
void op_equal(Stack *s);
void op_greater_than(Stack *s);
void op_zero_equal(Stack *s);
void op_zero_less(Stack *s);
void op_zero_greater(Stack *s);
void op_and(Stack *s);
void op_or(Stack *s);
void op_xor(Stack *s);
void op_min(Stack *s);
void op_max(Stack *s);
void op_emit(Stack *s);
void op_space();
void op_cr();
void op_spaces(Stack *s);
void op_count(Stack *s, int *m);
void op_type(Stack *s, int *m);
void op_print(Stack *s);

/* pseudo operation */
void op_exit();

/* helpers */
void to_uppercase(char *str);
bool is_number(const char *token);
void init_stack(Stack *s);

#endif

