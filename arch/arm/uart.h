
#define putchar uart_putc

void uart_init();
void uart_putc(unsigned char c);
unsigned char uart_getc();
void uart_puts(const char *str);
char *uart_gets();