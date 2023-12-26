#include "LoRaManager.h"

/**
 * @brief Initializes the LoRaManager.
 *
 * This function initializes the MemoryManager, creates an instance of LoRaDriver,
 * and configures the LoRa transceiver with specific settings.
 *
 * @return ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t LoRaManager::Initialize_(void){
    esp_err_t result = ESP_OK;

    this->memory_manager = MemoryManager::GetInstance();
    this->lora_driver = new LoRaDriver();
    result += this->lora_driver->Initialize();
    this->lora_driver->SetFrequency(Regions::BRAZIL);
    this->lora_driver->SetCRCMode(CRCMode::ENABLE);
    this->lora_driver->SetImplicitHeaderMode(64);

    ESP_ERROR_CHECK(result);

    return result;
}

/**
 * @brief Executes the LoRaManager functionality in an infinite loop.
 *
 * This function runs an infinite loop with a delay of 100 milliseconds. It checks if
 * there is new data in the LORA_WRITE_AREA of the MemoryManager. If there is new data,
 * it reads the data, sends a packet using the LoRa transceiver, and sets the transceiver
 * to receiver mode. If there is data received, it reads the received data, and writes it
 * to the LORA_READ_AREA of the MemoryManager.
 */
void LoRaManager::Execute(void){

    this->Initialize_();

    while(1){
        if (memory_manager->IsAreaDataNew(LORA_WRITE_AREA)){
            uint16_t area_size = 0;
            memory_manager->Read(LORA_WRITE_AREA, &area_size, this->tx_buffer_);
            this->lora_driver->SendPacket(this->tx_buffer_, area_size);
        }
        this->lora_driver->SetReceiverMode();
        if (this->lora_driver->isDataInReceiver()){
            uint8_t recieved_bytes = this->lora_driver->ReceivePacket(this->rx_buffer_, sizeof(this->rx_buffer_));
            this->memory_manager->Write(LORA_READ_AREA, recieved_bytes, this->rx_buffer_);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}