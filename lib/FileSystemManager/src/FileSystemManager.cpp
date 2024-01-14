#include "FileSystemManager.h"

#include "esp_spiffs.h"
#include "esp_log.h"
#include "string.h"

static const char *TAG = "FileSystemManager";

esp_err_t FileSystemManager::Initialize(void){
    esp_vfs_spiffs_conf_t config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };

    esp_vfs_spiffs_register(&config);

    FILE *file = fopen("/spiffs/hello.txt", "r");
    if (file == NULL)
    {
        ESP_LOGE(TAG, "ROM_DATABASE does not exist!");
    }

    fclose(file);

    return ESP_OK;
}

esp_err_t FileSystemManager::ReadFile(char *filepath, uint8_t *output_pointer, uint16_t maximum_buffer_size){
    auto result = ESP_OK;

    do {
        if (output_pointer == NULL){
            ESP_LOGE(TAG, "Invalid output pointer");
            result = ESP_FAIL;
            break;
        }

        FILE *file = fopen(filepath, "r");

        if (file == NULL){
            ESP_LOGE(TAG, "File: %s does not exist!", filepath);
            result = ESP_FAIL;
            break;
        }

        size_t file_size = this->GetFileSize_(file);
        ESP_LOGI(TAG, "Buffer Size %d!", file_size);

        if (maximum_buffer_size < file_size){
            ESP_LOGE(TAG, "Buffer too short to receive file!");
            result = ESP_FAIL;
            fclose(file);
            break;
        }

        fread(output_pointer, 1, file_size, file);
        fclose(file);

    }while(0);


    return result;
}

size_t FileSystemManager::GetFileSize_(FILE *file){
    size_t file_size = 0;

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return file_size;
}

/**
 * @brief Returns the singleton instance
 * 
 * @return FileSystemManager* 
 */
FileSystemManager* FileSystemManager::GetInstance(void)
{
    if (singleton_pointer_ == nullptr)
    {
        singleton_pointer_ = new FileSystemManager();
    }

    return singleton_pointer_;
}