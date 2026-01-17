//
// Created by Brandon on 1/11/26.
//

#ifndef RGBLIB_SYSTEMEVENTS_H
#define RGBLIB_SYSTEMEVENTS_H

#include "Types.h"
#include "IRButtonType.h"

namespace rgb {

// Nothing happened...
struct NullEvent {
};

struct TimeEvent {
  Timestamp when;
};

// System woke up from its sleep
struct WakeEvent : public TimeEvent {};

// System is going to sleep soon
struct SleepEvent : public TimeEvent {
  // Duration timeLimit; // how long before the system ACTUALLY goes to sleep
};

// Car's engine has started
struct OBDIIConnected : public TimeEvent {};

// Car's engine has stopped
struct OBDIIDisconnected : public TimeEvent {};

// Car's engine has started
struct CarEngineStarted : public TimeEvent {};

// Car's engine has stopped
struct CarEngineStopped : public TimeEvent {};

// Hard brake threshold was crossed
struct HardBrakeDetected : public TimeEvent {};

struct IRButtonPressed : public TimeEvent {
  IRButtonType button{};
};

}

#endif //RGBLIB_SYSTEMEVENTS_H
