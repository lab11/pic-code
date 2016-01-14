#ifndef __BLE_CONFIG_H
#define __BLE_CONFIG_H

#include "simple_ble.h"

simple_ble_service_t upload_service_handle = {
    .uuid128 = {{0x73, 0xd7, 0x5e, 0x49, 0x0c, 0x60, 0x47, 0x72,
                  0x8f, 0x5d, 0xb1, 0xc3, 0x78, 0x78, 0x91, 0x89}}
};
#define UPLOAD_CHAR_BINBLOB_SHORT_UUID  0x0C61
#define UPLOAD_CHAR_LOADCODE_SHORT_UUID 0x0C62

typedef struct ble_app_s {
    simple_ble_char_t       char_upload_binblob_handle;
    uint32_t                upload_bin_blob_buffer[125]; // Buffer to store raw binary blobs of applications to be loaded
    uint16_t                upload_bin_blob_length; // Length of blob received
    bool                    upload_bin_blob_flag; // Indicates new blob received
    simple_ble_char_t       char_upload_loadcode_handle;
    bool                    upload_load_code_flag; // Flag to indicate when code is ready to be loaded
} ble_app_t;

#endif
