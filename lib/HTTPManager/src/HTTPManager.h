#ifndef HTTP_MANAGER_GUARD
#define HTTP_MANAGER_GUARD

#include "MemoryManager.h"

#include "esp_http_server.h"
#include "esp_err.h"

/**
 * A class that manages the HTTP connections.
 */
class HTTPManager{
    public:
    HTTPManager(const HTTPManager& obj) = delete;
    esp_err_t Initialize(void);
    esp_err_t StartHTTPServer(void);
    esp_err_t StopHTTPServer(void);
    
    public:
    static HTTPManager* GetInstance(void);

    private:
    void InitializeRequestList_(void);
    esp_err_t ReadFilesFromSPIFFS_(void);
    esp_err_t RegisterHandlers_(void);
    HTTPManager(){};

    private:
    static constexpr uint8_t maximum_requests_list_size = 2;
    static constexpr uint16_t html_page_maximum_size = 4096;
    static constexpr uint16_t css_styles_maximum_size = 2048;
    static HTTPManager* singleton_pointer_;

    private:
    httpd_uri_t http_requests_list_[maximum_requests_list_size];
    uint8_t html_page_[html_page_maximum_size];
    uint8_t css_styles_[css_styles_maximum_size];
    httpd_handle_t server_ = NULL;
    httpd_config_t config_;
};

#endif /* HTTP_MANAGER_GUARD */