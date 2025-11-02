//
// Created by Brandon on 10/25/25.
//

#ifndef RGBLIB_IRREMOTE_H
#define RGBLIB_IRREMOTE_H

#include "Sensor.h"

namespace rgb {

class IRRemote : public Sensor {
protected:
  auto start() -> bool override;
  auto read() -> void override;

private:

};

}


#endif //RGBLIB_IRREMOTE_H
