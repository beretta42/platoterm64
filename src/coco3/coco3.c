#include "coco3.h"


extern unsigned int scalex[];
extern unsigned int scaley[];

#define screen ((unsigned char *)0x6000)

int debug( int y )
{
    return y + 0;
}

static void ptermDrawPoint(int x, int y)
{
    tgi_setpixel(scalex[x],scaley[y]);
}

void tgi_setpixel (int x, int y)
{
    unsigned char rem;
    unsigned char mask;
    int off;
    
    rem = 7 - (x & 7);
    mask = 1 << rem;
    x >>= 3;
    off = y * 40 + x;
    screen[off] = screen[off] | mask;
}


#define init0 *((unsigned char *)0xff90)
#define video *((unsigned char *)0xff98)
#define res   *((unsigned char *)0xff99)
#define vscr  *((unsigned char *)0xff9c)
#define oreg  *((unsigned int  *)0xff9d)
#define pal   ((unsigned char *)0xffb0)
#define border *((unsigned char *)0xff9a)

void tgi_init (void)
{
    init0 = 0x4c;
    video = 0x80;
    res = 0x0c;
    vscr = 0;
    oreg = 0xec00;
    pal[0] = 0x00;
    pal[1] = 0xff;
    border = 0x88;
}



void tgi_line (int x1, int y1, int x2, int y2)
{
 int dx, dy; int stepx, stepy; dx = x2 - x1; dy = y2 - y1; if (dx < 0) { dx = -dx; stepx = -1; } else { stepx = 1; } if (dy < 0) { dy = -dy; stepy = -1; } else { stepy = 1; } dx <<= 1; dy <<= 1;
// draw first point 
tgi_setpixel (x1, y1);
//check for shallow line
 if (dx > dy) { int fraction = dy - (dx >> 1); while (x1 != x2) { if (fraction >= 0) { y1 += stepy; fraction -= dx; } x1 += stepx; fraction += dy; tgi_setpixel (x1, y1); } } //otherwise steep line 
else { int fraction = dx - (dy >> 1); while (y1 != y2) { if (fraction >= 0) { x1 += stepx; fraction -= dy; } y1 += stepy; fraction += dx; tgi_setpixel (x1, y1); } }
}

void tgi_setcolor(unsigned char color)
{

}


void tgi_bar(int x1, int y1, int x2, int y2)
{
    

}

void tgi_clear (void)
{
    memset(screen, 0, 7680);
}


void memset(char *s, int c, int n)
{
    while(n--)
	*s++ = c;
}



void platform_scroll_up( void )
{
    unsigned char *d = screen;
    unsigned char *s = screen + (40 * 6);
    int count = 7680 - (40 * 6);
    while (count--)
	*d++ = *s++;
    count = 40 * 6;
    while (count--)
	*d++ = 0;
}

