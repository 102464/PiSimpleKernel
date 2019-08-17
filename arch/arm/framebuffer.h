typedef struct {
    float r;
    float g;
    float b;
    float a;
} color_t;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_DEPTH 32 /* 16 or 32-bit */

#define COLOR_DELTA 0.05 /* Float from 0 to 1 incremented by this amount */

void init_framebuffer(int width, int height, int depth);
volatile unsigned char *get_fbAddr(void);
void fb_write(color_t _color, int x, int y);
void draw_rectangle(int x, int y, int size_x, int size_y);
void drawBox(int x, int y, int size);
void fb_put(char *chr, int x, int y);
// void fb_puts(volatile unsigned char *fb, char chr[], int row, int line, int
// pitch);
void fb_puts(char *chr);