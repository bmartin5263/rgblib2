////
//// Created by Brandon on 10/8/25.
////
//#include "esp_log.h"
//#include "esp_vfs_fat.h"
//#include "driver/sdmmc_host.h"
//#include "sdmmc_cmd.h"
//
//static FILE *log_file = nullptr;
//static SemaphoreHandle_t log_mutex = nullptr;
//
//// Custom vprintf that writes to BOTH serial and SD
//int custom_log_vprintf(const char *fmt, va_list args) {
//  // Always write to serial
//  int serial_result = vprintf(fmt, args);
//
//  // Also write to SD card if available
//  if (log_file != nullptr && xSemaphoreTake(log_mutex, pdMS_TO_TICKS(100))) {
//    // Make a copy of args since va_list can only be used once
//    va_list args_copy;
//    va_copy(args_copy, args);
//    vfprintf(log_file, fmt, args_copy);
//    va_end(args_copy);
//
//    // Optionally flush (trades performance for safety)
//    // fflush(log_file);  // Uncomment for immediate write
//
//    xSemaphoreGive(log_mutex);
//  }
//
//  return serial_result;
//}
//
//// Initialize SD card logging
//esp_err_t init_sd_logging() {
//  log_mutex = xSemaphoreCreateMutex();
//
//  // Mount SD card
//  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
//  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
//
//  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
//    .format_if_mount_failed = false,
//    .max_files = 5,
//    .allocation_unit_size = 16 * 1024
//  };
//
//  sdmmc_card_t *card;
//  esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config,
//                                          &mount_config, &card);
//  if (ret != ESP_OK) {
//    ESP_LOGE("SD", "Failed to mount SD card");
//    return ret;
//  }
//
//  // Open log file (append mode)
//  log_file = fopen("/sdcard/device.log", "a");
//  if (log_file == nullptr) {
//    ESP_LOGE("SD", "Failed to open log file");
//    return ESP_FAIL;
//  }
//
//  // Set custom log function
//  esp_log_set_vprintf(custom_log_vprintf);
//
//  ESP_LOGI("SD", "SD card logging initialized");
//  return ESP_OK;
//}