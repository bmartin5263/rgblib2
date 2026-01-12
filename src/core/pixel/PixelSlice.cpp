//
// Created by Brandon on 1/19/25.
//

#include "PixelSlice.h"

namespace rgb {

PixelSlice::PixelSlice(Pixel* head, uint size) :
  mHead(head), mSize(size)
{
}

auto PixelSlice::length() const -> uint {
  return mSize;
}

auto PixelSlice::data() -> Pixel* {
  return mHead;
}

auto PixelSlice::data() const -> const Pixel* {
  return mHead;
}

}