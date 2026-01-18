//
// Created by Brandon on 1/18/26.
//

#ifndef RGBLIB_CHASEPROGRESSION_H
#define RGBLIB_CHASEPROGRESSION_H

#include "Types.h"

namespace rgb {

struct ChaseProgression {
  Duration duration{};
  bool isDelay{};

  static constexpr ChaseProgression ConstantTime(Duration duration) {
    return {duration, false};
  }

  static constexpr ChaseProgression ConstantSpeed(Duration delay) {
    return {delay, true};
  }
};


}

#endif //RGBLIB_CHASEPROGRESSION_H
