/*
 * Code to actually cause a blinking action
 */

#include "led.h"

typedef struct {
    unsigned int* entry_loc;        /* Entry point for user application */
    unsigned int* init_data_loc;    /* Data initialization information in flash */
    unsigned int init_data_size;    /* Size of initialization information */
    unsigned int got_start_offset;  /* Offset to start of GOT */
    unsigned int got_end_offset;    /* Offset to end of GOT */
    unsigned int plt_start_offset;  /* Offset to start of PLT */
    unsigned int plt_end_offset;    /* Offset to end of PLT */
    unsigned int bss_start_offset;  /* Offset to start of BSS */
    unsigned int bss_end_offset;    /* Offset to end of BSS */
} Load_Info;

extern void main(void);
extern unsigned int* _etext;
extern unsigned int* _edata;
extern unsigned int* _got;
extern unsigned int* _egot;
extern unsigned int* _plt;
extern unsigned int* _eplt;
extern unsigned int* _bss;
extern unsigned int* _ebss;

// Load Info is used by the runtime in order to load the application
//  Note that locations in the text section assume .text starts at 0x10000000
//  and are therefore updated
__attribute__ ((section(".load_info"), used))
Load_Info app_info = {
    .entry_loc          = (unsigned int*)((unsigned int)main - 0x10000000),
    .init_data_loc      = (unsigned int*)((unsigned int)(&_etext) - 0x10000000),
    .init_data_size     = (unsigned int)(&_edata),
    .got_start_offset   = (unsigned int)(&_got),
    .got_end_offset     = (unsigned int)(&_egot),
    .plt_start_offset   = (unsigned int)(&_plt),
    .plt_end_offset     = (unsigned int)(&_eplt),
    .bss_start_offset   = (unsigned int)(&_bss),
    .bss_end_offset     = (unsigned int)(&_ebss),
};

/* Pin definitions for various cc2538 systems */
#define ATUM_RED_LED   3
#define ATUM_BLUE_LED  4
#define ATUM_GREEN_LED 5

#define SDL_RED_LED    1
#define SDL_GREEN_LED  0
#define SDL_BIG_LED    5

// global variables get placed in .data and are referenced in GOT
static unsigned int BLUE_LED = ATUM_BLUE_LED;
static unsigned int GREEN_LED = ATUM_GREEN_LED;
// unitialized global varaibles get placed in .bss and are referenced in GOT
static unsigned int RED_LED;
static unsigned int TIME_DELAY;

void main () {
    // Initialize variables
    RED_LED = ATUM_RED_LED;
    TIME_DELAY = 400000;
    
    init_led(RED_LED);
    init_led(BLUE_LED);
    init_led(GREEN_LED);

    while(1) {

        //led_off(RED_LED);
        led_off(BLUE_LED);
        //led_on(GREEN_LED);
        for (volatile unsigned int i=0; i<TIME_DELAY; i++);

        //led_on(RED_LED);
        led_on(BLUE_LED);
        //led_off(GREEN_LED);
        for (volatile unsigned int i=0; i<TIME_DELAY; i++);
    }
}

