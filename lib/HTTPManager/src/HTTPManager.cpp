#include <HTTPManager.h>

#include "esp_log.h"

#include "MemoryManager.h"
#include "FileSystemManager.h"
#include "./Requests/RequestData.h"

esp_err_t RequestHandler(httpd_req_t *req)
{
    if (strcmp(req->uri, "/index") == 0){
        httpd_resp_set_type(req, "text/html");
    }
    else if (strcmp(req->uri, "/styles.css") == 0){
        httpd_resp_set_type(req, "text/css");
    }
    httpd_resp_send(req, (const char *)req->user_ctx, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/**
 * The function 'Execute' runs an infinite loop with a delay of 1000 milliseconds.
 */
void HTTPManager::Execute(void){
    uint8_t connection_status = NOT_CONNECTED;
    
    this->Initialize_();
    
    while(1){
        this->memory_manager_->Read(CONNECTION_AREA, sizeof(connection_status), &connection_status);

        if (this->last_status != connection_status){
            if (connection_status == NOT_CONNECTED){
                this->StopHTTPServer();
            }
            else if (connection_status == CONNECTED){
                this->StartHTTPServer();
            }

            this->last_status = connection_status;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * The function initializes the HTTPManager by reading files from SPIFFS and initializing the request
 * list.
 * 
 * @return an 'esp_err_t' value.
 */
esp_err_t HTTPManager::Initialize_(void)
{
    auto result = ESP_OK;

    this->config_ = HTTPD_DEFAULT_CONFIG();
    this->memory_manager_ = MemoryManager::GetInstance();

    result += this->ReadFilesFromSPIFFS_();
    this->InitializeRequestList_();

    return result;
}


/**
 * The function starts an HTTP server and registers handlers.
 * 
 * @return an `esp_err_t` value.
 */
esp_err_t HTTPManager::StartHTTPServer(void){
    auto result = ESP_OK;

    if (httpd_start(&this->server_, &this->config_) == ESP_OK)
    {
        result += RegisterHandlers_();
    }

    return result;
}

/**
 * The function `StopHTTPServer` stops the HTTP server.
 * 
 * @return an esp_err_t, which is a type defined in the ESP-IDF framework for error handling.
 */
esp_err_t HTTPManager::StopHTTPServer(void){

    return httpd_stop(&this->server_);
}

/**
 * The function InitializeRequestList_ initializes a list of HTTP request URIs and their corresponding
 * handlers.
 */
void HTTPManager::InitializeRequestList_(void){

    RequestData request_index((char*)"text/html", this->html_page_);
    RequestData request_css((char*)"text/css", this->css_styles_);

    httpd_uri_t uri_get_index = {
        .uri = "/index",
        .method = HTTP_GET,
        .handler = RequestHandler,
        .user_ctx = this->html_page_,
    };

    httpd_uri_t uri_get_css = {
        .uri = "/styles.css",
        .method = HTTP_GET,
        .handler = RequestHandler,
        .user_ctx = this->css_styles_,
    };

    this->http_requests_list_[0] = uri_get_index;
    this->http_requests_list_[1] = uri_get_css;
}

/**
 * The function 'ReadFilesFromSPIFFS_' reads the contents of two files ('index.html' and 'styles.css')
 * from the SPIFFS file system and stores them in the 'html_page_' and 'css_styles_' variables
 * respectively.
 * 
 * @return an esp_err_t, which is a type defined in the ESP-IDF framework for error handling.
 */
esp_err_t HTTPManager::ReadFilesFromSPIFFS_(void){
    auto result = ESP_OK;
    auto file_system_manager = FileSystemManager::GetInstance();

    memset_s(this->html_page_, 0, sizeof(this->html_page_));
    memset_s(this->css_styles_, 0, sizeof(this->css_styles_));

    result += file_system_manager->ReadFile((char*)"/spiffs/WebServer/index.html", this->html_page_, sizeof(this->html_page_));
    result += file_system_manager->ReadFile((char*)"/spiffs/WebServer/styles.css", this->css_styles_, sizeof(this->css_styles_));

    return result;
}

/**
 * The function iterates through a list of HTTP request handlers and registers them with an HTTP
 * server.
 * 
 * @return an 'esp_err_t' value.
 */
esp_err_t HTTPManager::RegisterHandlers_(void){
    auto result = ESP_OK;

    for (int i = 0; i < this->maximum_requests_list_size; i++){
        result += httpd_register_uri_handler(this->server_, &this->http_requests_list_[i]);
    }

    return result;
}