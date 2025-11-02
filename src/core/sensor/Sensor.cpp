//
// Created by Brandon on 10/25/25.
//

#include "Sensor.h"

namespace rgb {

auto Sensor::doRead() -> void {
  read();
}

auto Sensor::doStart() -> bool {
  return start();
}

}