//
// Created by Brandon on 9/14/25.
//

#ifndef RGBLIB2_SYSTEM_H
#define RGBLIB2_SYSTEM_H

#include "Types.h"
#include "esp_timer.h"

namespace rgb::system {

static auto microTime() -> microseconds_t {
  return esp_timer_get_time();
}

static auto milliTime() -> milliseconds_t {
  return microTime() / 1000;
}

}

#endif //RGBLIB2_SYSTEM_H
