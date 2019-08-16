#include <stdarg.h>
int kvprintf(char const *fmt, void (*func)(int, void*), void *arg, int radix, va_list ap);
void printf(const char *fmt, ...);
void sprintf(char *buffer, const char *fmt, ...);