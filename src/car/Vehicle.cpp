//
// Created by Brandon on 2/24/25.
//

#include "Vehicle.h"
#include "Config.h"
#include "Clock.h"
#include "Assertions.h"
#include "Util.h"
#include "Application.h"

namespace rgb {

auto OBDDestroyer::operator()(COBD& c) const noexcept -> void {
  c.end();
}

auto Vehicle::connect(PinNumber rx, PinNumber tx) -> bool {
  auto lock = std::unique_lock { mu };
  if (obdHandle->getState() == OBD_STATES::OBD_CONNECTED) {
    mConnected = true;
    return true;
  }

  INFO("Vehicle Connecting");

  obdHandle.reset({});
  mConnected = false;

  if (!obdHandle->begin(rx.value, tx.value)) {
    ERROR("Vehicle begin() failed");
//    FAIL("Vehicle begin() failed", Color::MAGENTA(.01f));
    return false;
  }

  if (!obdHandle->init()) {
    ERROR("Vehicle init() failed");
//    FAIL("Vehicle init() failed", Color::MAGENTA(.01f));
    return false;
  }

//  debugLed[0] = Color::PURPLE();

  INFO("OBD2 ready");
//  digitalWrite(rgb::config::LED_VEHICLE_CONNECTED, LOW); TODO
  mConnected = true;
  mLastResponse = Clock::Now();

  Application::instance->publishSystemEvent(OBDIIConnected{Clock::Now()});

  return true;
}

auto Vehicle::disconnect() -> void {
  auto lock = std::unique_lock { mu };
  obdHandle.reset({});
  mConnected = false;
  Application::instance->publishSystemEvent(OBDIIDisconnected{Clock::Now()});
}

auto Vehicle::update() -> void {
  if (!mConnected) { return; }
  auto lock = std::unique_lock { mu };

  if (!mConnected) { return; }
  readPID(PID_RPM, mRpm, ToUint);

  if (!mConnected) { return; }
  readPID(PID_COOLANT_TEMP, mCoolantTemp, ToFahrenheit);

  if (!mConnected) { return; }
  readPID(PID_SPEED, mSpeed, ToUint);
}

auto Vehicle::rpm() const -> revs_per_minute {
  return mRpm;
}

auto Vehicle::coolantTemp() const -> fahrenheit {
  return mCoolantTemp;
}

auto Vehicle::fuelLevel() const -> percent {
  return mFuelLevel;
}

auto Vehicle::throttlePosition() const -> percent {
  return mThrottlePosition;
}

auto Vehicle::speed() const-> mph {
  return mSpeed;
}

auto Vehicle::inLowPowerMode() const -> bool {
  return mLowPowerMode;
}

auto Vehicle::isConnected() const -> bool {
  return mConnected;
}

auto Vehicle::setLowPowerMode(bool value) -> void {
  auto lock = std::unique_lock { mu };
  mLowPowerMode = value;
  if (mLowPowerMode) {
//    digitalWrite(rgb::config::LED_VEHICLE_CONNECTED, HIGH); TODO
  }
  else {
//    digitalWrite(rgb::config::LED_VEHICLE_CONNECTED, LOW); TODO
  }
}

auto Vehicle::setTimeout(Duration timeout) -> void {
  // Convert once to avoid needing to divide every frame
  this->timeoutMs = static_cast<int>(timeout.asMilliseconds());
}

}