//
// Created by Brandon on 1/5/25.
//

#include "Clock.h"
#include "Log.h"
#include "System.h"

namespace rgb {

auto Clock::Instance() -> Clock& {
  static Clock instance;
  return instance;
}

auto Clock::start(frames_t targetFps) -> void {
  this->mTargetFps = targetFps;
  this->mMaxMicrosPerFrame = 1000000 / targetFps;
}

auto Clock::startTick() -> void {
  mTickStart = System::MicroTime();
  auto elapsed = mTickStart - mLastFrameRateCheck;

  if (elapsed >= 1'000'000) { // Update every second
    INFO("FPS: %llu", mFpsCounter);

    // TODO
//    if (mFpsCounter < (mTargetFps / 2)) {
//      gpio_set_level(static_cast<gpio_num_t>(config::LED_DROPPING_FRAMES), 0);
//    }
//    else {
//      gpio_set_level(static_cast<gpio_num_t>(config::LED_DROPPING_FRAMES), 1);
//    }

    mLastFps = mFpsCounter;
    mFpsCounter = 0;
    mLastFrameRateCheck = mTickStart;

  }

  ++mFpsCounter;
  ++mFrames;
}

auto Clock::frames() const -> frames_t {
  return mFrames;
}

auto Clock::milli() const -> milliseconds_t {
  return System::MicroTime() / 1000;
}

auto Clock::stopTick() -> void {
  auto stop = System::MicroTime();
  auto duration = stop - mTickStart;
  if (duration >= mMaxMicrosPerFrame) {
    return;
  }

  auto sleep = mMaxMicrosPerFrame - duration;
  mDelta = duration + sleep;
  System::MicroSleep(sleep);
}

auto Clock::Start(frames_t fps) -> void {
  Instance().start(fps);
}

auto Clock::StartTick() -> void {
  Instance().startTick();
}

auto Clock::Frames() -> frames_t {
  return Instance().frames();
}

auto Clock::Milli() -> milliseconds_t {
  return Instance().milli();
}

auto Clock::Now() -> Timestamp {
  return Timestamp{System::MicroTime()};
}

auto Clock::Delta() -> Duration {
  return Instance().delta();
}

auto Clock::Fps() -> frames_t {
  return Instance().fps();
}

auto Clock::StopTick() -> void {
  Instance().stopTick();
}

auto Clock::delta() const -> Duration {
  return Duration { mDelta };
}

auto Clock::fps() const -> frames_t {
  return mLastFps;
}

}