#ifndef FILE_SYSTEM_MANAGER_GUARD
#define FILE_SYSTEM_MANAGER_GUARD

#include "esp_err.h"

/**
 * A class that manages the File System Requests.
 */
class FileSystemManager{
    public:
    FileSystemManager(const FileSystemManager& obj) = delete;
    esp_err_t Initialize(void);
    esp_err_t ReadFile(char *filepath, uint8_t *output_pointer, uint16_t maximum_buffer_size);
    static FileSystemManager* GetInstance(void);

    private:
    FileSystemManager(){};
    size_t GetFileSize_(FILE *file);

    private:
    static FileSystemManager* singleton_pointer_;
    
    private:
    uint8_t memory_area_array_size;

};

#endif /* FILE_SYSTEM_MANAGER_GUARD */