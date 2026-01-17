//
// Created by Brandon on 10/25/25.
//

#ifndef RGBLIB_IRREMOTE_H
#define RGBLIB_IRREMOTE_H

#include <driver/rmt_rx.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <optional>
#include "Sensor.h"
#include "Pin.h"

namespace rgb {

struct IRCommand {
  u16 address;
  u16 command;
};

class IRRemote : public Sensor {
public:
  explicit IRRemote(PinNumber pin);

  std::optional<IRCommand> lastCommand;

protected:
  auto doStart() -> bool override;
  auto doRead() -> void override;

private:
  static auto parseNecFrame(rmt_symbol_word_t* symbols, size_t symbolCount) -> std::optional<IRCommand>;
  static auto necCheckInRange(uint duration, uint target) -> bool;

  static constexpr uint NEC_LEADING_CODE_DURATION_0 = 9000;
  static constexpr uint NEC_LEADING_CODE_DURATION_1 = 4500;
  static constexpr uint NEC_PAYLOAD_ZERO_DURATION_0 = 560;
  static constexpr uint NEC_PAYLOAD_ZERO_DURATION_1 = 560;
  static constexpr uint NEC_PAYLOAD_ONE_DURATION_0 = 560;
  static constexpr uint NEC_PAYLOAD_ONE_DURATION_1 = 1690;
  static constexpr uint NEC_DECODE_MARGIN = 200;

  PinNumber pin;
  rmt_channel_handle_t rxChannel = nullptr;
  rmt_receive_config_t rxConfig = {};
  rmt_symbol_word_t rxBuffer[256] = {};
  QueueHandle_t rxQueue = nullptr;
};

}


#endif //RGBLIB_IRREMOTE_H
