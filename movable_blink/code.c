/*
 * Code to actually cause a blinking action
 */

/* GPIO pin definitions */
#define GPIO_C_BASE  0x400DB000  // GPIO C base address
#define GPIO_D_BASE  0x400DC000  // GPIO D base address
#define GPIO_DIR     0x00000400  // Direction offset
#define GPIO_DATA    0x00000000  // Data offset

/* Pin definitions for various cc2538 systems */
#define ATUM_LEDS_BASE GPIO_D_BASE
#define ATUM_RED_LED   3
#define ATUM_BLUE_LED  4
#define ATUM_GREEN_LED 5

#define SDL_LEDS_BASE  GPIO_C_BASE
#define SDL_RED_LED    1
#define SDL_GREEN_LED  0
#define SDL_BIG_LED    5


void func () {
    // Select which LED to blink
    const unsigned int LED_BASE = ATUM_LEDS_BASE;
    
    *((volatile unsigned int*)(LED_BASE | GPIO_DIR)) |= (1 << ATUM_RED_LED); // Sets the LED pin to be an output
    *((volatile unsigned int*)(LED_BASE | GPIO_DIR)) |= (1 << ATUM_BLUE_LED); // Sets the LED pin to be an output
    *((volatile unsigned int*)(LED_BASE | GPIO_DIR)) |= (1 << ATUM_GREEN_LED); // Sets the LED pin to be an output

    while(1) {
        volatile int i;

        *((volatile unsigned int*)(((LED_BASE) | GPIO_DATA) + ((1 << ATUM_RED_LED) << 2))) = 0x00; // LED on
        *((volatile unsigned int*)(((LED_BASE) | GPIO_DATA) + ((1 << ATUM_BLUE_LED) << 2))) = 0x00; // LED on
        *((volatile unsigned int*)(((LED_BASE) | GPIO_DATA) + ((1 << ATUM_GREEN_LED) << 2))) = 0x00; // LED on
        for (i=0; i<400000; i++);

        *((volatile unsigned int*)(((LED_BASE) | GPIO_DATA) + ((1 << ATUM_RED_LED) << 2))) = 0xFF; // LED off
        *((volatile unsigned int*)(((LED_BASE) | GPIO_DATA) + ((1 << ATUM_BLUE_LED) << 2))) = 0xFF; // LED off
        *((volatile unsigned int*)(((LED_BASE) | GPIO_DATA) + ((1 << ATUM_GREEN_LED) << 2))) = 0xFF; // LED off
        for (i=0; i<400000; i++);
    }
}

