#include "../MemoryAreaTemplate.h"

#ifndef MEMORY_AREA_CONNECTION_GUARD
#define MEMORY_AREA_CONNECTION_GUARD

class MemoryAreaConnection: public MemoryAreaTemplate{
    public:
    esp_err_t Initialize(void) override;
};

#endif /* MEMORY_AREA_CONNECTION_GUARD */