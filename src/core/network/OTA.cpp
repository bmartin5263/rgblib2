//
// Created by Brandon on 10/25/25.
//

#include "OTA.h"

namespace rgb {

auto OTA::Implementation() -> rgb::priv::OTAImpl& {
  static auto impl = priv::OTAImpl{};
  return impl;
}


}