#include "forth.h"


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

/*
 *  Memory
 */

/* @ -> fetch from memory address [M.01] */
void op_fetch(Stack *s, int *m) {
    int addr = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds at @\n");
        exit(EXIT_FAILURE);
    }
    push(s, m[addr]);
}

/* ! -> store to memory address [M.02] */
void op_store(Stack *s, int *m) {
    int addr = pop(s);
    int value = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds at !\n");
        exit(EXIT_FAILURE);
    }
    m[addr] = value;
}

/* C@ -> CSTORE -> store a byte [M.03] */
void op_cstore(Stack *s, uint8_t *m) {
    int addr = pop(s);
    int value = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds in C!\n");
        exit(EXIT_FAILURE);
    }
    m[addr] = (uint8_t)(value & 0xFF);
}

/* C! -> CFETCH -> fetch a byte [M.04] */
void op_cfetch(Stack *s, uint8_t *m) {
    int addr = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds in C@\n");
        exit(EXIT_FAILURE);
    }
    uint8_t byte = ((uint8_t *)m)[addr];
    push(s, byte);
}

/* ? [M.05] */
void op_question(Stack *s, int *m) {
    int addr = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds at !\n");
        exit(EXIT_FAILURE);
    }
    int value = *((int *)(m + addr));
    fprintf(stdout, "%d\n", value);
}

/* MOVE [M.07] */
void op_move(Stack *s, uint8_t *m) {
    int u = pop(s);       // number of bytes
    int dest = pop(s);    // destination addr
    int src = pop(s);     // source addr

    if (u <= 0) 
        return;

    if (src < dest && src + u > dest) {
        // Overlapping, copy backwards
        for (int i = u - 1; i >= 0; i--) {
            m[dest + i] = m[src + i];
            fprintf(stdout, "m[%d] = m[%d] -> m[%d] = %d\n", dest+i, src+i, dest+i, m[dest + i]);
        }
    } 
    else {
        // No overlap or safe to copy forwards
        for (int i = 0; i < u; i++) {
            m[dest + i] = m[src + i];
            fprintf(stdout, "m[%d] = m[%d] -> m[%d] = %d\n", dest+i, src+i, dest+i, m[dest + i]);
        }
    }
}

/* CMOVE [M.08] */
void op_cmove(Stack *s, uint8_t *m) {
    int count = pop(s);
    int src = pop(s);
    int dest = pop(s);
    if (count < 0 || src < 0 || dest < 0) {
        fprintf(stderr, "CMOVE error: Negative address or count\n");
        exit(EXIT_FAILURE);
    }
    uint32_t ucount = (uint32_t)count;
    uint32_t usrc = (uint32_t)src;
    uint32_t udest = (uint32_t)dest;
    uint32_t mem_size_bytes = MEMORY_SIZE * sizeof(int);
    if (usrc + ucount > mem_size_bytes || udest + ucount > mem_size_bytes) {
        fprintf(stderr, "CMOVE error: Memory access out of bounds\n");
        exit(EXIT_FAILURE);
    }
    memmove(m + udest, m + usrc, ucount);
}

/* FILL [M.09] */
void op_fill(Stack *s, uint8_t *m) {
    int value = pop(s);    // value to fill
    int count = pop(s);    // number of bytes to fill
    int addr = pop(s);     // destination address
    if (addr < 0 || count < 0) {
        fprintf(stderr, "FILL error: Negative address or count\n");
        exit(EXIT_FAILURE);
    }
    size_t uaddr = (size_t)addr;
    size_t ucount = (size_t)count;
    if (uaddr + ucount > MEMORY_SIZE * sizeof(int)) {
        fprintf(stderr, "FILL error: Memory access out of bounds\n");
        exit(EXIT_FAILURE);
    }
    memset(m + uaddr, value, ucount);
}


/* 
 *  STACK 
 */

/* DUP [S.01] */
void op_dup(Stack *s) {
    push(s, peek(s));
}

/* DROP [S.02] */
void op_drop(Stack *s) {
    pop(s);
}

/* SWAP [S.03] */
void op_swap(Stack *s) {
    int a = pop(s);
    int b = pop(s);
    push(s, a);
    push(s, b);
}

/* OVER [S.04] */
void op_over(Stack *s) {
    if (s->top < 2) {
        fprintf(stderr, "Stack underflow for OVER!\n");
        exit(EXIT_FAILURE);
    }
    int x = s->data[s->top - 2];
    push(s, x);
}

/* ROT [S.05] */
void op_rot(Stack *s) {
    if (s->top < 3) {
        fprintf(stderr, "Stack underflow for ROT!\n");
        exit(EXIT_FAILURE);
    }
    int c = pop(s);    
    int b = pop(s);     
    int a = pop(s);     
    push(s, b);         
    push(s, c);         
    push(s, a);        
}

/* PICK [S.06] */
void op_pick(Stack *s) {
    if (s->top < 1) {
        fprintf(stderr, "Stack underflow for PICK!\n");
        exit(EXIT_FAILURE);
    }
    int n = pop(s);  // the index
    if (n < 0 || n >= s->top) {
        fprintf(stderr, "Invalid PICK index: %d\n", n);
        exit(EXIT_FAILURE);
    }
    int value = s->data[s->top - 1 - n];
    push(s, value);
}

/* ROLL [S.07] */
void op_roll(Stack *s) {
    if (s->top < 1) {
        fprintf(stderr, "Stack underflow for ROLL!\n");
        exit(EXIT_FAILURE);
    }
    int n = pop(s);  // depth to roll
    if (n < 0 || n >= s->top) {
        fprintf(stderr, "Invalid ROLL index: %d\n", n);
        exit(EXIT_FAILURE);
    }
    int index = s->top - 1 - n;
    int value = s->data[index];
    for (int i = index; i < s->top - 1; i++) {
        s->data[i] = s->data[i + 1];
    }
    s->data[s->top - 1] = value;
}

/* DEPTH [S.09] */
void op_depth(Stack *s) {
    push(s, s->top);
}

/* >R -> TO R [S.10] */
void op_to_r(Stack *s, Stack *rs) {
    if (s->top == 0) {
        fprintf(stderr, "Stack underflow for >R!\n");
        exit(EXIT_FAILURE);
    }
    int value = pop(s);
    push(rs, value);
}

/* R> -> R FROM [S.11] */
void op_r_from(Stack *s, Stack *rs) {
    if (rs->top == 0) {
        fprintf(stderr, "Return stack underflow for R>!\n");
        exit(EXIT_FAILURE);
    }
    int value = pop(rs);
    push(s, value);
}

/* R@ -> R FETCH [S.12] */
void op_r_fetch(Stack *s, Stack *rs) {
    if (rs->top == 0) {
        fprintf(stderr, "Return stack empty for R@!\n");
        exit(EXIT_FAILURE);
    }
    int value = rs->data[rs->top - 1];
    push(s, value);
}


/* 
 *  COMPARE
 */

/* LT -> LESS THAN [C.01] */
void op_less_than(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, (a < b) ? -1 : 0);
}

/* EQ -> EQUAL [C.02] */
void op_equal(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, (a == b) ? -1 : 0);
}

/* GT -> GREATER THAN [C.03] */
void op_greater_than(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, (a > b) ? -1 : 0);
}

/* NEG [C.04] */
void op_zero_less(Stack *s) {
    int a = pop(s);
    push(s, (a < 0) ? -1 : 0);
}

/* ZERO [C.05] */
void op_zero_equal(Stack *s) {
    int a = pop(s);
    push(s, (a == 0) ? -1 : 0);
}

/* POS [C.06] */
void op_zero_greater(Stack *s) {
    int a = pop(s);
    push(s, (a > 0) ? -1 : 0);
}

/* NOT [C.09] */
void op_not(Stack *s) {
    int a = pop(s);
    push(s, ~a);
}


/*
 *  LOGICAL
 */

 /* ADD [L.01] */
void op_add(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a + b);
}

/* SUB [L.02] */
void op_sub(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a - b);
}

/* MUL [L.03] */
void op_mul(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a * b);
}

/* DIV [L.04] */
void op_div(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    if (b == 0) {
        fprintf(stderr, "Division by zero!\n");
        exit(EXIT_FAILURE);
    }
    push(s, a / b);
}

/* MOD [L.05] */
void op_mod(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    if (b == 0) {
        fprintf(stderr, "Modulo by zero!\n");
        exit(EXIT_FAILURE);
    }
    push(s, a % b);
}

/* DIVMOD [L.06] */
void op_divmod(Stack *s) {
    int divisor = pop(s);
    int dividend = pop(s);

    if (divisor == 0) {
        fprintf(stderr, "/MOD error: Division by zero\n");
        exit(EXIT_FAILURE);
    }

    int quotient = dividend / divisor;
    int remainder = dividend % divisor;

    push(s, remainder);
    push(s, quotient);
}


/* 1+ [L.07] */
void op_one_plus(Stack *s) {
    push(s, pop(s) + 1);
}

/* 1- [L.08] */
void op_one_minus(Stack *s) {
    push(s, pop(s) - 1);
}

/* 2+ [L.09] */
void op_two_plus(Stack *s) {
    push(s, pop(s) + 2);
}

/* 2- [L.10] */
void op_two_minus(Stack *s) {
    push(s, pop(s) - 2);
}

/* D+ [L.11] */
void op_d_plus(Stack *s) {
    int d2_high = pop(s);
    int d2_low  = pop(s);
    int d1_high = pop(s);
    int d1_low  = pop(s);

    unsigned int low_sum = (unsigned int)d1_low + (unsigned int)d2_low;
    int carry = (low_sum < (unsigned int)d1_low); // overflow detection

    int high_sum = d1_high + d2_high + carry;

    push(s, low_sum);
    push(s, high_sum);
}

/* MAX [L.16] */
void op_max(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, (a > b) ? a : b);
}

/* MIN [L.17] */
void op_min(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, (a < b) ? a : b);
}

/* ABS [L.18] */
void op_abs(Stack *s) {
    int a = pop(s);
    push(s, (a < 0) ? -a : a);
}

/* NEGATE [L.19] */
void op_negate(Stack *s) {
    int a = pop(s);
    push(s, -a);
}

/* DNEGATE [L.20] */
void op_dnegate(Stack *s) {
    if (s->top < 2) {
        fprintf(stderr, "Stack underflow for DNEGATE\n");
        return;
    }

    int high = pop(s);
    int low  = pop(s);

    int64_t value = ((int64_t)(uint32_t)high << 32) | (uint32_t)low;
    value = -value;

    push(s, (int)(value >> 32));       // high part
    push(s, (int)(value & 0xFFFFFFFF)); // low part
}

/* AND [L.21] */
void op_and(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a & b);
}

/* OR [L.22] */
void op_or(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a | b);
}

/* XOR [L.23] */
void op_xor(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a ^ b);
}


/*
 *  INPUT/OUTPUT - CHARACTERS 
 */

/* CR [IOC.01] */
void op_cr() {
    putchar('\n');
    fflush(stdout);
}

/* EMIT [IOC.02] */
void op_emit(Stack *s) {
    int value = pop(s);
    if (value < 0 || value > 255) {
        fprintf(stderr, "Invalid EMIT value: %d\n", value);
        exit(EXIT_FAILURE);
    }
    putchar(value);
    fflush(stdout); 
}

/* SPACE [IOC.03] */
void op_space() {
    putchar(' ');
    fflush(stdout);
}

/* SPACES [IOC.04] */
void op_spaces(Stack *s) {
    int count = pop(s);
    if (count < 0) {
        fprintf(stderr, "Invalid SPACES count: %d\n", count);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < count; i++) {
        putchar(' ');
    }
    fflush(stdout);
}

/* TYPE [IOC.06] */
void op_type(Stack *s, int *m) {
    int len = pop(s);
    int addr = pop(s);
    if (addr < 0 || addr + len > MEMORY_SIZE) {
        fprintf(stderr, "Invalid memory range in TYPE\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < len; i++) {
        int val = m[addr + i];
        if (val < 0 || val > 255) {
            fprintf(stderr, "Invalid character code in TYPE: %d\n", val);
            exit(EXIT_FAILURE);
        }
        putchar(val);
    }
    fflush(stdout);
}

/* COUNT [IOC.07] */
void op_count(Stack *s, int *m) {
    int addr = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        fprintf(stderr, "Invalid address in COUNT\n");
        exit(EXIT_FAILURE);
    }
    int len = m[addr];
    if (addr + 1 >= MEMORY_SIZE) {
        fprintf(stderr, "COUNT results in out-of-bounds address\n");
        exit(EXIT_FAILURE);
    }
    push(s, addr + 1);  // Address of first char
    push(s, len);       // Length
}


/*
 *  INPUT/OUTPUT - NUMBERS 
 */

/* . -> print and remove [ION.03] */
void op_print(Stack *s) {
    fprintf(stdout, "%d\n", pop(s));
}

/* EXIT -- pseudo command */
void op_exit() {
    exit(EXIT_SUCCESS);
}

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

DictEntry dictionary[] = {
/* COMPUTATION */
/* [C.01] */     {       LT, OP_0, {.fp_s       = op_less_than      } }, 
/* [C.02] */     {       EQ, OP_0, {.fp_s       = op_equal          } }, 
/* [C.03] */     {       GT, OP_0, {.fp_s       = op_greater_than   } }, 
/* [C.04] */     {      NEG, OP_0, {.fp_s       = op_zero_less      } },
/* [C.05] */     {     ZERO, OP_0, {.fp_s       = op_zero_equal     } },
/* [C.06] */     {      POS, OP_0, {.fp_s       = op_zero_greater   } },
/* [C.09] */     {      NOT, OP_0, {.fp_s       = op_not            } },
/* [IOC.01] */   {       CR, OP,   {.fp         = op_cr             } },

/* IO-CHARACTERS */
/* [IOC.02] */   {     EMIT, OP_0, {.fp_s       = op_emit           } },
/* [IOC.03] */   {    SPACE, OP,   {.fp         = op_space          } },
/* [IOC.04] */   {   SPACES, OP_0, {.fp_s       = op_spaces         } },
/* [IOC.06] */   {     TYPE, OP_2, {.fp_s_m     = op_type           } },
/* [IOC.07] */   {    COUNT, OP_2, {.fp_s_m     = op_count          } },

/* LOGICAL */
/* [L.01] */     {      ADD, OP_0, {.fp_s       = op_add            } },
/* [L.02] */     {      SUB, OP_0, {.fp_s       = op_sub            } },
/* [L.03] */     {      MUL, OP_0, {.fp_s       = op_mul            } },
/* [L.04] */     {      DIV, OP_0, {.fp_s       = op_div            } },
/* [L.05] */     {      MOD, OP_0, {.fp_s       = op_mod            } },
/* [L.06] */     {   DIVMOD, OP_0, {.fp_s       = op_divmod         } },
/* [L.07] */     { ONE_PLUS, OP_0, {.fp_s       = op_one_plus       } },
/* [L.08] */     {  ONE_MIN, OP_0, {.fp_s       = op_one_minus      } },
/* [L.09] */     { TWO_PLUS, OP_0, {.fp_s       = op_two_plus       } },
/* [L.10] */     {  TWO_MIN, OP_0, {.fp_s       = op_two_minus      } },
/* [L.11] */     {    DPLUS, OP_0, {.fp_s       = op_d_plus         } },
/* [L.16] */     {      MAX, OP_0, {.fp_s       = op_max            } },
/* [L.17] */     {      MIN, OP_0, {.fp_s       = op_min            } },
/* [L.18] */     {      ABS, OP_0, {.fp_s       = op_abs            } },
/* [L.19] */     {   NEGATE, OP_0, {.fp_s       = op_negate         } },
/* [L.20] */     {  DNEGATE, OP_0, {.fp_s       = op_dnegate        } },
/* [L.21] */     {      AND, OP_0, {.fp_s       = op_and            } },
/* [L.22] */     {       OR, OP_0, {.fp_s       = op_or             } },
/* [L.23] */     {      XOR, OP_0, {.fp_s       = op_xor            } },

/* MEMORY */
/* [M.01] */     {    FETCH, OP_2, {.fp_s_m     = op_fetch          } },
/* [M.02] */     {    STORE, OP_2, {.fp_s_m     = op_store          } },
/* [M.03] */     {   CFETCH, OP_3, {.fp_s_bm    = op_cfetch         } },
/* [M.04] */     {   CSTORE, OP_3, {.fp_s_bm    = op_cstore         } },
/* [M.05] */     { QUESTION, OP_2, {.fp_s_m     = op_question       } },
/* [M.07] */     {     MOVE, OP_3, {.fp_s_bm    = op_move           } },
/* [M.08] */     {    CMOVE, OP_3, {.fp_s_bm    = op_cmove          } },
/* [M.09] */     {     FILL, OP_3, {.fp_s_bm    = op_fill           } },

/* STACK */
/* [S.01] */     {      DUP, OP_0, {.fp_s       = op_dup            } },
/* [S.02] */     {     DROP, OP_0, {.fp_s       = op_drop           } },
/* [S.03] */     {     SWAP, OP_0, {.fp_s       = op_swap           } },
/* [S.04] */     {     OVER, OP_0, {.fp_s       = op_over           } },
/* [S.05] */     {      ROT, OP_0, {.fp_s       = op_rot            } },
/* [S.06] */     {     PICK, OP_0, {.fp_s       = op_pick           } },
/* [S.07] */     {     ROLL, OP_0, {.fp_s       = op_roll           } },
/* [S.09] */     {    DEPTH, OP_0, {.fp_s       = op_depth          } },
/* [S.10] */     {      TOR, OP_1, {.fp_s_rs    = op_to_r           } },
/* [S.11] */     {    RFROM, OP_1, {.fp_s_rs    = op_r_from         } },
/* [S.12] */     {   RFETCH, OP_1, {.fp_s_rs    = op_r_fetch        } },

/* IO-NUMBERS */
/* [ION.03] */   {    PRINT, OP_0, {.fp_s       = op_print          } },

/* PSEUDO */
/* PSEUDO */     {     EXIT, OP,   {.fp         = op_exit           } },
/* SENTINEL */   {     NULL, OP_0, {NULL                            } }
};   
 
DictEntry *find_entry(const char *word) {
    for (int i = 0; dictionary[i].word != NULL; i++) {
        if (strcmp(dictionary[i].word, word) == 0) {
            return &dictionary[i];
        }
    }
    return NULL;
}

void interpret(Stack *stack, Stack *return_stack, int *memory, char *line) {
    char *token = strtok(line, " \t\r\n");
    while (token != NULL) {
        to_uppercase(token);

        DictEntry *entry = find_entry(token);
        if (entry) {
            switch (entry->type) {
                case OP:
                    if (entry->func.fp) entry->func.fp(); 
                    break;
                case OP_0:
                    if (entry->func.fp_s) entry->func.fp_s(stack); 
                    break;
                case OP_1:
                    if (entry->func.fp_s_rs) entry->func.fp_s_rs(stack, return_stack);
                    break;
                case OP_2:
                    if (entry->func.fp_s_m) entry->func.fp_s_m(stack, memory);                    
                    break;
                case OP_3:
                     if (entry->func.fp_s_bm) entry->func.fp_s_bm(stack, (uint8_t *)memory);                    
                    break;               
                default:
                    fprintf(stdout, "Unknown op type\n");
                    exit(EXIT_FAILURE);
                    break;
            }
        } else if (is_number(token)) {
            push(stack, atoi(token));
        } else {
            fprintf(stdout, "Unknown word: %s\n", token);
        }

        token = strtok(NULL, " \t\r\n");
    }
}

void init_stack(Stack *s) {
    s->top = 0;
}

/*
 *  Main
 */
int main() {
    Stack stack;
    Stack return_stack;
    int memory[MEMORY_SIZE];
    char line[LINE_SIZE + 1];

    init_stack(&stack);
    init_stack(&return_stack);

    fprintf(stdout, "YAFI - 32-bit Forth79 Interpreter (C) - 2025.\n");
    fprintf(stdout, "YAFI - Yet Another Forth Interpreter.\n\n");
    fprintf(stdout, "Type 'exit' to quit.\n");

    while (true) {
        fprintf(stdout, "> ");
        if (!fgets(line, LINE_SIZE, stdin)) 
            break;
        interpret(&stack, &return_stack, memory, line);

        fprintf(stdout, "\nStack: ");
        for (int i = 0; i < stack.top; i++) {
            fprintf(stdout, "%d ", stack.data[i]);
        }
        fprintf(stdout, "\n");
    }

    return EXIT_SUCCESS;
}
