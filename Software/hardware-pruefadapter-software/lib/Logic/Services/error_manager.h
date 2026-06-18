/**
 * @file error_manager.h
 * @brief Ein globaler Error Manager mit Schatten-Register.
 *
 * Dieser Manager sammelt Fehler während eines Logik-Zyklus zunächst in einem
 * Schatten-Speicher. Erst am Ende des Zyklus werden diese Fehler offiziell
 * übernommen. Dies verhindert Race-Conditions und unkontrolliertes Flackern
 * von Fehlerzuständen (SPS-Architektur).
 *
 * @author Techniker_Team_2025_26
 * @date 2026-03-25
 * @version 1.0.0
 * @ingroup Services
 * @copyright Copyright (c) 2026 SOTEC GmBH & Co KG
 */

#ifndef HARDWARE_PRUEFADAPTER_SERVICES_ERROR_MANAGER_H_
#define HARDWARE_PRUEFADAPTER_SERVICES_ERROR_MANAGER_H_

//====================System Header===========================
#include <cstdint>
#include <mutex>
#include <string>

//===================Projekt Header===========================
#include "api_types.h"
#include "error_code.h"
#include "logger_service.h"
#include "system_config.h"

namespace hardware_pruefadapter {
namespace logic {

/**
 * @class ErrorManager
 * @brief Verwaltet zyklische System- und Hardwarefehler Thread-sicher.
 */
class ErrorManager {
 public:
  ErrorManager() = default;
  ~ErrorManager() = default;

  /** * @brief Fügt einen Laufzeitfehler in den Schatten-Speicher des aktuellen
   * Zyklus ein.
   * @param context Angabe zum Fehlerort (z.B. "ADC 1" oder "AnalogOut 2").
   * @param error Der spezifische Fehlercode.
   */
  void AddCycleError(const std::string& context, types::ErrorCode error);

  /** * @brief Wertet am Ende des Logik-Zyklus den Schatten-Speicher aus.
   * Kopiert die neuen Fehler in das Hauptregister und loggt Fehler,
   * die sich im Vergleich zum letzten Zyklus von selbst geheilt haben.
   */
  void ResolveErrors();

  /** * @brief Liefert die Anzahl der offiziell aktiven Fehler im System.
   * @return Anzahl der Fehler (0 bedeutet, das System ist fehlerfrei).
   */
  uint8_t GetErrorCount() const;

  /** * @brief Füllt das Status-Objekt für die Web-API mit den aktiven Fehlern.
   * @param dto Referenz auf das Data-Transfer-Object, das befüllt werden soll.
   */
  void FillErrorStatus(types::SystemStatusDto& dto) const;

  /** * @brief Leert den Schatten-Speicher zu Beginn eines neuen Logik-Zyklus.
   */
  void ClearShadowErrors();

 private:
  //==============Private Attribute==============

  mutable std::mutex error_mutex_;

  // Offizielles Hauptregister (Wird für API und Auswertungen genutzt)
  types::ErrorEvent cycle_errors_[config::kMaxCycleErrors]{};
  std::uint8_t cycle_error_count_ = 0;

  // Schatten-Register (Sammelt Fehler während des laufenden Zyklus)
  types::ErrorEvent shadow_cycle_errors_[config::kMaxCycleErrors]{};
  std::uint8_t shadow_cycle_error_count_ = 0;
};

}  // namespace logic
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_SERVICES_ERROR_MANAGER_H_