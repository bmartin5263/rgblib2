//
// Created by Brandon on 10/25/25.
//

#include "OTAESP32.h"
#include "Log.h"

#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

auto OTAESP32::start() -> bool {
  return false;
}

auto OTAESP32::update() -> void {
  esp_http_client_config_t config = {
    .url = "url",
    .cert_pem = NULL,  // Add server certificate for HTTPS
    .timeout_ms = 5000,
    .keep_alive_enable = true,
  };

  esp_https_ota_config_t ota_config = {
    .http_config = &config,
  };

  esp_https_ota_handle_t https_ota_handle = NULL;
  esp_err_t err = esp_https_ota_begin(&ota_config, &https_ota_handle);

  if (err != ESP_OK) {
    ERROR("OTA begin failed");
    return;
  }

  while (1) {
    err = esp_https_ota_perform(https_ota_handle);
    if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
      break;
    }

    // Track progress
    int image_size = esp_https_ota_get_image_size(https_ota_handle);
    int downloaded = esp_https_ota_get_image_len_read(https_ota_handle);
    INFO("Progress: %d/%d bytes", downloaded, image_size);
  }

  if (esp_https_ota_is_complete_data_received(https_ota_handle) != true) {
    ERROR("Complete data was not received");
  }

  err = esp_https_ota_finish(https_ota_handle);
  if (err == ESP_OK) {
    INFO("OTA successful! Restarting...");
    esp_restart();
  } else {
    ERROR("OTA failed: %s", esp_err_to_name(err));
  }
}
