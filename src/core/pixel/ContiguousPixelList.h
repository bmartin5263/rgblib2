//
// Created by Brandon on 1/10/26.
//

#ifndef RGBLIB_CONTIGUOUSPIXELLIST_H
#define RGBLIB_CONTIGUOUSPIXELLIST_H

#include "PixelList.h"

namespace rgb {
class PixelSlice;
class ContiguousPixelList : public PixelList {
public:
  virtual auto data() -> Pixel* = 0;
  virtual auto data() const -> const Pixel* = 0;

  auto slice(uint length) -> PixelSlice;
  auto slice(uint start, uint length) -> PixelSlice;
  auto get(uint pixel) const -> const Pixel* override;

  auto begin() -> Pixel*;
  auto begin() const -> const Pixel*;
  auto end() -> Pixel*;
  auto end() const -> const Pixel*;
};


}

#endif //RGBLIB_CONTIGUOUSPIXELLIST_H
