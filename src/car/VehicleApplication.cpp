//
// Created by Brandon on 10/22/25.
//

#include "VehicleApplication.h"
#include "Clock.h"
#include "Timer.h"
#include "EventBus.h"
#include "LEDCircuit.h"
#include "OTA.h"

using namespace rgb;

VehicleApplication* VehicleApplication::instance = nullptr;

void my_task(void* args) {
  auto vehicle = static_cast<Vehicle*>(args);
  auto TX = PinNumber{1};
  auto RX = PinNumber{2};
  while (true) {
    vehicle->connect(RX, TX);
    while (vehicle->isConnected()) {
      vehicle->update();
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

auto VehicleApplication::run() -> void {
  INFO("Tick rate: %d Hz\n", configTICK_RATE_HZ);
  INFO("Tick period: %lu ms\n", portTICK_PERIOD_MS);

  buildApplication();
  initialize();

  xTaskCreatePinnedToCore(
    my_task,
    "MyTask",
    2048,
    &vehicle,
    3,
    nullptr,
    1
  );

  PublishEvent<WakeEvent>();
  INFO("Started Application");

  clock.forever([&](){
    baseUpdate();
    baseDraw();
  });
}

auto VehicleApplication::baseUpdate() -> void {
//  OTA::Update();
  for (auto& sensor : mSensors) {
    sensor->doRead();
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

auto VehicleApplication::baseDraw() -> void {
  for (auto led : mLeds) {
    led->reset();
  }
  draw();
  Debug::Draw();
  for (auto led : mLeds) {
    led->display();
  }
}

auto VehicleApplication::buildApplication() -> void {
  instance = this;
  auto appBuilder = VehicleApplicationBuilder{};
  setup(appBuilder);

  mLeds = std::move(appBuilder.mLeds);
  mSensors = std::move(appBuilder.mSensors);
  mEventMap = std::move(appBuilder.mEventMap);
}

auto VehicleApplication::initialize() -> void {
  std::for_each(std::begin(mLeds), std::end(mLeds), [](auto led){ led->start(); });
  std::for_each(std::begin(mSensors), std::end(mSensors), [](auto sensor){ sensor->doStart(); });
  OTA::Start();
}

