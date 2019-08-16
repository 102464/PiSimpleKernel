#include "mailbox.h"
#include "stdio.h"
#include "panic.h"
#include "framebuffer.h"
#include "stdio.h"

void init_framebuffer(int width, int height, int depth){
    PropertyInit();
    PropertyAddTag(TAG_ALLOCATE_BUFFER);
    PropertyAddTag(TAG_SET_PHYSICAL_SIZE, width, height);
    PropertyAddTag(TAG_SET_VIRTUAL_SIZE, width, height * 2);
    PropertyAddTag(TAG_SET_DEPTH, depth);
    PropertyAddTag(TAG_GET_PITCH);
    PropertyAddTag(TAG_GET_PHYSICAL_SIZE);
    PropertyAddTag(TAG_GET_DEPTH);
    PropertyProcess();
}

volatile unsigned char* get_fbAddr(void){
    mailbox_property_t *mp;
    if ((mp = PropertyGet(TAG_ALLOCATE_BUFFER)))
    {
        volatile unsigned char* fb = (unsigned char*)mp->data.buffer_32[0];
        printf("Framebuffer address: 0x%08x\n", fb);
        return fb;
    }else{
        panic("Failed to get framebuffer address");
        return 0;
    }
}

void fb_write(volatile unsigned char *fb, color_t color, int x, int y, int pitch){
    int offset = (x * (SCREEN_DEPTH) >> 3) + (y * pitch);
    fb[offset++] = color.r;
    fb[offset++] = color.g;
    fb[offset++] = color.b;
    fb[offset++] = color.a;
    //printf("Offset: 0x%08x\n", offset);
}