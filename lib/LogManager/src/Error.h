#ifndef ERROR_GUARD
#define ERROR_GUARD

#include "esp_err.h"

class Error {
    public:
    int error_code;
    int priority;
};

#endif /* ERROR_GUARD */