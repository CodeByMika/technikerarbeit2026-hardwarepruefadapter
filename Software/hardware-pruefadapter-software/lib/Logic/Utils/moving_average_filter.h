/**
 * @file moving_average_filter.h
 * @brief Ein generischer Ringpuffer zur Berechnung des gleitenden Mittelwerts.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-03-24
 * @version 1.0.0
 * @ingroup Utils
 */
#ifndef HARDWARE_PRUEFADAPTER_UTILS_MOVING_AVERAGE_FILTER_H_
#define HARDWARE_PRUEFADAPTER_UTILS_MOVING_AVERAGE_FILTER_H_

#include <cstdint>
#include "system_config.h"

namespace hardware_pruefadapter {
namespace utils {

/**
 * @class MovingAverageFilter
 * @brief Speichert die letzten N Werte und berechnet deren Durchschnitt.
 */
class MovingAverageFilter {
 public:
  MovingAverageFilter() = default;
  ~MovingAverageFilter() = default;

  /** @brief Fügt einen neuen Messwert in den Ringpuffer ein. */
  void AddValue(std::uint16_t new_value);

  /** @brief Berechnet den Durchschnitt aller aktuell gespeicherten Werte. */
  std::uint16_t GetAverage() const;

  /** @brief Leert den Puffer (z.B. nach einem Verbindungsabbruch zum Sensor). */
  void Reset();

 private:
  std::uint16_t buffer_[config::kAdcFilterSamples]{0};
  std::uint8_t index_{0};
  std::uint8_t count_{0};
};

}  // namespace utils
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_UTILS_MOVING_AVERAGE_FILTER_H_