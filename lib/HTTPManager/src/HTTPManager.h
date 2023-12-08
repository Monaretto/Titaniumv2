#ifndef HTTP_MANAGER_GUARD
#define HTTP_MANAGER_GUARD

#include "MemoryManager.h"
#include "ProcessTemplate.h"

#include "esp_http_server.h"
#include "esp_err.h"

/**
 * A class that manages the HTTP connections.
 */
class HTTPManager : public ProcessTemplate{
    public:
    HTTPManager() : ProcessTemplate(this, "HTTP Proccess", 1024, 1, &this->process_handler) { };
    
    public:
    void Execute(void);

    private:
    esp_err_t Initialize_(void);
    esp_err_t StartHTTPServer_(void);
    esp_err_t StopHTTPServer_(void);
    esp_err_t ReadFilesFromSPIFFS_(void);
    esp_err_t RegisterHandlers_(void);
    void      InitializeRequestList_(void);

    private:
    static constexpr uint8_t maximum_requests_list_size = 2;
    static constexpr uint16_t html_page_maximum_size = 4096;
    static constexpr uint16_t css_styles_maximum_size = 2048;

    private:
    httpd_uri_t http_requests_list_[maximum_requests_list_size];
    uint8_t html_page_[html_page_maximum_size];
    uint8_t css_styles_[css_styles_maximum_size];
    httpd_handle_t server_ = NULL;
    httpd_config_t config_;
    TaskHandle_t process_handler = NULL;
    MemoryManager* memory_manager_ = NULL;
};

#endif /* HTTP_MANAGER_GUARD */