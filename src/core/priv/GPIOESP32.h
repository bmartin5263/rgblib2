//
// Created by Brandon on 10/6/25.
//

#ifndef RGBLIB_GPIOESP32_H
#define RGBLIB_GPIOESP32_H

#include "GPIOBase.h"

namespace rgb::priv {

class GPIOESP32 : public GPIOBase {
public:
  static constexpr auto DEFAULT_PULL_UP = gpio_pullup_t::GPIO_PULLUP_ENABLE;
  static constexpr auto DEFAULT_PULL_DOWN = gpio_pulldown_t::GPIO_PULLDOWN_DISABLE;
  static constexpr auto DEFAULT_INTR_TYPE = gpio_int_type_t::GPIO_INTR_DISABLE;

  auto activatePin(PinNumber pin, PinMode mode) -> void;
private:
  static constexpr auto map(PinMode mode) -> gpio_mode_t;
};

auto GPIOESP32::activatePin(PinNumber pin, PinMode mode) -> void {
  auto io_conf = gpio_config_t {
    .pin_bit_mask = (1ULL << pin.value),
    .mode = map(mode),
    .pull_up_en = DEFAULT_PULL_UP,  // Enable internal pull-up
    .pull_down_en = DEFAULT_PULL_DOWN,
    .intr_type = DEFAULT_INTR_TYPE
  };
  gpio_config(&io_conf);
}

constexpr auto GPIOESP32::map(rgb::PinMode mode) -> gpio_mode_t {
  switch (mode) {
    case PinMode::WRITE:
      return gpio_mode_t::GPIO_MODE_OUTPUT;
  }
}

}
#endif //RGBLIB_GPIOESP32_H
