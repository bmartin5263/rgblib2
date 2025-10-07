/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <cstdio>
#include <driver/gpio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_system.h"
#include "LEDStrip.h"
#include "driver/uart.h"
#include "Assertions.h"
#include "Log.h"
#include "esp_log.h"
#include "GPIO.h"

using namespace rgb;

// Base event concept (optional, for type safety)
template<typename T>
concept Event = requires {
  typename T::event_tag; // Each event must have an event_tag type
};

constexpr auto stickLen = 16;
constexpr auto CAN_UART = UART_NUM_1;
constexpr auto CAN_BUFFER_SIZE = 1024;
constexpr auto CAN_RX_PIN = 18;
constexpr auto CAN_TX_PIN = 17;

static QueueHandle_t uart_queue;

auto configureCanUart() {
  auto uartConfig = uart_config_t {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 0,
    .source_clk = UART_SCLK_DEFAULT,
    .flags = {
      .allow_pd = 0,
    }
  };

  // Install UART driver
  ASSERT(uart_param_config(CAN_UART, &uartConfig) == ESP_OK, "UART configuration failed");
  ASSERT(uart_set_pin(CAN_UART, CAN_TX_PIN, CAN_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) == ESP_OK, "CAN UART pin set failed");
  ASSERT(uart_driver_install(CAN_UART, CAN_BUFFER_SIZE * 2, 0, 10, &uart_queue, 0) == ESP_OK, "Driver install failed");
  INFO("UART initialized on TX:%d RX:%d", CAN_TX_PIN, CAN_RX_PIN);
}

/**
 * This is esp32s3 chip with 2 CPU core(s), WiFi/BLE,
 * silicon revision v0.2
 * 2MB external flash
 * Minimum free heap size: 382152 bytes
 */
extern "C" auto app_main() -> void {
  INFO("Starting Application");

  // Configure
  auto stick = LEDStrip<stickLen, rgb::format::GRBW>(pin_num{9});
  auto ledStrip = LEDStrip<40, rgb::format::GRB>(pin_num{46});
  auto debugLed = LEDStrip<1, rgb::format::GRB>(pin_num{38});

  // Initialize
  stick.start();
  debugLed.start();
  ledStrip.start();

  // Setup Input Pin
  gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << 6),
    .mode = gpio_mode_t::GPIO_MODE_INPUT,
    .pull_up_en = gpio_pullup_t::GPIO_PULLUP_ENABLE,  // Enable internal pull-up
    .pull_down_en = gpio_pulldown_t::GPIO_PULLDOWN_DISABLE,
    .intr_type = gpio_int_type_t::GPIO_INTR_DISABLE
  };
  gpio_config(&io_conf);

  GPIO::ActivatePin(PinNumber{6}, PinMode::WRITE);


  configureCanUart();
  vTaskDelay(100 / portTICK_PERIOD_MS);

  // Main Loop
  INFO("Beginning Main Loop");
  while (true) {
    // Prepare LEDs
    stick.reset();
    debugLed.reset();
    ledStrip.reset();

    // User update code
    int level = gpio_get_level(gpio_num_t::GPIO_NUM_6);
    INFO("Display! %i", level);
    if (level > 0) {
      debugLed.fill(Color::BLUE(.3f));
    }
    ledStrip.fill(Color::RED(.3f));

    constexpr auto greenLen = 11;
    constexpr auto yellowLen = 3;
    constexpr auto redLen = 2;

    static_assert(greenLen + yellowLen + redLen == stickLen);

    stick.fill(Color::GREEN(.02f), greenLen);
    stick.fill(Color::YELLOW(.02f), greenLen, yellowLen);
    stick.fill(Color::RED(.02f), greenLen + yellowLen, redLen);
    stick.fill(Color(.3f, 0.0f, 1.0f) * .02f);

    // Display LEDs
    debugLed.display();
    ledStrip.display();
    stick.display();

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

    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
}
