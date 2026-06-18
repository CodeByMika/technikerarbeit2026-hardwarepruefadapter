/**
 * @file process_imager_service.h
 * @brief Prozessabbild-Service (Datenbank für alle I/O-Zustände).
 */
#ifndef HARDWARE_PRUEFADAPTER_SERVICES_PROCESS_IMAGER_SERVICE_H_
#define HARDWARE_PRUEFADAPTER_SERVICES_PROCESS_IMAGER_SERVICE_H_

#include <cstdint>
#include <mutex>

#include "../Utils/data_converter_utils.h"
#include "logger_service.h"
#include "system_config.h"
#include "system_types.h"

namespace hardware_pruefadapter {
namespace logic {

class ProcessImager {
 public:
  ProcessImager() = default;
  ~ProcessImager() = default;

  void SyncUpdateToActive();

  std::uint16_t GetVoltage(types::IO::Group group, std::uint8_t channel) const;
  void SetVoltage(types::IO::Group group, std::uint8_t channel,
                  std::uint16_t voltage_mv);

  std::uint16_t GetUpdateVoltage(types::IO::Group group,
                                 std::uint8_t channel) const;
  void SetUpdateVoltage(types::IO::Group group, std::uint8_t channel,
                        std::uint16_t voltage_mv);

  std::uint16_t GetReadbackVoltage(types::IO::Group group,
                                   std::uint8_t channel) const;
  void SetReadbackVoltage(types::IO::Group group, std::uint8_t channel,
                          std::uint16_t voltage_mv);

  std::uint16_t GetAdjustedDacVoltage(std::uint8_t channel) const;
  void SetAdjustedDacVoltage(std::uint8_t channel, std::uint16_t voltage_mv);

  std::uint16_t GetDigitalReference(types::IO::Direction direction,
                                    std::uint8_t channel) const;
  void SetDigitalReference(types::IO::Direction direction, std::uint8_t channel,
                           std::uint16_t voltage_mv);

  std::int8_t GetDigitalState(types::IO::Group group,
                              std::uint8_t channel) const;
  void ClearSystemChangeFlags();
  std::uint32_t GetSystemChangeFlags() const;
  void AddSystemChangeFlagMask(std::uint32_t mask);
  bool HasChangeFlagMask(std::uint32_t mask) const;

  std::int8_t GetLastSystemErrorState() const;
  void SetLastSystemErrorState(std::int8_t state);

 private:
  mutable std::mutex data_lock_;

  std::uint16_t voltage_[types::IO::kIoGroupCount][config::kNumChannels]{};
  std::uint16_t update_voltage_[types::IO::kIoGroupCount]
                               [config::kNumChannels]{};
  std::uint16_t reverse_voltage_check_[types::IO::kIoGroupCount / 2]
                                      [config::kNumChannels]{};
  std::uint16_t digital_reference_mv_[types::IO::kIoGroupCount /
                                      2][config::kNumChannels] = {
      {config::kMaxSystemVoltageMv, config::kMaxSystemVoltageMv,
       config::kMaxSystemVoltageMv, config::kMaxSystemVoltageMv},
      {config::kMaxSystemVoltageMv, config::kMaxSystemVoltageMv,
       config::kMaxSystemVoltageMv, config::kMaxSystemVoltageMv}};

  std::uint16_t adjusted_dac_voltage_[config::kNumChannels]{};

  std::uint32_t system_change_flags_ = 0;
  std::int8_t last_system_error_state_ = -1;
};

}  // namespace logic
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_SERVICES_PROCESS_IMAGER_SERVICE_H_