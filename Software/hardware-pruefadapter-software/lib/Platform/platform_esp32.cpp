#ifdef ESP32_ENV

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>

#include "../../include/system_config.h"
#include "../Adapters/ADS1X15Adapter/ads1x15_adapter.h"
#include "../Adapters/Dac8574Adapter/dac8574_adapter.h"
#include "../Adapters/ESPAsyncWebServerAdapter/esp_async_server_adapter.h"
#include "../Adapters/Lp50xxAdapter/lp50xx_adapter.h"
#include "../Adapters/SerialAdapter/serial_adapter.h"
#include "../Logic/Services/logger_service.h"
#include "platform_factory.h"

namespace hardware_pruefadapter {
namespace platform {

void Init() {
  Serial.begin(115200);

  // Startet Sofort nach Initialisierung
  // std::uint32_t start_time = millis();
  // while (!Serial && (millis() - start_time < 3000));

  LOG("[PLATFORM] ESP32 Init...");

  for (int i = 0; i < 4; i++) {
    pinMode(config::kDigitalOutPinsLowVolt[i], OUTPUT);
    digitalWrite(config::kDigitalOutPinsLowVolt[i], LOW);

    pinMode(config::kDigitalOutPinsHighVolt[i], OUTPUT);
    digitalWrite(config::kDigitalOutPinsHighVolt[i], LOW);
  }
  LOG("[PLATFORM] GPIOs für Digital-Out konfiguriert.");

  // I2C Starten
  if (Wire.begin(config::kI2c0SdaPin, config::kI2c0SclPin,
                 config::kI2cFrequency)) {
    LOG("[PLATFORM] I2C Wire0 started. SDA: %d SCL: %d", config::kI2c0SdaPin,
        config::kI2c0SclPin);
    // i2cIsConnected(config::kAdcI2cAddressAnalogIn, 0);
    // i2cIsConnected(config::kDacI2cAddressAnalogOut, 0);
    // i2cIsConnected(config::kLedDriver1I2cAddress, 0);
  } else {
    LOG_ERROR("[PLATFORM] I2C Wire0 failed.");
  }

  if (Wire1.begin(config::kI2c1SdaPin, config::kI2c1SclPin,
                  config::kI2cFrequency)) {
    LOG("[PLATFORM] I2C Wire1 started. SDA: %d SCL: %d", config::kI2c1SdaPin,
        config::kI2c1SclPin);
    // i2cIsConnected(config::kAdcI2cAddressAnalogIn, 1);
    // i2cIsConnected(config::kDacI2cAddressAnalogOut, 1);
    // i2cIsConnected(config::kLedDriver1I2cAddress, 1);
  } else {
    LOG_ERROR("[PLATFORM] I2C Wire1 failed.");
  }
}

void NetworkSetup() {
  LOG("[PLATFORM] Starting WiFi AP...");
  WiFi.softAP(config::kSsid, config::kPassword);
  LOG("[PLATFORM] IP Address: %s", WiFi.softAPIP().toString().c_str());
}

std::unique_ptr<interfaces::IWebServer> CreateWebServer() {
  return std::make_unique<adapters::EspAsyncServerAdapter>(
      config::kWebServerPort);
}

std::unique_ptr<interfaces::IAnalogToDigitalConverter> CreateAdc() {
  return std::make_unique<adapters::Ads1x15Adapter>();
}

std::unique_ptr<interfaces::IDigitalToAnalogConverter> CreateDac() {
  return std::make_unique<adapters::Dac8574Adapter>();
}

std::unique_ptr<interfaces::ILedDriver> CreateLedDriver() {
  return std::make_unique<adapters::Lp50xxAdapter>();
}

std::unique_ptr<interfaces::ISerialInterface> CreateSerial(
    types::Serial::Interface uart_num) {
  return std::make_unique<adapters::SerialAdapter>();
}

void i2cIsConnected(std::uint8_t address, std::uint8_t i2c_bus_nr) {
  // 0: success.
  // 1: data too long to fit in transmit buffer.
  // 2: received NACK on transmit of address.
  // 3: received NACK on transmit of data.
  // 4: other error.
  // 5: timeout error.
  if (i2c_bus_nr > 1) {
    LOG_ERROR("Invalid I2C bus number. Must be 0 or 1.");
    return;
  }
  TwoWire& i2c_bus = (i2c_bus_nr == 0) ? Wire : Wire1;

  i2c_bus.beginTransmission(address);
  std::uint8_t error = i2c_bus.endTransmission();

  if (error == 0) {
    const char* device_name = "";

    if (address == config::kLedDriver1I2cAddress ||
        address == config::kLedDriver2I2cAddress ||
        address == config::kLedDriver3I2cAddress)
      device_name = " (LP50xx LED Driver)";
    else if (address == config::kAdcI2cAddressAnalogIn)
      device_name = " (ADS1115 ADC)";
    else if (address == config::kDacI2cAddressAnalogOut)
      device_name = " (DAC8574 DAC)";
    else if (address == config::kLedDriverBroadcastAddress)
      device_name = " (LP50xx Broadcast)";

    LOG("[I2C] Device found at address 0x%02X%s", address, device_name);

  } else if (error == 4) {
    LOG("[I2C] Unknown error at address 0x%02X", address);
  } else if (error == 5) {
    LOG("[I2C] Timeout error at address 0x%02X", address);
  } else {
    LOG("[I2C] No device at address 0x%02X", address);
  }
}

void SetDigitalOutPin(std::uint8_t channel, bool state, std::uint16_t ref_mv) {
  if (ref_mv > 5000) {
    // High Voltage (12V, 24V)
    digitalWrite(config::kDigitalOutPinsLowVolt[channel], LOW);
    digitalWrite(config::kDigitalOutPinsHighVolt[channel], state ? HIGH : LOW);
  } else {
    // Low Voltage (1.8V, 3.3V, 5V)
    digitalWrite(config::kDigitalOutPinsHighVolt[channel], LOW);
    digitalWrite(config::kDigitalOutPinsLowVolt[channel], state ? HIGH : LOW);
  }
}

void SetSimulatedReferenceVoltage(types::IO::Direction direction,
                                  std::uint8_t channel,
                                  std::uint16_t voltage_mv) {
  // Wird im ESP32-Kontext physikalisch auf der Platine umgesetzt, hier also ein
  // Leerlauf.
}

}  // namespace platform
}  // namespace hardware_pruefadapter

#endif