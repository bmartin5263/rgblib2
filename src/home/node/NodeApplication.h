//
// Created by Brandon on 10/28/25.
//

#ifndef RGBLIB_NODEAPPLICATION_H
#define RGBLIB_NODEAPPLICATION_H

#include <unordered_map>

#include "Clock.h"
#include "Func.h"
#include "EventConcept.h"
#include "EventType.h"

namespace rgb::home {

class NodeApplication {
  using EventMap = std::unordered_map<uint, Consumer<const Event&>>;

public:

  NodeApplication() = default;
  virtual ~NodeApplication() = default;
  NodeApplication(const NodeApplication& rhs) = default;
  NodeApplication(NodeApplication&& rhs) noexcept = default;
  NodeApplication& operator=(const NodeApplication& rhs) = default;
  NodeApplication& operator=(NodeApplication&& rhs) noexcept = default;

  auto run() -> void;

  template<EventConcept T>
  auto publishEvent(const T& event = {}) -> void;

private:
  EventMap eventMap{};
};

template<EventConcept T>
auto NodeApplication::publishEvent(const T& event) -> void {
  if (eventMap.contains(T::UID)) {
    eventMap[T::UID](event);
  }
}


}

#endif //RGBLIB_NODEAPPLICATION_H
