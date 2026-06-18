/**
 * @file platform_factory.h
 * @brief Abstraktions-Factory für plattformspezifische Hardware.
 *
 * Trennt die Initialisierung und Erstellung von Hardware-Objekten
 * (ESP32 vs. PC-Simulation) sauber auf.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-03-25
 * @version 1.0.0
 * @ingroup Platform
 */
#ifndef HARDWARE_PRUEFADAPTER_PLATFORM_FACTORY_H_
#define HARDWARE_PRUEFADAPTER_PLATFORM_FACTORY_H_

#include <memory>

#include "../Interfaces/analog_to_digital_interface.h"
#include "../Interfaces/digital_to_analog_interface.h"
#include "../Interfaces/led_interface.h"
#include "../Interfaces/serial_interface.h"
#include "../Interfaces/web_server_interface.h"

namespace hardware_pruefadapter {
namespace platform {

/** @brief Initialisiert die plattformspezifische Basis-Hardware (Serial, I2C,
 * GPIO). */
void Init();

/** @brief Startet das Netzwerk (WLAN AP oder lokaler Simulator). */
void NetworkSetup();

// --- Factory Methoden ---
std::unique_ptr<interfaces::IWebServer> CreateWebServer();
std::unique_ptr<interfaces::IAnalogToDigitalConverter> CreateAdc();
std::unique_ptr<interfaces::IDigitalToAnalogConverter> CreateDac();
std::unique_ptr<interfaces::ILedDriver> CreateLedDriver();
std::unique_ptr<interfaces::ISerialInterface> CreateSerial(
    types::Serial::Interface uart_num);

// --- Plattform-Hilfsmethoden ---
void i2cIsConnected(std::uint8_t address, std::uint8_t i2c_bus_nr);
void SetDigitalOutPin(std::uint8_t channel, bool state, std::uint16_t ref_mv);
void SetSimulatedReferenceVoltage(types::IO::Direction direction,
                                  std::uint8_t channel,
                                  std::uint16_t voltage_mv);

}  // namespace platform
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_PLATFORM_FACTORY_H_