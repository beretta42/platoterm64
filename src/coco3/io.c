/**
 * PLATOTerm64 - A PLATO Terminal for the Commodore 64
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * io.h - Input/output functions (serial/ethernet)
 */

#include <stdint.h>
#include "../io.h"
#include "../protocol.h"

void ser_open( void );
void ser_close( void );
char ser_put(char c);
char ser_put_clean(char c);
char ser_get(char *c);

static uint8_t ch = 0;

/**
 * io_init() - Set-up the I/O
 */
void io_init(void){
    ser_open();
    TTY = -1;
    /*    char *str = "tcp connect irata.online 8005\r\n";
    while (*str)
	ser_put_clean(*str++);
    */
};

/**
 * io_open() - Open the device
 */
void io_open(void){
};

/**
 * io_send_byte(b) - Send specified byte out
 */
void io_send_byte(uint8_t b){
    ser_put(b);
};

/**
 * io_main() - The IO main loop
 */
void io_main(void){
    io_recv_serial();
};

/**
 * io_recv_serial() - Receive and interpret serial data.
 */
void io_recv_serial(void){
    if (!ser_get(&ch)) ShowPLATO(&ch,1);
};

/**
 * io_done() - Called to close I/O
 */
void io_done(void){
    ser_close();
};

