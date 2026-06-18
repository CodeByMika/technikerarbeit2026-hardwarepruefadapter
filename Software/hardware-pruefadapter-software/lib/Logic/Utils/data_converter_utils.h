/**
 * @file data_converter_utils.h
 * @brief Zustandsloser Helfer für reine Mathematik und Daten-Konvertierung.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-03-24
 * @version 1.0.0
 * @ingroup Utils
 */
#ifndef HARDWARE_PRUEFADAPTER_UTILS_DATA_CONVERTER_UTILS_H_
#define HARDWARE_PRUEFADAPTER_UTILS_DATA_CONVERTER_UTILS_H_

#include <cstdint>
#include "color_types.h"

namespace hardware_pruefadapter {
namespace utils {
namespace data_converter {

/**
 * @brief Berechnet den logischen Zustand (HIGH/LOW/FLOAT) basierend auf Prozent-Schwellen.
 * @param voltage_mv Gemessene Spannung
 * @param ref_mv Eingestellte Referenzspannung (z.B. 24000 für 24V)
 * @param low_th_percent Schwelle für LOW (z.B. 20%)
 * @param high_th_percent Schwelle für HIGH (z.B. 80%)
 * @return std::int8_t 0 für LOW, 1 für HIGH, -1 für FLOAT/Fehler
 */
inline std::int8_t CalculateDigitalState(std::uint16_t voltage_mv, std::uint16_t ref_mv, 
                                         std::uint8_t low_th_percent, std::uint8_t high_th_percent) {
  std::uint16_t low_threshold = (ref_mv * low_th_percent) / 100;
  std::uint16_t high_threshold = (ref_mv * high_th_percent) / 100;

  if (voltage_mv <= low_threshold) return 0; // LOW
  if (voltage_mv >= high_threshold) return 1; // HIGH
  return -1; // FLOAT (Graubereich / Kabelbruch)
}

/**
 * @brief Bestimmt die Farbe der Hardware-Status-LED für einen IO-Kanal.
 * @param voltage_mv Die aktuelle Spannung auf dem Kanal
 * @param digital_state Der berechnete digitale Zustand (falls anwendbar)
 * @param is_digital True, wenn es sich um einen digitalen I/O handelt
 * @return types::ColorName Die Zielfarbe der LED
 */
inline types::ColorName DetermineIoLedColor(std::uint16_t voltage_mv, std::int8_t digital_state, bool is_digital) {
  if (is_digital) {
    if (digital_state == 1) return types::ColorName::kGreen;
    if (digital_state == -1) return types::ColorName::kRed; // FLOAT = Wackelkontakt = Rot
    return types::ColorName::kBlack; // LOW = Aus
  } else {
    // Bei Analogen Signalen leuchtet die LED grün, sobald Spannung anliegt
    return (voltage_mv > 0) ? types::ColorName::kGreen : types::ColorName::kBlack;
  }
}

} // namespace data_converter
} // namespace utils
} // namespace hardware_pruefadapter

#endif // HARDWARE_PRUEFADAPTER_UTILS_DATA_CONVERTER_UTILS_H_