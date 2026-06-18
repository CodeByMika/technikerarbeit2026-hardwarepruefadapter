/**
 * @file mailbox_service.h
 * @brief Thread-sicherer Postkasten zwischen Web-API und Hardware-Loop.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-03-26
 * @version 1.0.0
 * @ingroup Services
 */
#ifndef HARDWARE_PRUEFADAPTER_SERVICES_PROCESS_MAILBOX_H_
#define HARDWARE_PRUEFADAPTER_SERVICES_PROCESS_MAILBOX_H_

#include <cstdint>
#include <mutex>
#include <string>

#include "logger_service.h"
#include "system_config.h"

namespace hardware_pruefadapter {
namespace logic {

struct IoTargetsSnapshot {
  bool new_analog[config::kNumChannels] = {false};
  std::uint16_t analog_mv[config::kNumChannels] = {0};
  bool new_digital[config::kNumChannels] = {false};
  bool digital_state[config::kNumChannels] = {false};
};

struct ReferenceConfigSnapshot {
  bool has_config = false;
  bool new_reference[2][config::kNumChannels] = {{false}};
  std::uint16_t reference_mv[2][config::kNumChannels] = {{0}};
};

struct SerialTxSnapshot {
  bool has_data = false;
  std::string payloads[config::kSerialDriverCount];
};

struct SerialConfigSnapshot {
  bool has_config = false;
  bool new_baudrate[config::kSerialDriverCount] = {false};
  std::uint32_t baudrates[config::kSerialDriverCount] = {0};
};

/**
 * @class ProcessMailbox
 * @brief Kapselt die asynchronen Befehle des Webservers thread-sicher.
 */
class Mailbox {
 public:
  Mailbox() = default;
  ~Mailbox() = default;

  // --- Setters (Werden vom asynchronen Web-API Thread gerufen) ---
  void SetAnalogTarget(std::uint8_t channel, std::uint16_t voltage_mv);
  void SetDigitalTarget(std::uint8_t channel, bool state);
  void SetDigitalReference(types::IO::Direction direction, std::uint8_t channel,
                           std::uint16_t voltage_mv);
  void AddSerialTx(types::Serial::Interface uart_num,
                   const std::string& payload);
  void SetSerialConfig(types::Serial::Interface uart_num,
                       std::uint32_t baudrate);

  // --- Consumers (Werden vom synchronen Hardware-Loop gerufen) ---
  IoTargetsSnapshot ConsumeIoTargets();
  ReferenceConfigSnapshot ConsumeReferenceConfig();
  SerialTxSnapshot ConsumeSerialTx();
  SerialConfigSnapshot ConsumeSerialConfig();

 private:
  mutable std::mutex mutex_;

  // I/O Puffer
  bool new_analog_data_[config::kNumChannels] = {false};
  std::uint16_t analog_target_mv_[config::kNumChannels] = {0};
  bool new_digital_data_[config::kNumChannels] = {false};
  bool digital_target_state_[config::kNumChannels] = {false};

  // Referenz Puffer
  bool has_reference_config_ = false;
  bool new_reference_[2][config::kNumChannels] = {{false}};
  std::uint16_t reference_target_mv_[2][config::kNumChannels] = {{0}};

  // Serial Puffer
  std::string serial_tx_payloads_[config::kSerialDriverCount];
  bool has_serial_tx_ = false;

  // Serial Config Puffer
  std::uint32_t serial_target_baudrate_[config::kSerialDriverCount] = {0};
  bool new_serial_baudrate_[config::kSerialDriverCount] = {false};
  bool has_serial_config_ = false;
};

}  // namespace logic
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_SERVICES_PROCESS_MAILBOX_H_