/**
 * PLATOTerm64 - A PLATO Terminal for the Commodore 64
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * keyboard.h - Keyboard functions
 */

#include <stdint.h>
#include "../keyboard.h"


/**
 * keyboard_out - If platoKey < 0x7f, pass off to protocol
 * directly. Otherwise, platoKey is an access key, and the
 * ACCESS key must be sent, followed by the particular
 * access key from PTAT_ACCESS.
 */
void keyboard_out(uint8_t platoKey){
};

/**
 * keyboard_main - Handle the keyboard presses
 */
void keyboard_main(void){
};

/**
 * keyboard_clear() - Clear the keyboard buffer
 */
void keyboard_clear(void){
};


