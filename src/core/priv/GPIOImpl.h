//
// Created by Brandon on 10/6/25.
//

#ifndef RGBLIB_GPIOIMPLEMENTATION_H
#define RGBLIB_GPIOIMPLEMENTATION_H

#include "GPIOESP32.h"

namespace rgb::priv {
class GPIOImpl : public GPIOESP32 {};
}

#endif //RGBLIB_GPIOIMPLEMENTATION_H
