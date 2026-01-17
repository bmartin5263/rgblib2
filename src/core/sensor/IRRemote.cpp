//
// Created by Brandon on 10/25/25.
//

#include "IRRemote.h"
#include <esp_log.h>
#include "Application.h"
#include "Clock.h"
#include "IRButtonType.h"

namespace rgb {

static bool rmtRxDoneCallback(
  rmt_channel_handle_t channel,
  const rmt_rx_done_event_data_t* eventData,
  void* userData
) {
  (void)channel;
  auto highTaskWakeup = BaseType_t{pdFALSE};
  auto queue = static_cast<QueueHandle_t>(userData);
  xQueueSendFromISR(queue, eventData, &highTaskWakeup);
  return highTaskWakeup == pdTRUE;
}

constexpr auto commandToButtonType(uint16_t command) -> IRButtonType {
  switch (command) {
    case 47685: return IRButtonType::ONE;
    case 47430: return IRButtonType::TWO;
    case 47175: return IRButtonType::THREE;
    case 47940: return IRButtonType::FOUR;
    case 48960: return IRButtonType::FIVE;
    case 48195: return IRButtonType::SIX;
    case 63495: return IRButtonType::SEVEN;
    case 59925: return IRButtonType::EIGHT;
    case 62985: return IRButtonType::NINE;
    case 58905: return IRButtonType::ZERO;
    case 59670: return IRButtonType::STAR;
    case 61965: return IRButtonType::POUND;
    case 59160: return IRButtonType::UP;
    case 42330: return IRButtonType::RIGHT;
    case 44370: return IRButtonType::DOWN;
    case 63240: return IRButtonType::LEFT;
    case 58140: return IRButtonType::OK;
    default: return IRButtonType::UNKNOWN;
  }
}

IRRemote::IRRemote(PinNumber pin)
  : pin{pin} {
}

auto IRRemote::doStart() -> bool {
  rxQueue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
  if (rxQueue == nullptr) {
    ERROR("Failed to create RX queue");
    return false;
  }

  auto channelConfig = rmt_rx_channel_config_t{
    .gpio_num = pin.to<gpio_num_t>(),
    .clk_src = RMT_CLK_SRC_DEFAULT,
    .resolution_hz = 1'000'000,
    .mem_block_symbols = 64,
    .flags = {
      .invert_in = false,
      .with_dma = false,
      .io_loop_back = false,
    },
    .intr_priority = 0
  };

  auto result = rmt_new_rx_channel(&channelConfig, &rxChannel);
  if (result != ESP_OK) {
    ERROR("Failed to create RX channel: %s", esp_err_to_name(result));
    return false;
  }

  auto callbackConfig = rmt_rx_event_callbacks_t{
    .on_recv_done = rmtRxDoneCallback,
  };

  result = rmt_rx_register_event_callbacks(rxChannel, &callbackConfig, rxQueue);
  if (result != ESP_OK) {
    ERROR("Failed to register callbacks: %s", esp_err_to_name(result));
    return false;
  }

  result = rmt_enable(rxChannel);
  if (result != ESP_OK) {
    ERROR("Failed to enable channel: %s", esp_err_to_name(result));
    return false;
  }

  rxConfig = rmt_receive_config_t{
    .signal_range_min_ns = 1250,
    .signal_range_max_ns = 12'000'000,
  };

  result = rmt_receive(rxChannel, rxBuffer, sizeof(rxBuffer), &rxConfig);
  if (result != ESP_OK) {
    ERROR("Failed to start receive: %s", esp_err_to_name(result));
    return false;
  }

  INFO("IR receiver started on pin %u", pin.value);
  return true;
}

auto IRRemote::doRead() -> void {
  auto eventData = rmt_rx_done_event_data_t{};

  if (xQueueReceive(rxQueue, &eventData, 0) == pdTRUE) {
    auto command = parseNecFrame(eventData.received_symbols, eventData.num_symbols);
    if (command.has_value()) {
      auto buttonType = commandToButtonType(command.value().command);
      if (buttonType != IRButtonType::UNKNOWN) {
        Application::instance->publishSystemEvent(IRButtonPressed{{Clock::Now()}, buttonType});
      }
      lastCommand = command;
    }

    rmt_receive(rxChannel, rxBuffer, sizeof(rxBuffer), &rxConfig);
  }
}

auto IRRemote::necCheckInRange(uint duration, uint target) -> bool {
  return duration >= (target - NEC_DECODE_MARGIN) &&
         duration <= (target + NEC_DECODE_MARGIN);
}

auto IRRemote::parseNecFrame(rmt_symbol_word_t* symbols, size_t symbolCount) -> std::optional<IRCommand> {
  if (symbolCount < 34) {
    return std::nullopt;
  }

  auto& leadingSymbol = symbols[0];
  if (!necCheckInRange(leadingSymbol.duration0, NEC_LEADING_CODE_DURATION_0) ||
      !necCheckInRange(leadingSymbol.duration1, NEC_LEADING_CODE_DURATION_1)) {
    INFO("leadingSymbol bad");
    return std::nullopt;
  }

  auto address = u16{0};
  auto command = u16{0};

  for (size_t i = 0; i < 16; ++i) {
    auto& symbol = symbols[1 + i];
    if (necCheckInRange(symbol.duration0, NEC_PAYLOAD_ONE_DURATION_0) &&
        necCheckInRange(symbol.duration1, NEC_PAYLOAD_ONE_DURATION_1)) {
      address |= (1 << i);
    } else if (necCheckInRange(symbol.duration0, NEC_PAYLOAD_ZERO_DURATION_0) &&
               necCheckInRange(symbol.duration1, NEC_PAYLOAD_ZERO_DURATION_1)) {
      // Bit is 0, nothing to set
    } else {
      INFO("bad %i", i);
      return std::nullopt;
    }
  }

  for (size_t i = 0; i < 16; ++i) {
    auto& symbol = symbols[17 + i];
    if (necCheckInRange(symbol.duration0, NEC_PAYLOAD_ONE_DURATION_0) &&
        necCheckInRange(symbol.duration1, NEC_PAYLOAD_ONE_DURATION_1)) {
      command |= (1 << i);
    } else if (necCheckInRange(symbol.duration0, NEC_PAYLOAD_ZERO_DURATION_0) &&
               necCheckInRange(symbol.duration1, NEC_PAYLOAD_ZERO_DURATION_1)) {
      // Bit is 0, nothing to set
    } else {
      INFO("bad %i (2)", i);
      return std::nullopt;
    }
  }

  return IRCommand{address, command};
}

}