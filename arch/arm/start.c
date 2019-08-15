#include <stddef.h>
#include <stdint.h>
#include "gpio.h"
#include "stdio.h"
#include "panic.h"
#include "mailbox.h"

static inline void mmio_write(uint32_t reg, uint32_t data)
{
    *(volatile uint32_t*)reg = data;
}

static inline uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t*)reg;
}


// Loop <delay> times in a way that the compiler won't optimize away
static inline void delay(int32_t count)
{
	asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
		 : "=r"(count): [count]"0"(count) : "cc");
}
 

 
void uart_init()
{
	// Disable UART0.
	mmio_write(UART0_CR, 0x00000000);
	// Setup the GPIO pin 14 && 15.
 
	// Disable pull up/down for all GPIO pins & delay for 150 cycles.
	mmio_write(GPPUD, 0x00000000);
	delay(150);
 
	// Disable pull up/down for pin 14,15 & delay for 150 cycles.
	mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
	delay(150);
 
	// Write 0 to GPPUDCLK0 to make it take effect.
	mmio_write(GPPUDCLK0, 0x00000000);
 
	// Clear pending interrupts.
	mmio_write(UART0_ICR, 0x7FF);
 
	// Set integer & fractional part of baud rate.
	// Divider = UART_CLOCK/(16 * Baud)
	// Fraction part register = (Fractional part * 64) + 0.5
	// UART_CLOCK = 3000000; Baud = 115200.
 
	// Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
	mmio_write(UART0_IBRD, 1);
	// Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
	mmio_write(UART0_FBRD, 40);
 
	// Enable FIFO & 8 bit data transmission (1 stop bit, no parity).
	mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
 
	// Mask all interrupts.
	mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
	                       (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
 
	// Enable UART0, receive & transfer part of UART.
	mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}
 
void uart_putc(unsigned char c)
{
	// Wait for UART to become ready to transmit.
	while ( mmio_read(UART0_FR) & (1 << 5) ) { }
	mmio_write(UART0_DR, c);
}
 
unsigned char uart_getc()
{
    // Wait for UART to have received something.
    while ( mmio_read(UART0_FR) & (1 << 4) ) { }
    return mmio_read(UART0_DR);
}
 
void uart_puts(const char* str)
{
	for (size_t i = 0; str[i] != '\0'; i ++)
		uart_putc((unsigned char)str[i]);
}
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
	// Declare as unused
	//(void) r0;
	//(void) r1;
	//(void) atags;
 
	uart_init();
	printf("uart_init() complete\r\n");
	printf("+------------------------------------------------------------+\r\n");
	printf("| EmbOS for Cortex-A7 (raspi2), copyright MyWorkstation Inc. |\r\n");
	printf("| BUILD STYLE: DEBUG                                         |\r\n");
	printf("+------------------------------------------------------------+\r\n");
	printf("EmbOS Kernel version 1.0.0\r\n");
	printf("args: r0=0x%x, r1=0x%x, atags=0x%x\r\n",r0, r1, atags);
	int test_var = 1;
	printf("Testing variable: test_var=%d, addr=0x%x\r\n",test_var,(uint32_t)&test_var);
	/*
	volatile uint32_t  __attribute__((aligned(16))) mbox[36];
	mbox[0] = 8*4;
	mbox[1] = MAILBOX_REQUEST;

	mbox[2] = 0x10004;
	mbox[3] = 8;
	mbox[4] = 8;
	mbox[5] = 0;
	mbox[6] = 0;

	mbox[7] = MAILBOX_TAG_LAST;

	if (mailbox_call(MB0_TAGS_ARM_TO_VC, mbox)) {
		printf("SN:[%x%x]\r\n",mbox[5],mbox[6]);
		}else{
			printf("Serial number not found on your device.\r\n");
	}
	if (mailbox_send(MB0_TAGS_ARM_TO_VC,0x10004,0,0)) {
		
	}
	*/
	PropertyAddTag(TAG_GET_VOLTAGE);
	PropertyAddTag(TAG_GET_BOARD_MAC_ADDRESS);
	mailbox_property_t *mp;
	mp = PropertyGet(TAG_GET_VOLTAGE);
	printf("Core Voltage: %dmV\r\n",mp->data.value_32);
	mp = PropertyGet(TAG_GET_BOARD_MODEL);
	printf("Board Model: %d\r\n",mp->data.value_32);
	mp = PropertyGet(TAG_GET_BOARD_MAC_ADDRESS);
	if( mp )
        printf( "MAC Address: %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X\r\n",
               mp->data.buffer_8[0], mp->data.buffer_8[1], mp->data.buffer_8[2],
               mp->data.buffer_8[3], mp->data.buffer_8[4], mp->data.buffer_8[5] );
    else
        printf( "MAC Address: NULL\r\n" );
    
	panic("System halted\n");
}