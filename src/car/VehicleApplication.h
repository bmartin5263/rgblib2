//
// Created by Brandon on 10/11/25.
//

#ifndef RGBLIB_VEHICLEAPPLICATION_H
#define RGBLIB_VEHICLEAPPLICATION_H

#include "Application.h"
#include "VehicleApplicationConfigurer.h"
#include "Clock.h"
#include "Vehicle.h"
#include "Timer.h"
#include "LEDCircuit.h"
#include "OTA.h"
#include <mutex>

namespace rgb {

void subtask(void* args);

template<typename ...UserEvents>
class VehicleApplication : public Application {
public:
  using AnyEvent = Event<UserEvents...>;
  using Configurer = VehicleApplicationConfigurer<UserEvents...>;

  VehicleApplication() = default;
  virtual ~VehicleApplication() = default;
  VehicleApplication(const VehicleApplication& rhs) = delete;
  VehicleApplication(VehicleApplication&& rhs) noexcept = delete;
  VehicleApplication& operator=(const VehicleApplication& rhs) = delete;
  VehicleApplication& operator=(VehicleApplication&& rhs) noexcept = delete;

  auto run() -> void;
  auto publishSystemEvent(const SystemEvent& event) -> void override;
  auto on(size_t uid, Consumer<const SystemEvent&> action) -> void override;
  auto getVehicle() -> Vehicle* override;

  static auto PublishEvent(const AnyEvent& event) -> void;

  Vehicle vehicle{};

protected:
  virtual constexpr auto setup(Configurer& app) -> void = 0;
  virtual auto update() -> void = 0;
  virtual auto draw() -> void = 0;

private:
  auto configureApplication() -> void;
  auto initialize() -> void;
  auto baseUpdate() -> void;
  auto baseDraw() -> void;


  Clock clock{300};
  std::vector<LEDCircuit*> mLeds{};
  std::vector<Sensor*> mSensors{};
  std::unordered_map<uint, std::vector<std::function<void(const AnyEvent&)>>> mEventMap{};
  mutable std::mutex mEventMutex{};
};

template<typename... UserEvents>
auto VehicleApplication<UserEvents...>::getVehicle() -> Vehicle* {
  return &vehicle;
}

template<typename ...UserEvents>
auto VehicleApplication<UserEvents...>::run() -> void {
  configureApplication();
  initialize();

  xTaskCreatePinnedToCore(subtask, "Subtask", RGB_OTHER_CORE_STACK_SIZE, this, RGB_OTHER_CORE_PRIORITY, nullptr, 1);

  PublishEvent(WakeEvent{});
  INFO("Started Application");

  clock.forever([&](){
    clock.printStats();
    baseUpdate();
    baseDraw();
  });
}

template<typename ...UserEvents>
auto VehicleApplication<UserEvents...>::baseUpdate() -> void {
  for (auto& sensor : mSensors) {
    sensor->doRead();
  }
  Timer::ProcessTimers();
  update();
}

template<typename ...UserEvents>
auto VehicleApplication<UserEvents...>::baseDraw() -> void {
  std::for_each(std::begin(mLeds), std::end(mLeds), [](auto led){ led->reset(); });
  draw();
  Debug::Draw();
  std::for_each(std::begin(mLeds), std::end(mLeds), [](auto led){ led->display(); });
}

template<typename ...UserEvents>
auto VehicleApplication<UserEvents...>::configureApplication() -> void {
  instance = this;
  auto appConfig = VehicleApplicationConfigurer<UserEvents...>{};
  setup(appConfig);

  mLeds = std::move(appConfig.mLeds);
  mSensors = std::move(appConfig.mSensors);
  mEventMap = std::move(appConfig.mEventMap);
}

template<typename ...UserEvents>
auto VehicleApplication<UserEvents...>::initialize() -> void {
  std::for_each(std::begin(mLeds), std::end(mLeds), [](auto led){ led->start(); });
  std::for_each(std::begin(mSensors), std::end(mSensors), [](auto sensor){ sensor->doStart(); });
}

template<typename... UserEvents>
auto VehicleApplication<UserEvents...>::publishSystemEvent(const SystemEvent& systemEvent) -> void {
  auto event = std::visit([](auto&& e) {
    return AnyEvent{e};
  }, systemEvent);
  auto uid = systemEvent.index();

  std::vector<Consumer<const AnyEvent&>> handlers;
  {
    std::lock_guard lock{mEventMutex};
    if (mEventMap.contains(uid)) {
      handlers = mEventMap[uid];
    }
  }

  for (auto& handler : handlers) {
    handler(event);
  }
}

template<typename... UserEvents>
auto VehicleApplication<UserEvents...>::on(size_t uid, Consumer<const SystemEvent&> action) -> void {
  std::lock_guard lock{mEventMutex};
  mEventMap[uid].push_back([action](auto& anyEvent) {
    if (auto systemEvent = narrow_variant<SystemEvent>(anyEvent)) {
      action(systemEvent.value());
    }
  });
}

template<typename ...UserEvents>
auto VehicleApplication<UserEvents...>::PublishEvent(const AnyEvent& event) -> void {
  auto self = static_cast<VehicleApplication<UserEvents...>*>(Application::instance);
  auto uid = event.index();

  std::vector<Consumer<const AnyEvent&>> handlers;
  {
    std::lock_guard lock{self->mEventMutex};
    if (self->mEventMap.contains(uid)) {
      handlers = self->mEventMap[uid];
    }
  }

  for (auto& handler : handlers) {
    handler(event);
  }
}

void connectVehicle(Vehicle* vehicle, Timestamp& connectAgain) {
  auto TX = PinNumber{42};
  auto RX = PinNumber{41};
  if (vehicle->connect(RX, TX)) {
    connectAgain = Timestamp::Max();
  }
  else {
    connectAgain = Clock::Now() + Duration::Seconds(1);
  }
}

void subtask(void* args) {
  INFO("Subtask Started");
  Clock clock{60};
  Timer timer{};
  TimerHandle connectVehicleHandle{};

  auto app = static_cast<Application*>(args);
  auto vehicle = app->getVehicle();

  TimerFunction connectVehicle = [&](auto& context){
    auto TX = PinNumber{42};
    auto RX = PinNumber{41};
    if (!vehicle->connect(RX, TX)) {
      context.repeatIn = Duration::Seconds(1);
    }
  };
  INFO("Timer = %p", &timer);
  app->on<OBDIIDisconnected>([timer = &timer, connectVehicle](auto& event){
    INFO("Attempting Reconnection Soon");
    INFO("Timer = %p", timer);
    timer->setTimeout(Duration::Seconds(10), connectVehicle);
  });
  timer.setImmediateTimeout(connectVehicle);

  clock.forever([&](){
    timer.processTimers();

    if (vehicle->isConnected()) {
      vehicle->update();
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  });
}

}

#endif //RGBLIB_VEHICLEAPPLICATION_H
