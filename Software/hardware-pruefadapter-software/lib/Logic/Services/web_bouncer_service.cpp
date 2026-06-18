/**
 * @file web_bouncer_service.cpp
 * @brief Implementierung des WebBouncer Services.
 */

#include "web_bouncer_service.h"

namespace hardware_pruefadapter {
namespace logic {

std::atomic<std::uint8_t> WebBouncer::active_requests_{0};

bool WebBouncer::AcquireSlot() {
  std::uint8_t current = active_requests_.load();
  while (current < config::kWebServerMaxClients) {
    if (active_requests_.compare_exchange_weak(current, current + 1)) {
      return true;
    }
  }
  return false;
}

void WebBouncer::ReleaseSlot() {
  std::uint8_t current = active_requests_.load();
  while (current > 0) {
    if (active_requests_.compare_exchange_weak(current, current - 1)) {
      return;
    }
  }
}

bool WebBouncer::AllowSseConnection(std::size_t current_connections) {
  return current_connections <= config::kWebServerMaxClients;
}

void WebBouncer::Reset() {
  active_requests_.store(0);
}

}  // namespace logic
}  // namespace hardware_pruefadapter