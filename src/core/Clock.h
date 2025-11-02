//
// Created by Brandon on 1/5/25.
//

#ifndef RGBLIB_CLOCK_H
#define RGBLIB_CLOCK_H

#include "Types.h"

namespace rgb {

class Clock {
public:
  explicit Clock(frames_t fps): mTargetFps(fps), mMaxMillisecondsPerFrame(1000 / fps)  {}

  template<class T>
  auto forever(const T& tickFunction) -> void;

  static auto Now() -> Timestamp;

private:

  auto startTick() -> void;
  auto stopTick() -> void;

  frames_t mFrames{};
  frames_t mFpsCounter{};
  frames_t mLastFps{};
  frames_t mTargetFps; // for detecting low FPS
  u64 mNextFrame{};
  microseconds_t mTickStart{};
  microseconds_t mLastFrameRateCheck{};
  microseconds_t mMaxMillisecondsPerFrame;
  microseconds_t mDelta{};
  bool lowFpsDetected{};
};

template<class T>
auto Clock::forever(const T& tickFunction) -> void {
  while (true) {
    startTick();
    tickFunction();
    stopTick();
  }
}

}


#endif //RGBLIB_CLOCK_H
