//
// Created by Brandon on 3/26/25.
//

#include "Log.h"
#include "LoggingSystem.h"
#include "System.h"

namespace rgb {

namespace {
constinit rgb::_priv::LoggingSystem loggingSystem = rgb::_priv::LoggingSystem{};
}

auto Logging::Start() -> void {
  loggingSystem.start();
}

}