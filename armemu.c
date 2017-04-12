#include <stdbool.h>
#include <stdio.h>

//gcc -o armemu armemu.c sum_a_s.s find_max_s.s fib_iter_s.s fib_rec_s.s find_str_s.s 

#define NREGS 16
#define STACK_SIZE 1024
#define SP 13
#define LR 14
#define PC 15

int total = 0;
int data = 0;
int memory = 0;
int branch = 0;

int sum_a_s(int *array, int n);
int find_max_s(int *array, int n);
int fib_rec_s(int n);
int fib_iter_s(int n);
int find_str_s(char *s, char *sub);
int add(int a, int b);

struct arm_state {
    unsigned int regs[NREGS];
    unsigned int cpsr;
    unsigned char stack[STACK_SIZE];
};

void init_arm_state(struct arm_state *as, unsigned int *func,
                   unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3)
{
    int i;

    /* zero out all arm state */
    for (i = 0; i < NREGS; i++) {
        as->regs[i] = 0;
    }

    as->cpsr = 0;

    for (i = 0; i < STACK_SIZE; i++) {
        as->stack[i] = 0;
    }

    as->regs[PC] = (unsigned int) func;
    as->regs[SP] = (unsigned int) &as->stack[STACK_SIZE];
    as->regs[LR] = 0;

    as->regs[0] = arg0;
    as->regs[1] = arg1;
    as->regs[2] = arg2;
    as->regs[3] = arg3;
}

bool is_add_inst(unsigned int iw)
{
    //printf("inside is add\n");
    unsigned int op;
    unsigned int opcode;

    op = (iw >> 26) & 0b11;
    opcode = (iw >> 21) & 0b1111;

    return (op == 0) && (opcode == 0b0100);
}

void armemu_add(struct arm_state *state)
{
    //printf("RUNNING ADD\n");
    unsigned int iw;
    unsigned int rd, rn, rm, im, imm8;

    iw = *((unsigned int *) state->regs[PC]);

    im = (iw >> 25) & 0b1;

    if (im == 0b1) {
        rd = (iw >> 12) & 0xF;
        rn = (iw >> 16) & 0xF;
        imm8 = iw & 0xFF;

        state->regs[rd] = state->regs[rn] + imm8;

    } else {
    
        rd = (iw >> 12) & 0xF;
        rn = (iw >> 16) & 0xF;
        rm = iw & 0xF;

        state->regs[rd] = state->regs[rn] + state->regs[rm];
    }
    if (rd != PC) {
        state->regs[PC] = state->regs[PC] + 4;
    }
}

bool is_sub_inst(unsigned int iw) {
    //printf("inside is sub\n");
    unsigned int op;
    unsigned int opcode;

    op = (iw >> 26) & 0b11;
    opcode = (iw >> 21) & 0b1111;

    return (op == 0) && (opcode == 0b0010);
}

void armemu_sub(struct arm_state *state) {
    //printf("RUNNING SUB\n");
    unsigned int iw;
    unsigned int rd, rn, rm, im, imm8;

    iw = *((unsigned int *) state->regs[PC]);

    im = (iw >> 25) & 0b1;

    if (im == 0b1) {
        rd = (iw >> 12) & 0xF;
        rn = (iw >> 16) & 0xF;
        imm8 = iw & 0xFF;

        state->regs[rd] = state->regs[rn] - imm8;

    } else {
    
        rd = (iw >> 12) & 0xF;
        rn = (iw >> 16) & 0xF;
        rm = iw & 0xF;

        state->regs[rd] = state->regs[rn] - state->regs[rm];
    }
    if (rd != PC) {
        state->regs[PC] = state->regs[PC] + 4;
    }
}

bool is_mov_inst(unsigned int iw) {
    //printf("inside is mov\n");
    unsigned int op, opcode;

    op = (iw >> 26) & 0b11;
    opcode = (iw >> 21) & 0b1111;

    return (op == 0) && (opcode == 0b1101);
}

void armemu_mov(struct arm_state *state) {
    unsigned iw, rd, im, src2;
    //printf("RUNNING MOV\n");

    iw = *((unsigned int *) state->regs[PC]);
    rd = (iw >> 12) & 0xF;

    im = (iw >> 25) & 0b1;

    if (im == 0b1) {
        rd = (iw >> 12) & 0xF;
        src2 = iw & 0xFF;

        state->regs[rd] = src2;

    } else {
        rd = (iw >> 12) & 0xF;
        src2 = iw & 0xF;
        state->regs[rd] = state->regs[src2];
    }

    if (rd != PC) {
        state->regs[PC] = state->regs[PC] + 4;
    }
}

bool is_cmp_inst(unsigned int iw) {
    //printf("inside is cmp\n");
    unsigned int op;
    unsigned int opcode;

    op = (iw >> 26) & 0b11;
    opcode = (iw >> 21) & 0b1111;

    return (op == 0) && (opcode == 0b1010);
}

void armemu_cmp(struct arm_state *state) {
    //printf("RUNNING CMP\n");
    state->cpsr = 0b0000;
    unsigned iw, src1, im, src2, rd;

    iw = *((unsigned int *) state->regs[PC]);
    rd = (iw >> 12) & 0xF;

    im = (iw >> 25) & 0b1;

    //printf("iw = %u\n",iw);

    if (im == 0b1) {
        src1 = (int) state->regs[(iw >> 16) & 0b1111];
        src2 = iw & 0xFF;
        if (src1 == src2) {
            state->cpsr = 0b0000;
        } else if (src1 < src2) {
            state->cpsr = 0b0001;
        } else if (src1 > src2) {
            state->cpsr = 0b0011;
        }
    } else {
        src1 = (int) state->regs[(iw >> 16) & 0b1111];
        src2 = (int) state->regs[iw & 0xF];
        //printf("src1 = %d\n", (short) src1);
        //printf("src2 = %d\n", (short) src2);
        if (src1 == src2) {
            state->cpsr = 0b0000;
        } else if (src1 < src2) {
            state->cpsr = 0b0001;
        } else if (src1 > src2) {
            state->cpsr = 0b0011;
        }
    }

    //printf("CPRR = %d\n", (short) state->cpsr);

    if (rd != PC) {
        state->regs[PC] = state->regs[PC] + 4;
    }
}

bool is_equal(struct arm_state *state) {
    if (state->cpsr == 0b0000) {
        return true;
    } else {
        return false;
    }
}

bool is_less_than(struct arm_state *state) {
    if (state->cpsr == 0b0001) {
        return true;
    } else {
        return false;
    }
}

bool is_greater_than(struct arm_state *state) {
    if (state->cpsr == 0b0011) {
        //printf("in greater than\n");
        return true;
    } else {
        return false;
    }
}

unsigned int get_cond(unsigned int iw) {
    return (iw >> 28) & 0xF;
}

bool check_cond(struct arm_state *state) {
    unsigned int iw, condition, rd;

    iw = *((unsigned int *) state->regs[PC]);

    condition = get_cond(iw);
    //printf("Condition = %u\n", condition);

    if (condition == 0b0000) {
        //printf("EQUALS\n");
        //equal
        return is_equal(state);
    } else if (condition == 0b0001) {
        //printf("NOT EQUALS\n");
        return !is_equal(state);
    } else if (condition == 0b1100) {
        //printf("Greater than\n");
        //greater than
        return is_greater_than(state);
    } else if (condition == 0b1101) {
        //printf("Less than or equal\n");
        //less than or equal
        return is_equal(state) | is_less_than(state);
    } else if (condition == 0b1011) {
        //less than
        //printf("less than\n");
        return is_less_than(state);
    }

    return true;
}

bool is_bx_inst(unsigned int iw)
{
    //printf("inside is bx\n");
    unsigned int bx_code;

    bx_code = (iw >> 4) & 0x00FFFFFF;

    return (bx_code == 0b000100101111111111110001);
}

void armemu_bx(struct arm_state *state)
{
    //printf("running bx\n");
    unsigned int iw;
    unsigned int rn;

    iw = *((unsigned int *) state->regs[PC]);
    rn = iw & 0b1111;

    state->regs[PC] = state->regs[rn];
}

 bool is_ldr(unsigned int iw) {
    //printf("inside is ldr\n");

    unsigned int opcode;
    unsigned int L;
    unsigned int B;

    opcode = (iw >> 26) & 0b11;
    L = (iw >> 20) & 0b1;
    B = (iw >> 22) & 0b1;

    return (opcode == 1) && (L == 0b1) && (B == 0b0);
}

void armemu_ldr(struct arm_state *state) {
    //printf("RUNNING LDR\n");
    unsigned int iw;
    unsigned int rd, rn, rm, b;

    iw = *((unsigned int *) state->regs[PC]);

    rd = (iw >> 12) & 0xF;
    rn = (iw >> 16) & 0xF;

    state->regs[rd] = *((unsigned int *) state->regs[rn]);


    if (rd != PC) {
        state->regs[PC] = state->regs[PC] + 4;
    }
}

 bool is_ldrb(unsigned int iw) {
    //printf("inside is ldrb\n");

    unsigned int opcode;
    unsigned int L;
    unsigned int B;

    opcode = (iw >> 26) & 0b11;
    L = (iw >> 20) & 0b1;
    B = (iw >> 22) & 0b1;

    return (opcode == 1) && (L == 0b1) && (B == 0b1);
}

void armemu_ldrb(struct arm_state *state) {
    //printf("RUNNING LDRB\n");
    unsigned int iw;
    unsigned int rd, rn, rm;

    iw = *((unsigned int *) state->regs[PC]);
    rd = (iw >> 12) & 0xF;
    rn = (iw >> 16) & 0xF;
    rm = iw & 0xF;

    unsigned int tmp = state->regs[rn] + state->regs[rm];

    unsigned int value = (*(unsigned int *) tmp);

    state->regs[rd] = (char) value;

    if (rd != PC) {
        state->regs[PC] = state->regs[PC] + 4;
    }
}

bool is_str(unsigned int iw) {
    //printf("inside is str\n");

    unsigned int opcode;
    unsigned int L;
    unsigned int B;

    opcode = (iw >> 26) & 0b11;
    L = (iw >> 20) & 0b1;
    B = (iw >> 22) & 0b1;

    return (opcode == 1) && (L == 0b0) && (B == 0b0);
}

void armemu_str(struct arm_state *state) {
    //printf("RUNNING STR\n");
    unsigned int iw;
    unsigned int rd, rn, rm;

    iw = *((unsigned int *) state->regs[PC]);

    rd = (iw >> 12) & 0xF;
    rn = (iw >> 16) & 0xF;

    *((unsigned int *) state->regs[rn]) = state->regs[rd];

    if (rd != PC) {
        state->regs[PC] = state->regs[PC] + 4;
    }
}
bool is_branch_inst(unsigned int iw)
{
    //printf("inside is branch\n");
    unsigned int op;
    unsigned int opcode;

    op = (iw >> 26) & 0b11;
    opcode = (iw >> 21) & 0b1111;

    return (op == 0b10);
}

void armemu_branch(struct arm_state *state)
{
    //printf("RUNNING BRANCH\n");
    unsigned int iw;
    signed int imm;
    unsigned int lflag;

    iw = *((signed int *) state->regs[PC]);
    imm = iw & 0xFFFFFF;

    if(((imm >> 23) & 0b1) == 1){
        imm = imm | 0xFF000000;
    }

    imm = imm << 2;
    lflag = (iw >> 24) & 0b1;

    if(lflag == 0){
        state->regs[PC] = state->regs[PC] + 8 + imm;
    } else{
        state->regs[LR] = state->regs[PC] + 8 - 4;
        state->regs[PC] = state->regs[PC] + 8 + imm;
    }
}

void armemu_one(struct arm_state *state)
{
    //printf("inside armemu_one\n");
    unsigned int iw, rd;
    iw = *((unsigned int *) state->regs[PC]);
    rd = (iw >> 12) & 0xF;

    //printf("Check Cond = %d\n", check_cond(state));
    if (check_cond(state)) {
        if (is_bx_inst(iw)) {
            //printf("inside bx\n");
            armemu_bx(state);
            branch++;
        } else if (is_add_inst(iw)) {
            //printf("inside add\n");
            armemu_add(state);
            data++;
        } else if (is_sub_inst(iw)) {
            //printf("inside sub\n");
            armemu_sub(state);
            data++;
        } else if (is_mov_inst(iw)) {
            //printf("inside mov\n");
            armemu_mov(state);
            data++;
        } else if (is_cmp_inst(iw)) {
            //printf("inside cmp\n");
            armemu_cmp(state);
            data++;
        } else if (is_ldr(iw)) {
            //printf("inside ldr\n");
            armemu_ldr(state);
            memory++;
        } else if (is_branch_inst(iw)) {
            //printf("inside branch\n");
            armemu_branch(state);
            branch++;
        } else if (is_str(iw)) {
            //printf("inside str\n");
            armemu_str(state);
            memory++;
        } else if (is_ldrb(iw)) {
            armemu_ldrb(state);
            memory++;
        }
    } else {
        state->regs[PC] = state->regs[PC] + 4;
    }
    //printf("END\n");
}

unsigned int armemu(struct arm_state *state) {
    total = 0;
    data = 0;
    memory = 0;
    branch = 0;

    while (state->regs[PC] != 0) {
        total++;
        armemu_one(state);
    }

    return state->regs[0];
}

void test_sum(void) {
    struct arm_state state;
    unsigned r;
    int a[5] = {1, 2, 3, 4, 5};
    int neg[5] = {1, 2, 3, -4, 5};
    int thou[1000];
    int i;
    for (i = 0; i < 1000; i++) {
        thou[i] = i;
    }
    printf("TEST SUM_A:\n");

    init_arm_state(&state, (unsigned int *) sum_a_s, (unsigned int) a, 5, 0, 0);
    r = armemu(&state);
    printf("Positive = %d, Expected = 15\n", r);
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
    printf("\n");

    init_arm_state(&state, (unsigned int *) sum_a_s, (unsigned int) neg, 5, 0, 0);
    r = armemu(&state);
    printf("Negative = %d, Expected = 7\n", r);
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
    printf("\n");

    init_arm_state(&state, (unsigned int *) sum_a_s, (unsigned int) thou, 1000, 0, 0);
    r = armemu(&state);
    printf("Array of 1000: %d, Expected = 499500\n", r);
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);

    printf("\n");
}

void test_max(void) {
    struct arm_state state;
    unsigned r;
    int a[5] = {1, 2, 3, 4, 5};
    int neg[5] = {-5, -4, -3, -1, -2};
    int thou[1000];
    int i;
    for (i = 0; i < 1000; i++) {
        thou[i] = i;
    }
    printf("TEST FIND MAX:\n");

    printf("Positive: ");
    init_arm_state(&state, (unsigned int *) find_max_s, (unsigned int) a, 5, 0, 0);
    r = armemu(&state);
    printf("Max = %d, Expected = 5\n", r);
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
    printf("\n");

    printf("Negative: ");
    init_arm_state(&state, (unsigned int *) find_max_s, (unsigned int) neg, 5, 0, 0);
    r = armemu(&state);
    printf("Max = %d, Expected = -1\n", r);
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
    printf("\n");

    printf("Array of 1000: ");
    init_arm_state(&state, (unsigned int *) find_max_s, (unsigned int) thou, 1000, 0, 0);
    r = armemu(&state);
    printf("Max = %d, Expected = 999\n", r);
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
    printf("\n");

}

void test_fib_rec(void) {
    struct arm_state state;
    unsigned r;
    int i;

    printf("TEST FIB_REC:\n");

    printf("testing 5\n");
    init_arm_state(&state, (unsigned int *) fib_rec_s, (unsigned int) 5, 0, 0, 0);
    r = armemu(&state);
    printf("r = %d, Expected = 5\n", r);
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
    printf("\n");

    printf("testing 20\n");
    init_arm_state(&state, (unsigned int *) fib_rec_s, (unsigned int) 20, 0, 0, 0);
    r = armemu(&state);
    printf("r = %d, Expected = 6765\n", r);
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
    printf("\n");

}

void test_fib_iter(void) {
    struct arm_state state;
    unsigned r;
    int i;

    printf("TEST FIB_ITER:\n");

    printf("testing 5\n");
    init_arm_state(&state, (unsigned int *) fib_iter_s, (unsigned int) 5, 0, 0, 0);
    r = armemu(&state);
    printf("r = %d, Expected = 5\n", r);
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
    printf("\n");

    printf("testing 20\n");
    init_arm_state(&state, (unsigned int *) fib_iter_s, (unsigned int) 20, 0, 0, 0);
    r = armemu(&state);
    printf("r = %d, Expected = 6765\n", r);
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
    printf("\n");

}

void test_find_str(void) {
    struct arm_state state;
    unsigned int r;
    char *s = "hellothere";
    char *sub = "hello";
    printf("TESTING FIND_STR\n");

    printf("String = %s\n", s);
    printf("Substring = %s\n", sub);
    init_arm_state(&state, (unsigned int *) find_str_s, (unsigned int) s, (unsigned int) sub, 0, 0);
    r = armemu(&state);
    printf("Substring index = %d\n", r);
    printf("Expected = 0\n");
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
    printf("\n");

    sub = "there";
    printf("String = %s\n", s);
    printf("Substring = %s\n", sub);
    init_arm_state(&state, (unsigned int *) find_str_s, (unsigned int) s, (unsigned int) sub, 0, 0);
    r = armemu(&state);
    printf("Substring index = %d\n", r);
    printf("Expected = 5\n");
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
    printf("\n");

    sub = "HelloThere";
    printf("String = %s\n", s);
    printf("Substring = %s\n", sub);
    init_arm_state(&state, (unsigned int *) find_str_s, (unsigned int) s, (unsigned int) sub, 0, 0);
    r = armemu(&state);
    printf("Substring index = %d\n", r);
    printf("Expected = -1\n");
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
    printf("\n");

    sub = "lLo";
    printf("String = %s\n", s);
    printf("Substring = %s\n", sub);
    init_arm_state(&state, (unsigned int *) find_str_s, (unsigned int) s, (unsigned int) sub, 0, 0);
    r = armemu(&state);
    printf("Substring index = %d\n", r);
    printf("Expected = -1\n");
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
    printf("\n");

    sub = "ere";
    printf("String = %s\n", s);
    printf("Substring = %s\n", sub);
    init_arm_state(&state, (unsigned int *) find_str_s, (unsigned int) s, (unsigned int) sub, 0, 0);
    r = armemu(&state);
    printf("Substring index = %d\n", r);
    printf("Expected = 7\n");
    printf("ANALYTICS = instructions: %d, memory: %d, data: %d, branch: %d\n", total, memory, data, branch);
}

int main(int argc, char **argv)
{
    struct arm_state state;
    unsigned int r;
    test_sum();
    test_max();
    test_fib_rec();
    test_fib_iter();
    test_find_str();

  
    return 0;
}