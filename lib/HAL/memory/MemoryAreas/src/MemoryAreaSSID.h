#include "../MemoryAreaTemplate.h"

#ifndef MEMORY_SSID_AREA_GUARD
#define MEMORY_SSID_AREA_GUARD    

class MemoryAreaSSID: public MemoryAreaTemplate {
public:
    esp_err_t Initialize(void) override {
        this->index = SSID_AREA;
        this->data = new uint8_t[32];
        this->size = 32;
        this->access_type = READ_WRITE;
        this->cached_time = 0;
        this->has_update = false;
        this->mutex_ = xSemaphoreCreateMutex();
        memset_s(this->data, 0, this->size);

        return ESP_OK;
    }
};

#endif /* MEMORY_SSID_AREA_GUARD  */