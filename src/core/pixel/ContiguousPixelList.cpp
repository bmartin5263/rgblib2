//
// Created by Brandon on 1/10/26.
//

#include "ContiguousPixelList.h"
#include "Assertions.h"
#include "PixelSlice.h"

namespace rgb {

auto ContiguousPixelList::slice(uint endExclusive) -> PixelSlice {
  return slice(0, endExclusive);
}

auto ContiguousPixelList::slice(uint start, uint endExclusive) -> PixelSlice {
  auto N = length();
  ASSERT(endExclusive < N, "Slice end is beyond length of chain");
  ASSERT(start < endExclusive, "Start must be before end");

  auto* newData = data() + start;
  auto length = endExclusive - start;
  return PixelSlice{newData, length};
}

auto ContiguousPixelList::get(uint pixel) const -> const Pixel* {
  ASSERT(pixel >= 0, "Pixel is negative");
  ASSERT(pixel < length(), "Pixel is out of bounds");
  return data() + pixel;
}

auto ContiguousPixelList::begin() -> Pixel* {
  return data();
}

auto ContiguousPixelList::begin() const -> const Pixel* {
  return data();
}

auto ContiguousPixelList::end() -> Pixel* {
  return data() + length();
}

auto ContiguousPixelList::end() const -> const Pixel* {
  return data() + length();
}

}