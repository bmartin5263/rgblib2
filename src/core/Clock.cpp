//
// Created by Brandon on 1/5/25.
//

#include "Clock.h"
#include "Log.h"
#include "System.h"

namespace rgb {

auto Clock::startTick() -> void {
  mTickStart = System::MilliTime();
  auto elapsed = mTickStart - mLastFrameRateCheck;

  if (elapsed >= 1'000'000) { // Update every second
    INFO("FPS: %llu", mFpsCounter);

    if (mFpsCounter < (mTargetFps / 2)) {
      if (!lowFpsDetected) {
        lowFpsDetected = true;
        // Do callback
      }
    }
    else {
      if (lowFpsDetected) {
        lowFpsDetected = false;
        // Do recover callback
      }
    }

    mLastFps = mFpsCounter;
    mFpsCounter = 0;
    mLastFrameRateCheck = mTickStart;
  }

  ++mFpsCounter;
  ++mFrames;
}

auto Clock::stopTick() -> void {
  auto stop = System::MilliTime();
  auto duration = stop - mTickStart;
  if (duration >= mMaxMillisecondsPerFrame) {
    return;
  }

  auto sleep = mMaxMillisecondsPerFrame - duration;
  mDelta = duration + sleep;
  System::MilliSleep(sleep);
}

auto Clock::Now() -> Timestamp {
  return Timestamp{System::MilliTime()};
}

}