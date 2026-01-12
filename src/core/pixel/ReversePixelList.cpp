//
// Created by Brandon on 1/10/26.
//

#include "ReversePixelList.h"

namespace rgb {

ReversePixelList::ReversePixelList(PixelList& source) :
  mSource(source)
{
}

auto ReversePixelList::length() const -> uint {
  return mSource.length();
}

auto ReversePixelList::get(uint pixel) const -> const Pixel* {
  return mSource.get(mSource.length() - 1 - pixel);
}

}
