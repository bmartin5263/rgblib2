//
// Created by Brandon on 9/14/25.
//

#ifndef RGBLIB2_SYSTEM_H
#define RGBLIB2_SYSTEM_H

#include "Types.h"
#include "esp_timer.h"
#include "esp_task.h"
#include "HostSystemImpl.h"

namespace rgb {

class System {
public:
  static auto MicroTime() -> microseconds_t;
  static auto MilliTime() -> milliseconds_t;
  static auto MicroSleep(microseconds_t time) -> void;
  static auto MilliSleep(milliseconds_t time) -> void;

private:
  static auto Implementation() -> priv::HostSystemImpl&;
  priv::HostSystemImpl impl;
};

auto System::Implementation() -> priv::HostSystemImpl& {
  static System instance;
  return instance.impl;
}

auto System::MicroTime() -> microseconds_t {
  return Implementation().microTime();
}

auto System::MilliTime() -> milliseconds_t {
  return Implementation().milliTime();
}

auto System::MicroSleep(rgb::microseconds_t time) -> void {
  return Implementation().microSleep(time);
}

auto System::MilliSleep(rgb::milliseconds_t time) -> void {
  return Implementation().milliSleep(time);
}

}

#endif //RGBLIB2_SYSTEM_H
