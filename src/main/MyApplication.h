//
// Created by Brandon on 10/11/25.
//

#ifndef RGBLIB_MYAPPLICATION_H
#define RGBLIB_MYAPPLICATION_H

#include "Application.h"
#include "Pin.h"

using namespace rgb;

//auto irReceiver = IRReceiver{};

auto stick = LEDStrip<16, rgb::format::GRBW>(PinNumber{10});
auto ledStrip = LEDStrip<40, rgb::format::GRB>(PinNumber{46});
auto debugLed = LEDStrip<1, rgb::format::GRB>(PinNumber{38});

class MyApplication : public Application {
protected:
  constexpr auto setup(ApplicationBuilder& app) -> void override {
    GPIO::ActivatePin(PinNumber{6}, PinMode::READ);

    app.addLEDs(stick)
        .addLEDs(ledStrip)
        .addLEDs(debugLed);

    app.addSensor([](){

    });

    app.on<MyEvent>([](auto event){

    });

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

    ledStrip.fill(Color::GREEN(.3f));
  }
};


#endif //RGBLIB_MYAPPLICATION_H
