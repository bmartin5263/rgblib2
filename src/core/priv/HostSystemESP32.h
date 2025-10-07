//
// Created by Brandon on 10/6/25.
//

#ifndef RGBLIB_HOSTSYSTEMESP32_H
#define RGBLIB_HOSTSYSTEMESP32_H

#include "Types.h"
#include "HostSystemBase.h"
#include "esp_timer.h"
#include "esp_task.h"

namespace rgb::priv {

class HostSystemESP32 : public HostSystemBase {
public:
  auto microTime() -> microseconds_t;
  auto milliTime() -> milliseconds_t;
  auto microSleep(microseconds_t time) -> void;
  auto milliSleep(milliseconds_t time) -> void;
};

auto HostSystemESP32::microTime() -> microseconds_t {
  return esp_timer_get_time();
}

auto HostSystemESP32::milliTime() -> milliseconds_t {
  return microTime() / 1000;
}

auto HostSystemESP32::microSleep(microseconds_t time) -> void {
  return vTaskDelay(time);
}

auto HostSystemESP32::milliSleep(milliseconds_t time) -> void {
  return vTaskDelay(time / portTICK_PERIOD_MS);
}

}

#endif //RGBLIB_HOSTSYSTEMESP32_H
