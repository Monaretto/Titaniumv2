#include "SerialProtocol.h"
#include "ByteConverterUtils.h"
#include "CRCUtils.h"

constexpr uint8_t START_BYTE_POSITION  = 0x00;
constexpr uint8_t DATA_LENGTH_POSITION = 0x01;
constexpr uint8_t START_COMMAND_POSITION = 0x03;
constexpr uint8_t START_MEMORY_AREA_POSITION = 0x04;
constexpr uint8_t END_BYTE_POSITION = 0x00;
constexpr uint8_t HEADER_SIZE = 0x05;
constexpr uint8_t FIXED_PROTOCOL_POSITION = 0x09;

constexpr uint8_t START_BYTE = 0x02;
constexpr uint8_t END_BYTE = 0x03;
constexpr uint8_t READ_OPERATION = 'R';
constexpr uint8_t WRITE_OPERATION = 'W'; 
constexpr uint16_t MAXIMUM_MESSAGE_SIZE = 1024;

/**
 * The function 'ProcessIncomingMessage' processes an incoming message by extracting relevant
 * information and validating it.
 * 
 * @param bytes A pointer to an array of bytes that represents the incoming message.
 * @param size The parameter 'size' represents the size of the 'bytes' array, which is the number of
 * elements in the array.
 * 
 * @return an 'esp_err_t' value, which is a type defined in the ESP-IDF framework.
 */
esp_err_t SerialProtocol::ProcessIncomingMessage(uint8_t *bytes, size_t size){
    esp_err_t result = ESP_FAIL;
    
    this->incoming_message_.start_byte = bytes[START_BYTE_POSITION];
    this->incoming_message_.data_length = ConvertBytesToUInt16(&bytes[DATA_LENGTH_POSITION]);
    this->incoming_message_.command = bytes[START_COMMAND_POSITION];
    this->incoming_message_.memory_area = bytes[START_MEMORY_AREA_POSITION];

    size_t crc_position = HEADER_SIZE + this->incoming_message_.data_length;
    size_t end_position = FIXED_PROTOCOL_POSITION + this->incoming_message_.data_length;

    this->incoming_message_.crc32 = ConvertBytesToUInt32(&bytes[crc_position]);
    this->incoming_message_.end_byte = bytes[end_position];

    result = this->ValidateIncomingMessage(bytes);

    if (result == ESP_OK){
        this->incoming_message_.data_pointer = &bytes[HEADER_SIZE];
    }

    return result;
}

esp_err_t SerialProtocol::ValidateIncomingMessage(uint8_t *bytes){
    esp_err_t result = ESP_FAIL;
    
    do{
        result = ValidateStartByte_(this->incoming_message_.start_byte);
        if ( result != ESP_OK)
            break;
        result = ValidateDataLength_(this->incoming_message_.data_length);
        if ( result != ESP_OK)
            break;
        result = ValidateCommand_(this->incoming_message_.command);
        if ( result != ESP_OK)
            break;
        result = ValidateCRC_(&this->incoming_message_, bytes);
        if ( result != ESP_OK)
            break;
        result = ValidateEndByte_(this->incoming_message_.end_byte);
        if ( result != ESP_OK)
            break;

    }while(0);

    return result;
}

esp_err_t SerialProtocol::ValidateStartByte_(uint8_t start_byte){
    return start_byte == START_BYTE ? ESP_OK : ESP_FAIL;
}

esp_err_t SerialProtocol::ValidateDataLength_(uint16_t data_length){
    return data_length <= MAXIMUM_MESSAGE_SIZE ? ESP_OK : ESP_ERR_INVALID_SIZE;
}

esp_err_t SerialProtocol::ValidateCommand_(uint8_t command){
    esp_err_t result = ESP_ERR_INVALID_ARG;

    do{
        if (command == 'R'){
            result = ESP_OK;
            break;
        }
        if (command == 'W'){
            result = ESP_OK;
            break;
        }
    }while(0);
    
    return result;
}

esp_err_t SerialProtocol::ValidateCRC_(const ProtocolData_st* incoming_message, uint8_t *bytes){
    uint32_t calculated_crc = CalculatedCRC32(bytes, incoming_message->data_length + HEADER_SIZE);
    return incoming_message->crc32 == calculated_crc ? ESP_OK : ESP_ERR_INVALID_CRC;
}

esp_err_t SerialProtocol::ValidateEndByte_(uint8_t end_byte){
    return end_byte == END_BYTE ? ESP_OK : ESP_FAIL;
}

ProtocolData_st SerialProtocol::GetMessage() const & {
    return this->incoming_message_;
}
