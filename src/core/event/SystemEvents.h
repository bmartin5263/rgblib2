//
// Created by Brandon on 1/11/26.
//

#ifndef RGBLIB_SYSTEMEVENTS_H
#define RGBLIB_SYSTEMEVENTS_H

// Nothing happened...
struct NullEvent {
};

// System woke up from its sleep
struct WakeEvent {
};

// System is going to sleep soon
struct SleepEvent {
};

// Car's engine has started
struct OBDIIConnected {
};

// Car's engine has stopped
struct OBDIIDisconnected {
};

// Car's engine has started
struct CarEngineStarted {
};

// Car's engine has stopped
struct CarEngineStopped {
};

#endif //RGBLIB_SYSTEMEVENTS_H
