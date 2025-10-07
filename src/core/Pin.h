//
// Created by Brandon on 10/6/25.
//

#ifndef RGBLIB_PIN_H
#define RGBLIB_PIN_H

#include "Types.h"
#include "Handle.h"

namespace rgb {

enum class PinMode {
  READ,
  WRITE
};

struct PinNumber {
  constexpr explicit PinNumber(u8 value): value(value) {}
  u8 value;
};

struct TurnOffPin {
  auto operator()(PinNumber number) -> void;
};

using Pin = Handle<PinNumber, TurnOffPin>;

}

#endif //RGBLIB_PIN_H
