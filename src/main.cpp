#include "nvs_flash.h"

#include <memory/MemoryManager.h>
#include <gpio/GPIOManager.h>
#include <spi/SPIManager.h>

#include <FileSystemManager.h>  
#include <CommandManager.h>
#include <GraphicDriverManager.h>
#include <SerialDriverManager.h>
#include <LoRaManager.h>
#include <NetworkManager.h>
#include <HTTPManager.h>

MemoryManager* MemoryManager::singleton_pointer_ = nullptr;
GPIOManager*   GPIOManager::singleton_pointer_ = nullptr;
SPIManager*    SPIManager::singleton_pointer_ = nullptr;
FileSystemManager* FileSystemManager::singleton_pointer_ = nullptr;

esp_err_t initialize_nvs(void)
{
    nvs_flash_erase();
    esp_err_t ret = nvs_flash_init();
    return ret;
}

int main(void)
{
  initialize_nvs();

  auto memory_manager = MemoryManager::GetInstance();
  auto gpio_manager = GPIOManager::GetInstance();
  auto spi_manager = SPIManager::GetInstance();
  auto file_system_manager = FileSystemManager::GetInstance();

  memory_manager->Initialize();
  gpio_manager->Initialize();
  spi_manager->Initialize();
  file_system_manager->Initialize();

  auto graphic_manager = new GraphicDriverManager;
  auto network_manager = new NetworkManager;
  auto serial_manager = new SerialDriverManager;
  auto lora_manager = new LoRaManager;
  auto http_manager = new HTTPManager;
  
  serial_manager->InitializeProcess();
  graphic_manager->InitializeProcess();
  network_manager->InitializeProcess();
  lora_manager->InitializeProcess();
  http_manager->InitializeProcess();

  
  return 0;
}

extern "C" void app_main(void) { main(); }