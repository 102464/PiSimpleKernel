#include <stdint.h>
#include <stdlib.h>

#include "panic.h"
#include "uart.h"
#include "stdio.h"
#include "mailbox.h"
#include "framebuffer.h"

int wid = 0, hei = 0, bpp = 0;
color_t color;
int pitch = 0;
volatile unsigned char *fb = NULL;

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atag)
{
    uart_init();
    printf("uart_init() completed\n");
    printf("+---------------------------------------------------------+\n");
    printf("| EmbOS for BCM2835 (raspi2), copyright MyWorkstation Inc.|\n");
    printf("| BUILD STYLE: DEBUG                                      |\n");
    printf("+---------------------------------------------------------+\n");
    printf("r0=%08X, r1=%08X, atag=%08X\n", r0, r1, atag);
    PropertyInit();
    PropertyAddTag(TAG_GET_VOLTAGE);
    PropertyAddTag(TAG_GET_BOARD_MODEL);
    PropertyAddTag(TAG_GET_ARM_MEMORY);
    PropertyProcess();
    mailbox_property_t* mp = PropertyGet(TAG_GET_VOLTAGE);
    if (mp){
        printf("Core voltage: %dmV\n",mp->data.buffer_32[0]);
    }else{
        printf("Core voltage 0mV\n");
    }
    mp = PropertyGet(TAG_GET_BOARD_MODEL);
    if (mp) {
        printf("Board model %d\n",mp->data.buffer_32[0]);
    }else{
        printf("Board model not found\n"); 
    
    }
    mp = PropertyGet(TAG_GET_ARM_MEMORY);
    if (mp) {
        printf("Total memory %d Bytes\n",mp->data.buffer_32[0]);
    }else{
        printf("Failed to read memory size\n"); 
    
    }
    //panic("BOOTLOOPED");

    
    printf("Contacting GPU for framebuffer.\n");
    init_framebuffer(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH);
    if ((mp = PropertyGet(TAG_GET_PHYSICAL_SIZE)))
    {
        wid = mp->data.buffer_32[0];
        hei = mp->data.buffer_32[1];
        printf("Initialized framebuffer with width: %d height: %d\n", wid, hei);
    }else{
        panic("Failed to initialize framebuffer");
    }
    if ((mp = PropertyGet(TAG_GET_DEPTH)))
    {
        bpp = mp->data.buffer_32[0];
        printf("BPP: %d\n",bpp);
    }else{
        panic("Failed to get screen depth");
    }
    if ((mp = PropertyGet(TAG_GET_PITCH)))
    {
        pitch = mp->data.buffer_32[0];
        printf("Pitch: %d bytes\n");
    }else{
        panic("Failed to get screen pitch");
    }
    fb = get_fbAddr();
    fb_puts("EmbOS Version 1.0.0\n");
    fb_puts("Copyright MyWorkstation Inc.\n");
    fb_puts("Starting drawing test...\n");
    color.r=0;
    color.g=255;
    color.b=0;
    color.a=1;
    drawBox(100, 100, 100);
    color.r=255;
    color.g=0;
    color.b=0;
    color.a=1;
    drawBox(150, 150, 100);
    color.r=0;
    color.g=0;
    color.b=255;
    color.a=1;
    drawBox(200, 200, 100);
    /*
    fb_putc(fb,"H",fcolor,bcolor,0,0,pitch);
    fb_putc(fb,"e",fcolor,bcolor,8,0,pitch);
    fb_putc(fb,"l",fcolor,bcolor,16,0,pitch);
    fb_putc(fb,"l",fcolor,bcolor,24,0,pitch);
    fb_putc(fb,"o",fcolor,bcolor,32,0,pitch);
    fb_putc(fb," ",fcolor,bcolor,40,0,pitch);
    fb_putc(fb,"W",fcolor,bcolor,48,0,pitch);
    fb_putc(fb,"o",fcolor,bcolor,56,0,pitch);
    fb_putc(fb,"r",fcolor,bcolor,64,0,pitch);
    fb_putc(fb,"l",fcolor,bcolor,72,0,pitch);
    fb_putc(fb,"d",fcolor,bcolor,80,0,pitch);
    fb_putc(fb,"!",fcolor,bcolor,88,0,pitch);
    */
    color.b=0;
    fb_puts("Test completed\n");
    fb_puts("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd");
    printf("uart_putc()\n");
    while(1){
        uart_putc(uart_getc());
    }
}