#include "SerialDriverManager.h"
#include "MemoryManager.h"

#include "SerialProtocol.h"

/**
 * Executes the SerialDriverManager.
 *
 * This function initializes the SerialDriverManager and continuously reads data from UART_NUM_0.
 * If the initialization fails, the process handler is deleted.
 * The function reads data from UART_NUM_0 and writes it to the display area in memory if the length of the data is 1024 bytes.
 * The function delays for 50 milliseconds after each iteration.
 *
 * @returns None
 */
void SerialDriverManager::Execute(void)
{
    auto memory_manager = MemoryManager::GetInstance();
    auto serial_protocol = SerialProtocol();

    uint8_t ACK_OK[7] = {0x41, 0x43, 0x4B, 0x4F, 0x4B, 0x00, 0x00};
    uint8_t ACK_NOK[8] = {0x41, 0x43, 0x4B, 0x4E, 0x4F, 0x4B, 0x00, 0x00};

    if (this->Initialize_() != ESP_OK){
        vTaskDelete(this->process_handler);
    }

    while(1){
        auto len = uart_read_bytes(UART_NUM_0, this->input_buffer_, this->buffer_size_, pdMS_TO_TICKS(200));

        if (len == 0){
            continue;
        }
        auto result = serial_protocol.ProcessIncomingMessage(this->input_buffer_, len);
        
        if (result == ESP_OK){
            auto message = serial_protocol.GetMessage();

            result = memory_manager->Write(
                static_cast<area_index_e>(message.memory_area),
                message.data_length,
                message.data_pointer
            );

            
            //workaround
            ACK_OK[5] = (result & 0xFF) + 0x30;
            ACK_OK[6] = ((result >> 8) & 0xFF) + 0x30;

            uart_write_bytes(UART_NUM_0, ACK_OK , sizeof(ACK_OK));
        }
        else
        {
            //workaround
            ACK_NOK[6] = (result & 0xFF) + 0x30;
            ACK_NOK[7] = ((result >> 8) & 0xFF) + 0x30;
            uart_write_bytes(UART_NUM_0, ACK_NOK , sizeof(ACK_NOK));
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/**
 * Initializes the SerialDriverManager.
 *
 * @returns The result of the initialization process.
 */
esp_err_t SerialDriverManager::Initialize_(void){
    esp_err_t result = ESP_OK;

    this->uart_config.baud_rate = this->baud_rate_;
    this->uart_config.data_bits = UART_DATA_8_BITS;
    this->uart_config.parity = UART_PARITY_DISABLE;
    this->uart_config.stop_bits = UART_STOP_BITS_1;
    this->uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    this->uart_config.source_clk = UART_SCLK_APB;


    result += uart_param_config(UART_NUM_0, &uart_config);
    result += uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    result += uart_driver_install(UART_NUM_0, this->buffer_size_ * 2, 0, 0, NULL, 0);

    return result;
}