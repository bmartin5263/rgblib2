//
// Created by Brandon on 10/28/25.
//

#ifndef RGBLIB_HUBUI_H
#define RGBLIB_HUBUI_H

#include "Clock.h"

namespace rgb::home {

class HubApplication;
class HubUI {
public:
  static auto Launch(HubApplication& application) -> void;
  static auto MainLoop(void* application) -> void;
  explicit HubUI(HubApplication& application);
  auto mainLoop() -> void;

private:
  HubApplication& application;
  Clock clock{1000};

};


}

#endif //RGBLIB_HUBUI_H
