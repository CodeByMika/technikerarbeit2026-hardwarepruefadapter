/**
 * @file hardware_bootstrapper.h
 * @brief Kapselt die Hardware-Initialisierung
 *
 * @author Techniker_Team_2025_26
 * @date 2026-03-26
 * @version 1.0.0
 * @ingroup Services
 */
#ifndef HARDWARE_PRUEFADAPTER_SERVICES_HARDWARE_BOOTSTRAPPER_H_
#define HARDWARE_PRUEFADAPTER_SERVICES_HARDWARE_BOOTSTRAPPER_H_

#include <cstdint>

#include "../../Interfaces/analog_to_digital_interface.h"
#include "../../Interfaces/digital_to_analog_interface.h"
#include "../../Interfaces/led_interface.h"
#include "../../Interfaces/serial_interface.h"
#include "../../Interfaces/web_server_interface.h"
#include "error_manager.h"
#include "logger_service.h"
#include "system_context.h"

namespace hardware_pruefadapter {
namespace logic {

/**
 * @brief Speichert den Initialisierungsstatus der Hardware-Bänke.
 */
struct SystemHealth {
  std::uint8_t adc_init_success = 0;
  std::uint8_t dac_init_success = 0;
  std::uint8_t led_init_success = 0;
  std::uint8_t serial_init_success = 0;
  bool is_healthy = false;
};

/**
 * @class HardwareBootstrapper
 * @brief Übernimmt das Setzen der I2C-Adressen, Baudraten und Start-Parameter.
 */
class HardwareBootstrapper {
 public:
  /**
   * @brief Führt den Boot-Vorgang für alle Geräte im Context aus.
   * @param ctx Der SystemContext mit den unkonfigurierten Treiber-Pointern.
   * @param error_manager Manager, um Boot-Fehler direkt einzutragen.
   * @return SystemHealth Das Ergebnis des Boot-Vorgangs.
   */
  static SystemHealth Boot(const core::SystemContext& ctx,
                           ErrorManager& error_manager);

  private:
  static SystemHealth AdcInit(const core::SystemContext& ctx,
                              ErrorManager& error_manager, SystemHealth health);
  static SystemHealth DacInit(const core::SystemContext& ctx,
                              ErrorManager& error_manager, SystemHealth health);
  static SystemHealth LedInit(const core::SystemContext& ctx,
                               ErrorManager& error_manager, SystemHealth health);
  static SystemHealth SerialInit(const core::SystemContext& ctx,
                                 ErrorManager& error_manager, SystemHealth health);
  static SystemHealth HealthCheck(SystemHealth health);
};

}  // namespace logic
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_SERVICES_HARDWARE_BOOTSTRAPPER_H_