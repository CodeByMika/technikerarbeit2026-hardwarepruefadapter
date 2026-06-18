/**
 * @file ads1x15_adapter.cpp
 * @brief Implementierung des ADS1115 Treibers.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-01-08
 * @version 1.0.0
 * @ingroup Adapters
 * @copyright Copyright (c) 2025 SOTEC GmBH & Co KG
 */

#include "ads1x15_adapter.h"

namespace hardware_pruefadapter {
namespace adapters {

//==================Public General====================
Ads1x15Adapter::Ads1x15Adapter()
    : i2c_device_address_(0x48),       // Default: Alles gegen GND
      current_gain_(4096),             // Default Gain
      maximum_bit_resolution_(32768),  // 15 Bit (Signed)
      current_mode_(false),            // false = Single Shot, true = continous
      external_internal_mode_(false),  // false = Extern, true = Intern
      diffential_mode_(0),  // 0 = Single-Ended, 1 = Diff 0-1, 2 = Diff 0-3, 3 =
                            // Diff 1-3, 4 = Diff 2-3
      is_online_(false) {
  // Initialisierungsliste wird genutzt, Konstruktor-Rumpf bleibt leer.
}

// Die Hilfsfunktion für Flankenauswertung und Logging
void Ads1x15Adapter::UpdateOnlineStatus(bool success) const {
  if (success != is_online_) {
    is_online_ = success;
  }
}

types::ErrorCode Ads1x15Adapter::Initialize(std::uint8_t address,
                                            std::uint16_t gain_mv,
                                            std::uint8_t i2c_bus_nr) {
  TwoWire* i2c_bus;

  switch (i2c_bus_nr) {
    case 0:
      i2c_bus = &Wire;  // I2C Bus 0 nutzen
      break;
    case 1:
      i2c_bus = &Wire1;  // I2C Bus 1 nutzen
      break;
    default:
      return types::ErrorCode::kErrorInvalidValue;  // Ungültige Bus-Nummer
  }

  if (!adc_.begin(address, i2c_bus)) {
    return types::ErrorCode::kErrorInitializeFailure;
  }

  UpdateOnlineStatus(true);
  i2c_device_address_ = address;

  if (!SetGain(gain_mv)) {
    return types::ErrorCode::kErrorInvalidValue;
  }

  current_gain_ = gain_mv;

  return types::ErrorCode::kSuccess;
}

bool Ads1x15Adapter::IsConnected() const {
  Wire.beginTransmission(i2c_device_address_);
  bool success = (Wire.endTransmission() == 0);
  UpdateOnlineStatus(success);
  return success;
}

bool Ads1x15Adapter::IsBusy() const { return false; }

//================Public Set Methoden=================

bool Ads1x15Adapter::SetMode(bool mode) {
  // Mapping: true = Continuous, false = Single-Shot
  current_mode_ = mode;
  return true;
}

bool Ads1x15Adapter::SetGain(std::uint16_t gain_mv) {
  adsGain_t adafruit_gain;

  switch (gain_mv) {
    case 6144:  // +/- 6.144V
      adafruit_gain = GAIN_TWOTHIRDS;
      break;
    case 4096:  // +/- 4.096V
      adafruit_gain = GAIN_ONE;
      break;
    case 2048:  // +/- 2.048V
      adafruit_gain = GAIN_TWO;
      break;
    case 1024:  // +/- 1.024V
      adafruit_gain = GAIN_FOUR;
      break;
    case 512:  // +/- 0.512V
      adafruit_gain = GAIN_EIGHT;
      break;
    case 256:  // +/- 0.256V
      adafruit_gain = GAIN_SIXTEEN;
      break;
    default:
      return false;  // Ungültiger Wert
  }

  // Zustand speichern
  current_gain_ = gain_mv;
  // Hardware setzen
  adc_.setGain(adafruit_gain);

  return true;
}

types::ErrorCode Ads1x15Adapter::SetDataRate(std::uint16_t sps) {
  types::ErrorCode status = types::ErrorCode::kSuccess;
  switch (sps) {
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
    case 250:
    case 475:
    case 860:
    default:
      status = types::ErrorCode::kErrorInvalidValue;
  }

  if (status == types::ErrorCode::kSuccess) {
    adc_.setDataRate(sps);
  }

  return status;
}

types::ErrorCode Ads1x15Adapter::SetDifferentialMode(
    std::uint8_t channel_low, std::uint8_t channel_high) {
  if (channel_low >= max_channel_num_ || channel_high >= max_channel_num_) {
    return types::ErrorCode::kErrorInvalidChannel;
  }
  if (channel_low == channel_high) {
    return types::ErrorCode::kErrorInvalidValue;
  }
  if (channel_low > channel_high) {
    std::swap(channel_low, channel_high);
  }
  if (!((channel_low == 0 && channel_high == 1) ||
        (channel_low == 0 && channel_high == 3) ||
        (channel_low == 1 && channel_high == 3) ||
        (channel_low == 2 && channel_high == 3))) {
    return types::ErrorCode::kErrorInvalidChannel;
  }

  // 0 = Single-Ended, 1 = Diff 0-1, 2 = Diff 0-3, 3 = Diff 1-3, 4 = Diff 2-3

  if (channel_low == 0 && channel_high == 1) {
    this->diffential_mode_ = 1;
  } else if (channel_low == 0 && channel_high == 3) {
    this->diffential_mode_ = 2;
  } else if (channel_low == 1 && channel_high == 3) {
    this->diffential_mode_ = 3;
  } else if (channel_low == 2 && channel_high == 3) {
    this->diffential_mode_ = 4;
  }

  return types::ErrorCode::kSuccess;
}

bool Ads1x15Adapter::SetInternExtern(bool intern_extern) {
  // False: Extern
  // True: Intern
  external_internal_mode_ = intern_extern;
  return true;
}

//================Public Get Methoden=================

types::ErrorCode Ads1x15Adapter::GetValue(std::uint8_t channel,
                                          std::int16_t* raw_out) const {
  std::int16_t raw;
  if (raw_out == nullptr) {
    return types::ErrorCode::kErrorProgramming;
  }

  if (channel >= (max_channel_num_)) {
    *raw_out = 0;
    return types::ErrorCode::kErrorInvalidChannel;
  }

  if (!is_online_) {
    *raw_out = 0;  // Rückgabe nullen, wenn offline
    return types::ErrorCode::kErrorDeviceNotFound;
  }

  if (diffential_mode_ == 0) {
    raw = adc_.readADC_SingleEnded(channel);
  } else if (diffential_mode_ == 1) {
    raw = adc_.readADC_Differential_0_1();
  } else if (diffential_mode_ == 2) {
    raw = adc_.readADC_Differential_0_3();
  } else if (diffential_mode_ == 3) {
    raw = adc_.readADC_Differential_1_3();
  } else if (diffential_mode_ == 4) {
    raw = adc_.readADC_Differential_2_3();
  } else {
    return types::ErrorCode::kErrorInvalidConfig;
  }

  *raw_out = raw;

  return types::ErrorCode::kSuccess;
}

bool Ads1x15Adapter::GetMode() const { return current_mode_; }

std::uint16_t Ads1x15Adapter::GetGain() const { return current_gain_; }

bool Ads1x15Adapter::GetInternExtern() const { return external_internal_mode_; }

//==============Hilfs Methoden==============

std::uint16_t Ads1x15Adapter::ConvertToVoltage(std::int16_t raw) const {
  // Berechnung basiert auf der linearen Skalierung:
  // Ein Rohwert von 32768 (2^15) entspräche theoretisch genau der
  // Gain-Spannung.

  if (raw < 0) {
    raw = -raw;  // Absolutwert für negative Werte
  }

  if (raw > maximum_bit_resolution_) {
    raw = maximum_bit_resolution_;  // Begrenzung auf Maximalwert
  }

  if (current_gain_ == 0) {
    return 0;  // Vermeidung von Division durch Null
  }

  if (raw == 0) {
    return 0;  // Schnellentscheidung für Nullwert
  }

  std::uint32_t voltage =
      (raw * current_gain_ * 100) / maximum_bit_resolution_;

  return static_cast<std::uint16_t>(voltage);
}

std::uint8_t Ads1x15Adapter::GetDifferentialMode() const {
  return diffential_mode_;
}

bool Ads1x15Adapter::GetOnlineStatus() const { return is_online_; }

types::ErrorCode Ads1x15Adapter::GetI2cAddress(std::uint8_t* address) const {
  if (address == nullptr) {
    return types::ErrorCode::kErrorProgramming;
  }
  *address = i2c_device_address_;
  return types::ErrorCode::kSuccess;
}

}  // namespace adapters
}  // namespace hardware_pruefadapter