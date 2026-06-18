#include <gtest/gtest.h>

#include "../lib/Logic/Utils/data_converter_utils.h"
#include "system_config.h"

namespace hardware_pruefadapter {
namespace test {

using namespace utils::data_converter;

// -------------------------------------------------------------------
// TEST 1: Logikschwellen (20% / 80%) korrekt auswerten
// -------------------------------------------------------------------
TEST(DataConverterUtilsTest, EvaluatesSpsLogicCorrectly) {
  std::uint16_t ref_mv = 24000;
  std::uint8_t low_th = config::kLogicLowThresholdPercent;
  std::uint8_t high_th = config::kLogicHighThresholdPercent;

  // Eindeutig LOW (z.B. 2V -> Unter 20% von 24V = 4.8V)
  EXPECT_EQ(CalculateDigitalState(2000, ref_mv, low_th, high_th), 0);

  // Eindeutig HIGH (z.B. 22V -> Über 80% von 24V = 19.2V)
  EXPECT_EQ(CalculateDigitalState(22000, ref_mv, low_th, high_th), 1);

  // Verbotener Float/Graubereich (z.B. 12V)
  EXPECT_EQ(CalculateDigitalState(12000, ref_mv, low_th, high_th), -1);
}

// -------------------------------------------------------------------
// TEST 2: Exakte Randwerte der Logikschwellen (Exakt 20% und 80%)
// -------------------------------------------------------------------
TEST(DataConverterUtilsTest, EvaluatesExactThresholdBoundaries) {
  std::uint16_t ref_mv = 10000;  // 10V Referenz für einfaches Rechnen
  std::uint8_t low_th = 20;      // 2000 mV
  std::uint8_t high_th = 80;     // 8000 mV

  EXPECT_EQ(CalculateDigitalState(2000, ref_mv, low_th, high_th),
            0);  // Exakt 20% ist noch LOW
  EXPECT_EQ(CalculateDigitalState(2001, ref_mv, low_th, high_th),
            -1);  // 1mV drüber ist FLOAT

  EXPECT_EQ(CalculateDigitalState(7999, ref_mv, low_th, high_th),
            -1);  // 1mV drunter ist FLOAT
  EXPECT_EQ(CalculateDigitalState(8000, ref_mv, low_th, high_th),
            1);  // Exakt 80% ist schon HIGH
}

// -------------------------------------------------------------------
// TEST 3: Bestimmung der Hardware-LED-Farben basierend auf Zuständen
// -------------------------------------------------------------------
TEST(DataConverterUtilsTest, DeterminesCorrectLedColor) {
  // Digitaler Zustand HIGH (1) -> LED muss Grün sein
  EXPECT_EQ(DetermineIoLedColor(24000, 1, true), types::ColorName::kGreen);

  // Digitaler Zustand Float (-1) -> LED muss Rot sein (Fehler/Wackelkontakt)
  EXPECT_EQ(DetermineIoLedColor(12000, -1, true), types::ColorName::kRed);

  // Analoges Signal (>0V) -> LED muss Grün sein (Kein Float-Fehler möglich)
  EXPECT_EQ(DetermineIoLedColor(1500, 0, false), types::ColorName::kGreen);

  // Analoges Signal (0V) -> LED aus
  EXPECT_EQ(DetermineIoLedColor(0, 0, false), types::ColorName::kBlack);
}

}  // namespace test
}  // namespace hardware_pruefadapter