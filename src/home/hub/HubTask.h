//
// Created by Brandon on 10/28/25.
//

#ifndef RGBLIB_HUBTASK_H
#define RGBLIB_HUBTASK_H

#include "Clock.h"

namespace rgb::home {

class HubApplication;
template<typename Derived>
class HubTask {
public:
  explicit HubTask(HubApplication& application)
    : application(application), clock{Derived::FPS} {}

  static auto Launch(HubApplication& application) -> void {
    xTaskCreatePinnedToCore(
      Derived::MainLoop,
      Derived::TASK_NAME,
      Derived::STACK_SIZE,
      &application,
      Derived::PRIORITY,
      nullptr,
      Derived::CORE
    );
  }

  static auto MainLoop(void* parameter) -> void {
    auto* app = static_cast<HubApplication*>(parameter);
    auto task = Derived{*app};
    task.mainLoop();  // Calls derived class's mainLoop
  }

protected:
  HubApplication& application;
  Clock clock;
};

}

#endif //RGBLIB_HUBTASK_H
