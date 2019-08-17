#include "mailbox.h"
#include "stdio.h"
#include "panic.h"
#include "framebuffer.h"
#include "stdio.h"
#include "font.h"

extern int wid, hei, bpp;
extern color_t color;
extern int pitch;
extern volatile unsigned char *fb;
int row, line;

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
    row = 1;
    line = 1;
}

volatile unsigned char* get_fbAddr(void){
    mailbox_property_t *mp;
    if ((mp = PropertyGet(TAG_ALLOCATE_BUFFER)))
    {
        volatile unsigned char* fb = (unsigned char*)mp->data.buffer_32[0];
        printf("get_fbAddr(): Framebuffer address: 0x%08x\n", fb);
        return fb;
    }else{
        panic("get_fbAddr(): Failed to get framebuffer address");
        return 0;
    }
}

void fb_write(color_t _color, int x, int y){
    if ((x>=SCREEN_WIDTH) || (y>=SCREEN_HEIGHT)){
        panic("fb_write() x or y error: x=%d, y=%d, pitch=%d\n",x,y,pitch);
    }
    int offset = (x * (SCREEN_DEPTH) >> 3) + (y * pitch);
    fb[offset++] = _color.r;
    fb[offset++] = _color.g;
    fb[offset++] = _color.b;
    fb[offset++] = _color.a;
}

void draw_rectangle(int x, int y, int size_x, int size_y){
    //printf("draw_rectangle(): Drawing rectangle size x: %d y: %d on location (%d,%d)\n", size_x, size_y, x, y);
    int _x = 0;
    int _y = 0;
    if ((_x + size_x > SCREEN_HEIGHT) || (_y + size_y > SCREEN_WIDTH)) {
        panic("draw_rectangle(): x or y error: x=%d, y=%d, size_x=%d, size_y=%d, height=%d, width=%d", \
         x, y, size_x, size_y, SCREEN_HEIGHT, SCREEN_WIDTH);
    }
    for (_x = x; _x <= x + size_x; _x++){
        for (_y = y; _y <= y + size_y; _y++){
            fb_write(color, _x, _y);
        }
    }
    //printf("draw_rectangle(): Done drawing\n");
}
void drawBox(int x, int y, int size){
    //printf("drawBox(): Drawing box size %d on location (%d,%d)\n", size, x, y);
    int _x = 0;
    int _y = 0;
    if ((_x + size > SCREEN_HEIGHT) || (_y + size > SCREEN_WIDTH)) {
        panic("drawBox(): x or y error: x=%d, y=%d, size=%d, height=%d, width=%d", x, y, size, SCREEN_HEIGHT, SCREEN_WIDTH);
    }
    for (_x = x; _x <= x + size; _x++){
         for (_y = y; _y <= y + size; _y++){
            fb_write(color, _x, _y);
        }
    }
    //printf("drawBox(): Done drawing\n");
}
void fb_put(char chr[], int x, int y){
    //int c = (int)chr[0];
    
    unsigned char *font = (unsigned char*)&fontdata_8x16; // Font data
    //color_t color;
    int cx, cy;
    color_t forecolor;
    forecolor.r=255;
    forecolor.g=255;
    forecolor.b=255;
    forecolor.a=1; // Set forecolor to white
    int mask[8]={128,64,32,16,8,4,2,1}; // Mask for shielding specific bits
    //printf("Print ASCII %d to framebuffer\n",*chr);
    if (*chr==10){ // Found \n?
        //printf("\\n detected\n");
        row=1; line++; // Move to first row, next line.
        return;
    }
    unsigned char *glyph = font + *chr * 16;
    //printf("row=%d, line=%d\n",row,line);
    for (cy=0;cy<16;cy++){
        for(cx=0;cx<8;cx++){
            if (glyph[cy]&mask[cx]){                
                fb_write(forecolor, x+cx, y+cy); // Bit is up
            }else{
                fb_write(color, x+cx, y+cy); // Bit is down
            }   
        }
    }
    row++; // Move to next row
}

void fb_puts(char *chr){
    //printf("fb_puts(): Put string %s to the framebuffer.\n",chr);
    int x=0;
    int y=0;
    int offset=0;
    while(*(chr+offset)){
        x = (row - 1) * 8; // location x
        if (x>=SCREEN_WIDTH) { // Reached end of screen?
            x=0; row=1; line++; // Move to first row, next line;
        }
        y = (line - 1) * 16; // location y
        //printf("Offset %d: ",offset);
        fb_put(chr+offset,x,y); // Put
        //row++;
        offset++;
    }
}
    