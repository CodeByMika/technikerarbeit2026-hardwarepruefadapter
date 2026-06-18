/**
 * @file analog_to_digital_interface.h
 * @brief Abstraktes Interface für Analog-Digital-Wandler (ADC).
 *
 * Definiert den Kontrakt, den jeder ADC-Treiber im System erfüllen muss.
 * Es gewährleistet die Austauschbarkeit der Hardware ohne Änderung der
 * Hauptlogik.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-01-29
 * @version 1.1.0
 * @ingroup Interfaces
 * @copyright Copyright (c) 2025 SOTEC GmBH & Co KG
 */

#ifndef HARDWARE_PRUEFADAPTER_INTERFACES_ANALOG_TO_DIGITAL_INTERFACE_H_
#define HARDWARE_PRUEFADAPTER_INTERFACES_ANALOG_TO_DIGITAL_INTERFACE_H_

//====================System Header===========================
#include <cstdint>

//===================Projekt Header===========================
#include "error_code.h"

namespace hardware_pruefadapter {
namespace interfaces {

/**
 * @class IAnalogToDigitalConverter
 * @brief Schnittstellendefinition für ADC-Treiber.
 *
 * Diese Klasse ist rein virtuell (Abstract Base Class).
 * Sie erzwingt die Implementierung von Methoden zur Initialisierung,
 * Konfiguration und zum Auslesen von Werten.
 */
class IAnalogToDigitalConverter {
 public:
  //====================Destruktor====================
  virtual ~IAnalogToDigitalConverter() = default;

  //====================Public General====================
  /**
   * @brief Initialisierung des ADC.
   *
   * @param[in] address Die I2C-Adresse als HEXWERT des Chips.
   * @param[in] gain Die gewünschte Messgenauigkeit.
   * @param[in] i2c_bus_nr Die I2C-Busnummer (für Systeme mit mehreren Bussen).
   * @return types::ErrorCode Gibt an, ob der Start erfolgreich war.
   */
  virtual types::ErrorCode Initialize(std::uint8_t address,
                                      std::uint16_t gain_mv,
                                      std::uint8_t i2c_bus_nr = 0) = 0;

  /**
   * @brief Prüft, ob der Chip antwortet.
   * @return true, wenn die Verbindung besteht.
   */
  virtual bool IsConnected() const = 0;

  /**
   * @brief Prüft, ob der Chip gerade eine Konvertierung durchführt.
   * @return true, wenn beschäftigt (Busy).
   */
  virtual bool IsBusy() const = 0;

  //================Public Set Methoden=================
  /**
   * @brief Konfiguriert den Aufnahmemodus.
   * @param[in] mode false = Single-Shot (Einzelmessung), true = Continuous
   * (Dauer).
   * @return true, wenn der Wert vom Chip unterstützt wird.
   */
  virtual bool SetMode(bool mode) = 0;

  /**
   * @brief Ändert die Empfindlichkeit des Sensors.
   * @param[in] gain Der neue Verstärkungswert.
   * @return true, wenn der Chip diesen Wert unterstützt.
   */
  virtual bool SetGain(std::uint16_t gain_mv) = 0;

  /**
   * @brief Setzt die Datenrate (Samples per Second).
   * @param[in] sps Die gewünschte Rate (z.B. 128, 860 bei ADS1115).
   * @return types::ErrorCode Fehler, wenn Rate nicht unterstützt.
   */
  virtual types::ErrorCode SetDataRate(std::uint16_t sps) = 0;

  /**
   * @brief Konfiguriert differenzielle Messung (A - B).
   * @param[in] channel_low Negativer Eingangskanal.
   * @param[in] channel_high Positiver Eingangskanal.
   * @return types::ErrorCode kSuccess oder Fehler bei ungültigen Kanälen.
   */
  virtual types::ErrorCode SetDifferentialMode(std::uint8_t channel_low,
                                               std::uint8_t channel_high) = 0;

  /**
   * @brief Legt fest, ob der AD Wandler externe oder interne Spannungen misst
   * @param[in] intern_extern false = Extern, true = Intern.
   * @return true bei Erfolg.
   */
  virtual bool SetInternExtern(bool intern_extern) = 0;

  //================Public Get Methoden=================
  /**
   * @brief Holt das rohe Messergebnis (Integer) ab.
   *
   * Der Wert ist abhängig von der Auflösung des ADC (z.B. 16-Bit -> -32768 bis
   * 32767). Die Methode berücksichtigt die aktuelle Konfiguration (z.B. Gain,
   * Differential Mode).
   *
   * @param[in] channel Kanalnummer.
   * @param[out] raw_out Zeiger auf int16_t für den Rohwert.
   * @return types::ErrorCode Status.
   */
  virtual types::ErrorCode GetValue(std::uint8_t channel,
                                    std::int16_t* raw_out) const = 0;

  /**
   * @brief Gibt die I2C-Adresse des ADC zurück.
   * @param[out] address Die I2C-Adresse als HEXWERT.
   * @return types::ErrorCode Status.
   */
  virtual types::ErrorCode GetI2cAddress(std::uint8_t* address) const = 0;

  /**
   * @brief Gibt den aktuellen Modus zurück.
   * @return false = Single-Shot, true = Continuous.
   */
  virtual bool GetMode() const = 0;

  /**
   * @brief Gibt zurück, ob der Chip online ist.
   * @return true, wenn der Chip erreichbar ist.
   */
  virtual bool GetOnlineStatus() const = 0;

  /**
   * @brief Gibt die aktuelle Verstärkung zurück.
   * @return std::uint16_t Aktueller Gain in Millivolt.
   */
  virtual std::uint16_t GetGain() const = 0;

  /**
   * @brief Gibt zurück, ob der ADC interne oder externe Spannungen misst.
   * @return false = Extern, true = Intern.
   */
  virtual bool GetInternExtern() const = 0;

  /**
   * @brief Gibt den aktuellen differenziellen Modus zurück.
   * @return std::uint8_t Der aktuelle differenzielle Modus.
   */
  virtual std::uint8_t GetDifferentialMode() const = 0;

  //==============Hilfs Methoden==============
  /**
   * @brief Rechnet den Rohwert des ADC in Volt um.
   * @param[in] raw Die Zahl, die der ADC liefert.
   * @return Der Spannungswert in Millivolt.
   */
  virtual std::uint16_t ConvertToVoltage(std::int16_t raw) const = 0;
};

}  // namespace interfaces
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_INTERFACES_ANALOG_TO_DIGITAL_INTERFACE_H_