#include "mmio.h"
#include <stdint.h>
// 中断处理基地址
#define INTERRUPT_CONTROLLER_BASE (MMIO_BASE + 0xB200)
/** @brief 中断控制器寄存器组 **/
typedef struct {
    volatile uint32_t IRQ_basic_pending;
    volatile uint32_t IRQ_pending_1;
    volatile uint32_t IRQ_pending_2;
    volatile uint32_t FIQ_control;
    volatile uint32_t Enable_IRQs_1;
    volatile uint32_t Enable_IRQs_2;
    volatile uint32_t Enable_basic_IRQs;
    volatile uint32_t Disable_IRQs_1;
    volatile uint32_t Disable_IRQs_2;
    volatile uint32_t Disable_basic_IRQs;
} irq_controller_t;

irq_controller_t *GetIRQController(void);
void enable_irq(void);
/** @brief ARM中断控制位
       参见BCM2835 ARM Peripherals manual, section 7.5 */
#define BASIC_ARM_TIMER_IRQ (1 << 0)
#define BASIC_ARM_MAILBOX_IRQ (1 << 1)
#define BASIC_ARM_DOORBELL_0_IRQ (1 << 2)
#define BASIC_ARM_DOORBELL_1_IRQ (1 << 3)
#define BASIC_GPU_0_HALTED_IRQ (1 << 4)
#define BASIC_GPU_1_HALTED_IRQ (1 << 5)
#define BASIC_ACCESS_ERROR_1_IRQ (1 << 6)
#define BASIC_ACCESS_ERROR_0_IRQ (1 << 7)