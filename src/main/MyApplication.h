//
// Created by Brandon on 10/11/25.
//

#ifndef RGBLIB_MYAPPLICATION_H
#define RGBLIB_MYAPPLICATION_H

#include "VehicleApplication.h"
#include "Pin.h"
#include "IRRemote.h"
#include "Timer.h"
#include "LEDStrip.h"
#include "GPIO.h"

using namespace rgb;

//auto irReceiver = IRReceiver{};

auto stick = LEDStrip<16, rgb::format::GRBW>(PinNumber{10});
auto ledStrip = LEDStrip<40, rgb::format::GRB>(PinNumber{46});
auto debugLed = LEDStrip<1, rgb::format::GRB>(PinNumber{38});
auto flag = false;
auto irRemote = IRRemote{};

class MyApplication : public VehicleApplication {
protected:
  constexpr auto setup(VehicleApplicationBuilder& app) -> void override {
    GPIO::ActivatePin(PinNumber{6}, PinMode::READ);

    app.addLEDs(stick)
        .addLEDs(ledStrip)
        .addLEDs(debugLed);

    app.addSensor(irRemote);

    app.on<ApplicationStartedEvent>([](auto& event){
      INFO("Application Started");
    });

    Timer::SetTimeout(Duration::Seconds(3), [](){
      flag = true;
    }).detach();
  }

  auto update() -> void override {

  }

  auto draw() -> void override {
    constexpr auto greenLen = 11;
    constexpr auto yellowLen = 3;
    constexpr auto redLen = 2;

    static_assert(greenLen + yellowLen + redLen == 16);

    stick.fill(Color::GREEN(.02f), greenLen);
    stick.fill(Color::YELLOW(.02f), greenLen, yellowLen);
    stick.fill(Color::RED(.02f), greenLen + yellowLen, redLen);

    int level = GPIO::ReadPin(PinNumber{6});
    if (level > 0) {
      debugLed.fill(Color::BLUE(.3f));
    }

    ledStrip.fill(Color(0, 1.0f, 1.0f) * .3f, 20);
    ledStrip.fill(Color(.8f, 0, 1.0f) * .3f, 20, 20);
    if (flag) {
      ledStrip.fill(Color(1.0f, 0, .4f) * .3f, 20, 20);
    }
  }
};


#endif //RGBLIB_MYAPPLICATION_H
