//
// Created by Brandon on 3/26/25.
//

#include "Log.h"
#include "System.h"

namespace rgb::log {
bool initialized = false;

constexpr static auto MS_PER_HOUR = 1000 * 60 * 60;
constexpr static auto MS_PER_MINUTE = 1000 * 60;
constexpr static auto MS_PER_SECOND = 1000;

auto printHeader(const char* level, const char* function) -> void {
  auto milliseconds = system::milliTime();
  auto hours = milliseconds / MS_PER_HOUR;
  milliseconds %= MS_PER_HOUR;
  auto minutes = milliseconds / MS_PER_MINUTE;
  milliseconds %= MS_PER_MINUTE;
  auto seconds = milliseconds / MS_PER_SECOND;
  milliseconds %= MS_PER_SECOND;
  printf("%02llu:%02llu:%02llu.%03llu [%-5s] [%-60s] : ", hours, minutes, seconds, milliseconds, level, function);
}

}