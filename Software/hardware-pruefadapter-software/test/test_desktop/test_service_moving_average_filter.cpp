#include <gtest/gtest.h>

#include "../include/system_config.h"
#include "../lib/Logic/Utils/moving_average_filter.h"

namespace hardware_pruefadapter {
namespace test {

// -------------------------------------------------------------------
// TEST 1: Berechnung des gleitenden Mittelwerts bei teilgefülltem Puffer
// -------------------------------------------------------------------
TEST(MovingAverageFilterTest, CalculatesAverageCorrectly) {
  utils::MovingAverageFilter filter;

  filter.AddValue(1000);
  filter.AddValue(2000);
  filter.AddValue(3000);

  EXPECT_EQ(filter.GetAverage(), 2000);
}

// -------------------------------------------------------------------
// TEST 2: Ringpuffer überschreibt älteste Werte bei Überlauf
// -------------------------------------------------------------------
TEST(MovingAverageFilterTest, RingBufferOverridesOldestValues) {
  utils::MovingAverageFilter filter;

  // Puffer komplett mit 0 füllen
  for (int i = 0; i < config::kAdcFilterSamples; i++) {
    filter.AddValue(0);
  }
  EXPECT_EQ(filter.GetAverage(), 0);

  // Überschreibt den ältesten Wert (0) mit 10000
  filter.AddValue(10000);

  EXPECT_EQ(filter.GetAverage(), 10000 / config::kAdcFilterSamples);
}

// -------------------------------------------------------------------
// TEST 3: Reset leert den Filter und verhindert Division durch Null
// -------------------------------------------------------------------
TEST(MovingAverageFilterTest, ResetClearsBufferAndPreventsDivByZero) {
  utils::MovingAverageFilter filter;

  // Leerer Puffer muss 0 zurückgeben (Division by Zero check)
  EXPECT_EQ(filter.GetAverage(), 0);

  filter.AddValue(5000);
  filter.Reset();

  EXPECT_EQ(filter.GetAverage(), 0);
}

}  // namespace test
}  // namespace hardware_pruefadapter