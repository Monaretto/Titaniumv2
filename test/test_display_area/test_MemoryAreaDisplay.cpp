#include "MemoryAreas/src/MemoryAreaDisplay.h"

#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void test_MemoryAreaDisplay() {
    auto result = ESP_FAIL;
    auto memory_area_instance = new MemoryAreaDisplay;

    result = memory_area_instance->Initialize();
    TEST_ASSERT_EQUAL(result, ESP_OK);
    result = memory_area_instance->Clear();
    TEST_ASSERT_EQUAL(result, ESP_OK);
    TEST_ASSERT_EQUAL(memory_area_instance->GetAccess(), READ_WRITE);
    TEST_ASSERT_EQUAL(memory_area_instance->GetSize(), 1024);
    TEST_ASSERT_EQUAL(memory_area_instance->GetAreaIndex(), 0);

    // Write and Read Area
    uint8_t expected_array[1024] = {0};
    uint8_t expected_array_read[1024] = {0};

    for (int i = 0; i < memory_area_instance->GetSize(); i++) {
        expected_array[i] = (uint8_t)(i % 256);
    }

    TEST_ASSERT_EQUAL(memory_area_instance->Write(expected_array, 1024), ESP_OK);
    TEST_ASSERT_EQUAL(memory_area_instance->Read(expected_array_read), ESP_OK);

    for (int i = 0; i < memory_area_instance->GetSize(); i++) {
        TEST_ASSERT_EQUAL(expected_array[i], expected_array_read[i]);
    }
}

void main_test(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));

    UNITY_BEGIN();
    RUN_TEST(test_MemoryAreaDisplay);
    UNITY_END();
}

extern "C" void app_main(void) { main_test(); }