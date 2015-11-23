/*
 * Code to actually cause a blinking action
 */

#include "squall.h"
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

//static uint32_t time_delay = 40000;

void main () {
    led_init(LED_0);
    const uint32_t time_delay = 640000;

    while (1) {

        led_on(LED_0);
        for (volatile int i=0; i<time_delay; i++);

        led_off(LED_0);
        for (volatile int i=0; i<time_delay; i++);
    }
}


/*
void led_on (unsigned int, unsigned char);
void led_off (unsigned int, unsigned char);

// global variables get placed in .data and are referenced in GOT
unsigned int LED_BASE = ATUM_LEDS_BASE;
unsigned int BLUE_LED = ATUM_BLUE_LED;
unsigned int GREEN_LED = ATUM_GREEN_LED;
// unitialized global varaibles get placed in .bss and are referenced in GOT
unsigned int RED_LED;
unsigned int TIME_DELAY;

void led_on (unsigned int led_base, unsigned char led_num) {
    *((volatile unsigned int*)(((led_base) | GPIO_DATA) + ((1 << led_num) << 2))) = 0x00; // LED on

    return;
}

void led_off (unsigned int led_base, unsigned char led_num) {
    *((volatile unsigned int*)(((led_base) | GPIO_DATA) + ((1 << led_num) << 2))) = 0xFF; // LED off

    return;
}

void main () {
    // Initialize variables
    RED_LED = ATUM_RED_LED;
    TIME_DELAY = 400000;
    
    *((volatile unsigned int*)(LED_BASE | GPIO_DIR)) |= (1 << RED_LED); // Sets the LED pin to be an output
    *((volatile unsigned int*)(LED_BASE | GPIO_DIR)) |= (1 << BLUE_LED); // Sets the LED pin to be an output
    *((volatile unsigned int*)(LED_BASE | GPIO_DIR)) |= (1 << GREEN_LED); // Sets the LED pin to be an output

    while(1) {

        led_off(LED_BASE, RED_LED);
        //led_off(LED_BASE, BLUE_LED);
        //led_on(LED_BASE, GREEN_LED);
        for (volatile unsigned int i=0; i<TIME_DELAY; i++);

        led_on(LED_BASE, RED_LED);
        //led_on(LED_BASE, BLUE_LED);
        //led_off(LED_BASE, GREEN_LED);
        for (volatile unsigned int i=0; i<TIME_DELAY; i++);
    }
}
*/

