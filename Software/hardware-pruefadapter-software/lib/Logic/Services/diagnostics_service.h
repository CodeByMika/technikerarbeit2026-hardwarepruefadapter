/**
 * @file diagnostics_service.h
 * @brief Kapselt statische Diagnose-Informationen über die Hardware.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-03-25
 * @version 1.0.0
 * @ingroup Services
 */
#ifndef HARDWARE_PRUEFADAPTER_SERVICES_DIAGNOSTICS_SERVICE_H_
#define HARDWARE_PRUEFADAPTER_SERVICES_DIAGNOSTICS_SERVICE_H_

#include <cstdint>
#include <string>

#include "api_types.h"
#include "error_manager.h"
#include "logger_service.h"
#include "system_config.h"

#ifdef ESP32_ENV
#include <Arduino.h>
#endif

namespace hardware_pruefadapter {
namespace logic {

/**
 * @class DiagnosticsService
 * @brief Stateless Service für Diagnose-Aufgaben.
 */
class DiagnosticsService {
 public:
  /** @brief Liefert eine fixe Liste der konfigurierten I2C-Chips. */
  static types::I2cDeviceList<config::kMaxI2cDevices> GetConfiguredI2cDevices();

  /** @brief Liefert die aktuelle System-Uptime in Millisekunden
   * (Hardware-unabhängig). */
  static std::uint32_t GetSystemUptimeMs();

  /** @brief Liefert den aktuellen System-Heap in Byte
   * (Hardware-unabhängig). */
  static std::uint32_t GetSystemFreeHeap();

  /** @brief Gibt System-Informationen (RAM, Core, Version) im Log aus. */
  static void LogSystemInfo();

  /** @brief Baut das Status-Paket für die Web-API zusammen. */
  static types::SystemStatusDto BuildSystemStatusDto(
      const ErrorManager& error_manager);
};

}  // namespace logic
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_SERVICES_DIAGNOSTICS_SERVICE_H_