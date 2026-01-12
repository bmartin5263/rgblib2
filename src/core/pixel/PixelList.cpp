//
// Created by Brandon on 1/5/25.
//

#include <utility>
#include "Assertions.h"
#include "PixelList.h"
#include "PixelSlice.h"
#include "Color.h"
#include "Point.h"

namespace rgb {

#ifdef RGB_DEBUG
constexpr auto DEBUG = true;
#else
constexpr auto DEBUG = false;
#endif

auto PixelList::fill(const Color& color) -> void {
  fill(color, 0, length());
}

auto PixelList::fill(const Color& color, uint range) -> void {
  fill(color, 0, range);
}

auto PixelList::fill(const Color& color, uint start, uint endExclusive) -> void {
  endExclusive = std::min(length(), endExclusive);
  for (auto i = start; i < endExclusive; ++i) {
    *get(i) = color;
  }
}

auto PixelList::clear() -> void {
  fill(Color::OFF());
}

auto PixelList::set(uint pixel, const Color& color) -> void {
  *get(pixel) = color;
}

auto PixelList::operator[](uint pixel) const -> const Pixel& {
  return *get(pixel);
}

auto PixelList::operator[](uint pixel) -> Pixel& {
  return const_cast<Pixel&>(std::as_const(*this).operator[](pixel));
}

auto PixelList::get(uint pixel) -> Pixel* {
  return const_cast<Pixel*>(std::as_const(*this).get(pixel));
}


}