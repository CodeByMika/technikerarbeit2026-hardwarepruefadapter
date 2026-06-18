/**
 * @file dac8574_adapter.h
 * @brief Konkreter Adapter für den DAC8574 Digital-Analog-Wandler.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-01-29
 * @version 1.0.0
 * @ingroup Adapters
 */
#ifndef HARDWARE_PRUEFADAPTER_ADAPTERS_DAC8574_ADAPTER_H_
#define HARDWARE_PRUEFADAPTER_ADAPTERS_DAC8574_ADAPTER_H_

#include <Arduino.h>
#include <DAC8574.h>
#include <Wire.h>

#include <cmath>
#include <cstdint>

#include "../Interfaces/digital_to_analog_interface.h"
#include "error_code.h"
#include "system_config.h"

namespace hardware_pruefadapter {
namespace adapters {

class Dac8574Adapter : public interfaces::IDigitalToAnalogConverter {
 public:
  Dac8574Adapter();
  ~Dac8574Adapter() override = default;

  types::ErrorCode Initialize(std::uint8_t address, std::uint16_t vref_mv,
                              std::uint8_t i2c_bus_nr) override;
  bool IsConnected() const override;
  bool SetMode(std::uint8_t mode) override;
  types::ErrorCode SetValue(std::uint8_t channel,
                            std::uint16_t voltage_mv) override;
  types::ErrorCode SetVoltageReference(std::uint16_t voltage_low_mv,
                                       std::uint16_t voltage_high_mv) override;
  types::ErrorCode GetValue(std::uint8_t channel,
                            std::uint16_t* voltage_mv_in) const override;
  types::ErrorCode GetMaxVoltage(std::uint16_t* voltage_high_mv) const override;
  types::ErrorCode GetGain(std::uint16_t* gain_mv) const override;
  std::uint8_t GetMode() const override;
  std::uint16_t ConvertToVoltage(std::uint16_t raw) const override;
  std::uint16_t ConvertToRaw(std::uint16_t voltage_mv) const override;

 private:
  std::uint8_t i2c_device_address_;
  std::uint16_t voltage_reference_mv_;
  std::uint8_t current_mode_;
  DAC8574 dac_;
};

}  // namespace adapters
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_ADAPTERS_DAC8574_ADAPTER_H_