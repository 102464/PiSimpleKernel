#include "interrupt.h"
#include "stdio.h"
static irq_controller_t *IRQController =
    (irq_controller_t *)INTERRUPT_CONTROLLER_BASE;

/** @brief 返回IRQ控制器寄存器组 **/
irq_controller_t *GetIRQController(void) { return IRQController; }
void enable_irq(void) {
    asm("mrs r0,cpsr\n"
        "bic r0,r0,#0x80\n"
        "msr cpsr_c,r0\n"
        :
        :
        : "r0");
}

void __attribute__((interrupt("IRQ"))) interrupt_handler(void) {
    printf("Timer interrupt occurred!\n");
}