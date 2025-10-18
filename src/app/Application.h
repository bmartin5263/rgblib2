//
// Created by Brandon on 10/11/25.
//

#ifndef RGBLIB_APPLICATION_H
#define RGBLIB_APPLICATION_H

#include "ApplicationBuilder.h"
#include "Clock.h"
#include "Timer.h"

namespace rgb {

class Application {
public:
  Application() = default;
  virtual ~Application() = default;
  Application(const Application& rhs) = default;
  Application(Application&& rhs) noexcept = default;
  Application& operator=(const Application& rhs) = default;
  Application& operator=(Application&& rhs) noexcept = default;

  auto run() -> void;
protected:
  virtual constexpr auto setup(ApplicationBuilder& setup) -> void = 0;
  virtual auto update() -> void = 0;
  virtual auto draw() -> void = 0;

private:
  auto baseUpdate() -> void;
  auto baseDraw() -> void;

  std::vector<LEDCircuit*> mLeds{};
  std::vector<Runnable> mSensors{};
  std::unordered_map<uint, Consumer<const Event&>> mEventMap{};
};

auto Application::run() -> void {
  auto appBuilder = ApplicationBuilder{};
  setup(appBuilder);

  mLeds = std::move(appBuilder.mLeds);
  mSensors = std::move(appBuilder.mSensors);
  mEventMap = std::move(appBuilder.mEventMap);

  INFO("Starting Application with %i leds", mLeds.size());

  std::for_each(std::begin(mLeds), std::end(mLeds), [](auto led){ led->start(); });

  auto fps = 300;
  Clock::Start(fps);
  while (true) {
    Clock::StartTick();
    baseUpdate();
    baseDraw();
    Clock::StopTick();
  }
}

auto Application::baseUpdate() -> void {
  for (auto& sensor : mSensors) {
    sensor();
  }
//  if constexpr (Wifi::Enabled()) { TODO
//    Wifi::Update();
//  }
//  if constexpr (OTASupport::Enabled()) { TODO
//    OTASupport::Update();
//  }

  Timer::ProcessTimers();
  update();
}

auto Application::baseDraw() -> void {
  std::for_each(std::begin(mLeds), std::end(mLeds), [](auto led){ led->reset(); });
  draw();
  Debug::Draw();
  std::for_each(std::begin(mLeds), std::end(mLeds), [](auto led){ led->display(); });
}

}

#endif //RGBLIB_APPLICATION_H
