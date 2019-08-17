
#include "panic.h"
#include "stdio.h"

#include "uart.h"
__attribute__((always_inline)) static inline unsigned int
getRegistervalue(registers registername) {
    register unsigned int result = 0;
    switch (registername) {
    case r0: {
        asm volatile("mov %0, r0\n" : "=r"(result));
        break;
    }
    case r1: {
        asm volatile("mov %0, r1\n" : "=r"(result));
        break;
    }
    case r2: {
        asm volatile("mov %0, r2\n" : "=r"(result));
        break;
    }
    case r3: {
        asm volatile("mov %0, r3\n" : "=r"(result));
        break;
    }
    case r4: {
        asm volatile("mov %0, r4\n" : "=r"(result));
        break;
    }
    case r5: {
        asm volatile("mov %0, r5\n" : "=r"(result));
    }
    case r6: {
        asm volatile("mov %0, r6\n" : "=r"(result));
        break;
    }
    case r7: {
        asm volatile("mov %0, r7\n" : "=r"(result));
        break;
    }
    case r8: {
        asm volatile("mov %0, r8\n" : "=r"(result));
        break;
    }
    case r9: {
        asm volatile("mov %0, r9\n" : "=r"(result));
        break;
    }
    case r10: {
        asm volatile("mov %0, r10\n" : "=r"(result));
        break;
    }
    case r11: {
        asm volatile("mov %0, fp\n" : "=r"(result));
        break;
    }
    case r12: {
        asm volatile("mov %0, r12\n" : "=r"(result));
        break;
    }
    case r13: {
        asm volatile("mov %0, sp\n" : "=r"(result));
        break;
    }
    case r14: {
        asm volatile("mov %0, lr\n" : "=r"(result));
        break;
    }
    case r15: {
        asm volatile("mov %0, pc\n" : "=r"(result));
        break;
    }
    case cpsr: {
        asm volatile("mrs %0, cpsr\n" : "=r"(result));
        break;
    }
    default: {
        result = 0xdeadbeef;
        break;
    }
    }

    return (result);
}
void panic(char *panicstr, ...) {

    printf("**panic** ");
    va_list ap;
    va_start(ap, panicstr);
    kvprintf(panicstr, putchar, (void *)1, 10, ap);
    va_end(ap);

    printf(" [CPU 0]\n");
    printf("Register information:\r\n");
    unsigned int _r0, _r1, _r2, _r3, _r4, _r5, _r6, _r7, _r8, _r9, _r10, _fp,
        _r12, _sp, _lr, _pc, _cpsr;
    _r0 = getRegistervalue(r0);
    _r1 = getRegistervalue(r1);
    _r2 = getRegistervalue(r2);
    _r3 = getRegistervalue(r3);
    _r4 = getRegistervalue(r4);
    _r5 = getRegistervalue(r5);
    _r6 = getRegistervalue(r6);
    _r7 = getRegistervalue(r7);
    _r8 = getRegistervalue(r8);
    _r9 = getRegistervalue(r9);
    _r10 = getRegistervalue(r10);
    _fp = getRegistervalue(r11);
    _r12 = getRegistervalue(r12);
    _sp = getRegistervalue(r13);
    _lr = getRegistervalue(r14);
    _pc = getRegistervalue(r15);
    _cpsr = getRegistervalue(cpsr);
    printf("R0=0x%08X R1=0x%08X R2=0x%08X R3=0x%08X  R4=0x%08X R5=0x%08X "
           "R6=0x%08X\n",
           _r0, _r1, _r2, _r3, _r4, _r5, _r6);

    printf("R7=0x%08X R8=0x%08X R9=0x%08X R10=0x%08X FP=0x%08X R12=0x%08X\n",
           _r7, _r8, _r9, _r10, _fp, _r12);

    printf("SP=0x%08X LR=0x%08X PC=0x%08X CPSR=0x%08X\n", _sp, _lr, _pc, _cpsr);
    printf("Detailed information: (Provided by the kernel)\n");

    printf("Stack Information: \n");
    int offset;
    unsigned int *stack;
    stack = (unsigned int *)_sp;
    unsigned int count = 0;
    printf("Stack pointer is at 0x%08x\n", _sp);
    printf("Frame pointer is at 0x%08x\n", _fp);
    printf("SP: 0x%08x\n", (unsigned int)stack);
    printf("Before SP:\n");
    for (offset = 0; offset >= -50; offset -= 4) {
        count++;
        if (*(stack + offset) == 0) {
            printf("%d: 0x%08X (offset %d): Empty\n", count, _sp + offset,
                   offset);
        } else {
            if (offset == 0) {
                printf("%d: 0x%08X (offset %d): Data 0x%08X <--$sp\n", count,
                       _sp + offset, offset, *(stack + offset));
            } else {
                printf("%d: 0x%08X (offset %d): Data 0x%08X\n", count,
                       _sp + offset, offset, *(stack + offset));
            }
        }
    };
    printf("After SP:\n");
    for (offset = 0; offset <= 50; offset += 4) {
        count++;
        if (*(stack + offset) == 0) {
            printf("%d: 0x%08X (offset %d): Empty\n", count, _sp + offset,
                   offset);
        } else {
            if (offset == 0) {
                printf("%d: 0x%08X (offset %d): Data 0x%08X <--$sp\n", count,
                       _sp + offset, offset, *(stack + offset));
            } else {
                printf("%d: 0x%08X (offset %d): Data 0x%08X\n", count,
                       _sp + offset, offset, *(stack + offset));
            }
        }
    };

    printf("----[End panic info]----\n");
    while (1)
        ;
}