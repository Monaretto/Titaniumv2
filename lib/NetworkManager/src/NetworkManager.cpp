#include "NetworkManager.h"
#include "memory/MemoryManager.hpp"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"

namespace AP {
    const uint8_t          ssid[] = "Titanium";
    const uint8_t          password[] = "root1234";
    const uint8_t          channel = 1;
    const uint8_t          visibility = 0;
    const uint8_t          max_connections = 1;
    const char*            ip = "192.168.0.1";
    const char*            gw = "192.168.0.1";
    const char*            netmask = "255.255.255.0";
    const uint8_t          beacon_interval = 100;
    const wifi_bandwidth_t bw = WIFI_BW_HT20;
    const wifi_ps_type_t   power_save = WIFI_PS_NONE;
}

namespace STA {
    const uint8_t ssid_max_lenght = 32;
    const uint8_t password_max_lenght = 64;
    const uint8_t max_retries = 5;
}

static void WiFiAppEventHandler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT){
        switch (event_id){
            case WIFI_EVENT_AP_START:
            case WIFI_EVENT_AP_STOP:
            case WIFI_EVENT_AP_STACONNECTED:
            case WIFI_EVENT_AP_STADISCONNECTED:
            case WIFI_EVENT_STA_START:
            case WIFI_EVENT_STA_CONNECTED:
            case WIFI_EVENT_STA_DISCONNECTED:
                break;
        }
    }
    else if (event_base == IP_EVENT){
        switch (event_id) {
        case IP_EVENT_STA_GOT_IP:
            break;
        }
    }
}

/**
 * @brief Initializes the network manager by configuring the Wi-Fi settings.
 *
 * This function initializes the network manager by setting up Wi-Fi, registering event handlers,
 * and connecting to a Wi-Fi network using the credentials stored in the memory manager.
 *
 * @return An `esp_err_t` value, which is the result of the initialization process.
 */
esp_err_t NetworkManager::Initialize(void){
    esp_err_t result = ESP_OK;
    this->_memory_manager = MemoryManager::GetInstance();

    result += this->RegisterWiFiEvents();
    result += esp_netif_init();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    result += esp_wifi_init(&cfg);
    result += esp_wifi_set_storage(WIFI_STORAGE_RAM);

    this->_esp_netif_sta = esp_netif_create_default_wifi_sta();
    this->_esp_netif_ap  = esp_netif_create_default_wifi_ap();

    wifi_config_t wifi_config;

    result += this->SetAccessPointMode(wifi_config);
    // this->SetStationMode(wifi_config);
    // this->SetCredentials(wifi_config);
    // wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    // wifi_config.sta.pmf_cfg.capable = true;
    // wifi_config.sta.pmf_cfg.required = false;

    // result += esp_wifi_set_mode(WIFI_MODE_APSTA);
    // result += esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    result += esp_wifi_start();
    return result;
}

/**
 * @brief Executes the network manager functionality in an infinite loop.
 *
 * This function runs an infinite loop with a delay of 50 milliseconds. It initializes the network,
 * and writes the connection status to the memory manager at regular intervals.
 */
void NetworkManager::Execute(void){
    auto memory_manager = MemoryManager::GetInstance();

    if (this->Initialize() != ESP_OK){
        vTaskDelete(this->_process_handler);
    }

    while(1){
        memory_manager->Write(CONNECTION_AREA, sizeof(this->_connection_area), &this->_connection_area);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Registers WiFi and IP event handlers.
 *
 * This function registers event handlers for WiFi and IP events to handle connection events.
 *
 * @return An `esp_err_t`, which is a type defined in the ESP-IDF framework for error handling.
 */
esp_err_t NetworkManager::RegisterWiFiEvents(void){
    esp_err_t result = ESP_OK;

    result += esp_event_loop_create_default();

    result += esp_event_handler_instance_register(
                                        WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &WiFiAppEventHandler,
                                        this,
                                        nullptr);

    result += esp_event_handler_instance_register(
                                        IP_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &WiFiAppEventHandler,
                                        this,
                                        nullptr);

    return result;
}

/**
 * @brief Sets the status of the WiFi connection.
 *
 * This function sets the status of the WiFi connection based on the provided status parameter.
 *
 * @param status A boolean value that represents the desired connection status of the WiFi network.
 */
void NetworkManager::SetWiFiConnection(uint8_t status){
    this->_connection_area.connection_status = status;
}

esp_err_t NetworkManager::SetStationMode(wifi_config_t& wifi_config){
    auto result = ESP_OK;

   this->SetCredentials(wifi_config);

    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    return result;
}

esp_err_t NetworkManager::SetAccessPointMode(wifi_config_t& ap_config){
    auto result = ESP_OK;

    memcpy_s(ap_config.ap.ssid, const_cast<uint8_t*>(AP::ssid), sizeof(AP::ssid));  
    memcpy_s(ap_config.ap.password, const_cast<uint8_t*>(AP::password), sizeof(AP::password));

    ap_config.ap.ssid_len        = sizeof(AP::ssid);  
    ap_config.ap.channel         = AP::channel;  
    ap_config.ap.ssid_hidden     = AP::visibility;  
    ap_config.ap.authmode        = WIFI_AUTH_WPA2_PSK;
    ap_config.ap.max_connection  = AP::max_connections;
    ap_config.ap.beacon_interval = AP::beacon_interval;

    esp_netif_ip_info_t ap_ip_info;
    memset_s(reinterpret_cast<uint8_t*>(&ap_ip_info), 0, sizeof(esp_netif_ip_info_t));

    esp_netif_dhcps_stop(this->_esp_netif_ap);
    inet_pton(AF_INET, AP::ip, &ap_ip_info.ip);
    inet_pton(AF_INET, AP::gw, &ap_ip_info.gw);
    inet_pton(AF_INET, AP::netmask, &ap_ip_info.netmask);

    result += esp_netif_set_ip_info(this->_esp_netif_ap, &ap_ip_info);
    result += esp_netif_dhcps_start(this->_esp_netif_ap);
    result += esp_wifi_set_mode(WIFI_MODE_APSTA);
    result += esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    result += esp_wifi_set_bandwidth(WIFI_IF_AP, AP::bw);
    result += esp_wifi_set_ps(AP::power_save);

    return result;
}

void NetworkManager::SetCredentials(wifi_config_t& wifi_config){
    this->_memory_manager->Read(CREDENTIALS_AREA, &this->_cred_area);

    memcpy_s(wifi_config.sta.ssid, this->_cred_area.sta_ssid, sizeof(wifi_config.sta.ssid));  
    memcpy_s(wifi_config.sta.password, this->_cred_area.sta_password, sizeof(wifi_config.sta.password));  
}

