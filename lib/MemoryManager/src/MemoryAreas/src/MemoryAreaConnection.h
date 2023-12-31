#include "../MemoryAreaTemplate.h"

#ifndef MEMORY_CONNECTION_AREA_GUARD
#define MEMORY_CONNECTION_AREA_GUARD    

class MemoryAreaConnection: public MemoryAreaTemplate {
public:
    esp_err_t Initialize(void) override {
        this->index = CONNECTION_AREA;
        this->data = new uint8_t[1];
        this->size = 1;
        this->access_type = READ_WRITE;
        this->cached_time = 0;
        this->has_update = false;
        this->mutex_ = xSemaphoreCreateMutex();
        memset_s(this->data, 0, this->size);

        return ESP_OK;
    }
};

#endif /* MEMORY_CONNECTION_AREA_GUARD  */