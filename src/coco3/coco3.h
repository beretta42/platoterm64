#define true (-1)
#define false (0)
#define TGI_COLOR_WHITE (0xff)
#define TGI_COLOR_BLACK (0x00)
#define TGI_COLOR_ORANGE (0xff)
#define NULL (0)
#define SER_ERR_OK 0
void tgi_setpixel (int x, int y);
void tgi_init (void);
void tgi_line (int x1, int y1, int x2, int y2);
void tgi_clear (void);
void tgi_setcolor(unsigned char color);
void tgi_bar(int x1, int y1, int x2, int y2);


char ser_put(char c);
char ser_put_clean(char c);
char ser_get(char *c);
void ser_open(void);
void memset(char *s, int c, int n);
int kbhit(void);
unsigned char cgetc(void);

void platform_scroll_up(void);
