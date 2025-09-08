//
// Created by Brandon on 1/19/25.
//

#ifndef RGBLIB_LEDMATRIX_H
#define RGBLIB_LEDMATRIX_H

#include <led_strip.h>
#include "Types.h"
#include "Assertions.h"
#include "PixelGrid.h"
#include "LEDCircuit.h"
#include "Point.h"

namespace rgb {

template <u16 COLUMNS, u16 ROWS, bool STAGGER = false>
class LEDMatrix : public PixelGrid, public LEDCircuit {
public:
  static constexpr auto N = COLUMNS * ROWS;

  constexpr explicit LEDMatrix(pin_num pin, led_pixel_format_t format, u16 offset = 0):
  pixels{}, leds{}, offset{offset}, format{format}, pin{pin}, reversed{false}, started{false}
  {
  }

  auto start() -> void {
    if (started) {
      return;
    }
    auto config = led_strip_config_t {
      .strip_gpio_num = pin,   // The GPIO that connected to the LED strip's data line
      .max_leds = N,          // The number of LEDs in the strip,
      .led_pixel_format = format, // Pixel format of your LED strip
      .led_model = LED_MODEL_WS2812,            // LED strip model
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

    ASSERT(led_strip_new_rmt_device(&config, &rmtConfig, &leds), "Failed to create LEDs");
    INFO("LED strip initialized");

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

  int zigzagToLinearIndex(int index) {
    int row = index / COLUMNS;
    int columnInRow = index % COLUMNS;

    if (row % 2 == 0) {
      return row * COLUMNS + (COLUMNS - 1 - columnInRow);
    } else {
      return row * COLUMNS + columnInRow;
    }
  }

  auto display() -> void override {
    if (reversed) {
      for (u16 i = 0; i < N; ++i) {
        auto& c = pixels[mapPixelToLED(N - 1 - i)];
        led_strip_set_pixel_rgbw(leds, i, FloatToByte(c.r), FloatToByte(c.g), FloatToByte(c.b), FloatToByte(c.w));
      }
    }
    else {
      if constexpr (STAGGER) {
        for (u16 i = 0; i < N; ++i) {
          auto pixel = pixels[i];
          auto led = mapPixelToLED(i);
          led_strip_set_pixel_rgbw(leds, zigzagToLinearIndex(led), FloatToByte(pixel.r), FloatToByte(pixel.g), FloatToByte(pixel.b), FloatToByte(pixel.w));
        }
      }
      else {
        for (u16 i = 0; i < N; ++i) {
          auto pixel = pixels[i];
          auto led = mapPixelToLED(i);
          led_strip_set_pixel_rgbw(leds, led, FloatToByte(pixel.r), FloatToByte(pixel.g), FloatToByte(pixel.b), FloatToByte(pixel.w));
        }
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

  auto get(Point point) -> Pixel* override {
    ASSERT(point.x >= 0, "Point.X is negative");
    ASSERT(point.y >= 0, "Pixel.Y is negative");
    ASSERT(point.x < COLUMNS, "Pixel.X is out of bounds");
    ASSERT(point.y < ROWS, "Pixel.Y is out of bounds");
    return getHead() + ((point.y * COLUMNS) + point.x);
  }

  auto set(Point point, const Color& color) -> void override {
    *(getHead() + ((point.y * COLUMNS) + point.x)) = color;
  }

  auto operator[](Point point) -> Color& override {
    return *(getHead() + ((point.y * COLUMNS) + point.x));
  }

private:
  Pixel pixels[N];
  led_strip_handle_t leds;
  int offset;
  led_pixel_format_t format;
  pin_num pin;
  bool reversed;
  bool started;
};

}


#endif //RGBLIB_LEDMATRIX_H
