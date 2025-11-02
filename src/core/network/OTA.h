//
// Created by Brandon on 10/25/25.
//

#ifndef RGBLIB_OTA_H
#define RGBLIB_OTA_H

#include "OTAImpl.h"

namespace rgb {

class OTA {
public:
  static auto Start() -> bool { return Implementation().start(); }
  static auto Update() -> void { Implementation().update(); };

private:
  static auto Implementation() -> priv::OTAImpl&;
};

}


#endif //RGBLIB_OTA_H
