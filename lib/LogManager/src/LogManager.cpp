#include "LogManager.h"

/**
 * Initializes the LogManager.
 *
 * @returns ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t LogManager::Initialize(void){
    this->memory_manager = MemoryManager::GetInstance();

    return ESP_OK;
}

/**
 * Initializes the LogManager.
 *
 * @returns ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t LogManager::Check(int error_code, int expected_error_code){
    if (error_code != expected_error_code){
        // search for the error
        //read from memory if there is a error with low priority than
        //write the error and the priority on the memory_manager
    }
}

/**
 * @brief Returns the singleton instance
 * 
 * @return LogManager* 
 */
LogManager* LogManager::GetInstance(void)
{
    if (singleton_pointer_ == nullptr)
    {
        singleton_pointer_ = new LogManager();
    }

    return singleton_pointer_;
}