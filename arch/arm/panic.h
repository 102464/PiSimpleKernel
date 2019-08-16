typedef enum {
    r0 = 0,
    r1 = 1,
    r2,
    r3,
    r4,
    r5,
    r6,
    r7,
    r8,
    r9,
    r10,
    r11,
    r12,
    r13,
    r14,
    r15,
    cpsr
} registers;



int printarg(char **out, int *varg);
void panic(char *panicstr, ...);
