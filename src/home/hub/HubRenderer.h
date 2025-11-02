//
// Created by Brandon on 10/28/25.
//

#ifndef RGBLIB_HUBRENDERER_H
#define RGBLIB_HUBRENDERER_H

#include "Clock.h"
#include "HubTask.h"

namespace rgb::home {

class HubRenderer : public HubTask<HubRenderer> {
public:
  using HubTask::HubTask;  // Inherit constructor

  static constexpr int FPS = 120;
  static constexpr const char* TASK_NAME = "Renderer";
  static constexpr size_t STACK_SIZE = 10000;
  static constexpr int PRIORITY = 1;
  static constexpr int CORE = 1;

  auto mainLoop() -> void;
};


}

#endif //RGBLIB_HUBRENDERER_H
