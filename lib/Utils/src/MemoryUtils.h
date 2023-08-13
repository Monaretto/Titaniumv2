#include <stdint.h>
#include "esp_err.h"

#ifndef MEMORY_UTILS_GUARD
#define MEMORY_UTILS_GUARD

esp_err_t memcpy_s(uint8_t *pOut, uint8_t *pIn, uint32_t size);
esp_err_t memset_s(uint8_t *pOut, uint8_t value, uint32_t size);

#endif /* MEMORY_UTILS_GUARD */