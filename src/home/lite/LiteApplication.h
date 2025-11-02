//
// Created by Brandon on 10/28/25.
//

#ifndef RGBLIB_LITEAPPLICATION_H
#define RGBLIB_LITEAPPLICATION_H

#include <unordered_map>

#include "Clock.h"
#include "Func.h"
#include "EventConcept.h"
#include "EventType.h"

namespace rgb::home {

class LiteApplication {
  using EventMap = std::unordered_map<uint, Consumer<const Event&>>;

public:

  LiteApplication() = default;
  virtual ~LiteApplication() = default;
  LiteApplication(const LiteApplication& rhs) = default;
  LiteApplication(LiteApplication&& rhs) noexcept = default;
  LiteApplication& operator=(const LiteApplication& rhs) = default;
  LiteApplication& operator=(LiteApplication&& rhs) noexcept = default;

  auto run() -> void;

  template<EventConcept T>
  auto publishEvent(const T& event = {}) -> void;

private:
  EventMap eventMap{};
};

template<EventConcept T>
auto LiteApplication::publishEvent(const T& event) -> void {
  if (eventMap.contains(T::UID)) {
    eventMap[T::UID](event);
  }
}


}

#endif //RGBLIB_LITEAPPLICATION_H
