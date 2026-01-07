//
// Created by Brandon on 10/22/25.
//

#ifndef RGBLIB_EVENTTYPE_H
#define RGBLIB_EVENTTYPE_H

#include <variant>
#include "Types.h"

namespace rgb {


struct WakeEvent {
  static constexpr uint UID = 2u;
};

struct SleepEvent {
  static constexpr uint UID = 3u;
};

struct NullEvent {
  static constexpr uint UID = 3u;
};

template<typename ...T>
using Event2 = std::variant<
  WakeEvent,
  SleepEvent,
  T...
>;

using Event = std::variant<
  WakeEvent,
  SleepEvent
>;

}
#endif //RGBLIB_EVENTTYPE_H
