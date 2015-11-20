
#ifndef LED_H
#define LED_H

/* GPIO pin definitions */
#define GPIO_C_BASE  0x400DB000  // GPIO C base address
#define GPIO_D_BASE  0x400DC000  // GPIO D base address
#define GPIO_DIR     0x00000400  // Direction offset
#define GPIO_DATA    0x00000000  // Data offset
#define ATUM_LEDS_BASE GPIO_D_BASE
#define SDL_LEDS_BASE  GPIO_C_BASE

// Function prototypes
void init_led(unsigned char led_num);
void led_on(unsigned char led_num);
void led_off(unsigned char led_num);

#endif /* LED_H */

