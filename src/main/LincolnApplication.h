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

using namespace rgb;

/*
 * LEDs
 * 0 : LEDStrip(12)
 * 1 : LEDStrip(12)
 * 2 : LEDStrip(12)
 *
 * RGBMatrix(3x3) : LED(0), LED(1), LED(2)
 */

auto ledStrip = LEDStrip<38, rgb::format::GRB>(PinNumber{10});
auto debugLed = LEDStrip<1, rgb::format::GRB>(PinNumber{38});
auto flag = false;

class LincolnApplication : public VehicleApplication {
protected:
  constexpr auto setup(VehicleApplicationBuilder& app) -> void override {
    /**
     * These LEDs will be redrawn every frame draw()
     */
    app.addLEDs(ledStrip)
        .addLEDs(debugLed);

    app.on<WakeEvent>([](auto& event){
      // Ring:
      // Enter into RPM display, doing a buildup animation

      // Glow:
      // Fill entire strip front to back

      // Fibers
      // Fill out from center
    });
    app.on<SleepEvent>([](auto& event){
      // Ring:
      // Do Sleep Animation

      // Glow:
      // Fade out

      // Fibers
      // Turn off random LEDs until fully turned off
    });

    Timer::SetTimeout(Duration::Seconds(3), [](){
      flag = true;
    }).detach();
  }

  auto update() -> void override {

  }

  float rpm = 0;
  static constexpr auto RPM_SMOOTHING_FACTOR = 0.03f;

  auto draw() -> void override {
    auto hue = Clock::Now().percentOf(Duration::Seconds(1));
    debugLed.fill(Color::HslToRgb(hue) * .3f);


    auto currentRpm = static_cast<float>(vehicle.rpm());
    rpm = (RPM_SMOOTHING_FACTOR * currentRpm + (1 - RPM_SMOOTHING_FACTOR) * rpm);

    ledStrip.fill(Color::HslToRgb(rpm / 7000));
  }
};


#endif //RGBLIB_LINCOLNAPPLICATION_H
