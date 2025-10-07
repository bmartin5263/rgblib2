//
// Created by Brandon on 3/26/25.
//

#include "Log.h"
#include "LoggingSystem.h"
#include "System.h"

namespace rgb {

namespace {
constinit rgb::priv::LoggingSystem loggingSystem = rgb::priv::LoggingSystem{};
}

auto Logging::Start() -> void {
  loggingSystem.start();
}

}