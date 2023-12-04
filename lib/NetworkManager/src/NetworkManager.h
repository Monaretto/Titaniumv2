#ifndef NETWORK_MANAGER_GUARD
#define NETWORK_MANAGER_GUARD

#include "ProcessTemplate.h"
#include "MemoryManager.h"

#include "esp_wifi.h"
#include "esp_err.h"

enum connection_status_e {
    NOT_CONNECTED = 0,
    CONNECTED,
};

/**
 * A class that manages the graphic driver process.
 */
class NetworkManager : public ProcessTemplate{
    public:
    NetworkManager() : ProcessTemplate(this, "Network Proccess", 10240, 1, &this->process_handler) { };
    void Execute(void);
    void SetWiFiConnection_(uint8_t status);
    esp_err_t result = ESP_FAIL;

    private:
    esp_err_t Initialize_(void);
    esp_err_t ConnectWiFi_(void);
    esp_err_t RegisterWiFiEvents_(void);
    esp_err_t InitializeAP_(void);
    esp_err_t InitializeSTA_(void);
    esp_err_t ChangeConnectionMode_(void);
    esp_err_t CleanUpConnection_(void);

    private:
    esp_netif_t* esp_netif_pointer_ = nullptr;
    TaskHandle_t process_handler = NULL;
    uint8_t connected_ = NOT_CONNECTED;
    uint8_t last_connected_ = 0;
    wifi_mode_t connection_mode_= WIFI_MODE_AP;
    wifi_mode_t last_connection_mode_ = WIFI_MODE_NULL;
    MemoryManager* memory_manager_ = NULL;
};

#endif /* NETWORK_MANAGER_GUARD */