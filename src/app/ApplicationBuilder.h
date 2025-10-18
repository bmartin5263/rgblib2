//
// Created by Brandon on 10/12/25.
//

#ifndef RGBLIB_APPLICATIONBUILDER_H
#define RGBLIB_APPLICATIONBUILDER_H

#include <vector>
#include <unordered_map>
#include <typeindex>
#include "Event.h"
#include "Func.h"

namespace rgb {

class LEDCircuit;
struct ApplicationBuilder {
  constexpr auto addLEDs(LEDCircuit& circuit) -> ApplicationBuilder&;
  constexpr auto addSensor(const Runnable& sensor) -> ApplicationBuilder&;

  template<EventConcept T>
  constexpr auto on(const Consumer<const Event&>& action) -> ApplicationBuilder&;

  std::vector<LEDCircuit*> mLeds{};
  std::vector<Runnable> mSensors{};
  std::unordered_map<uint, Consumer<const Event&>> mEventMap{};
};

constexpr auto ApplicationBuilder::addLEDs(rgb::LEDCircuit& circuit) -> ApplicationBuilder& {
  mLeds.push_back(&circuit);
  return *this;
}

constexpr auto ApplicationBuilder::addSensor(const Runnable& sensor) -> ApplicationBuilder& {
  mSensors.push_back(sensor);
  return *this;
}

template<EventConcept T>
constexpr auto ApplicationBuilder::on(const Consumer<const Event&>& action) -> ApplicationBuilder& {
  mEventMap[T::uid] = action;
  return *this;
}

}

#endif //RGBLIB_APPLICATIONBUILDER_H
