/**
 * @file led_interface.h
 * @brief Abstraktes Interface zur Steuerung von LED-Treibern.
 *
 * Entkoppelt die physikalische LED-Hardware (z.B. LP50xx) von der
 * restlichen Systemlogik. Ermöglicht das gezielte Setzen von Farben.
 *
 * @author Techniker_Team_2025_26
 * @date 2025-12-18
 * @version 1.0.0
 * @ingroup Interfaces
 * @copyright Copyright (c) 2025 SOTEC GmBH & Co KG
 */
#ifndef HARDWARE_PRUEFADAPTER_INTERFACES_LED_INTERFACE_H_
#define HARDWARE_PRUEFADAPTER_INTERFACES_LED_INTERFACE_H_

#include <cstdint>

#include "color_types.h"
#include "error_code.h"
#include "system_types.h"

namespace hardware_pruefadapter {
namespace interfaces {

/**
 * @class ILedDriver
 * @brief Schnittstelle für LED-Controller.
 */
class ILedDriver {
 public:
  virtual ~ILedDriver() = default;

  /**
   * @brief Startet und konfiguriert den LED-Chip.
   * @param address I2C-Adresse des Chips.
   * @param config Konfigurationsbyte (z.B. Helligkeit, Modus).
   * @param chip_nr Laufende Nummer des Chips für systemweite Zuordnung.
   * @param i2c_bus_nr Verwendeter I2C-Bus.
   * @return kSuccess bei Erfolg.
   */
  virtual types::ErrorCode Initialize(std::uint8_t address, std::uint8_t config,
                                      std::uint8_t chip_nr,
                                      std::uint8_t i2c_bus_nr = 0) = 0;

  /**
   * @brief Setzt die Farbe für spezifizierte LEDs.
   * @param channel Kanal-Index oder Bitmaske der Ziel-LEDs.
   * @param color Zielfarbe aus dem ColorName enum.
   * @param address_type Zieladressierung (Normal oder Broadcast an alle Chips).
   * @return kSuccess bei Erfolg.
   */
  virtual types::ErrorCode SetColor(
      std::uint8_t channel, types::ColorName color,
      types::AddressType address_type = types::AddressType::kNormal) = 0;

  /**
   * @brief Schaltet alle LEDs des Treibers hart ab.
   * @return kSuccess bei Erfolg.
   */
  virtual types::ErrorCode TurnOff() = 0;
};

}  // namespace interfaces
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_INTERFACES_LED_INTERFACE_H_