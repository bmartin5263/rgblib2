//
// Created by Brandon on 10/28/25.
//

#ifndef RGBLIB_HUBAPPLICATION_H
#define RGBLIB_HUBAPPLICATION_H

#include <unordered_map>

#include "Clock.h"
#include "Func.h"
#include "EventConcept.h"
#include "EventType.h"

namespace rgb::home {

class HubApplication {
  using EventMap = std::unordered_map<uint, Consumer<const Event&>>;

public:

  HubApplication() = default;
  virtual ~HubApplication() = default;
  HubApplication(const HubApplication& rhs) = default;
  HubApplication(HubApplication&& rhs) noexcept = default;
  HubApplication& operator=(const HubApplication& rhs) = default;
  HubApplication& operator=(HubApplication&& rhs) noexcept = default;

  auto run() -> void;

  template<EventConcept T>
  auto publishEvent(const T& event = {}) -> void;

private:
  EventMap eventMap{};
};

template<EventConcept T>
auto HubApplication::publishEvent(const T& event) -> void {
  if (eventMap.contains(T::UID)) {
    eventMap[T::UID](event);
  }
}

}


#endif //RGBLIB_HUBAPPLICATION_H
