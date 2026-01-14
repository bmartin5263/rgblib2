//
// Created by Brandon on 1/5/25.
//

#ifndef RGBLIB_CLOCK_H
#define RGBLIB_CLOCK_H

#include "Types.h"
#include "System.h"
#include "Log.h"
#define INCLUDE_xTaskDelayUntil 1

namespace rgb {

class Clock {
public:
  explicit Clock(frames_t fps): mTargetFps(fps), mMaxMillisecondsPerFrame(1000 / fps)  {}

  template<class T>
  auto forever(const T& tickFunction) -> void;

  auto printStats() -> void;

  static auto Now() -> Timestamp;

private:

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
  INFO("Tick rate: %d Hz\n", configTICK_RATE_HZ);
  INFO("Tick period: %lu ms\n", portTICK_PERIOD_MS);

  auto xLastWakeTime = xTaskGetTickCount();
  auto xFrequency = pdMS_TO_TICKS(5); // 5ms = 200 FPS

  while (true) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    mTickStart = System::MilliTime();
    auto elapsed = mTickStart - mLastFrameRateCheck;

    if (elapsed >= 1000) { // Update every second
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

    tickFunction();
  }
}

}


#endif //RGBLIB_CLOCK_H
