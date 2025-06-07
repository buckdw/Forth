#ifndef FORTH_H_
#define FORTH_H_

/*
 * Project:         Diederick's Forth-79 Interpreter
 * Description:     Forth interpreter main logic
 * Author:          Diederick
 * Created:         2025-06-04
 * License:         MIT
 * Remarks:         beware that a memory cell is 32-bits. 
 *                  The memory model is based on a array of int
 *                  Beware, some memory operations are byte oriented (verbs with C)
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
    OP_2,   // f(Stack *s, int *m)
    OP_3    // f(Stack *s, uint8_t *m)
} OpType;

typedef void (*OpFunc)();
typedef void (*OpFunc_S)(Stack *s);
typedef void (*OpFunc_S_RS)(Stack *s, Stack *rs);
typedef void (*OpFunc_S_M)(Stack *s, int *m); 
typedef void (*OpFunc_S_BM)(Stack *s, uint8_t *m);

typedef struct {
    const char *word;
    OpType type;
    union {
        OpFunc fp;
        OpFunc_S fp_s;
        OpFunc_S_RS fp_s_rs;
        OpFunc_S_M fp_s_m;
        OpFunc_S_BM fp_s_bm;
    } func;
} DictEntry;

#define ABS         "ABS"
#define ADD         "+"
#define AND         "AND"
#define CFETCH      "C@"
#define CMOVE       "CMOVE"
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
#define FILL        "FILL"
#define GT          ">"
#define LT          "<"
#define MAX         "MAX"
#define MIN         "MIN"
#define MOD         "MOD"
#define MOVE        "MOVE"
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
#define QUESTION    "?"
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
/* [C.01] */ void op_less_than(Stack *s);
/* [C.02] */ void op_equal(Stack *s);
/* [C.03] */ void op_greater_than(Stack *s);
/* [C.04] */ void op_zero_less(Stack *s);
/* [C.05] */ void op_zero_equal(Stack *s);
/* [C.06] */ void op_zero_greater(Stack *s);
/* [C.09] */ void op_not(Stack *s);
/* [IOC.01] */ void op_cr();
/* [IOC.02] */ void op_emit(Stack *s);
/* [IOC.03] */ void op_space();
/* [IOC.04] */ void op_spaces(Stack *s);
/* [IOC.06] */ void op_type(Stack *s, int *m);
/* [IOC.07] */void op_count(Stack *s, int *m);
/* [ION.03] */ void op_print(Stack *s);
/* [L.01] */ void op_add(Stack *s);
/* [L.02] */ void op_sub(Stack *s);
/* [L.03] */ void op_mul(Stack *s);
/* [L.04] */ void op_div(Stack *s);
/* [L.05] */ void op_mod(Stack *s);
/* [L.07] */ void op_one_plus(Stack *s);
/* [L.08] */ void op_one_minus(Stack *s);
/* [L.09] */ void op_two_plus(Stack *s);
/* [L.10] */ void op_two_minus(Stack *s);
/* [L.11] */ void op_d_plus(Stack *s);
/* [L.16] */ void op_max(Stack *s);
/* [L.17] */void op_min(Stack *s);
/* [L.18] */ void op_abs(Stack *s);
/* [L.19] */ void op_negate(Stack *s);
/* [L.20] */ void op_dnegate(Stack *s);
/* [L.21] */ void op_and(Stack *s);
/* [L.22] */ void op_or(Stack *s);
/* [L.23] */ void op_xor(Stack *s);
/* [M.01] */ void op_fetch(Stack *s, int *m);
/* [M.02] */ void op_store(Stack *s, int *m);
/* [M.03] */ void op_cfetch(Stack *s, uint8_t *m);
/* [M.04] */ void op_cstore(Stack *s, uint8_t *m);
/* [M.05] */ void op_question(Stack *s, int *m);
/* [M.07] */ void op_move(Stack *s, uint8_t *m);
/* [M.08] */ void op_cmove(Stack *s, uint8_t *m);
/* [M.09] */ void op_fill(Stack *s, uint8_t *m);
/* [S.01] */ void op_dup(Stack *s);
/* [S.02] */ void op_drop(Stack *s);
/* [S.03] */ void op_swap(Stack *s);
/* [S.04] */ void op_over(Stack *s);
/* [S.05] */ void op_rot(Stack *s);
/* [S.06] */ void op_pick(Stack *s);
/* [S.07] */ void op_roll(Stack *s);
/* [S.09] */ void op_depth(Stack *s);
/* [S.10] */ void op_to_r(Stack *s, Stack *rs);
/* [S.11] */ void op_r_from(Stack *s, Stack *rs);
/* [S.12] */ void op_r_fetch(Stack *s, Stack *rs);

/* pseudo */
void op_exit();

/* helpers */
void to_uppercase(char *str);
bool is_number(const char *token);
void init_stack(Stack *s);

#endif

