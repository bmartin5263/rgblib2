//
// Created by Brandon on 1/5/25.
//

#ifndef RGBLIB_PIXELLIST_H
#define RGBLIB_PIXELLIST_H

#include "Types.h"
#include "Pixel.h"

namespace rgb {

struct Point;
class PixelList {
public:
  [[nodiscard]] virtual auto length() const -> uint = 0;
  [[nodiscard]] virtual auto get(uint pixel) const -> const Pixel* = 0;

  [[nodiscard]] auto get(uint pixel) -> Pixel*;
  [[nodiscard]] auto operator[](uint pixel) -> Pixel&;
  [[nodiscard]] auto operator[](uint pixel) const -> const Pixel&;

  auto fill(const Color& color) -> void;
  auto fill(const Color& color, uint range) -> void;
  auto fill(const Color& color, uint start, uint endExclusive) -> void;
  auto clear() -> void;
  auto set(uint pixel, const Color& color) -> void;

  PixelList() = default;
  PixelList(const PixelList& rhs) = default;
  PixelList(PixelList&& rhs) noexcept = default;
  PixelList& operator=(const PixelList& rhs) = default;
  PixelList& operator=(PixelList&& rhs) noexcept = default;
  virtual ~PixelList() = default;
};

using PixelStrip = PixelList;
using PixelRing = PixelList;

}

#endif //RGBLIB_PIXELLIST_H
