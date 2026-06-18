#include <gtest/gtest.h>

#include "../include/system_config.h"
#include "../lib/Logic/Services/web_bouncer_service.h"

namespace hardware_pruefadapter {
namespace test {

// -------------------------------------------------------------------
// TEST 1: Acquire blockiert bei Erreichen des Limits
// -------------------------------------------------------------------
TEST(WebBouncerTest, AcquireSlot_LimitsAtMaxClients) {
  logic::WebBouncer::Reset();

  for (int i = 0; i < config::kWebServerMaxClients; i++) {
    EXPECT_TRUE(logic::WebBouncer::AcquireSlot());
  }

  EXPECT_FALSE(logic::WebBouncer::AcquireSlot());
}

// -------------------------------------------------------------------
// TEST 2: Freigabe erlaubt genau eine neue Anfrage
// -------------------------------------------------------------------
TEST(WebBouncerTest, ReleaseSlot_FreesSpaceForNewRequests) {
  logic::WebBouncer::Reset();

  for (int i = 0; i < config::kWebServerMaxClients; i++) {
    logic::WebBouncer::AcquireSlot();
  }
  EXPECT_FALSE(logic::WebBouncer::AcquireSlot());

  logic::WebBouncer::ReleaseSlot();

  EXPECT_TRUE(logic::WebBouncer::AcquireSlot());
  EXPECT_FALSE(logic::WebBouncer::AcquireSlot());
}

// -------------------------------------------------------------------
// TEST 3: Asynchroner Underflow-Schutz
// -------------------------------------------------------------------
TEST(WebBouncerTest, ReleaseSlot_DoesNotUnderflow) {
  logic::WebBouncer::Reset();

  // Fehlerhafte Doppel-Freigabe erzwingen
  logic::WebBouncer::ReleaseSlot();
  logic::WebBouncer::ReleaseSlot();

  // Zähler muss stabil bei 0 geblieben sein
  EXPECT_TRUE(logic::WebBouncer::AcquireSlot());
}

// -------------------------------------------------------------------
// TEST 4: SSE Stream Limitierung
// -------------------------------------------------------------------
TEST(WebBouncerTest, AllowSseConnection_LimitsMaxStreams) {
  logic::WebBouncer::Reset();

  EXPECT_TRUE(logic::WebBouncer::AllowSseConnection(config::kWebServerMaxClients - 1));
  EXPECT_FALSE(logic::WebBouncer::AllowSseConnection(config::kWebServerMaxClients + 1));
}

}  // namespace test
}  // namespace hardware_pruefadapter