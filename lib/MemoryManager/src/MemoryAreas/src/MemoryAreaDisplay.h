#include "../MemoryAreaTemplate.h"

#ifndef MEMORY_AREA_DISPLAY_GUARD
#define MEMORY_AREA_DISPLAY_GUARD

class MemoryAreaDisplay: public MemoryAreaTemplate{
    public:
    esp_err_t Initialize(void) override;
};

#endif /* MEMORY_AREA_DISPLAY_GUARD */