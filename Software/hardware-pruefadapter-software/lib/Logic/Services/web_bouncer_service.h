/**
 * @file web_bouncer_service.h
 * @brief Service zur Begrenzung von parallelen Web-Anfragen.
 *
 * Schützt den Mikrocontroller vor RAM-Überläufen durch zu viele 
 * gleichzeitige HTTP-Clients oder SSE-Streams.
 */
#ifndef HARDWARE_PRUEFADAPTER_SERVICES_WEB_BOUNCER_SERVICE_H_
#define HARDWARE_PRUEFADAPTER_SERVICES_WEB_BOUNCER_SERVICE_H_

#include <atomic>
#include <cstdint>
#include "system_config.h"

namespace hardware_pruefadapter {
namespace logic {

class WebBouncer {
 public:
  static bool AcquireSlot();
  static void ReleaseSlot();
  static bool AllowSseConnection(std::size_t current_connections);
  static void Reset(); // Ausschließlich für Unit Tests

 private:
  static std::atomic<std::uint8_t> active_requests_;
};

}  // namespace logic
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_SERVICES_WEB_BOUNCER_SERVICE_H_