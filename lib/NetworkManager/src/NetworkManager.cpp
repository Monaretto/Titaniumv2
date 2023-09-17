#include "NetworkManager.h"
#include "esp_system.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"

#include "lwip/err.h"
#include "lwip/sys.h"
static const char *TAG = "wifi softAP";
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if ((event_base != WIFI_EVENT) && (event_base != IP_EVENT))
        return;

    NetworkManager * network_pointer_object = static_cast<NetworkManager *>(arg);

    switch (event_id){
        case WIFI_EVENT_STA_START:
        case WIFI_EVENT_STA_DISCONNECTED:
            network_pointer_object->SetWiFiConnection_(false);
            network_pointer_object->result = esp_wifi_connect();
        break;
        case IP_EVENT_STA_GOT_IP:
            network_pointer_object->SetWiFiConnection_(true);
        break;
        default:
        break;
    }
}

/**
 * The function initializes the network manager by configuring the Wi-Fi settings.
 * 
 * @return an `esp_err_t` value, which is the result of the initialization process.
 */
esp_err_t NetworkManager::Initialize_(void){
    esp_err_t result = ESP_OK;
    this->memory_manager_ = MemoryManager::GetInstance();

    result += esp_netif_init();
    result += esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    result += esp_wifi_init(&cfg);
    result += esp_wifi_set_mode(WIFI_MODE_NULL);

    result += this->RegisterWiFiEvents_();
    result += esp_wifi_set_storage(WIFI_STORAGE_RAM);

    return result;
}

/**
 * The function `Execute` runs an infinite loop with a delay of 50 milliseconds.
 */
void NetworkManager::Execute(void){
    this->Initialize_();

    while(1){
        if (this->last_connection_mode_ != this->connection_mode_){
            this->CleanUpConnection_();
            this->ChangeConnectionMode_();
        }

        if (this->last_connected_ != this->connected_){
            this->memory_manager_->Write(CONNECTION_AREA, sizeof(this->connected_), &this->connected_);
            this->last_connected_ = this->connected_;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * The function `ChangeConnectionMode_` initializes the network connection mode based on the current
 * mode set in `connection_mode_`.
 * 
 * @return an esp_err_t, which is a type defined in the ESP-IDF framework for error codes.
 */
esp_err_t NetworkManager::ChangeConnectionMode_(void){
    esp_err_t result = ESP_OK;

    if (this->connection_mode_ == WIFI_MODE_STA){
        result = this->InitializeSTA_();
    } else if (this->connection_mode_ == WIFI_MODE_AP){
        result = this->InitializeAP_();
    }

    return result;
}

/**
 * The function initializes the access point (AP) mode of the network manager with a specific SSID,
 * password, and maximum number of connections.
 * 
 * @return an esp_err_t, which is an error code indicating the success or failure of the function.
 */
esp_err_t NetworkManager::InitializeAP_(void){
    esp_err_t result = ESP_OK;

    const char SSID_AP[9] = "Titanium";
    const char PASSWORD_AP[9] = "12345678";
    static const uint8_t MAX_STA_CONN = 3;

    esp_netif_create_default_wifi_ap();

    wifi_config_t wifi_config;
    memcpy_s(wifi_config.ap.ssid, (uint8_t*)SSID_AP, sizeof(SSID_AP));
    memcpy_s(wifi_config.ap.password, (uint8_t*)PASSWORD_AP, sizeof(PASSWORD_AP));
    wifi_config.ap.ssid_len = sizeof(PASSWORD_AP);
    wifi_config.ap.max_connection = MAX_STA_CONN;
    wifi_config.ap.channel = 1;
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;


    result += esp_wifi_set_mode(WIFI_MODE_AP);
    // ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    result += esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    result += esp_wifi_start();

    if (result == ESP_OK){
        this->last_connection_mode_ = WIFI_MODE_AP;
    }

    return result;
}

/**
 * The function InitializeSTA_ initializes the ESP32 WiFi station mode with the provided SSID and
 * password.
 * 
 * @return an esp_err_t, which is an error code indicating the success or failure of the initialization
 * process.
 */
esp_err_t NetworkManager::InitializeSTA_(void){
    esp_err_t result = ESP_OK;
    wifi_config_t wifi_config;

    this->esp_netif_pointer_ = esp_netif_create_default_wifi_sta();

    this->memory_manager_->Read(SSID_AREA, sizeof(wifi_config.sta.ssid), wifi_config.sta.ssid);
    this->memory_manager_->Read(PASSWORD_AREA, sizeof(wifi_config.sta.password), wifi_config.sta.password);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    result += esp_wifi_set_mode(WIFI_MODE_STA);
    result += esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    result += esp_wifi_start();

    if (result == ESP_OK){
        this->last_connection_mode_ = WIFI_MODE_STA;
    }

    return result;
}

/**
 * The function `CleanUpConnection_` disconnects from the Wi-Fi network, stops the Wi-Fi module, and
 * sets the Wi-Fi mode to NULL.
 * 
 * @return an esp_err_t, which is a type defined in the ESP-IDF framework for error handling.
 */
esp_err_t NetworkManager::CleanUpConnection_(void){
    esp_err_t result = ESP_OK;

    result += esp_wifi_disconnect();
    result += esp_wifi_stop();
    result += esp_wifi_set_mode(WIFI_MODE_NULL);

    return result;
}


/**
 * The function `RegisterWiFiEvents_` registers event handlers for WiFi and IP events.
 * 
 * @return an esp_err_t, which is a type defined in the ESP-IDF framework for error handling.
 */
esp_err_t NetworkManager::RegisterWiFiEvents_(void){
    esp_err_t result = ESP_OK;

    result += esp_event_handler_instance_register(
                                        WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &event_handler,
                                        this,
                                        nullptr);

    result += esp_event_handler_instance_register(
                                        IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &event_handler,
                                        this,
                                        nullptr);

    return result;
}

/**
 * The function sets the status of the WiFi connection.
 * 
 * @param status The status parameter is a boolean value that represents the desired connection status
 * of the WiFi network.
 */
void NetworkManager::SetWiFiConnection_(uint8_t status){
    this->connected_ = status;
}
