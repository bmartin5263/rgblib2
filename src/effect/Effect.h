//
// Created by Brandon on 1/18/26.
//

#ifndef RGBLIB_EFFECT_H
#define RGBLIB_EFFECT_H

#include "Iterable.h"
#include "Types.h"

namespace rgb {

class PixelList;
class Effect {
public:

  virtual auto reset(Timestamp now) -> void {};
  virtual auto update(Timestamp now) -> void {};
  virtual auto draw(Timestamp now, PixelList& pixels) -> void = 0;

  auto draw(Timestamp now, Iterable<PixelList*> pixelLists) {
    for (auto pixelList : pixelLists) {
      draw(now, *pixelList);
    }
  }

  Effect() = default;
  Effect(const Effect& rhs) = default;
  Effect(Effect&& rhs) noexcept = default;
  Effect& operator=(const Effect& rhs) = default;
  Effect& operator=(Effect&& rhs) noexcept = default;
  virtual ~Effect() = default;
};

}

#endif //RGBLIB_EFFECT_H
