//
// Created by Brandon on 10/11/25.
//

#ifndef RGBLIB_LINCOLNAPPLICATION_H
#define RGBLIB_LINCOLNAPPLICATION_H

#include "VehicleApplication.h"
#include "Pin.h"
#include "IRRemote.h"
#include "Timer.h"
#include "LEDStrip.h"
#include "GPIO.h"
#include "CustomEvents.h"
#include "LEDs.h"

using namespace rgb;

LEDStrip<38, rgb::format::GRB> ledStrip = LEDStrip<38, rgb::format::GRB>(PinNumber{10});
LEDStrip<1, rgb::format::GRB> debugLed = LEDStrip<1, rgb::format::GRB>(PinNumber{38});
auto flag = false;

auto levelIncreaseHandle = TimerHandle{};
auto level = 0;
auto level2 = 0u;

class LincolnApplication : public VehicleApplication<HighwayModeEntered, HighwayModeExited> {
protected:
  constexpr auto setup(VehicleApplication::Configurer& app) -> void override {
    /**
     * These LEDs will be redrawn every frame draw()
     */
    app.addLEDs(ledStrip)
        .addLEDs(debugLed);

    app.on<OBDIIConnected>([](auto& event){
      flag = true;
      levelIncreaseHandle = Timer::ContinuouslyWhile([](){
        ++level;
        return level < ledStrip.length();
      });
    });
    app.on<OBDIIDisconnected>([](auto& event){
      levelIncreaseHandle = Timer::ContinuouslyWhile([](){
        --level;
        return level > 0;
      });
    });
    app.on<HighwayModeEntered>([](auto& event){

    });
    app.on<HighwayModeExited>([](auto& event){

    });
  }

  auto update() -> void override {
//    INFO("didUpdate");
//    if (flag && level < ledStrip.length()) {
//      ++level;
//    }
    level2 += 1;
    level2 = level2 % ledStrip.length();
  }

  float rpm = 0;
  static constexpr auto RPM_SMOOTHING_FACTOR = 0.03f;

  auto draw() -> void override {
    auto hue = Clock::Now().percentOf(Duration::Seconds(1));
    debugLed.fill(Color::HslToRgb(hue) * .3f);

    auto currentRpm = static_cast<float>(vehicle.rpm());
    rpm = (RPM_SMOOTHING_FACTOR * currentRpm + (1 - RPM_SMOOTHING_FACTOR) * rpm);

    ledStrip.fill(Color::HslToRgb(rpm / 7000), level);
    ledStrip[level2] = Color::FAKE_WHITE();
  }
};


#endif //RGBLIB_LINCOLNAPPLICATION_H
