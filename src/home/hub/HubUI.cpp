//
// Created by Brandon on 10/28/25.
//

#include "freertos/FreeRTOS.h"
#include "HubUI.h"

namespace rgb::home {

auto HubUI::Launch(HubApplication& rootApplication) -> void {
  xTaskCreatePinnedToCore(
    HubUI::MainLoop,
    "UI",
    10000,
    &rootApplication,
    1,
    nullptr,
    0                // Core 0
  );
}

auto HubUI::MainLoop(void* parameter) -> void {
  auto application = static_cast<HubApplication*>(parameter);
  auto renderer = HubUI{*application};
  renderer.mainLoop();
}

HubUI::HubUI(HubApplication& application) : application(application) {}

auto HubUI::mainLoop() -> void {
  while (true) {

  }
}

}