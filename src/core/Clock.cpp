//
// Created by Brandon on 1/5/25.
//

#include "Clock.h"
#include "Log.h"
#include "System.h"

namespace rgb {

auto Clock::Now() -> Timestamp {
  return Timestamp{System::MicroTime()};
}

auto Clock::printStats() -> void {
  auto elapsed = mTickStart - mLastFrameRateCheck;
  if (elapsed > 1000) {
    INFO("FPS: %llu", mLastFps);
  }
}

}