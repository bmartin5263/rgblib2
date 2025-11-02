//
// Created by Brandon on 10/28/25.
//

#include <freertos/idf_additions.h>
#include "HubApplication.h"
#include "Log.h"
#include "Clock.h"
#include "OTA.h"
#include "Timer.h"
#include "HubRenderer.h"
#include "HubUI.h"

namespace rgb::home {

auto HubApplication::run() -> void {
  HubUI::Launch(*this);
  HubRenderer::Launch(*this);
  // Kill this task
  vTaskDelete(nullptr);
}

}