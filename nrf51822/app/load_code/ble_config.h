#ifndef __BLE_CONFIG_H
#define __BLE_CONFIG_H

typedef struct ble_app_s {
    uint16_t                    service_handle;
    ble_gatts_char_handles_t    char_upload_binblob_handle;
    uint32_t                     upload_bin_blob_buffer[125]; // Buffer to store raw binary blobs of applications to be loaded
    uint16_t                    upload_bin_blob_length; // Length of blob received
    bool                        upload_bin_blob_flag; // Indicates new blob received
    ble_gatts_char_handles_t    char_upload_loadcode_handle;
    bool                        upload_load_code_flag; // Flag to indicate when code is ready to be loaded
} ble_app_t;

#endif
