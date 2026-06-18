/**
 * @file process_imager_service.cpp
 * @brief Implementierung des Prozessabbilds.
 */

#include "process_imager_service.h"

namespace hardware_pruefadapter {
namespace logic {

void ProcessImager::SyncUpdateToActive() {
  std::lock_guard<std::mutex> lock(data_lock_);
  for (std::uint8_t j = 0; j < types::IO::kIoGroupCount; j++) {
    for (std::uint8_t i = 0; i < config::kNumChannels; i++) {
      std::uint32_t bit_pos = i + (j * types::IO::kIoGroupCount);
      if (voltage_[j][i] != update_voltage_[j][i]) {
        system_change_flags_ |= (1UL << bit_pos);
        voltage_[j][i] = update_voltage_[j][i];
        core::Logger::Log(
            "[SYSTEM] Änderung am Prozessabbild erkannt: Bank: %d, Channel: "
            "%d, Neuer Wert: %d mV",
            j, i, voltage_[j][i]);
        if (j == types::IO::kAnalogOut) {
          adjusted_dac_voltage_[i] = voltage_[j][i];
        }
      }
    }
  }
}

std::uint16_t ProcessImager::GetVoltage(types::IO::Group group,
                                        std::uint8_t channel) const {
  if (group >= types::IO::kIoGroupCount || channel >= config::kNumChannels)
    return 0;
  std::lock_guard<std::mutex> lock(data_lock_);
  return voltage_[group][channel];
}

void ProcessImager::SetVoltage(types::IO::Group group, std::uint8_t channel,
                               std::uint16_t voltage_mv) {
  if (group >= types::IO::kIoGroupCount || channel >= config::kNumChannels ||
      voltage_mv > config::kMaxSystemVoltageMv)
    return;
  std::lock_guard<std::mutex> lock(data_lock_);
  voltage_[group][channel] = voltage_mv;
}

std::uint16_t ProcessImager::GetUpdateVoltage(types::IO::Group group,
                                              std::uint8_t channel) const {
  if (group >= types::IO::kIoGroupCount || channel >= config::kNumChannels)
    return 0;
  std::lock_guard<std::mutex> lock(data_lock_);
  return update_voltage_[group][channel];
}

void ProcessImager::SetUpdateVoltage(types::IO::Group group,
                                     std::uint8_t channel,
                                     std::uint16_t voltage_mv) {
  if (group >= types::IO::kIoGroupCount || channel >= config::kNumChannels ||
      voltage_mv > config::kMaxSystemVoltageMv)
    return;
  std::lock_guard<std::mutex> lock(data_lock_);
  update_voltage_[group][channel] = voltage_mv;
}

std::uint16_t ProcessImager::GetReadbackVoltage(types::IO::Group group,
                                                std::uint8_t channel) const {
  if ((group != types::IO::kAnalogOut && group != types::IO::kDigitalOut) ||
      channel >= config::kNumChannels)
    return 0;
  std::lock_guard<std::mutex> lock(data_lock_);
  return reverse_voltage_check_[group - 2][channel];
}

void ProcessImager::SetReadbackVoltage(types::IO::Group group,
                                       std::uint8_t channel,
                                       std::uint16_t voltage_mv) {
  if ((group != types::IO::kAnalogOut && group != types::IO::kDigitalOut) ||
      channel >= config::kNumChannels ||
      voltage_mv > config::kMaxSystemVoltageMv)
    return;
  std::lock_guard<std::mutex> lock(data_lock_);
  reverse_voltage_check_[group - 2][channel] = voltage_mv;
}

std::uint16_t ProcessImager::GetAdjustedDacVoltage(std::uint8_t channel) const {
  if (channel >= config::kNumChannels) return 0;
  std::lock_guard<std::mutex> lock(data_lock_);
  return adjusted_dac_voltage_[channel];
}

void ProcessImager::SetAdjustedDacVoltage(std::uint8_t channel,
                                          std::uint16_t voltage_mv) {
  if (channel >= config::kNumChannels ||
      voltage_mv > config::kMaxSystemVoltageMv)
    return;
  std::lock_guard<std::mutex> lock(data_lock_);
  adjusted_dac_voltage_[channel] = voltage_mv;
}

std::uint16_t ProcessImager::GetDigitalReference(types::IO::Direction direction,
                                                 std::uint8_t channel) const {
  if (channel >= config::kNumChannels) return 0;
  std::lock_guard<std::mutex> lock(data_lock_);
  return digital_reference_mv_[direction][channel];
}

void ProcessImager::SetDigitalReference(types::IO::Direction direction,
                                        std::uint8_t channel,
                                        std::uint16_t voltage_mv) {
  if (channel >= config::kNumChannels ||
      voltage_mv > config::kMaxSystemVoltageMv)
    return;

  std::lock_guard<std::mutex> lock(data_lock_);
  digital_reference_mv_[direction][channel] = voltage_mv;

  core::Logger::LogInfo(
      "[SYSTEM] Referenzspannung fuer Digital %s Kanal %d auf %d mV gesetzt",
      types::IO::ToString(direction), channel + 1, voltage_mv);
}

std::int8_t ProcessImager::GetDigitalState(types::IO::Group group,
                                           std::uint8_t channel) const {
  if ((group != types::IO::kDigitalIn && group != types::IO::kDigitalOut) ||
      channel >= config::kNumChannels)
    return -1;

  std::lock_guard<std::mutex> lock(data_lock_);

  types::IO::Direction direction = types::IO::GetDirection(group);
  std::uint16_t voltage = (direction == types::IO::kInput)
                              ? voltage_[group][channel]
                              : reverse_voltage_check_[direction][channel];
  std::uint16_t ref_mv = digital_reference_mv_[direction][channel];

  return utils::data_converter::CalculateDigitalState(
      voltage, ref_mv, config::kLogicLowThresholdPercent,
      config::kLogicHighThresholdPercent);
}

void ProcessImager::ClearSystemChangeFlags() {
  std::lock_guard<std::mutex> lock(data_lock_);
  system_change_flags_ = 0;
}

std::uint32_t ProcessImager::GetSystemChangeFlags() const {
  std::lock_guard<std::mutex> lock(data_lock_);
  return system_change_flags_;
}

void ProcessImager::AddSystemChangeFlagMask(std::uint32_t mask) {
  std::lock_guard<std::mutex> lock(data_lock_);
  system_change_flags_ |= mask;
}

bool ProcessImager::HasChangeFlagMask(std::uint32_t mask) const {
  std::lock_guard<std::mutex> lock(data_lock_);
  return (system_change_flags_ & mask) != 0;
}

std::int8_t ProcessImager::GetLastSystemErrorState() const {
  std::lock_guard<std::mutex> lock(data_lock_);
  return last_system_error_state_;
}

void ProcessImager::SetLastSystemErrorState(std::int8_t state) {
  std::lock_guard<std::mutex> lock(data_lock_);
  last_system_error_state_ = state;
}

}  // namespace logic
}  // namespace hardware_pruefadapter