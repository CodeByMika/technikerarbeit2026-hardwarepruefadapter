/**
 * @file mailbox_service.cpp
 * @brief Implementierung der ProcessMailbox.
 */

#include "mailbox_service.h"

namespace hardware_pruefadapter {
namespace logic {

void Mailbox::SetAnalogTarget(std::uint8_t channel, std::uint16_t voltage_mv) {
  if (channel >= config::kNumChannels ||
      voltage_mv > config::kMaxSystemVoltageMv)
    return;
  std::lock_guard<std::mutex> lock(mutex_);
  analog_target_mv_[channel] = voltage_mv;
  new_analog_data_[channel] = true;
  LOG("[MAILBOX] Neuer Analog Wert für Kanal %d: %.2f V empfangen.\n",
      channel + 1, voltage_mv / 1000.0f);
}

void Mailbox::SetDigitalTarget(std::uint8_t channel, bool state) {
  if (channel >= config::kNumChannels) return;
  std::lock_guard<std::mutex> lock(mutex_);
  digital_target_state_[channel] = state;
  new_digital_data_[channel] = true;
  LOG("[MAILBOX] Neuer Digital Wert für Kanal %d: %s empfangen.\n", channel + 1,
      state ? "ON" : "OFF");
}

void Mailbox::SetDigitalReference(types::IO::Direction direction,
                                  std::uint8_t channel,
                                  std::uint16_t voltage_mv) {
  if (channel >= config::kNumChannels ||
      voltage_mv > config::kMaxSystemVoltageMv)
    return;
  std::lock_guard<std::mutex> lock(mutex_);
  reference_target_mv_[direction][channel] = voltage_mv;
  new_reference_[direction][channel] = true;
  has_reference_config_ = true;

  LOG("[MAILBOX] Neue Referenzspannung für %s Kanal %d: %.2f V empfangen.\n",
      types::IO::ToString(direction), channel + 1, voltage_mv / 1000.0f);
}

void Mailbox::AddSerialTx(types::Serial::Interface uart_num,
                          const std::string& payload) {
  if (uart_num >= types::Serial::kInterfaceCount || payload.empty()) return;
  std::lock_guard<std::mutex> lock(mutex_);
  serial_tx_payloads_[uart_num].append(payload);
  has_serial_tx_ = true;
}

void Mailbox::SetSerialConfig(types::Serial::Interface uart_num,
                              std::uint32_t baudrate) {
  if (uart_num >= types::Serial::kInterfaceCount ||
      baudrate > config::kMaxSerialBaudRate)
    return;
  std::lock_guard<std::mutex> lock(mutex_);
  serial_target_baudrate_[uart_num] = baudrate;
  new_serial_baudrate_[uart_num] = true;
  has_serial_config_ = true;
}

IoTargetsSnapshot Mailbox::ConsumeIoTargets() {
  IoTargetsSnapshot snapshot;
  std::lock_guard<std::mutex> lock(mutex_);

  for (std::uint8_t i = 0; i < config::kNumChannels; i++) {
    if (new_analog_data_[i]) {
      snapshot.new_analog[i] = true;
      snapshot.analog_mv[i] = analog_target_mv_[i];
      new_analog_data_[i] = false;
    }
    if (new_digital_data_[i]) {
      snapshot.new_digital[i] = true;
      snapshot.digital_state[i] = digital_target_state_[i];
      new_digital_data_[i] = false;
    }
  }
  return snapshot;
}

ReferenceConfigSnapshot Mailbox::ConsumeReferenceConfig() {
  ReferenceConfigSnapshot snapshot;
  std::lock_guard<std::mutex> lock(mutex_);

  if (has_reference_config_) {
    snapshot.has_config = true;
    for (int d = 0; d < 2; d++) {
      for (int c = 0; c < config::kNumChannels; c++) {
        if (new_reference_[d][c]) {
          snapshot.new_reference[d][c] = true;
          snapshot.reference_mv[d][c] = reference_target_mv_[d][c];
          new_reference_[d][c] = false;
        }
      }
    }
    has_reference_config_ = false;
  }
  return snapshot;
}

SerialTxSnapshot Mailbox::ConsumeSerialTx() {
  SerialTxSnapshot snapshot;
  std::lock_guard<std::mutex> lock(mutex_);

  if (has_serial_tx_) {
    snapshot.has_data = true;
    for (int i = 0; i < config::kSerialDriverCount; i++) {
      if (!serial_tx_payloads_[i].empty()) {
        snapshot.payloads[i] = std::move(serial_tx_payloads_[i]);
      }
    }
    has_serial_tx_ = false;
  }
  return snapshot;
}

SerialConfigSnapshot Mailbox::ConsumeSerialConfig() {
  SerialConfigSnapshot snapshot;
  std::lock_guard<std::mutex> lock(mutex_);

  if (has_serial_config_) {
    snapshot.has_config = true;
    for (int i = 0; i < config::kSerialDriverCount; i++) {
      if (new_serial_baudrate_[i]) {
        snapshot.new_baudrate[i] = true;
        snapshot.baudrates[i] = serial_target_baudrate_[i];
        new_serial_baudrate_[i] = false;
      }
    }
    has_serial_config_ = false;
  }
  return snapshot;
}

}  // namespace logic
}  // namespace hardware_pruefadapter