/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_chip_info.h"
#include "LEDStrip.h"
#include "Log.h"
#include "esp_log.h"
#include "GPIO.h"
#include "MyApplication.h"

MyApplication myApplication;

using namespace rgb;

//constexpr auto CAN_UART = UART_NUM_1;
//constexpr auto CAN_BUFFER_SIZE = 1024;
//constexpr auto CAN_RX_PIN = 18;
//constexpr auto CAN_TX_PIN = 17;

//static QueueHandle_t uart_queue;

//auto configureCanUart() {
//  auto uartConfig = uart_config_t {
//    .baud_rate = 115200,
//    .data_bits = UART_DATA_8_BITS,
//    .parity    = UART_PARITY_DISABLE,
//    .stop_bits = UART_STOP_BITS_1,
//    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//    .rx_flow_ctrl_thresh = 0,
//    .source_clk = UART_SCLK_DEFAULT,
//    .flags = {
//      .allow_pd = 0,
//      .backup_before_sleep = 0
//    }
//  };
//
//  // Install UART driver
//  ASSERT(uart_param_config(CAN_UART, &uartConfig) == ESP_OK, "UART configuration failed");
//  ASSERT(uart_set_pin(CAN_UART, CAN_TX_PIN, CAN_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) == ESP_OK, "CAN UART pin set failed");
//  ASSERT(uart_driver_install(CAN_UART, CAN_BUFFER_SIZE * 2, 0, 10, &uart_queue, 0) == ESP_OK, "Driver install failed");
//  INFO("UART initialized on TX:%d RX:%d", CAN_TX_PIN, CAN_RX_PIN);
//}

/**
 * This is esp32s3 chip with 2 CPU core(s), WiFi/BLE,
 * silicon revision v0.2
 * 2MB external flash
 * Minimum free heap size: 382152 bytes
 */
extern "C" auto app_main() -> void {
  INFO("Starting Application");
  myApplication.run();
//    uint8_t data[128];
//
//    uart_write_bytes(CAN_UART, "AT\r\n", 4);
//    vTaskDelay(pdMS_TO_TICKS(500));  // Wait for response
//    int len = uart_read_bytes(CAN_UART, data, sizeof(data)-1, pdMS_TO_TICKS(100));
//    if (len > 0) {
//      data[len] = '\0';  // Null terminate
//      INFO("Response (\\r\\n): %s", (char*)data);
//    } else {
//      INFO("No response with \\r\\n");
//    }
//    uart_flush_input(CAN_UART);  // Clear buffer
//
//    uart_write_bytes(CAN_UART, "AT\n", 4);
//    vTaskDelay(pdMS_TO_TICKS(500));  // Wait for response
//    len = uart_read_bytes(CAN_UART, data, sizeof(data)-1, pdMS_TO_TICKS(100));
//    if (len > 0) {
//      data[len] = '\0';  // Null terminate
//      INFO("Response (\\n): %s", (char*)data);
//    } else {
//      INFO("No response with \\n");
//    }
//    uart_flush_input(CAN_UART);  // Clear buffer
}
