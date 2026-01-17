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

auto irRemote = IRRemote{PinNumber{8}};

class LincolnApplication : public VehicleApplication<HighwayModeEntered, HighwayModeExited> {
protected:
  constexpr auto setup(VehicleApplication::Configurer& app) -> void override {
    /**
     * These LEDs will be redrawn every frame draw()
     */
    app.addLEDs(ledStrip)
        .addLEDs(debugLed);

    app.addSensor(irRemote);


    app.on<WakeEvent>([](auto& event) {
      // Do startup animations
    });


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
    app.on<IRButtonPressed>([](auto& event) {
      switch (event.button) {
        case IRButtonType::ONE: INFO("Button 1 pressed"); break;
        case IRButtonType::TWO: INFO("Button 2 pressed"); break;
        case IRButtonType::THREE: INFO("Button 3 pressed"); break;
        case IRButtonType::FOUR: INFO("Button 4 pressed"); break;
        case IRButtonType::FIVE: INFO("Button 5 pressed"); break;
        case IRButtonType::SIX: INFO("Button 6 pressed"); break;
        case IRButtonType::SEVEN: INFO("Button 7 pressed"); break;
        case IRButtonType::EIGHT: INFO("Button 8 pressed"); break;
        case IRButtonType::NINE: INFO("Button 9 pressed"); break;
        case IRButtonType::ZERO: INFO("Button 0 pressed"); break;
        case IRButtonType::STAR: INFO("Button * pressed"); break;
        case IRButtonType::POUND: INFO("Button # pressed"); break;
        case IRButtonType::UP: INFO("Button UP pressed"); break;
        case IRButtonType::DOWN: INFO("Button DOWN pressed"); break;
        case IRButtonType::LEFT: INFO("Button LEFT pressed"); break;
        case IRButtonType::RIGHT: INFO("Button RIGHT pressed"); break;
        case IRButtonType::OK: INFO("Button OK pressed"); break;
        case IRButtonType::UNKNOWN: INFO("Unknown pressed"); break;
      }
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
