#include "MemoryAreaSSID.h"

/**
 * Initializes the MemoryAreaSSID object.
 *
 * @returns ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t MemoryAreaSSID::Initialize(void){

    this->index = SSID_AREA;
    this->data = new uint8_t[32];
    this->size = 32;
    this->access_type = READ_WRITE;
    this->cached_time = 0;
    this->has_update = false;
    this->mutex_ = xSemaphoreCreateMutex();
    memset_s(this->data, 0, this->size);

    //Workaround
    uint8_t default_value[] = "NETPARQUE_PAOLA";
    memcpy_s(this->data, default_value, sizeof(default_value));

    return ESP_OK;
}