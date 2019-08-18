#include <stdint.h>
#include <stdlib.h>

#include "framebuffer.h"
#include "interrupt.h"
#include "mailbox.h"
#include "panic.h"
#include "stdio.h"
#include "timer.h"
#include "uart.h"

int wid = 0, hei = 0, bpp = 0;
color_t color;
int pitch = 0;
volatile unsigned char *fb = NULL;

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atag) {
    // 初始化UART串口
    uart_init();
    printf("uart_init() completed\n");
    printf("+---------------------------------------------------------+\n");
    printf("| EmbOS for BCM2835 (raspi2), copyright MyWorkstation Inc.|\n");
    printf("| BUILD STYLE: DEBUG                                      |\n");
    printf("+---------------------------------------------------------+\n");
    printf("r0=0x%08X, r1=0x%08X, atag=0x%08X\n", r0, r1, atag);
    PropertyInit();
    //向VideoCore发送数据请求
    PropertyAddTag(TAG_GET_VOLTAGE);
    PropertyAddTag(TAG_GET_BOARD_MODEL);
    PropertyAddTag(TAG_GET_ARM_MEMORY);
    //处理请求
    PropertyProcess();
    //取得请求结果
    mailbox_property_t *mp = PropertyGet(TAG_GET_VOLTAGE);
    if (mp) {
        printf("Core voltage: %dmV\n", mp->data.buffer_32[0]);
    } else {
        printf("Core voltage 0mV\n");
    }
    mp = PropertyGet(TAG_GET_BOARD_MODEL);
    if (mp) {
        printf("Board model %d\n", mp->data.buffer_32[0]);
    } else {
        printf("Board model not found\n");
    }
    mp = PropertyGet(TAG_GET_ARM_MEMORY);
    if (mp) {
        printf("Total memory %d Bytes\n", mp->data.buffer_32[0]);
    } else {
        printf("Failed to read memory size\n");
    }
    // panic("BOOTLOOPED");
    // 初始化framebuffer(帧缓冲)
    printf("Contacting GPU for framebuffer.\n");
    init_framebuffer(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH);
    // 取得屏幕宽高
    if ((mp = PropertyGet(TAG_GET_PHYSICAL_SIZE))) {
        wid = mp->data.buffer_32[0];
        hei = mp->data.buffer_32[1];
        printf("Initialized framebuffer with width: %d height: %d\n", wid, hei);
    } else {
        panic("Failed to initialize framebuffer");
    }
    //取得颜色深度
    if ((mp = PropertyGet(TAG_GET_DEPTH))) {
        bpp = mp->data.buffer_32[0];
        printf("BPP: %d\n", bpp);
    } else {
        panic("Failed to get screen depth");
    }
    if ((mp = PropertyGet(TAG_GET_PITCH))) {
        pitch = mp->data.buffer_32[0];
        printf("Pitch: %d bytes\n");
    } else {
        panic("Failed to get screen pitch");
    }
    fb = get_fbAddr(); // 取得framebuffer地址
    printf("Welcome to EmbOS command prompt (version 1.0.0)\n");
    // int offset = 0;
    for (;;) {
        char *str;
        printf("UART> ");
        str = uart_gets();
        printf("\nYou input: ");
        uart_puts(str);

        printf("\n");
        // offset = 0;
    }
}