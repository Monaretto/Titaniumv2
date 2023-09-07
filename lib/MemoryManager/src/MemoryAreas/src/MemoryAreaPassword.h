#include "../MemoryAreaTemplate.h"

#ifndef MEMORY_AREA_PASSWORD_GUARD
#define MEMORY_AREA_PASSWORD_GUARD

class MemoryAreaPassword: public MemoryAreaTemplate{
    public:
    esp_err_t Initialize(void) override;
};

#endif /* MEMORY_AREA_PASSWORD_GUARD */