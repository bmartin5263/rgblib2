//
// Created by Brandon on 10/12/25.
//

#ifndef RGBLIB_VEHICLEAPPLICATIONBUILDER2_H
#define RGBLIB_VEHICLEAPPLICATIONBUILDER2_H

#include <vector>
#include <unordered_map>
#include <typeindex>
#include "EventType.h"
#include "Func.h"
#include "Sensor.h"

namespace rgb {

class LEDCircuit;

template<typename ...UserEvents>
struct VehicleApplicationBuilder {
  constexpr auto addLEDs(LEDCircuit& circuit) -> VehicleApplicationBuilder&;
  constexpr auto addSensor(Sensor& sensor) -> VehicleApplicationBuilder&;

  template<typename T>
  constexpr auto on(Consumer<const T&> action) -> VehicleApplicationBuilder&;

  std::vector<LEDCircuit*> mLeds{};
  std::vector<Sensor*> mSensors{};
  std::unordered_map<uint, Consumer<const Event<UserEvents...>&>> mEventMap{};
};

template<typename ...UserEvents>
constexpr auto VehicleApplicationBuilder<UserEvents...>::addLEDs(rgb::LEDCircuit& circuit) -> VehicleApplicationBuilder<UserEvents...>& {
  mLeds.push_back(&circuit);
  return *this;
}

template<typename ...UserEvents>
constexpr auto VehicleApplicationBuilder<UserEvents...>::addSensor(Sensor& sensor) -> VehicleApplicationBuilder<UserEvents...>& {
  mSensors.push_back(&sensor);
  return *this;
}

template<typename ...UserEvents>
template<typename T>
constexpr auto VehicleApplicationBuilder<UserEvents...>::on(Consumer<const T&> action) -> VehicleApplicationBuilder<UserEvents...>& {
  auto index = EventIndex_v<T, Event<UserEvents...>>;
  mEventMap[index] = [action](auto& e) {
    action(std::get<T>(e));
  };
  return *this;
}

}

#endif //RGBLIB_VEHICLEAPPLICATIONBUILDER_H
