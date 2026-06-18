/**
 * @file led_manager_service.h
 * @brief Service zur Auswertung des Prozessabbilds und Steuerung der Hardware-LEDs.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-03-26
 * @version 1.0.0
 * @ingroup Services
 */
#ifndef HARDWARE_PRUEFADAPTER_SERVICES_LED_MANAGER_SERVICE_H_
#define HARDWARE_PRUEFADAPTER_SERVICES_LED_MANAGER_SERVICE_H_

#include <cstdint>
#include <string>

#include "../../Interfaces/led_interface.h"
#include "../Utils/data_converter_utils.h"
#include "error_manager.h"
#include "process_imager_service.h"
#include "system_config.h"
#include "system_context.h"

namespace hardware_pruefadapter {
namespace logic {

/**
 * @class LedManager
 * @brief Steuert die RGB-Status-LEDs auf Basis des Prozessabbilds.
 */
class LedManager {
 public:
  /**
   * @brief Erstellt den LedManager.
   * @param ctx SystemContext für den Zugriff auf die LED-Treiber-Pointer.
   */
  explicit LedManager(const core::SystemContext& ctx);
  ~LedManager() = default;

  /** @brief Speichert die Bitmaske der intakten LED-Treiber nach dem Booten. */
  void SetHealthMask(std::uint8_t led_init_success_mask);

  /** @brief Aktualisiert alle I/O-LEDs auf Basis geänderter System-Flags. */
  void UpdateHardwareLeds();

  /** @brief Aktualisiert die globale System-Status-LED. */
  void UpdateSystemLed();

  /** @brief Schaltet alle System- und I/O-LEDs explizit aus (Schwarz). */
  void TurnAllLedsOff();

  /** @brief Setzt alle LEDs auf die angegebene Farbe. */
  void AllLedsOn(types::ColorName color);

 private:
  const core::SystemContext& ctx_;
  std::uint8_t health_mask_ = 0;
};

}  // namespace logic
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_SERVICES_LED_MANAGER_SERVICE_H_