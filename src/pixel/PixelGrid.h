//
// Created by Brandon on 1/5/25.
//

#ifndef RGBLIB_PIXELGRID_H
#define RGBLIB_PIXELGRID_H

#include "PixelList.h"

namespace rgb {

struct Point;
class PixelGrid : public PixelList {
public:
  [[nodiscard]]
  virtual auto get(Point point) -> Pixel* = 0;
  [[nodiscard]]
  virtual auto operator[](uint column, uint row) -> Pixel& = 0;
  virtual auto set(Point point, const Color& color) -> void = 0;
};

}

#endif //RGBLIB_PIXELGRID_H
