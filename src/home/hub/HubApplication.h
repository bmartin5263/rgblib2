//
// Created by Brandon on 10/28/25.
//

#ifndef RGBLIB_HUBAPPLICATION_H
#define RGBLIB_HUBAPPLICATION_H

#include <unordered_map>

#include "Clock.h"
#include "Func.h"
#include "EventType.h"

namespace rgb::home {

class HubApplication {
public:

  HubApplication() = default;
  virtual ~HubApplication() = default;
  HubApplication(const HubApplication& rhs) = default;
  HubApplication(HubApplication&& rhs) noexcept = default;
  HubApplication& operator=(const HubApplication& rhs) = default;
  HubApplication& operator=(HubApplication&& rhs) noexcept = default;

  auto run() -> void;

private:
};

}


#endif //RGBLIB_HUBAPPLICATION_H
