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
#include "PixelStitch.h"
#include "ReversePixelList.h"
#include "ChasingEffect.h"
#include "DeadPixelList.h"
#include "PixelSlice.h"
#include "ChasingEffectV2.h"

using namespace rgb;

// LEDs
LEDStrip<38, rgb::format::GRB> topStrip = LEDStrip<38, rgb::format::GRB>(PinNumber{10});
LEDStrip<38, rgb::format::GRB> bottomStrip = LEDStrip<38, rgb::format::GRB>(PinNumber{11});
LEDStrip<1, rgb::format::GRB> debugLed = LEDStrip<1, rgb::format::GRB>(PinNumber{38});
auto ring = LEDStrip<12, rgb::format::GRBW>(PinNumber{12});

// Pixels
auto deadPixels50 = DeadPixelList{50};
auto deadPixels2 = DeadPixelList{0};
auto topLeft = topStrip.slice(19);
auto topLeftReverse = ReversePixelList{topLeft};
auto topLeftReverseExt = PixelStitch{topLeftReverse, deadPixels50};
auto topRight = topStrip.slice(19, topStrip.length());
auto topRightExt = PixelStitch{topRight, deadPixels50};
auto bottomLeft = bottomStrip.slice(19);
auto bottomLeftReverse = ReversePixelList{bottomLeft};
auto bottomLeftReverseExt = PixelStitch{bottomLeftReverse, deadPixels50};
auto bottomRight = bottomStrip.slice(19, bottomStrip.length());
auto bottomRightExt = PixelStitch{bottomRight, deadPixels50};

auto ringExt = PixelStitch{ring, deadPixels2};
auto reverseRing = ReversePixelList{ring};
auto reverseRingExt = PixelStitch{reverseRing, deadPixels2};

auto reverseBottomStrip = ReversePixelList{bottomStrip};
auto tempStitch = PixelStitch{topStrip, reverseBottomStrip};
auto stitch = PixelStitch{tempStitch, deadPixels50};

// ConstantTime vs ConstantSpeed
auto shortLedStrip = bottomStrip.slice(20);
auto& longLedStrip = topStrip;

auto level = 0;
auto level2 = 0u;
auto levelIncreaseHandle = TimerHandle{};
auto irRemote = IRRemote{PinNumber{4}};

auto chasingEffect = ChasingEffect{};
auto ringChasingEffect = ChasingEffect{};
auto reverseRingChasingEffect = ChasingEffect{};
auto chasingEffectV2 = ChasingEffectV2{};

auto firstUpdate = true;

EffectHandle e1;

class LincolnApplication : public VehicleApplication<HighwayModeEntered, HighwayModeExited> {
protected:
  constexpr auto setup(VehicleApplication::Configurer& app) -> void override {
    chasingEffect.delay = Duration::Milliseconds(10);
    chasingEffect.shader = [](auto pixel, auto& params) {
      return Color::HslToRgb(Clock::Now().percentOf(Duration::Seconds(5)));
    };
    chasingEffect.trailLength = Length::Ratio(.2f);
    chasingEffect.buildup = true;

    chasingEffectV2.shader = [](auto pixel, auto& params) {
      return Color::HslToRgb(Clock::Now().percentOf(Duration::Seconds(5)));
    };
    chasingEffectV2.trailLength = Length::Ratio(.2f);
    chasingEffectV2.buildup = true;

    ringChasingEffect.delay = Duration::Milliseconds(50);
    ringChasingEffect.shader = [](auto pixel, auto& params) {
      return pixel + (Color::RED(.1f) * (params.positionRatio));
    };
    ringChasingEffect.trailLength = Length::Units(6);
    ringChasingEffect.buildup = true;

    reverseRingChasingEffect.delay = Duration::Milliseconds(50);
    reverseRingChasingEffect.shader = [](auto pixel, auto& params) {
      return pixel + (Color::MAGENTA(.1f) * (params.positionRatio));
    };
    reverseRingChasingEffect.trailLength = Length::Units(6);
    reverseRingChasingEffect.buildup = true;

    /**
     * These LEDs will be redrawn every frame draw()
     */
    app.addLEDs(topStrip);
    app.addLEDs(bottomStrip);
    app.addLEDs(debugLed);
    app.addLEDs(ring);

    app.addSensor(irRemote);

    e1 = Effects::Start(chasingEffect, longLedStrip);

    app.on<WakeEvent>([](auto& event) {
//      chasingEffect.reset();
//      ringChasingEffect.reset();
    });


    app.on<OBDIIConnected>([](auto& event){
      levelIncreaseHandle = Timer::ContinuouslyWhile([](){
        ++level;
        return level < topStrip.length();
      });
      INFO("Done OBDIIConnected");
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
        case IRButtonType::ONE:
          e1.stop();
          INFO("Button 1 pressed");
          break;
        case IRButtonType::TWO:
          e1 = Effects::Start(chasingEffect, longLedStrip);
          INFO("Button 2 pressed");
          break;
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
        case IRButtonType::UP:
          INFO("Button UP pressed");
          ++ringChasingEffect.trailLength;
          ++reverseRingChasingEffect.trailLength;
          break;
        case IRButtonType::DOWN:
          INFO("Button DOWN pressed");
          --ringChasingEffect.trailLength;
          --reverseRingChasingEffect.trailLength;
          break;
        case IRButtonType::LEFT: INFO("Button LEFT pressed"); break;
        case IRButtonType::RIGHT: INFO("Button RIGHT pressed"); break;
        case IRButtonType::OK:
          INFO("Button OK pressed");
          ringChasingEffect.reset(Clock::Now());
          reverseRingChasingEffect.reset(Clock::Now());
          break;
        case IRButtonType::UNKNOWN: INFO("Unknown pressed"); break;
      }
    });
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

//    topStrip.fill(Color::HslToRgb(rpm / 7000), level);
//    topStrip.fill(Color{.3f, 0.0f, 1.0f});
//    bottomStrip.fill(Color{.3f, 0.0f, 1.0f});


//    stitch[level2] = Color::FAKE_WHITE();

//    chasingEffect.draw(topRightExt);
//    chasingEffect.draw(topLeftReverseExt);
//    chasingEffect.draw(bottomLeftReverseExt);
//    chasingEffect.draw(bottomRightExt);

    // Constant Speed
//    chasingEffect.delay = Duration::Milliseconds(100);
//    chasingEffect.draw(longLedStrip);
//    chasingEffect.draw(shortLedStrip);

    // Constant Time
//    chasingEffectV2.progression = ChaseProgression::ConstantSpeed(Duration::Milliseconds(100));
//    chasingEffectV2.draw(Clock::Now(), longLedStrip);
//    chasingEffectV2.draw(Clock::Now(), shortLedStrip);

//    ring.fill(Color::HslToRgb(rpm / 7000) * .3f, level);
//    ringChasingEffect.shift = 0;
//    ringChasingEffect.draw(Clock::Now(), ringExt);
//    ringChasingEffect.shift = 6;
//    ringChasingEffect.draw(Clock::Now(), ringExt);
  }
};


#endif //RGBLIB_LINCOLNAPPLICATION_H
