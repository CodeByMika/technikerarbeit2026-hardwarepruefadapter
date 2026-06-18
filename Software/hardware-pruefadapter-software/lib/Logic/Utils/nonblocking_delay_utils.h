/**
 * @file nonblocking_delay_utils.h
 * @brief Hilfsklasse für blockierungsfreie Verzögerungen.
 * * Nutzt den gekapselten DiagnosticsService, um plattformunabhängig
 * (ESP32 oder Native C++) die Systemzeit abzufragen.
 */
#ifndef HARDWARE_PRUEFADAPTER_UTILS_NONBLOCKING_DELAY_H_
#define HARDWARE_PRUEFADAPTER_UTILS_NONBLOCKING_DELAY_H_

#include <cstdint>

#include "../Services/diagnostics_service.h"

namespace hardware_pruefadapter {
namespace utils {

class NonBlockingDelay {
 private:
  std::uint32_t previous_millis_;
  std::uint32_t interval_;

 public:
  /**
   * @brief Konstruktor: Setzt das gewünschte Intervall
   * @param delay_time_ms Das Intervall in Millisekunden
   */
  explicit NonBlockingDelay(std::uint32_t delay_time_ms)
      : previous_millis_(0), interval_(delay_time_ms) {}

  /**
   * @brief Gibt true zurück, wenn die Zeit abgelaufen ist.
   */
  bool isReady() {
    // Nutzt den bereits plattformübergreifend gekapselten Service
    std::uint32_t current_millis =
        logic::DiagnosticsService::GetSystemUptimeMs();

    if (current_millis - previous_millis_ >= interval_) {
      previous_millis_ = current_millis;  // Automatisch zurücksetzen
      return true;
    }
    return false;
  }
};

}  // namespace utils
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_UTILS_NONBLOCKING_DELAY_H_