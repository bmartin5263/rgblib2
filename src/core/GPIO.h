//
// Created by Brandon on 10/6/25.
//

#ifndef RGBLIB_GPIO_H
#define RGBLIB_GPIO_H

#include "Types.h"
#include "priv/GPIOImpl.h"
#include "Pin.h"

namespace rgb {

struct GPIOConfiguration {
  PinMode mode;
};

class GPIO {
public:
  static auto ActivatePin(PinNumber pin, PinMode mode) -> void;

private:
  static auto Implementation() -> priv::GPIOImpl&;
  priv::GPIOImpl impl;
};

auto GPIO::Implementation() -> priv::GPIOImpl& {
  static GPIO instance;
  return instance.impl;
}

auto GPIO::ActivatePin(PinNumber pin, PinMode mode) -> void {
  Implementation().activatePin(pin, mode);
}

}


#endif //RGBLIB_GPIO_H
