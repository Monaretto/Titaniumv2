#include "nvs_flash.h"

#include <FileSystemManager.h>
#include <MemoryManager.h>
#include <HTTPManager.h>

#include <CommandManager.h>
#include <GraphicDriverManager.h>
#include <SerialDriverManager.h>
#include <NetworkManager.h>


FileSystemManager* FileSystemManager::singleton_pointer_ = nullptr;
HTTPManager* HTTPManager::singleton_pointer_ = nullptr;
MemoryManager* MemoryManager::singleton_pointer_ = nullptr;

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
  auto file_system_manager = FileSystemManager::GetInstance();
  auto http_manager = HTTPManager::GetInstance();

  auto graphic_manager = new GraphicDriverManager;
  auto network_manager = new NetworkManager;
  auto serial_manager = new SerialDriverManager;
  
  memory_manager->Initialize();
  file_system_manager->Initialize();
  http_manager->Initialize();

  graphic_manager->InitializeProcess();
  network_manager->InitializeProcess();
  serial_manager->InitializeProcess();
  

  return 0;
}

extern "C" void app_main(void) { main(); }