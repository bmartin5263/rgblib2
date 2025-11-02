//
// Created by Brandon on 10/11/25.
//

#ifndef RGBLIB_VEHICLEAPPLICATION_H
#define RGBLIB_VEHICLEAPPLICATION_H

#include "VehicleApplicationBuilder.h"
#include "Clock.h"

namespace rgb {

class VehicleApplication {
  static VehicleApplication* instance;

public:
  VehicleApplication() = default;
  virtual ~VehicleApplication() = default;
  VehicleApplication(const VehicleApplication& rhs) = default;
  VehicleApplication(VehicleApplication&& rhs) noexcept = default;
  VehicleApplication& operator=(const VehicleApplication& rhs) = default;
  VehicleApplication& operator=(VehicleApplication&& rhs) noexcept = default;

  auto run() -> void;

  template<EventConcept T>
  static auto PublishEvent(const T& event = {}) -> void;

protected:
  virtual constexpr auto setup(VehicleApplicationBuilder& setup) -> void = 0;
  virtual auto update() -> void = 0;
  virtual auto draw() -> void = 0;

private:
  auto buildApplication() -> void;
  auto initialize() -> void;
  auto baseUpdate() -> void;
  auto baseDraw() -> void;

  Clock clock{300};
  std::vector<LEDCircuit*> mLeds{};
  std::vector<Sensor*> mSensors{};
  std::unordered_map<uint, Consumer<const Event&>> mEventMap{};
};

template<EventConcept T>
auto VehicleApplication::PublishEvent(const T& event) -> void {
  if (instance->mEventMap.contains(T::UID)) {
    instance->mEventMap[T::UID](event);
  }
}

}

#endif //RGBLIB_VEHICLEAPPLICATION_H
