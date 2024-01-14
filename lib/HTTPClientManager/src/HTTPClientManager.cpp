#include <HTTPManager.h>

#include "esp_log.h"

#include "memory/MemoryManager.hpp"
#include "FileSystemManager.h"

esp_err_t RequestHandlerGet(httpd_req_t *req)
{
    auto memory_manager = MemoryManager::GetInstance();
    if (req->method == HTTP_GET) {
        if (strcmp(req->uri, "/index") == 0) {
            httpd_resp_set_type(req, "text/html");
        }
        else if (strcmp(req->uri, "/styles.css") == 0) {
            httpd_resp_set_type(req, "text/css");
        }
        else {
            httpd_resp_set_type(req, "application/json");
        }
        const char *handler_ctx = (const char *)req->user_ctx;
        httpd_resp_send(req, handler_ctx, strlen(handler_ctx));
        
    }

    return ESP_OK;
}

void GetSsidAndPass(const char* httpContent)
{
    auto memory_manager = MemoryManager::GetInstance();

    char ssid[100], pass[100];
    uint8_t ssid_size = 0;
    uint8_t pass_size = 0;

    char *p_Separator = strstr(httpContent, "&");

    if(p_Separator != NULL)
    {
        char *p_Ssid = strstr(httpContent, "input1=");
        if(p_Ssid != NULL && p_Separator > p_Ssid)
        {
            ssid_size = (int)(p_Separator - p_Ssid) - strlen("input1=");
            memcpy(ssid, (p_Ssid + strlen("input1=")), ssid_size );
            ssid[ssid_size] = '\0';
            printf("SSID: %s\n", ssid);
            memory_manager->Write(SSID_AREA, ssid_size, (uint8_t *)ssid);
        }

        char *p_Pass = strstr(httpContent, "input2=");
        if(p_Pass != NULL && p_Separator < p_Pass)
        {
            pass_size = (int)(p_Pass - p_Separator) - strlen("input2=");
            memcpy(pass, (p_Pass + strlen("input2=")), pass_size );
            pass[pass_size] = '\0';
            printf("pass: %s\n", pass);
            memory_manager->Write(PASSWORD_AREA, pass_size, (uint8_t *)pass);
        }
    }
}

esp_err_t RequestHandlerPost(httpd_req_t *req)
{
    
    if (req->method == HTTP_POST) {
        if (strcmp(req->uri, "/submitButton") == 0) {
            int ret = 0;
            uint16_t remaining = req->content_len;
            if(remaining >= 100)
            {
                return ESP_FAIL;
            }

            char content[100];
            while (remaining > 0)
            {
                ret = httpd_req_recv(req, content, 100);
                printf("Hrer %d\n", remaining);
                if(ret <= 0)
                {
                    if(ret == 0)
                    {
                        httpd_resp_send_408(req);
                    }
                    return ESP_FAIL;
                }
                remaining -= ret;
            }

            GetSsidAndPass(content);
            
            content[req->content_len] = '\0';
            printf("Here %s\n", content);
            httpd_resp_send(req, "Sucess!", strlen("Sucess!"));
           
        }
    }

    return ESP_OK;
}

/**
 * The function 'Execute' runs an infinite loop with a delay of 1000 milliseconds.
 */
void HTTPManager::Execute(void)
{
    uint8_t connection_status = NOT_CONNECTED;
    uint16_t connection_status_size = 0;

    this->Initialize_();

    while (1)
    {
        this->memory_manager_->Read(CONNECTION_AREA, &connection_status_size, &connection_status);

        if (this->conection_status_ != connection_status){
            if (connection_status == NOT_CONNECTED){
                this->StopHTTPServer_();
            }
            else if (connection_status == CONNECTED){
                this->StartHTTPServer_();
            }

            this->conection_status_ = connection_status;
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
esp_err_t HTTPManager::StartHTTPServer_(void)
{
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
esp_err_t HTTPManager::StopHTTPServer_(void)
{

    return httpd_stop(&this->server_);
}

/**
 * The function InitializeRequestList_ initializes a list of HTTP request URIs and their corresponding
 * handlers.
 */
void HTTPManager::InitializeRequestList_(void)
{

    httpd_uri_t uri_get_index = {
        .uri = "/index",
        .method = HTTP_GET,
        .handler = RequestHandlerGet,
        .user_ctx = this->html_page_,
    };

    httpd_uri_t uri_get_css = {
        .uri = "/styles.css",
        .method = HTTP_GET,
        .handler = RequestHandlerGet,
        .user_ctx = this->css_styles_,
    };

    httpd_uri_t uri_post = {
        .uri = "/submitButton",
        .method = HTTP_POST,
        .handler = RequestHandlerPost,
        .user_ctx = NULL,
    };

    this->http_requests_list_[0] = uri_get_index;
    this->http_requests_list_[1] = uri_get_css;
    this->http_requests_list_[2] = uri_post;
}

/**
 * The function 'ReadFilesFromSPIFFS_' reads the contents of two files ('index.html' and 'styles.css')
 * from the SPIFFS file system and stores them in the 'html_page_' and 'css_styles_' variables
 * respectively.
 *
 * @return an esp_err_t, which is a type defined in the ESP-IDF framework for error handling.
 */
esp_err_t HTTPManager::ReadFilesFromSPIFFS_(void)
{
    auto result = ESP_OK;
    auto file_system_manager = FileSystemManager::GetInstance();

    memset_s(this->html_page_, 0, sizeof(this->html_page_));
    memset_s(this->css_styles_, 0, sizeof(this->css_styles_));

    result += file_system_manager->ReadFile((char *)"/spiffs/WebServer/index.html", this->html_page_, sizeof(this->html_page_));
    // result += file_system_manager->ReadFile((char *)"/spiffs/WebServer/styles.css", this->css_styles_, sizeof(this->css_styles_));

    return result;
}

/**
 * The function iterates through a list of HTTP request handlers and registers them with an HTTP
 * server.
 *
 * @return an 'esp_err_t' value.
 */
esp_err_t HTTPManager::RegisterHandlers_(void)
{
    auto result = ESP_OK;

    for (int i = 0; i < this->maximum_requests_list_size; i++)
    {
        result += httpd_register_uri_handler(this->server_, &this->http_requests_list_[i]);
    }

    return result;
}