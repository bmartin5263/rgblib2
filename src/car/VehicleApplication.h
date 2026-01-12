//
// Created by Brandon on 10/11/25.
//

#ifndef RGBLIB_VEHICLEAPPLICATION_H
#define RGBLIB_VEHICLEAPPLICATION_H

#include "Application.h"
#include "VehicleApplicationBuilder.h"
#include "Clock.h"
#include "Vehicle.h"
#include "Timer.h"
#include "LEDCircuit.h"
#include "OTA.h"
#include "LEDs.h"

namespace rgb {

void updateVehicle(void* args) {
  INFO("Update Vehicle Started");
  auto vehicle = static_cast<Vehicle*>(args);
  auto TX = PinNumber{42};
  auto RX = PinNumber{41};
  auto now = Clock::Now();

  Timestamp connectAgain;
  if (vehicle->connect(RX, TX)) {
    connectAgain = Timestamp::Max();
  }
  else {
    connectAgain = now + Duration::Seconds(1);
  }
  while (true) {
    now = Clock::Now();
    if (now >= connectAgain) {
      if (vehicle->connect(RX, TX)) {
        connectAgain = Timestamp::Max();
      }
      else {
        connectAgain = now + Duration::Seconds(1);
      }
    }

    vehicle->update();

    if (!vehicle->isConnected() && connectAgain == Timestamp::Max()) {
      connectAgain = now + Duration::Seconds(5);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

template<typename ...UserEvents>
class VehicleApplication : public Application {
public:
  using AnyEvent = Event<UserEvents...>;
  using Builder = VehicleApplicationBuilder<UserEvents...>;

  VehicleApplication() = default;
  virtual ~VehicleApplication() = default;
  VehicleApplication(const VehicleApplication& rhs) = delete;
  VehicleApplication(VehicleApplication&& rhs) noexcept = delete;
  VehicleApplication& operator=(const VehicleApplication& rhs) = delete;
  VehicleApplication& operator=(VehicleApplication&& rhs) noexcept = delete;

  auto run() -> void;
  auto publishSystemEvent(const SystemEvent& event) -> void override;

  static auto PublishEvent(const AnyEvent& event) -> void;

protected:
  virtual constexpr auto setup(Builder& setup) -> void = 0;
  virtual auto update() -> void = 0;
  virtual auto draw() -> void = 0;
  Vehicle vehicle{};

private:
  auto buildApplication() -> void;
  auto initialize() -> void;
  auto baseUpdate() -> void;
  auto baseDraw() -> void;


  Clock clock{300};
  std::vector<LEDCircuit*> mLeds{};
  std::vector<Sensor*> mSensors{};
  std::unordered_map<size_t, Consumer<const Event<UserEvents...>&>> mEventMap{};
};

template<typename ...UserEvents>
auto VehicleApplication<UserEvents...>::run() -> void {
  INFO("Tick rate: %d Hz\n", configTICK_RATE_HZ);
  INFO("Tick period: %lu ms\n", portTICK_PERIOD_MS);
  buildApplication();
  initialize();

  xTaskCreatePinnedToCore(
    updateVehicle,
    "UpdateVehicle",
    8192,
    &vehicle,
    3,
    nullptr,
    1
  );

  PublishEvent(WakeEvent{});
  publishSystemEvent(WakeEvent{});
  INFO("Started Application");

  clock.forever([&](){
    baseUpdate();
    baseDraw();
  });
}

template<typename ...UserEvents>
auto VehicleApplication<UserEvents...>::baseUpdate() -> void {
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

template<typename ...UserEvents>
auto VehicleApplication<UserEvents...>::baseDraw() -> void {
  for (auto led : mLeds) {
    if (led != &debugLed) {
      led->reset();
    }
  }
  draw();
  Debug::Draw();
  for (auto led : mLeds) {
    led->display();
  }
}

template<typename ...UserEvents>
auto VehicleApplication<UserEvents...>::buildApplication() -> void {
  instance = this;
  auto appBuilder = VehicleApplicationBuilder<UserEvents...>{};
  setup(appBuilder);

  mLeds = std::move(appBuilder.mLeds);
  mSensors = std::move(appBuilder.mSensors);
  mEventMap = std::move(appBuilder.mEventMap);
}

template<typename ...UserEvents>
auto VehicleApplication<UserEvents...>::initialize() -> void {
  std::for_each(std::begin(mLeds), std::end(mLeds), [](auto led){ led->start(); });
  std::for_each(std::begin(mSensors), std::end(mSensors), [](auto sensor){ sensor->doStart(); });
  OTA::Start();
}

template<typename... UserEvents>
auto VehicleApplication<UserEvents...>::publishSystemEvent(const SystemEvent& systemEvent) -> void {
  auto event = std::visit([](auto&& e) {
    return AnyEvent{e};
  }, systemEvent);
  auto uid = systemEvent.index();
  if (mEventMap.contains(uid)) {
    mEventMap[uid](event);
  }
}

template<typename ...UserEvents>
auto VehicleApplication<UserEvents...>::PublishEvent(const AnyEvent& anyEvent) -> void {
  auto self = static_cast<VehicleApplication<UserEvents...>*>(Application::instance);
  auto uid = anyEvent.index();
  if (self->mEventMap.contains(uid)) {
    self->mEventMap[uid](anyEvent);
  }
}

}

#endif //RGBLIB_VEHICLEAPPLICATION_H
