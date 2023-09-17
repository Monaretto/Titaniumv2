class RequestData {
public:
    RequestData(char* content_type, char* data) {
        this->content_type = content_type;
        this->data = data;
    }
    char* content_type = nullptr;
    char* data = nullptr;
};