/**
 * @file digital_to_analog_interface.h
 * @brief Blaupause zur Steuerung von Digital-Analog-Wandlern (DAC).
 *
 * Ein DAC ist das Gegenstück zum ADC. Er macht aus Zahlen im Computer
 * eine echte Spannung.
 *
 * Anwendungsbeispiele:
 * - Lautstärke regeln (Audio-Signal ausgeben)
 * - Helligkeit einer Lampe steuern
 * - Einen Motor beschleunigen
 *
 * Dieser Bauplan stellt sicher, dass wir der Hardware einfach sagen können:
 * "Gib bitte 3,5 Volt auf Leitung 1 aus", egal welcher Chip verbaut ist.
 *
 * @author Techniker_Team_2025_26
 * @date 2025-12-18
 * @version 1.0.0
 * @ingroup Interfaces
 * @copyright Copyright (c) 2025 SOTEC GmBH & Co KG
 */

#ifndef HARDWARE_PRUEFADAPTER_INTERFACES_DIGITAL_TO_ANALOG_INTERFACE_H_
#define HARDWARE_PRUEFADAPTER_INTERFACES_DIGITAL_TO_ANALOG_INTERFACE_H_

//====================System Header===========================
#include <cstdint>

//===================Projekt Header===========================
#include "error_code.h"

namespace hardware_pruefadapter {
namespace interfaces {

/**
 * @class IDigitalToAnalogConverter
 * @brief Schnittstelle, um Spannungen zu erzeugen.
 *
 * Definiert die Befehle, um Ausgangsspannungen zu setzen und
 * den Chip zu konfigurieren.
 */
class IDigitalToAnalogConverter {
 public:
  //==================Destruktor========================
  virtual ~IDigitalToAnalogConverter() = default;

  //==================Public General====================
  /**
   * @brief Initialisierung des DAC.
   *
   * @param[in] address Die I2C-Adresse des Bauteils.
   * @param[in] vref_mv Die Referenzspannung (die maximale Spannung, die der
   * Chip kennt in Millivolt).
   * @param[in] i2c_bus_nr Die Nummer des I2C-Busses (falls mehrere vorhanden).
   * @return types::ErrorCode Status des Starts.
   */
  virtual types::ErrorCode Initialize(std::uint8_t address,
                                      std::uint16_t vref_mv,
                                      std::uint8_t i2c_bus_nr = 0) = 0;

  /**
   * @brief Prüft, ob der DAC erreichbar ist.
   * @return true bei erfolgreicher Verbindung.
   */
  virtual bool IsConnected() const = 0;

  //================Public Set Methoden=================
  /**
   * @brief Stellt den Modus ein.
   * @param[in] mode Eine Zahl, die den Modus bestimmt.
   * @return true, wenn der Modus erfolgreich eingestellt wurde.
   */
  virtual bool SetMode(std::uint8_t mode) = 0;

  /**
   * @brief Setzt den Ausgang über eine gewünschte Spannung.
   *
   * Der Treiber rechnet die Volt-Zahl automatisch in den passenden
   * Rohwert um.
   *
   * @param[in] channel Der Ausgangskanal.
   * @param[in] voltage_mv Die gewünschte Spannung in Millivolt.
   * @return types::ErrorCode Fehler, wenn Spannung zu hoch für den Chip.
   */
  virtual types::ErrorCode SetValue(std::uint8_t channel,
                                    std::uint16_t voltage_mv) = 0;

  /**
   * @brief Konfiguriert den Spannungsbereich.
   * @param[in] voltage_low_mv Minimum Spannung in Millivolt.
   * @param[in] voltage_high_mv Höchste Spannung in Millivolt.
   * @return types::ErrorCode Fehler bei ungültigen Werten.
   */
  virtual types::ErrorCode SetVoltageReference(
      std::uint16_t voltage_low_mv, std::uint16_t voltage_high_mv) = 0;

  //================Public Get Methoden=================
  /**
   * @brief Liest zurück, welcher Wert aktuell gesetzt ist.
   * @param[in] channel Der Kanal.
   * @param[out] voltage_mv_in Speicher für das Ergebnis in Millivolt.
   * @return types::ErrorCode Fehlerstatus.
   */
  virtual types::ErrorCode GetValue(std::uint8_t channel,
                                    std::uint16_t* voltage_mv_in) const = 0;

  /**
   * @brief Gibt die maximal mögliche Spannung zurück.
   * @param[out] voltage_high_mv Speicher für den Maximalwert in Millivolt.
   * @return types::ErrorCode Fehlerstatus.
   */
  virtual types::ErrorCode GetMaxVoltage(
      std::uint16_t* voltage_high_mv) const = 0;

  /**
   * @brief Gibt die aktuelle Verstärkung zurück.
   * @param[out] gain_mv Speicher für den Gain-Faktor in Millivolt.
   * @return types::ErrorCode Fehlerstatus.
   */
  virtual types::ErrorCode GetGain(std::uint16_t* gain_mv) const = 0;

  /**
   * @brief Gibt den aktuellen Modus zurück.
   * @return Das Modus-Byte.
   */
  virtual std::uint8_t GetMode() const = 0;

  //==============Protected Interne Methoden==============
  /**
   * @brief Hilfsfunktion: Wandelt Rohwert in Spannung.
   * @param[in] raw Der Rohwert.
   * @return std::uint16_t Die Spannung in Millivolt.
   */
  virtual std::uint16_t ConvertToVoltage(std::uint16_t raw) const = 0;

  /**
   * @brief Hilfsfunktion: Wandelt Spannung in Rohwert.
   * @param[in] voltage_mv Die Spannung in Millivolt.
   * @return uint16_t Der Rohwert.
   */
  virtual std::uint16_t ConvertToRaw(std::uint16_t voltage_mv) const = 0;
};

}  // namespace interfaces
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_INTERFACES_DIGITAL_TO_ANALOG_INTERFACE_H_