//
// Created by Brandon on 1/19/25.
//

#ifndef RGBLIB_LEDSTRIP_H
#define RGBLIB_LEDSTRIP_H

#include <led_strip.h>
#include "Types.h"
#include "Pin.h"
#include "Assertions.h"
#include "Color.h"
#include "PixelList.h"
#include "PixelSlice.h"
#include "LEDCircuit.h"
#include "Log.h"
#include "LEDCore.h"

namespace rgb {

template <u16 N, const RgbFormat FORMAT, led_model_t MODEL = LED_MODEL_WS2812>
class LEDStrip : public PixelList, public LEDCircuit {
  Pixel pixels[N];
  led_strip_handle_t leds;
  int offset;
  PinNumber pin;
  bool reversed;
  bool started;

public:
  constexpr explicit LEDStrip(
    PinNumber pin,
    u16 offset = 0
  ):
    pixels{}, leds{}, offset{offset}, pin{pin}, reversed{false}, started{false}
  {
  }

  auto start() -> void {
    if (started) {
      return;
    }
    auto config = led_strip_config_t {
      .strip_gpio_num = pin.to<int>(),   // The GPIO that connected to the LED strip's data line
      .max_leds = N,          // The number of LEDs in the strip,
      .led_pixel_format = FORMAT.nativeFormat, // Pixel format of your LED strip
      .led_model = MODEL,            // LED strip model
      .flags {
        .invert_out = false,                // whether to invert the output signal
      }
    };

    // LED strip backend configuration: RMT
    auto rmtConfig = led_strip_rmt_config_t {
      .clk_src = RMT_CLK_SRC_DEFAULT,        // different clock source can lead to different power consumption
      .resolution_hz = 10 * 1000 * 1000,    // RMT counter clock frequency: 10MHz
      .mem_block_symbols = 0,
      .flags {
        .with_dma = false,               // DMA feature is available on ESP32-S3/C3/H2/P4 chips
      }
    };

    auto result = led_strip_new_rmt_device(&config, &rmtConfig, &leds);
    if (result == ESP_ERR_INVALID_ARG) {
      printf("Invalid Argument\n");
    }
    else if (result == ESP_ERR_NO_MEM) {
      printf("No Mem\n");
    }
    else if (result == ESP_FAIL) {
      printf("ESP_FAIL\n");
    }
    ASSERT(result == ESP_OK, "LED failed\n");

    printf("Started LEDs\n");
    started = true;
  }

  auto getHead() -> Pixel* override {
    return pixels;
  }

  auto getHead() const -> const Pixel* override {
    return pixels;
  }

  auto getSize() const -> u16 override {
    return N;
  }

  auto getOffset() const -> u16 {
    return offset;
  }

  auto reset() -> void override {
    clear();
  }

  auto display() -> void override {
    if (reversed) {
      for (u16 i = 0; i < N; ++i) {
        auto& pixel = pixels[mapPixelToLED(N - 1 - i)];
        FORMAT.writer(leds, i, pixel);
      }
    }
    else {
      for (u16 i = 0; i < N; ++i) {
        auto pixel = pixels[i];
        auto led = mapPixelToLED(i);
        FORMAT.writer(leds, led, pixel);
      }
    }
    led_strip_refresh(leds);
  }

  auto setOffset(int amount) -> void {
    offset = amount;
  }

  auto setReversed(bool value) -> void {
    reversed = value;
  }

  auto isReversed() const -> bool {
    return reversed;
  }

  auto toggleReversed() -> bool {
    auto previous = reversed;
    setReversed(!reversed);
    return previous;
  }

  auto mapPixelToLED(u16 pixel) -> u16 {
    return (pixel + offset) % N;
  }
};

}


#endif //RGBLIB_LEDSTRIP_H
