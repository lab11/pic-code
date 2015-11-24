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

#include "ble_config.h"
#include "squall.h"
#include "led.h"

#include "simple_ble.h"
#include "simple_adv.h"

static ble_app_t app;

// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
    .platform_id       = 0x40,              // used as 4th octect in device BLE address
    .device_id         = DEVICE_ID_DEFAULT,
    .adv_name          = DEVICE_NAME,       // used in advertisements if there is room
    .adv_interval      = MSEC_TO_UNITS(500, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(10, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(10, UNIT_1_25_MS)
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
//extern unsigned char* _eapps;

void load_app (unsigned char* app_start) {
    /* Perform application load functions */
    //Load_Info* code_info = (Load_Info*)&_apps;
    //unsigned int* flash_location = (unsigned int*)&_apps;
    Load_Info* code_info = (Load_Info*)app_start;
    unsigned int* flash_location = (unsigned int*)app_start;
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

// Randomly generated UUID
const ble_uuid128_t upload_uuid128 = {
    {0x73, 0xd7, 0x5e, 0x49, 0x0c, 0x60, 0x47, 0x72,
     0x8f, 0x5d, 0xb1, 0xc3, 0x78, 0x78, 0x91, 0x89}
};

ble_uuid_t upload_uuid;

#define UPLOAD_SHORT_UUID               0x0C60
#define UPLOAD_CHAR_BINBLOB_SHORT_UUID  0x0C61
#define UPLOAD_CHAR_LOADCODE_SHORT_UUID 0x0C62

void services_init (void) {

    // Add main app upload service
    app.service_handle = simple_ble_add_service(&upload_uuid128,
                                                &upload_uuid,
                                                UPLOAD_SHORT_UUID);

    // Add the characteristic to write data blobs
    simple_ble_add_characteristic(0, 1, 0, // read, write, notify
                                  upload_uuid.type,
                                  UPLOAD_CHAR_BINBLOB_SHORT_UUID,
                                  500, (uint8_t*)app.upload_bin_blob_buffer,
                                  app.service_handle,
                                  &app.char_upload_binblob_handle);

    // Add the characteristic to notify with ACKs
    //TODO

    // Add the characteristic to say app is complete, load it
    simple_ble_add_characteristic(0, 1, 0, // read, write, notify
                                  upload_uuid.type,
                                  UPLOAD_CHAR_LOADCODE_SHORT_UUID,
                                  1, (uint8_t*)&app.upload_load_code_flag,
                                  app.service_handle,
                                  &app.char_upload_loadcode_handle);

    // Add the characteristic for app loading status
    //TODO

}

static void writeWord(uint32_t* address, uint32_t value) {

    // enable writing to flash
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy); // spin until ready to write

    // write word to flash
    *address = value;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy); // spin until written

    // Turn off flash write enable and wait until the NVMC is ready
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy); // spin until ready to read
}

// text segment goes up to about 0x00020000, Flash goes to 0x00040000
static uint32_t* app_blob_addr = (uint32_t*)0x00030000;

void ble_evt_write (ble_evt_t* p_ble_evt) {
    ble_gatts_evt_write_t* p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_evt_write->handle == app.char_upload_binblob_handle.value_handle) {
        // binary blob with new app code
        //app.upload_bin_blob_buffer = p_evt_write->data;
        memcpy((uint8_t*)app.upload_bin_blob_buffer, p_evt_write->data, p_evt_write->len);
        app.upload_bin_blob_length = p_evt_write->len;
        app.upload_bin_blob_flag   = true;

    } else if (p_evt_write->handle == app.char_upload_loadcode_handle.value_handle) {
        // load code from previously received binary blobs
        if (p_evt_write->data[0] != 0) {
            app.upload_load_code_flag = true;
        } else {
            app.upload_load_code_flag = false;
        }

    }
}

void ble_error(uint32_t error_code) {
    // app_error_handler was called
    led_on(LED_0);
}


int main (void) {
    led_init(LED_0);
    led_off(LED_0);

    // initialize app
    app.upload_load_code_flag = false;
    app.upload_bin_blob_flag = false;
    memset((uint8_t*)app.upload_bin_blob_buffer, 0x00, 500);

    // Setup BLE
    simple_ble_init(&ble_config);

    // Advertise because why not
    simple_adv_only_name();

    while (1) {
        power_manage();

        if (app.upload_load_code_flag) {
            // load code!
            load_app((unsigned int*)0x00030000);
            //load_app((unsigned char*)&_apps);
        }

        if (app.upload_bin_blob_flag) {
            app.upload_bin_blob_flag = false;

            // write code to flash
            for (uint16_t i=0; i<(app.upload_bin_blob_length/4); i++) {
                writeWord(app_blob_addr++, (app.upload_bin_blob_buffer)[i]);
            }
        }
    }
}

