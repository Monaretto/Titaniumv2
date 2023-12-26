#ifndef LORA_MANAGER_GUARD
#define LORA_MANAGER_GUARD

#include "stdint.h"
#include "esp_err.h"

#include "ProcessTemplate.h"
#include "memory/MemoryManager.h"
#include "Driver/LoRaDriver.h"

namespace Regions {
    constexpr uint32_t EUROPE    = 868e6;
    constexpr uint32_t US        = 915e6;
    constexpr uint32_t ASIA      = 920e6;
    constexpr uint32_t AUSTRALIA = 915e6;
    constexpr uint32_t CHINA     = 782e6;
    constexpr uint32_t INDIA     = 866e6;
    constexpr uint32_t KOREA     = 920e6;
    constexpr uint32_t JAPAN     = 920e6;
    constexpr uint32_t BRAZIL    = 915e6;
}

/**
 * Manages LoRa communication.
 */
class LoRaManager : public ProcessTemplate{
    public:
    LoRaManager() : ProcessTemplate(this, "LoRa Proccess", 1024*3, 1, &this->process_handler) { };
    void Execute(void);

    private:
    esp_err_t Initialize_(void);

    private:
    TaskHandle_t   process_handler = NULL;
    MemoryManager* memory_manager = nullptr;
    LoRaDriver*    lora_driver = nullptr;
    uint8_t tx_buffer_[64] = {0};
    uint8_t rx_buffer_[64] = {0};
};

#endif /* LORA_MANAGER_GUARD */