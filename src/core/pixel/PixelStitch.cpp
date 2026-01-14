//
// Created by Brandon on 1/11/26.
//

#include "PixelStitch.h"
#include "Assertions.h"

namespace rgb {

PixelStitch::PixelStitch(PixelList* leftStart, uint leftLength, PixelList* rightStart, uint rightLength):
  leftStart(leftStart), rightStart(rightStart), leftLength(leftLength), rightLength(rightLength) {
  ASSERT(leftLength > 0, "Left Stitch cannot be 0-length");
  ASSERT(rightLength > 0, "Right Stitch cannot be 0-length");
}

auto PixelStitch::length() const -> uint {
  return leftLength + rightLength;
}

auto PixelStitch::get(uint pixel) const -> const Pixel* {
  if (pixel < leftLength) {
    return leftStart->get(pixel);
  }
  else {
    return rightStart->get(pixel);
  }
}

}