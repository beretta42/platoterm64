/**
 * PLATOTerm64 - A PLATO Terminal for the Commodore 64
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * screen.h - Display output functions
 */

#include "../screen.h"
#include "../scale.h"
#include "../font.h"

extern padBool FastText;

uint8_t CharWide = 5;
uint8_t CharHigh = 8;
padPt TTYLoc;

#define screen ((unsigned char *)0x6200)
#define init0 *((unsigned char *)0xff90)
#define video *((unsigned char *)0xff98)
#define res   *((unsigned char *)0xff99)
#define vscr  *((unsigned char *)0xff9c)
#define oreg  *((unsigned int  *)0xff9d)
#define pal   ((unsigned char *)0xffb0)
#define border *((unsigned char *)0xff9a)

unsigned char color = 0xff;

/* absolute value */
int abs(int x){
    if (x >= 0)
	return x;
    else
	return -x;
}

/* set a pixel with color */
void set(int x, int y){
    unsigned char mask;
    int off;
    
    mask = 0x80 >> (x & 7);
    off = y * 40 + (x >> 3);
    /* for 1 bpp this works, but should be
       more like: screen = (screen & ~mask) | (color & mask) */
    if (color){
	screen[off] |=  mask;
    }
    else 
	screen[off] &= ~mask;
}

/* draw a line */
void line(int x1, int y1, int x2, int y2){
    int dx, dy; 
    int stepx, stepy;
    dx = x2 - x1;
    dy = y2 - y1;
    if (dx < 0) { 
	dx = -dx;
	stepx = -1;
    } else { 
	stepx = 1; 
    } 
    if (dy < 0)  { 
	dy = -dy;
	stepy = -1;
    } else {
	stepy = 1;
    } 
    dx <<= 1; 
    dy <<= 1;
    // draw first point 
    set(x1, y1);
    //check for shallow line
    if (dx > dy) { 
	int fraction = dy - (dx >> 1); 
	while (x1 != x2) { 
	    if (fraction >= 0) {
		y1 += stepy;
		fraction -= dx;
	    } 
	    x1 += stepx;
	    fraction += dy;
	    set(x1, y1);
	}
    } //otherwise steep line 
    else { 
	int fraction = dx - (dy >> 1);
	while (y1 != y2) {
	    if (fraction >= 0) {
		x1 += stepx;
		fraction -= dy;
	    } 
	    y1 += stepy; 
	    fraction += dx; 
	    set(x1, y1);
	}
    }
}

/* draw a bar */
void bar( int x1, int y1, int x2, int y2 ){
    int d;
    int h;
    if (y1 < y2){
	d = 1;
	h = y2 - y1;
    } else {
	d = -1;
	h = y1 - y2;
    }
    do {
	line(x2,y1,x1,y1);
	y1 += d;
    } while (h--);
}


/* set color based on mode */
void set_color_mode( void ){
    if (CurMode == ModeErase || CurMode == ModeInverse)
	color = 0x00;
    else
	color = 0xff;
}

/**
 * screen_init() - Set up the screen
 */
void screen_init(void){    
    init0 = 0x4c;
    video = 0x80;
    res = 0x0c;
    vscr = 0;
    oreg = 0xec40;
    pal[0] = 0x00;
    pal[1] = 0xff;
    border = 0x88;
};

/**
 * screen_cycle_foreground()
 * Go to the next foreground color in palette
 */
void screen_cycle_foreground(void){
};

/**
 * screen_cycle_background()
 * Go to the next background color in palette
 */
void screen_cycle_background(void){
};

/**
 * screen_cycle_border()
 * Go to the next border color in palette
 */
void screen_cycle_border(void){
};

/**
 * screen_update_colors() - Set the terminal colors
 */
void screen_update_colors(void){
};

/**
 * screen_wait(void) - Sleep for approx 16.67ms
 */
void screen_wait(void){
};

/**
 * screen_beep(void) - Beep the terminal
 */
void screen_beep(void){
};

/**
 * screen_clear - Clear the screen
 */
void screen_clear(void){
    int x = 7680;
    unsigned char *p = screen;
    while(x--)
	*p++ = 0;
};

/**
 * screen_block_draw(Coord1, Coord2) - Perform a block fill from Coord1 to Coord2
 */
void screen_block_draw(padPt* Coord1, padPt* Coord2){
    set_color_mode();
    bar(scalex[Coord1->x],scaley[Coord1->y],
	scalex[Coord2->x],scaley[Coord2->y]);
};

/**
 * screen_dot_draw(Coord) - Plot a mode 0 pixel
 */
void screen_dot_draw(padPt* Coord){
    set_color_mode();
    set(scalex[Coord->x],scaley[Coord->y]);
};

/**
 * screen_line_draw(Coord1, Coord2) - Draw a mode 1 line
 */
void screen_line_draw(padPt* Coord1, padPt* Coord2){
    set_color_mode();
    line(scalex[Coord1->x], scaley[Coord1->y],
	 scalex[Coord2->x], scaley[Coord2->y]);
};

/**
 * screen_char_draw(Coord, ch, count) - Output buffer from ch* of length count as PLATO characters
 */
void screen_char_draw(padPt* Coord, unsigned char* ch, unsigned char count){
    int16_t offset; /* due to negative offsets */
    uint16_t x;      /* Current X and Y coordinates */
    uint16_t y;
    uint16_t* px;   /* Pointers to X and Y coordinates used for actual plotting */
    uint16_t* py;
    uint8_t i; /* current character counter */
    uint8_t a; /* current character byte */
    int8_t b; /* current character row bit signed */
    uint8_t width=5;
    uint8_t height=6;
    uint16_t deltaX=1;
    uint16_t deltaY=1;
    uint8_t mainColor=0xff;
    uint8_t altColor=0x00;
    uint8_t *p;
  
  switch(CurMem)
    {
    case M0:
      offset=-32;
      break;
    case M1:
      offset=64;
      break;
    case M2:
      /* TODO: custom charsets */
      break;
    case M3:
      /* TODO: custom charsets */
      break;
    }

  if (CurMode==ModeRewrite)
    {
      altColor=0x00;
    }
  else if (CurMode==ModeInverse)
    {
      altColor=0xff;
    }
  
  if (CurMode==ModeErase || CurMode==ModeInverse)
    mainColor=0x00;
  else
    mainColor=0xff;

  color = mainColor;

  x=scalex[(Coord->x&0x1FF)];
  y=scaley[(Coord->y)+14&0x1FF];
  
  if (FastText==padF)
    {
      goto chardraw_with_fries;
    }

  y=scaley[(Coord->y)+14&0x1FF];

  /* the diet chardraw routine - fast text output. */
  
  for (i=0;i<count;++i)
    {
      a=*ch;
      ++ch;
      a+=offset;
      p=&font[fontptr[a]];

      // Line 1
      b=*p;

      if (b<0)
	set(x,y);
      ++x;
      b<<=1;

      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;

      ++y;
      x-=width;
      ++p;

      // Line 2
      b=*p;
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;

      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;

      ++y;
      x-=width;
      ++p;

      // Line 3
      b=*p;
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;

      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;

      ++y;
      x-=width;
      ++p;

      // Line 4
      b=*p;
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;

      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;

      ++y;
      x-=width;
      ++p;

      // Line 5
      b=*p;
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;

      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;

      ++y;
      x-=width;
      ++p;

      // Line 6
      b=*p;
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;

      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;
      
      if (b<0)
	set(x,y);
      ++x;
      b<<=1;

      ++y;
      x-=width;
      ++p;
      
      x+=width;
      y-=height;
    }

  return;
  
 chardraw_with_fries:
  if (Rotate)
    {
      deltaX=-abs(deltaX);
      width=-abs(width);
      px=&y;
      py=&x;
    }
    else
    {
      px=&x;
      py=&y;
    }
  
  if (ModeBold)
    {
      deltaX = deltaY = 2;
      width<<=1;
      height<<=1;
      goto bold_chardraw_with_fries;
    }
  
  for (i=0;i<count;++i)
    {
      a=*ch;
      ++ch;
      a+=offset;
      p=&font[fontptr[a]];

      color = altColor;
      bar(x,y,x+width,y+height); // Take care of all the zero bits at once.
      color = mainColor;

      // Line 1
      b=*p;

      if (b<0)
	{
	  set(x,y);
	}
      ++x;
      b<<=1;

      if (b<0)
	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;

      ++y;
      x-=width;
      ++p;

      // Line 2
      b=*p;
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;

      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;

      ++y;
      x-=width;
      ++p;

      // Line 3
      b=*p;
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;

      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;

      ++y;
      x-=width;
      ++p;

      // Line 4
      b=*p;
      if (b<0)
	{
	  set(x,y);
	}
      ++x;
      b<<=1;

      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;

      ++y;
      x-=width;
      ++p;

      // Line 5
      b=*p;
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;

      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;

      ++y;
      x-=width;
      ++p;

      // Line 6
      b=*p;
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;

      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	}
      ++x;
      b<<=1;

      ++y;
      x-=width;
      ++p;

      x+=width;
      y-=height;
    }

  return; // Done with chardraw_with_fries
  
 bold_chardraw_with_fries:
  for (i=0;i<count;++i)
    {
      a=*ch;
      ++ch;
      a+=offset;
      p=&font[fontptr[a]];

      color = altColor;
      bar(x,y,x+width,y+height); // Take care of all the zero bits at once.
      color = mainColor;

      // Line 1
      b=*p;

      if (b<0)
	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;

      if (b<0)
	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;

      y+=deltaY;
      x-=width;
      ++p;

      // Line 2
      b=*p;
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;

      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;

      y+=deltaY;
      x-=width;
      ++p;

      // Line 3
      b=*p;
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;

      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;

      y+=deltaY;
      x-=width;
      ++p;

      // Line 4
      b=*p;
      if (b<0)
	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;

      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;

      y+=deltaY;
      x-=width;
      ++p;

      // Line 5
      b=*p;
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;

      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;

      y+=deltaY;
      x-=width;
      ++p;

      // Line 6
      b=*p;
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;

      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;
      
      if (b<0)
      	{
	  set(x,y);
	  set(*px+1,*py);
	  set(*px,*py+1);
	  set(*px+1,*py+1);
	}
      x+=deltaX;
      b<<=1;

      y+=deltaY;
      x-=width;
      ++p;
      
      /* for (j=0;j<FONT_SIZE_Y;++j) */
      /* 	{ */
      /* 	  b=*p; */
	  
      /* 	  for (k=0;k<FONT_SIZE_X;++k) */
      /* 	    { */
      /* 	      if (b<0) /\* check sign bit. *\/ */
      /* 		{ */
      /* 		  if (ModeBold) */
      /* 		    { */
      /* 		      set(*px+1,*py); */
      /* 		      set(*px,*py+1); */
      /* 		      set(*px+1,*py+1); */
      /* 		    } */
      /* 		  set(*px,*py); */
      /* 		} */

      /* 	      x += deltaX; */
      /* 	      b<<=1; */
      /* 	    } */

      /* 	  y+=deltaY; */
      /* 	  x-=width; */
      /* 	  ++p; */
      /* 	} */

      x+=width;
      y-=height;
    }

  return;
};

/**
 * screen_tty_char - Called to plot chars when in tty mode
 */
void screen_tty_char(padByte theChar){
    if ((theChar >= 0x20) && (theChar < 0x7F)) {
	screen_char_draw(&TTYLoc, &theChar, 1);
	TTYLoc.x += CharWide;
    }
    else if ((theChar == 0x0b)) /* Vertical Tab */
	{
	    TTYLoc.y += CharHigh;
	}
    else if ((theChar == 0x08) && (TTYLoc.x > 7))	/* backspace */
	{
	    TTYLoc.x -= CharWide;
	    color = 0x00;
	    bar(scalex[TTYLoc.x],scaley[TTYLoc.y],scalex[TTYLoc.x+CharWide],scaley[TTYLoc.y+CharHigh]);
	    color = 0xff;
	}
    else if (theChar == 0x0A)			/* line feed */
	TTYLoc.y -= CharHigh;
    else if (theChar == 0x0D)			/* carriage return */
	TTYLoc.x = 0;
    
    if (TTYLoc.x + CharWide > 511) {	/* wrap at right side */
	TTYLoc.x = 0;
	TTYLoc.y -= CharHigh;
    }
    
    if (TTYLoc.y < 0) {
	screen_clear();
	TTYLoc.y=495;
    }
};

/**
 * screen_done()
 */
void screen_done(void){
};


