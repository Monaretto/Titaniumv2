#include "MemoryAreaPassword.h"

/**
 * Initializes the MemoryAreaPassword object.
 *
 * @returns ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t MemoryAreaPassword::Initialize(void){

    this->index = PASSWORD_AREA;
    this->data = new uint8_t[64];
    this->size = 64;
    this->access_type = READ_WRITE;
    this->cached_time = 0;
    this->has_update = false;
    this->mutex_ = xSemaphoreCreateMutex();
    memset_s(this->data, 0, this->size);

    //Workaround
    uint8_t default_value[] = "NPQ196253";
    memcpy_s(this->data, default_value, sizeof(default_value));

    return ESP_OK;
}