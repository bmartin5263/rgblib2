//
// Created by Brandon on 10/22/25.
//

#ifndef RGBLIB_EVENTTYPE_H
#define RGBLIB_EVENTTYPE_H

#include <variant>
#include "Types.h"

namespace rgb {

struct MyEvent {
  static constexpr uint UID = 0u;
};

struct ApplicationStartedEvent {
  static constexpr uint UID = 1u;
};

using Event = std::variant<
  ApplicationStartedEvent,
  MyEvent
>;

}
#endif //RGBLIB_EVENTTYPE_H
