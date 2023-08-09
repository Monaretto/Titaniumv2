#include "stdint.h"
#include "esp_err.h"

#include "MemoryManager.h"

#ifndef LOG_MANAGER_GUARD
#define LOG_MANAGER_GUARD

/**
 * Manages all the errors and log into the memory and access for a system.
 */
class LogManager{
    public:
    LogManager(const LogManager& obj) = delete;
    esp_err_t Initialize(void);
    esp_err_t Check(int error_code, int expected_error_code);
    static LogManager* GetInstance(void);
    private:
    LogManager(){};
    MemoryManager *memory_manager = nullptr;
    static LogManager* singleton_pointer_;
};

#endif /* LOG_MANAGER_GUARD */