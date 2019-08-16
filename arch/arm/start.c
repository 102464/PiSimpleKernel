#include <stdint.h>
#include <stdlib.h>

#include "panic.h"
#include "uart.h"
#include "stdio.h"
#include "mailbox.h"
#include "framebuffer.h"



void kernel_main(uint32_t r0, uint32_t r1, uint32_t atag)
{
    uart_init();
    printf("uart_init() completed\n");
    PropertyInit();
    PropertyAddTag(TAG_GET_VOLTAGE);
    PropertyAddTag(TAG_GET_BOARD_MODEL);
    PropertyProcess();
    mailbox_property_t* mp = PropertyGet(TAG_GET_VOLTAGE);
    printf("Core voltage: %dmV\n",mp->data.value_32);
    mp = PropertyGet(TAG_GET_BOARD_MODEL);
    printf("Board Model: %d\n",mp->data.value_32);
    //panic("BOOTLOOPED");

    int wid = 0, hei = 0, bpp = 0;
    int x, y, pitch = 0;
    color_t color;
    volatile unsigned char *fb = NULL;
    int pixel_offset;
    int r, g, b, a;
    float cd = COLOR_DELTA;
    unsigned int frame_count = 0;
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

    color.r = 0;
    color.g = 0;
    color.b = 255;
    color.a = 1.0;
    for( y = 0; y < hei; y++ )
        {
            color.r += ( 1.0 / hei );
            color.b = 0;

            for( x = 0; x < wid; x++ )
            {
                pixel_offset = ( x * ( bpp >> 3 ) ) + ( y * pitch );

                r = (int)( color.r * 0xFF ) & 0xFF;
                g = (int)( color.g * 0xFF ) & 0xFF;
                b = (int)( color.b * 0xFF ) & 0xFF;
                a = (int)( color.b * 0xFF ) & 0xFF;

                
                    /* Four bytes to write */
                fb[ pixel_offset++ ] = r;
                fb[ pixel_offset++ ] = g;
                fb[ pixel_offset++ ] = b;
                fb[ pixel_offset++ ] = a;
                

                color.b += ( 1.0 / wid );
            }
        }

        /* Scroll through the green colour */
        color.g += cd;
        if( color.g > 1.0 )
        {
            color.g = 1.0;
            cd = -COLOR_DELTA;
        }
        else if( color.g < 0.0 )
        {
            color.g = 0.0;
            cd = COLOR_DELTA;
        }
    
}