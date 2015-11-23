/*
 * Send an advertisement periodically
 */

#include <stdbool.h>
#include <stdint.h>

#include "nrf_gpio.h"
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "ble_debug_assert_handler.h"

#include "squall.h"
#include "led.h"

#include "simple_ble.h"
#include "simple_adv.h"


// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
    .platform_id       = 0x40,              // used as 4th octect in device BLE address
    .device_id         = DEVICE_ID_DEFAULT,
    .adv_name          = DEVICE_NAME,       // used in advertisements if there is room
    .adv_interval      = MSEC_TO_UNITS(500, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS)
};

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

extern unsigned char* _apps;
extern unsigned char* _eapps;

void load_app(void) {
    /* Perform application load functions */
    Load_Info* code_info = (Load_Info*)&_apps;
    unsigned int* flash_location = (unsigned int*)&_apps;
    unsigned int* sram_location = (unsigned int*)0x20003000; // After main .data & .bss, before Stack
    // copy data into data section
    //  Data start location assumes .text starts at address 0, and needs to be
    //  updated to the actual location in flash
    unsigned int* init_data_src = (unsigned int*)((unsigned int)(*code_info).init_data_loc + (unsigned int)flash_location);
    unsigned int* init_data_end = (unsigned int*)((unsigned int)init_data_src + (*code_info).init_data_size);
    unsigned int* data_dst = sram_location;
    while (init_data_src < init_data_end) {
        *data_dst++ = *init_data_src++;
    }
    // zero out bss section
    unsigned int* bss_start = (unsigned int*)((unsigned int)sram_location + (*code_info).bss_start_offset);
    unsigned int bss_size = (*code_info).bss_end_offset - (*code_info).bss_start_offset;
    unsigned int* bss_end   = (unsigned int*)((unsigned int)bss_start + bss_size);
    while (bss_start < bss_end) {
        *bss_start++ = 0;
    }
    // fixup GOT
    unsigned int* got_start = (unsigned int*)((unsigned int)sram_location + (*code_info).got_start_offset);
    unsigned int got_size = (*code_info).got_end_offset - (*code_info).got_start_offset;
    unsigned int* got_end   = (unsigned int*)((unsigned int)got_start + got_size);
    while (got_start < got_end) {
        *got_start++ += (unsigned int)sram_location;
    }
    //TODO: fixup PLT to enable shared libraries


    /* Context Switch */
    // set base register (r6)
    //  Needs to point to the beginning of the GOT section, which is at the
    //  start of the SRAM for the data section.
    //  Hard coded to sram_location for now
    __asm(" ldr     r6,=0x20003000      \n"
          );
    //TODO: set stack pointer
    //TODO: move to user mode
    // Jump into application
    //  Entry location assumes .text starts at address 0, and needs to be
    //  updated to the actual location in flash
    unsigned int jmp = ((unsigned int)(*code_info).entry_loc + (unsigned int)flash_location);
    void (*fn)(void) = (void (*)(void))(((unsigned int)jmp) | 1); // |1 is very important!! Must stay in thumb mode
    fn();

    // never reached
}

int main(void) {
    led_init(LED_0);

    // Setup BLE
    simple_ble_init(&ble_config);

    // Advertise because why not
    simple_adv_only_name();

    led_on(LED_0);

    load_app();

    // never reached
    led_off(LED_0);

/*
    while (1) {
        led_on(LED_0);
        for (volatile int i=0; i<40000; i++);

        led_off(LED_0);
        for (volatile int i=0; i<40000; i++);
    }
*/

    while (1) {
        power_manage();
    }

}

