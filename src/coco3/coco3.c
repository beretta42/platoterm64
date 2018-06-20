#include "coco3.h"

extern unsigned int scalex[];
extern unsigned int scaley[];

unsigned char color = 0xff;

void TTYChar(unsigned char b);

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
    if (color){
	screen[off] = screen[off] | mask;
    }
    else 
	screen[off] = screen[off] & ~mask;
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

void tgi_setcolor(unsigned char c)
{
    color = c;
}


void tgi_bar(int x1, int y1, int x2, int y2)
{
    int d;
    int h;
    if (y1 < y2){
	d = 1;
	h = y2 - y1;
    }
    else { 
	d = -1;
	h = y1 - y2;
    }
    do {
	tgi_line(x2,y1,x1,y1);
	y1 += d;
    } while (h--);
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

void mouse_move(int x, int y)
{
    // todo
}

void waitvsync(void)
{
    // todo
}

int abs(int x)
{
    /* todo / fime : 
       check usage of this function... 
       it looks like an effective noop in calling code */
    return x;
}


/* Home rolled keyboard handling */
#define keystrobe (*(volatile unsigned char *)0xff02)
#define keyread   (*(volatile unsigned char *)0xff00)
unsigned char ktab[8];
unsigned char ktab1[8];
volatile unsigned char key;
volatile unsigned char meta;

/* keycode to ascii table */
unsigned char atab[] = {
    '@', 'h', 'p', 'x', '0', '8', 13 /*enter*/,
    'a', 'i', 'q', 'y', '1', '9', 0 /*clear*/,
    'b', 'j', 'r', 'z', '2', ':', 0 /*break*/,
    'c', 'k', 's', 0/*up*/, '3', ';', 0 /*alt*/,
    'd', 'l', 't', 0/*dn*/, '4', ',', 0 /*cntl*/,
    'e', 'm', 'u', 0/*lf*/, '5', '-', 0 /*f1*/,
    'f', 'n', 'v', 0/*rt*/, '6', '.', 0 /*f2*/,
    'g', 'o', 'w', ' ', '7', '/', 0 /*shift*/,
};

/* keycode to shifted ascii table */
unsigned char satab[] = {
    '@', 'H', 'P', 'X', '0', '(', 10 /*enter*/,
    'A', 'I', 'Q', 'Y', '!', ')', 0 /*clear*/,
    'B', 'J', 'R', 'Z', '"', '*', 0 /*break*/,
    'C', 'K', 'S', 0/*up*/, '#', '+', 0 /*alt*/,
    'D', 'L', 'T', 0/*dn*/, '$', '<', 0 /*cntl*/,
    'E', 'M', 'U', 0/*lf*/, '%', '=', 0 /*f1*/,
    'F', 'N', 'V', 0/*rt*/, '&', '>', 0 /*f2*/,
    'G', 'O', 'W', ' ', '\'', '?', 0 /*shift*/,
};

/* keycode to plato normal */
unsigned char knone[] = {
    0x98, 'H', 'P', 'X', 0x00, 0x08, 0x16 /*enter*/,
    'A', 'I', 'Q', 'Y', 0x01, 0x09, 0 /*clear*/,
    'B', 'J', 'R', 'Z', 0x02, 0x7c, 0 /*break*/,
    'C', 'K', 'S', 0x0c, 0x03, 0x5c, 0 /*alt*/,
    'D', 'L', 'T', 0/*dn*/, 0x04, 0x21, 0 /*cntl*/,
    'E', 'M', 'U', 0x0d/*lf*/, 0x05, 0x5b, 0 /*f1*/,
    'F', 'N', 'V', 0/*rt*/, 0x06, 0x21, 0 /*f2*/,
    'G', 'O', 'W', 0x40, 0x07, 0x5d, 0 /*shift*/,
};

/* keycode to plato shifted */
unsigned char kshift[] = {
    '@', 'h', 'p', 'x', '0', '8', 0x36 /*enter*/,
    'a', 'i', 'q', 'y', '1', '9', 0 /*clear*/,
    'b', 'j', 'r', 'z', '2', ':', 0 /*break*/,
    'c', 'k', 's', 0/*up*/, '3', ';', 0 /*alt*/,
    'd', 'l', 't', 0/*dn*/, '4', ',', 0 /*cntl*/,
    'e', 'm', 'u', 0/*lf*/, '5', '-', 0 /*f1*/,
    'f', 'n', 'v', 0/*rt*/, '6', '.', 0 /*f2*/,
    'g', 'o', 'w', 0x60, '7', 0x7d, 0 /*shift*/,
};

/* keycode to plato control */
unsigned char kcntl[] = {
    '@', 0x15, 'p', 'x', '0', '8', 0x33 /*enter*/,
    0x1c, 'i', 'q', 'y', '1', '9', 0 /*clear*/,
    0x18, 'j', 'r', 'z', '2', ':', 0 /*break*/,
    'c', 'k', 0x1a, 0/*up*/, '3', ';', 0 /*alt*/,
    0x19, 0x1d, 't', 0/*dn*/, '4', ',', 0 /*cntl*/,
    0x17, 'm', 'u', 0/*lf*/, '5', '-', 0 /*f1*/,
    'f', 'n', 'v', 0/*rt*/, '6', '.', 0 /*f2*/,
    'g', 'o', 'w', ' ', '7', '/', 0 /*shift*/,
};

/* keycode to plato shift-cntls */
unsigned char kshcntl[] = {
    '@', 0x35, 'p', 'x', '0', '8', 0x33 /*enter*/,
    0x3c, 'i', 'q', 'y', '1', '9', 0 /*clear*/,
    0x38, 'j', 'r', 'z', '2', ':', 0 /*break*/,
    'c', 'k', 0x3a, 0/*up*/, '3', ';', 0 /*alt*/,
    0x39, 0x3d, 't', 0/*dn*/, '4', ',', 0 /*cntl*/,
    0x37, 'm', 'u', 0/*lf*/, '5', '-', 0 /*f1*/,
    'f', 'n', 'v', 0/*rt*/, '6', '.', 0 /*f2*/,
    'g', 'o', 'w', 0x60, '7', 0x7d, 0 /*shift*/,
};




void pnibble(unsigned char b)
{
    b &= 0xf;
    if (b > 9)
	TTYChar( b - 10 + 'a');
    else
	TTYChar( b + '0' );
}

void pbyte(unsigned char b)
{
    pnibble(b>>4);
    pnibble(b);
}

/* poll the coco keyboard - called from interrupt */
void kpoll(void)
{
    unsigned char m = 0;
    unsigned char b = 0xfe;
    int i,j;
    /* copy existing ktab to prime */
    for(i = 0; i < 8; i++)
	ktab1[i] = ktab[i];
    /* read keys into table */
    for(i = 0; i < 8; i++){
	keystrobe = b;
	ktab[i] = (~keyread) & 0x7f;
	b = (b << 1) + 1;
    }
    /* gather and mask shift keys */
    m += (ktab[7] & 0x40) ? 1 : 0;
    ktab[7] &= ~0x40;
    m <<= 1;
    m += (ktab[4] & 0x40) ? 1 : 0;
    ktab[4] &= ~0x40;
    m <<= 1;
    m += (ktab[3] & 0x40) ? 1 : 0;
    ktab[3] &= ~0x40;
    /* find new char code, if any */
    for (i = 0; i < 8; i++) {
	b = (ktab[i] ^ ktab1[i]) & ktab[i];
	for (j = 0; j < 7; j++) {
	    if (b & 1) {
		key = i * 7 + j + 1;
		meta = m;
		return;
	    }
	    b = b >> 1;
	}
    }
}

/* is there a key waiting? */
int kbhit(void) 
{
    return key;
}

/* fixme: this is supposed to return ascii not keycode */
unsigned char cgetc(void)
{
    unsigned char k = key;
    key = 0;
    if (meta & 4){
	TTYChar(satab[k-1]);
	return satab[k-1];
    }
    else {
	TTYChar(atab[k-1]);
	return atab[k-1];
    }
}

void handle_key(unsigned char);

void platform_handle_keyboard(void)
{
    int k;

    if(key){
	k = key;
	key = 0;
	if (meta == 0)
	    handle_key(knone[k-1]);
	else if (meta == 4)
	    handle_key(kshift[k-1]);
	else if (meta == 2)
	    handle_key(kcntl[k-1]);
	else if (meta == 6)
	    handle_key(kshcntl[k-1]);
    }
    return;
}
