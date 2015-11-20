
#include "led.h"

// global variables get placed in .data and are refereneced in GOT
static unsigned int LED_BASE = ATUM_LEDS_BASE;

void init_led (unsigned char led_num) {
    *((volatile unsigned int*)(LED_BASE | GPIO_DIR)) |= (1 << led_num); // Sets the LED pin to be an output
    return;
}

void led_on (unsigned char led_num) {
    *((volatile unsigned int*)(((LED_BASE) | GPIO_DATA) + ((1 << led_num) << 2))) = 0x00; // LED on
    return;
}

void led_off (unsigned char led_num) {
    *((volatile unsigned int*)(((LED_BASE) | GPIO_DATA) + ((1 << led_num) << 2))) = 0xFF; // LED off
    return;
}

