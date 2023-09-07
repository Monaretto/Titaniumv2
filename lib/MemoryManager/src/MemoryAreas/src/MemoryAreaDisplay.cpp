#include "MemoryAreaDisplay.h"

/**
 * Initializes the MemoryAreaDisplay object.
 *
 * @returns ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t MemoryAreaDisplay::Initialize(void){

    this->index = DISPLAY_AREA;
    this->data = new uint8_t[1024];
    this->size = 1024;
    this->access_type = READ_WRITE;
    this->cached_time = 0;
    this->has_update = false;
    this->mutex_ = xSemaphoreCreateMutex();
    memset_s(this->data, 0, this->size);

    return ESP_OK;
}