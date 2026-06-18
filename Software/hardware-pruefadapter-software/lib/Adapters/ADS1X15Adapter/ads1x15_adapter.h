/**
 * @file ads1x15_adapter.h
 * @brief Adapter für den ADS1115 Analog-Digital-Wandler.
 *
 * Verbindet das generische IAnalogToDigitalConverter Interface mit der Adafruit
 * ADS1X15 Bibliothek.
 */

#ifndef HARDWARE_PRUEFADAPTER_ADAPTERS_ADS1X15_ADAPTER_H_
#define HARDWARE_PRUEFADAPTER_ADAPTERS_ADS1X15_ADAPTER_H_

//====================System Header===========================
#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <Wire.h>

#include <cmath>
#include <cstdint>

//===================Projekt Header===========================
#include "../Interfaces/analog_to_digital_interface.h"
#include "error_code.h"
#include "system_config.h"

namespace hardware_pruefadapter {
namespace adapters {

class Ads1x15Adapter : public interfaces::IAnalogToDigitalConverter {
 public:
  Ads1x15Adapter();
  ~Ads1x15Adapter() override = default;

  types::ErrorCode Initialize(std::uint8_t address, std::uint16_t gain_mv,
                              std::uint8_t i2c_bus_nr) override;
  bool IsConnected() const override;
  bool IsBusy() const override;
  bool SetMode(bool mode) override;
  bool SetGain(std::uint16_t gain_mv) override;
  types::ErrorCode SetDataRate(std::uint16_t sps) override;
  types::ErrorCode SetDifferentialMode(std::uint8_t channel_low,
                                       std::uint8_t channel_high) override;
  bool SetInternExtern(bool intern_extern) override;
  types::ErrorCode GetValue(std::uint8_t channel,
                            std::int16_t* raw_out) const override;
  bool GetMode() const override;
  std::uint16_t GetGain() const override;
  bool GetInternExtern() const override;
  std::uint16_t ConvertToVoltage(std::int16_t raw) const override;
  std::uint8_t GetDifferentialMode() const override;
  bool GetOnlineStatus() const override;
  types::ErrorCode GetI2cAddress(std::uint8_t* address) const override;

 private:
  mutable Adafruit_ADS1015 adc_;
  std::uint8_t i2c_device_address_;
  std::uint16_t current_gain_;
  std::uint16_t maximum_bit_resolution_;
  bool current_mode_;
  std::uint8_t max_channel_num_ = config::kNumChannels;
  std::uint8_t diffential_mode_;
  bool external_internal_mode_;
  mutable bool is_online_{false};
  void UpdateOnlineStatus(bool success) const;
};

}  // namespace adapters
}  // namespace hardware_pruefadapter
#endif