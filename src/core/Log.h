//
// Created by Brandon on 12/8/24.
//

#ifndef NEOPIXELS_LOG_H
#define NEOPIXELS_LOG_H

#include <cstdio>
#include "esp_log.h"

#if defined (RGB_DEBUG)
#if !defined(RGB_LOG_LEVEL)
#define RGB_LOG_LEVEL 2
#endif
#endif

#if defined (RGB_VERBOSE)
#undef RGB_LOG_LEVEL
#define RGB_LOG_LEVEL 3
#endif

namespace rgb {

class Logging {
public:
  static auto Start() -> void;
};

// Errors are always logged right now
#define ERROR(format, ...) do { \
  ESP_LOGE(__PRETTY_FUNCTION__, format, ##__VA_ARGS__); \
} while(false)

#if RGB_LOG_LEVEL >= 1
#define WARN(format, ...) do { \
  ESP_LOGW(__PRETTY_FUNCTION__, format, ##__VA_ARGS__); \
} while(false)
#else
#define WARN(format, ...)
#endif

#if RGB_LOG_LEVEL >= 2
#define INFO(format, ...) do { \
  ESP_LOGI(__PRETTY_FUNCTION__, format, ##__VA_ARGS__); \
} while(false)
#else
#define INFO(format, ...)
#endif

#if RGB_LOG_LEVEL >= 3
#define TRACE(format, ...) do { \
  ESP_LOGT(__PRETTY_FUNCTION__, format, ##__VA_ARGS__); \
} while(false)
#else
#define TRACE(format, ...)
#endif

}


#endif //NEOPIXELS_LOG_H
