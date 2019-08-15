
#include "stdio.h"
#include "panic.h"
#include "power.h"
void panic(char *panicstr, ...)
{
    printf("**panic** occurred in CPU 0!\r\n");
    printf("Detailed information:\r\n");

    register int *varg = (int *)(&panicstr);
	printarg(0, varg);
        
    while(1);
}