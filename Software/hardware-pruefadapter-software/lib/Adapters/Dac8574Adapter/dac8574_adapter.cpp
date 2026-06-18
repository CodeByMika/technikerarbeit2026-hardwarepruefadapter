/**
 * @file dac8574_adapter.cpp
 * @brief Implementierung des DAC8574 Treibers.
 */

#include "dac8574_adapter.h"

namespace hardware_pruefadapter {
namespace adapters {

Dac8574Adapter::Dac8574Adapter()
    : i2c_device_address_(0x4C),
      voltage_reference_mv_(5000),
      current_mode_(0) {}

types::ErrorCode Dac8574Adapter::Initialize(std::uint8_t address,
                                            std::uint16_t vref_mv,
                                            std::uint8_t i2c_bus_nr) {
  TwoWire* i2c_bus;
  switch (i2c_bus_nr) {
    case 0:
      i2c_bus = &Wire;
      break;
    case 1:
      i2c_bus = &Wire1;
      break;
    default:
      return types::ErrorCode::kErrorInvalidValue;
  }

  dac_ = DAC8574(address, i2c_bus);
  i2c_device_address_ = address;
  voltage_reference_mv_ = vref_mv;

  if (!dac_.begin()) {
    return types::ErrorCode::kErrorInitializeFailure;
  }

  return types::ErrorCode::kSuccess;
}

bool Dac8574Adapter::IsConnected() const {
  Wire.beginTransmission(i2c_device_address_);
  return (Wire.endTransmission() == 0);
}

bool Dac8574Adapter::SetMode(std::uint8_t mode) {
  current_mode_ = mode;
  // TODO: Modus in Register schreiben
  return true;
}

types::ErrorCode Dac8574Adapter::SetValue(std::uint8_t channel,
                                          std::uint16_t voltage_mv) {
  if (channel > config::kNumChannels-1) {
    return types::ErrorCode::kErrorInvalidChannel;
  }

  if (voltage_mv > voltage_reference_mv_) {
    return types::ErrorCode::kErrorInvalidValue;
  }

  std::uint16_t raw_value = ConvertToRaw(voltage_mv);

  if (raw_value > 65535) {
    return types::ErrorCode::kErrorInvalidValue;
  }

  dac_.write(channel, raw_value);
  return types::ErrorCode::kSuccess;
}

types::ErrorCode Dac8574Adapter::SetVoltageReference(
    std::uint16_t voltage_low_mv, std::uint16_t voltage_high_mv) {
  if (voltage_high_mv <= voltage_low_mv) {
    return types::ErrorCode::kErrorInvalidValue;
  }
  voltage_reference_mv_ = voltage_high_mv;
  return types::ErrorCode::kSuccess;
}

types::ErrorCode Dac8574Adapter::GetValue(std::uint8_t channel,
                                          std::uint16_t* voltage_mv_in) const {
  if (voltage_mv_in == nullptr) {
    return types::ErrorCode::kErrorProgramming;
  }

  // TODO: Wert aus Register lesen oder Cache
  std::uint16_t raw_simulated = 0;
  *voltage_mv_in = ConvertToVoltage(raw_simulated);
  return types::ErrorCode::kSuccess;
}

types::ErrorCode Dac8574Adapter::GetMaxVoltage(
    std::uint16_t* voltage_high_mv) const {
  if (voltage_high_mv == nullptr) {
    return types::ErrorCode::kErrorProgramming;
  }
  *voltage_high_mv = voltage_reference_mv_;
  return types::ErrorCode::kSuccess;
}

types::ErrorCode Dac8574Adapter::GetGain(std::uint16_t* gain_mv) const {
  if (gain_mv == nullptr) {
    return types::ErrorCode::kErrorProgramming;
  }
  *gain_mv = 1000;
  return types::ErrorCode::kSuccess;
}

std::uint8_t Dac8574Adapter::GetMode() const { return current_mode_; }

std::uint16_t Dac8574Adapter::ConvertToVoltage(std::uint16_t raw) const {
  return (static_cast<uint16_t>(raw) * voltage_reference_mv_) / 65535;
}

std::uint16_t Dac8574Adapter::ConvertToRaw(std::uint16_t voltage_mv) const {
  return static_cast<std::uint16_t>(
      (voltage_mv * 65535 / voltage_reference_mv_));
}

}  // namespace adapters
}  // namespace hardware_pruefadapter