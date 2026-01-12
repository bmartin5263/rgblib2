//
// Created by Brandon on 1/10/26.
//

#ifndef RGBLIB_APPLICATION_H
#define RGBLIB_APPLICATION_H

#include "EventType.h"

namespace rgb {

class Application {
public:
  static Application* instance;
  virtual auto publishSystemEvent(const SystemEvent& event) -> void = 0;
};

}

#endif //RGBLIB_APPLICATION_H
