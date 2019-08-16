#include <stdint.h>

#include "panic.h"
#include "uart.h"
#include "stdio.h"

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atag)
{
    uart_init();
    printf("uart_init() completed\n");
    panic("BOOTLOOPED");
}