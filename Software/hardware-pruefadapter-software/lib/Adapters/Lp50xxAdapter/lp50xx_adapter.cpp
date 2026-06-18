/**
 * @file lp50xx_adapter.cpp
 * @brief Implementierung des LP50xx LED-Treibers.
 */

#include "lp50xx_adapter.h"

#include <Arduino.h>
#include <Wire.h>

#include "../../Logic/Services/logger_service.h"
#include "color_types.h"
#include "error_code.h"
#include "system_config.h"

namespace hardware_pruefadapter {
namespace adapters {

Lp50xxAdapter::Lp50xxAdapter() : i2c_device_address_(0x28), chip_nr_(0) {}

types::ErrorCode Lp50xxAdapter::Initialize(std::uint8_t address,
                                           std::uint8_t config_val,
                                           std::uint8_t chip_nr,
                                           std::uint8_t i2c_bus_nr) {
  if (address <= 0 || address > 127) {
    LOG_ERROR(
        "Ungültige I2C-Adresse: %d. Adresse muss zwischen 1 und 127 liegen.",
        address);
    return types::ErrorCode::kErrorInvalidPinConfig;
  }
  if (chip_nr >= config::kLedDriverCount) {
    LOG_ERROR(
        "Ungültige Chip-Nummer: %d. Chip-Nummer muss zwischen 0 und %d liegen.",
        chip_nr, config::kLedDriverCount - 1);
    return types::ErrorCode::kErrorInvalidPinConfig;
  }

  if (config_val > 0xFF) {
    LOG_ERROR(
        "Ungültige Konfiguration: %d. Konfigurationswert muss zwischen 0 und "
        "255 liegen.",
        config_val);
    return types::ErrorCode::kErrorInvalidConfig;
  }

  if (i2c_bus_nr > 1) {
    LOG_ERROR("Ungültige I2C-Busnummer: %d. Busnummer muss 0 oder 1 sein.",
              i2c_bus_nr);
    return types::ErrorCode::kErrorInvalidPinConfig;
  }

  TwoWire& i2c_bus = (i2c_bus_nr == config::kI2cBus0) ? Wire : Wire1;

  i2c_bus.beginTransmission(address);
  std::uint16_t error = i2c_bus.endTransmission();
  if (error != 0) {
    LOG_ERROR("Gerät auf I2C-Adresse %d nicht gefunden. Error: %d", address,
              error);
    return types::ErrorCode::kErrorDeviceNotFound;
  }

  if (address != 0xFF) {
    i2c_device_address_ = address;
  }
  chip_nr_ = chip_nr;
  LOG_INFO("LP50xx Adapter: I2C-Adresse gesetzt auf 0x%02X, Chip-Nummer: %d",
           i2c_device_address_, chip_nr_);

  led_driver_.Begin(i2c_device_address_);

  led_driver_.Configure(config_val);

  led_driver_.SetLEDConfiguration(BGR);

  return types::ErrorCode::kSuccess;
}

types::ErrorCode Lp50xxAdapter::SetColor(std::uint8_t channel,
                                         types::ColorName color,
                                         types::AddressType address_type) {
  types::RgbColor rgb = types::GetRgbFromColorName(color);

  EAddressType address_type_ext =
      (address_type == types::AddressType::kBroadcast) ? EAddressType::Broadcast
                                                       : EAddressType::Normal;

  // LOG_INFO(
  //     "Setting LED at Driver %d, Channel %d to color %s with address type %s",
  //     chip_nr_ + 1, channel + 1, types::ColorNameToString(color).data(),
  //     (address_type_ext == EAddressType::Broadcast) ? "Broadcast" : "Normal");

  if (address_type_ext == EAddressType::Normal) {
    led_driver_.SetLEDColor(channel, rgb.red, rgb.green, rgb.blue,
                            address_type_ext);
  } else if (address_type_ext == EAddressType::Broadcast) {
    led_driver_.SetLEDColor(channel, rgb.red, rgb.green, rgb.blue,
                            EAddressType::Broadcast);
  }

  return types::ErrorCode::kSuccess;
}

types::ErrorCode Lp50xxAdapter::TurnOff() {
  led_driver_.SetGlobalLedOff(types::kLedGlobalOff);
  return types::ErrorCode::kSuccess;
}

}  // namespace adapters
}  // namespace hardware_pruefadapter