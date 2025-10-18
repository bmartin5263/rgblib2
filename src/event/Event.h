//
// Created by Brandon on 10/14/25.
//

#ifndef RGBLIB_EVENT_H
#define RGBLIB_EVENT_H

#include <variant>

namespace rgb {

// Base event concept (optional, for type safety)
template<typename T>
concept EventConcept = requires {
  typename T::event_tag; // Each event must have an event_tag type
  { T::uid } -> std::convertible_to<uint>;
  requires std::same_as<decltype(T::uid), const uint>;
};

struct MyEvent {
  using event_tag = MyEvent;
  static constexpr uint uid = 10u;
};

using Event = std::variant<MyEvent>;

}

#endif //RGBLIB_EVENT_H
