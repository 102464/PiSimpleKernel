typedef struct {
    float r;
    float g;
    float b;
    float a;
} color_t;

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480
#define SCREEN_DEPTH    32      /* 16 or 32-bit */

#define COLOR_DELTA    0.05    /* Float from 0 to 1 incremented by this amount */

void init_framebuffer(int width, int height, int depth);
volatile unsigned char* get_fbAddr(void);
void fb_write(volatile unsigned char *fb, color_t color, int x, int y, int pitch);