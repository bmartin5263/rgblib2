//
// Created by Brandon on 10/12/25.
//

#ifndef RGBLIB_VEHICLEAPPLICATIONBUILDER_H
#define RGBLIB_VEHICLEAPPLICATIONBUILDER_H

#include <vector>
#include <unordered_map>
#include <typeindex>
#include "EventConcept.h"
#include "EventType.h"
#include "Func.h"
#include "Sensor.h"

namespace rgb {

class LEDCircuit;
struct VehicleApplicationBuilder {
  constexpr auto addLEDs(LEDCircuit& circuit) -> VehicleApplicationBuilder&;
  constexpr auto addSensor(Sensor& sensor) -> VehicleApplicationBuilder&;

  template<EventConcept T>
  constexpr auto on(Consumer<const T&> action) -> VehicleApplicationBuilder&;

  std::vector<LEDCircuit*> mLeds{};
  std::vector<Sensor*> mSensors{};
  std::unordered_map<uint, Consumer<const Event&>> mEventMap{};
};

constexpr auto VehicleApplicationBuilder::addLEDs(rgb::LEDCircuit& circuit) -> VehicleApplicationBuilder& {
  mLeds.push_back(&circuit);
  return *this;
}

constexpr auto VehicleApplicationBuilder::addSensor(Sensor& sensor) -> VehicleApplicationBuilder& {
  mSensors.push_back(&sensor);
  return *this;
}

template<EventConcept T>
constexpr auto VehicleApplicationBuilder::on(Consumer<const T&> action) -> VehicleApplicationBuilder& {
  mEventMap[T::UID] = [action](auto& e) {
    action(std::get<T>(e));
  };
  return *this;
}

}

#endif //RGBLIB_VEHICLEAPPLICATIONBUILDER_H
