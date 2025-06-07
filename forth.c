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

/* ADD */
void op_add(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a + b);
}

/* SUB */
void op_sub(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a - b);
}

/* MUL */
void op_mul(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a * b);
}

/* DIV */
void op_div(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    if (b == 0) {
        fprintf(stderr, "Division by zero!\n");
        exit(EXIT_FAILURE);
    }
    push(s, a / b);
}

/* MOD */
void op_mod(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    if (b == 0) {
        fprintf(stderr, "Modulo by zero!\n");
        exit(EXIT_FAILURE);
    }
    push(s, a % b);
}

/* 1+ */
void op_one_plus(Stack *s) {
    push(s, pop(s) + 1);
}

/* 1- */
void op_one_minus(Stack *s) {
    push(s, pop(s) - 1);
}

/* 2+ */
void op_two_plus(Stack *s) {
    push(s, pop(s) + 2);
}

/* 2- */
void op_two_minus(Stack *s) {
    push(s, pop(s) - 2);
}

/* D+ */
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

/* NEGATE */
void op_negate(Stack *s) {
    int a = pop(s);
    push(s, -a);
}

/* DNEGATE */
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


/* ABS */
void op_abs(Stack *s) {
    int a = pop(s);
    push(s, (a < 0) ? -a : a);
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

/* ! -> store to memory address */
void op_store(Stack *s, int *m) {
    int addr = pop(s);
    int value = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds at !\n");
        exit(EXIT_FAILURE);
    }
    m[addr] = value;
}

/* @ -> fetch from memory address */
void op_fetch(Stack *s, int *m) {
    int addr = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds at @\n");
        exit(EXIT_FAILURE);
    }
    push(s, m[addr]);
}

/* CSTORE -> store a byte */
void op_cstore(Stack *s, uint8_t *m) {
    int addr = pop(s);
    int value = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds in C!\n");
        exit(EXIT_FAILURE);
    }
    m[addr] = (uint8_t)(value & 0xFF);
}

/* CFETCH -> fetch a byte */
void op_cfetch(Stack *s, uint8_t *m) {
    int addr = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds in C@\n");
        exit(EXIT_FAILURE);
    }
    uint8_t byte = ((uint8_t *)m)[addr];
    push(s, byte);
}

/* OVER */
void op_over(Stack *s) {
    if (s->top < 2) {
        fprintf(stderr, "Stack underflow for OVER!\n");
        exit(EXIT_FAILURE);
    }
    int x = s->data[s->top - 2];
    push(s, x);
}

/* PICK */
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

/* DEPTH */
void op_depth(Stack *s) {
    push(s, s->top);
}

/* ROLL */
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

/* >R -> TO R */
void op_to_r(Stack *s, Stack *rs) {
    if (s->top == 0) {
        fprintf(stderr, "Stack underflow for >R!\n");
        exit(EXIT_FAILURE);
    }
    int value = pop(s);
    push(rs, value);
}

/* R> -> R FROM */
void op_r_from(Stack *s, Stack *rs) {
    if (rs->top == 0) {
        fprintf(stderr, "Return stack underflow for R>!\n");
        exit(EXIT_FAILURE);
    }
    int value = pop(rs);
    push(s, value);
}

/* R@ -> R FETCH */
void op_r_fetch(Stack *s, Stack *rs) {
    if (rs->top == 0) {
        fprintf(stderr, "Return stack empty for R@!\n");
        exit(EXIT_FAILURE);
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

/* AND */
void op_and(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a & b);
}

/* OR */
void op_or(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a | b);
}

/* XOR */
void op_xor(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, a ^ b);
}

/* MIN */
void op_min(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, (a < b) ? a : b);
}

/* MAX */
void op_max(Stack *s) {
    int b = pop(s);
    int a = pop(s);
    push(s, (a > b) ? a : b);
}

/* EMIT */
void op_emit(Stack *s) {
    int value = pop(s);
    if (value < 0 || value > 255) {
        fprintf(stderr, "Invalid EMIT value: %d\n", value);
        exit(EXIT_FAILURE);
    }
    putchar(value);
    fflush(stdout); 
}

/* SPACE */
void op_space() {
    putchar(' ');
    fflush(stdout);
}

/* CR */
void op_cr() {
    putchar('\n');
    fflush(stdout);
}

/* SPACES */
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

/* COUNT */
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

/* TYPE */
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

/* MOVE */
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

/* ? */
void op_question(Stack *s, int *m) {
    int addr = pop(s);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds at !\n");
        exit(EXIT_FAILURE);
    }
    int value = *((int *)(m + addr));
    fprintf(stdout, "%d\n", value);
}

/* FILL */
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

/* CMOVE */
void op_cmove(Stack *s, uint8_t *m) {
    int n = pop(s);
    int dst = pop(s);
    int src = pop(s);

    if (src < 0 || src + n > MEMORY_SIZE * sizeof(int) ||
        dst < 0 || dst + n > MEMORY_SIZE * sizeof(int)) {
        fprintf(stderr, "CMOVE: memory access out of bounds\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        m[dst + i] = m[src + i];
    }
}

/* . -> print and remove */
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
/* OPERATOR */     {      ADD, OP_0, {.fp_s       = op_add            } },
/* OPERATOR */     {      SUB, OP_0, {.fp_s       = op_sub            } },
/* OPERATOR */     {      MUL, OP_0, {.fp_s       = op_mul            } },
/* OPERATOR */     {      DIV, OP_0, {.fp_s       = op_div            } },
/* OPERATOR */     {      MOD, OP_0, {.fp_s       = op_mod            } },
/* OPERATOR */     { ONE_PLUS, OP_0, {.fp_s       = op_one_plus       } },
/* OPERATOR */     {  ONE_MIN, OP_0, {.fp_s       = op_one_minus      } },
/* OPERATOR */     { TWO_PLUS, OP_0, {.fp_s       = op_two_plus       } },
/* OPERATOR */     {  TWO_MIN, OP_0, {.fp_s       = op_two_minus      } },
/* OPERATOR */     {    DPLUS, OP_0, {.fp_s       = op_d_plus         } },
/* LOGICAL */      {   NEGATE, OP_0, {.fp_s       = op_negate         } },
/* LOGICAL */      {  DNEGATE, OP_0, {.fp_s       = op_dnegate        } },
/* OPERATOR */     {      ABS, OP_0, {.fp_s       = op_abs            } },
/* STACK */        {      DUP, OP_0, {.fp_s       = op_dup            } },
/* STACK */        {     DROP, OP_0, {.fp_s       = op_drop           } },
/* STACK */        {     SWAP, OP_0, {.fp_s       = op_swap           } },
/* STACK */        {      ROT, OP_0, {.fp_s       = op_rot            } },
/* MEMORY */       {    FETCH, OP_2, {.fp_s_m     = op_fetch          } },
/* MEMORY */       {    STORE, OP_2, {.fp_s_m     = op_store          } },
/* MEMORY */       {   CFETCH, OP_3, {.fp_s_bm    = op_cfetch         } },
/* MEMORY */       {   CSTORE, OP_3, {.fp_s_bm    = op_cstore         } },
/* STACK */        {     OVER, OP_0, {.fp_s       = op_over           } },
/* STACK */        {     PICK, OP_0, {.fp_s       = op_pick           } },
/* STACK */        {    DEPTH, OP_0, {.fp_s       = op_depth          } },
/* STACK */        {     ROLL, OP_0, {.fp_s       = op_roll           } },
/* STACK */        {      TOR, OP_1, {.fp_s_rs    = op_to_r           } },
/* STACK */        {    RFROM, OP_1, {.fp_s_rs    = op_r_from         } },
/* STACK */        {   RFETCH, OP_1, {.fp_s_rs    = op_r_fetch        } },
/* COMPARE */      {      NOT, OP_0, {.fp_s       = op_not            } },
/* COMPARE */      {       LT, OP_0, {.fp_s       = op_less_than      } }, 
/* COMPARE */      {       EQ, OP_0, {.fp_s       = op_equal          } }, 
/* COMPARE */      {       GT, OP_0, {.fp_s       = op_greater_than   } }, 
/* COMPARE */      {     ZERO, OP_0, {.fp_s       = op_zero_equal     } },
/* COMPARE */      {      NEG, OP_0, {.fp_s       = op_zero_less      } },
/* COMPARE */      {      POS, OP_0, {.fp_s       = op_zero_greater   } },
/* LOGICAL */      {      AND, OP_0, {.fp_s       = op_and            } },
/* LOGICAL */      {       OR, OP_0, {.fp_s       = op_or             } },
/* LOGICAL */      {      XOR, OP_0, {.fp_s       = op_xor            } },
/* LOGICAL */      {      MIN, OP_0, {.fp_s       = op_min            } },
/* LOGICAL */      {      MAX, OP_0, {.fp_s       = op_max            } },
/* IO */           {     EMIT, OP_0, {.fp_s       = op_emit           } },
/* IO */           {    SPACE, OP,   {.fp         = op_space          } },
/* IO */           {       CR, OP,   {.fp         = op_cr             } },
/* IO */           {   SPACES, OP_0, {.fp_s       = op_spaces         } },
/* IO */           {    COUNT, OP_2, {.fp_s_m     = op_count          } },
/* IO */           {     TYPE, OP_2, {.fp_s_m     = op_type           } },
/* MEMORY */       {     MOVE, OP_3, {.fp_s_bm    = op_move           } },
/* MEMORY */       {    CMOVE, OP_3, {.fp_s_bm    = op_cmove          } },
/* MEMORY */       { QUESTION, OP_2, {.fp_s_m     = op_question       } },
/* MEMORY */       {     FILL, OP_3, {.fp_s_bm    = op_fill           } },
/* IO */           {    PRINT, OP_0, {.fp_s       = op_print          } },
/* PSEUDO */       {     EXIT, OP,   {.fp         = op_exit           } },
/* SENTINEL */     {     NULL, OP_0, {NULL                            } }
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

    fprintf(stdout, "YAFI - 32-bit Forth-79 Interpreter (C) - 2025\n");
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
