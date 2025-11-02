//
// Created by Brandon on 10/14/25.
//

#ifndef RGBLIB_EVENTCONCEPT_H
#define RGBLIB_EVENTCONCEPT_H

#include "Types.h"

namespace rgb {

template<typename T>
concept EventConcept = requires {
  { T::UID } -> std::convertible_to<uint>;
  requires std::same_as<decltype(T::UID), const uint>;
};

}

#endif //RGBLIB_EVENTCONCEPT_H
