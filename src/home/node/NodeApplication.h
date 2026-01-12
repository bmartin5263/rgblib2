//
// Created by Brandon on 10/28/25.
//

#ifndef RGBLIB_NODEAPPLICATION_H
#define RGBLIB_NODEAPPLICATION_H

#include <unordered_map>

#include "Clock.h"
#include "Func.h"
#include "EventType.h"

namespace rgb::home {

class NodeApplication {
public:

  NodeApplication() = default;
  virtual ~NodeApplication() = default;
  NodeApplication(const NodeApplication& rhs) = default;
  NodeApplication(NodeApplication&& rhs) noexcept = default;
  NodeApplication& operator=(const NodeApplication& rhs) = default;
  NodeApplication& operator=(NodeApplication&& rhs) noexcept = default;

  auto run() -> void;
private:
};


}

#endif //RGBLIB_NODEAPPLICATION_H
