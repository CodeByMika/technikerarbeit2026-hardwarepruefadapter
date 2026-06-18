#include <gtest/gtest.h>

#include "../lib/Logic/Services/process_imager_service.h"
#include "system_config.h"

namespace hardware_pruefadapter {
namespace test {

// -------------------------------------------------------------------
// TEST 1: Synchronisation der Puffer setzt Hardware-Change-Flags
// -------------------------------------------------------------------
TEST(ProcessImagerTest, SyncUpdateToActive_TriggersChangeFlags) {
  logic::ProcessImager imager;

  imager.SyncUpdateToActive();
  EXPECT_EQ(imager.GetSystemChangeFlags(), 0);

  imager.SetUpdateVoltage(types::IO::kAnalogOut, 2, 5000);
  imager.SyncUpdateToActive();

  std::uint32_t expected_mask =
      (1UL << (2 + (types::IO::kAnalogOut * types::IO::kIoGroupCount)));

  EXPECT_TRUE(imager.HasChangeFlagMask(expected_mask));
  EXPECT_EQ(imager.GetVoltage(types::IO::kAnalogOut, 2), 5000);
  EXPECT_EQ(imager.GetAdjustedDacVoltage(2), 5000);


  imager.ClearSystemChangeFlags();

  imager.SyncUpdateToActive();
  EXPECT_FALSE(imager.HasChangeFlagMask(expected_mask));
}

// -------------------------------------------------------------------
// TEST 2: Sicherer Lesezugriff auf ungültige Kanäle liefert 0
// -------------------------------------------------------------------
TEST(ProcessImagerTest, SafeGetters_ReturnZeroOnInvalidChannels) {
  logic::ProcessImager imager;

  // Verwende absichtlich einen Kanal, der über dem in der Config festgelegten
  // Limit liegt
  auto bad_volt =
      imager.GetVoltage(types::IO::kAnalogIn, config::kNumChannels + 1);
  EXPECT_EQ(bad_volt, 0);

  auto bad_rb = imager.GetReadbackVoltage(types::IO::kAnalogIn, 0);
  EXPECT_EQ(bad_rb, 0);

  auto bad_state = imager.GetDigitalState(types::IO::kAnalogIn, 0);
  EXPECT_EQ(bad_state, -1);
}

// -------------------------------------------------------------------
// TEST 3: Ungültige Eingaben für Referenzspannungen abfangen
// -------------------------------------------------------------------
TEST(ProcessImagerTest, SetDigitalReference_IgnoresInvalidInputs) {
  logic::ProcessImager imager;

  imager.SetDigitalReference(types::IO::kInput, 0, 5000);

  // Test auf Spannungslimit (Dynamisch gebunden an die Config, verhindert
  // uint16_t Overflows)
  std::uint16_t invalid_voltage =
      static_cast<std::uint16_t>(config::kMaxSystemVoltageMv + 1000);
  imager.SetDigitalReference(types::IO::kInput, 1, invalid_voltage);

  // Test auf ungültige Richtungen
  imager.SetDigitalReference(static_cast<types::IO::Direction>(5), 0, 5000);

  EXPECT_EQ(imager.GetDigitalReference(types::IO::kInput, 0), 5000);
  // Muss auf Standardwert geblieben sein, da die ungültige Spannung verworfen
  // wurde
  EXPECT_EQ(imager.GetDigitalReference(types::IO::kInput, 1),
            config::kMaxSystemVoltageMv);
}

// -------------------------------------------------------------------
// TEST 4: GetDigitalState berechnet Zustände korrekt
// -------------------------------------------------------------------
TEST(ProcessImagerTest, GetDigitalState_CalculatesStatesCorrectly) {
  logic::ProcessImager imager;

  imager.SetDigitalReference(types::IO::kInput, 0, 10000);

  imager.SetVoltage(types::IO::kDigitalIn, 0, 9000);
  EXPECT_EQ(imager.GetDigitalState(types::IO::kDigitalIn, 0), 1);

  imager.SetVoltage(types::IO::kDigitalIn, 0, 1000);
  EXPECT_EQ(imager.GetDigitalState(types::IO::kDigitalIn, 0), 0);

  imager.SetVoltage(types::IO::kDigitalIn, 0, 5000);
  EXPECT_EQ(imager.GetDigitalState(types::IO::kDigitalIn, 0), -1);
}

// -------------------------------------------------------------------
// TEST 5: AdjustedDacVoltage speichert und limitiert Werte korrekt
// -------------------------------------------------------------------
TEST(ProcessImagerTest, AdjustedDacVoltage_StoresAndLimitsValues) {
  logic::ProcessImager imager;

  // 1. Regulärer Fall: Valider Wert auf gültigem Kanal
  imager.SetAdjustedDacVoltage(0, 12000);
  EXPECT_EQ(imager.GetAdjustedDacVoltage(0), 12000);

  // 2. Out-of-Bounds: Ungültiger Kanal (darf nicht abstürzen, muss ignoriert
  // werden)
  imager.SetAdjustedDacVoltage(config::kNumChannels + 1, 5000);
  EXPECT_EQ(imager.GetAdjustedDacVoltage(config::kNumChannels + 1), 0);

  // 3. Out-of-Bounds: Ungültige Spannung über System-Limit
  std::uint16_t invalid_voltage = config::kMaxSystemVoltageMv + 1000;
  imager.SetAdjustedDacVoltage(1, invalid_voltage);

  // Da der Wert abgelehnt wurde, muss der Kanal weiterhin auf 0 stehen
  EXPECT_EQ(imager.GetAdjustedDacVoltage(1), 0);
}

// -------------------------------------------------------------------
// TEST 6: Setter blockieren ungültige Eingaben restlos (Out-Of-Bounds)
// -------------------------------------------------------------------
TEST(ProcessImagerTest, Setters_IgnoreInvalidInputs) {
  logic::ProcessImager imager;
  std::uint16_t invalid_voltage = config::kMaxSystemVoltageMv + 1000;
  std::uint8_t invalid_channel = config::kNumChannels + 1;
  types::IO::Group invalid_group = types::IO::kIoGroupCount;

  // 1. SetVoltage
  imager.SetVoltage(invalid_group, 0, 5000);
  imager.SetVoltage(types::IO::kAnalogIn, invalid_channel, 5000);
  imager.SetVoltage(types::IO::kAnalogIn, 0, invalid_voltage);
  EXPECT_EQ(imager.GetVoltage(types::IO::kAnalogIn, 0), 0);

  // 2. SetUpdateVoltage
  imager.SetUpdateVoltage(invalid_group, 0, 5000);
  imager.SetUpdateVoltage(types::IO::kAnalogIn, invalid_channel, 5000);
  imager.SetUpdateVoltage(types::IO::kAnalogIn, 0, invalid_voltage);
  EXPECT_EQ(imager.GetUpdateVoltage(types::IO::kAnalogIn, 0), 0);

  // 3. SetReadbackVoltage (Darf nur AnalogOut und DigitalOut bedienen!)
  imager.SetReadbackVoltage(types::IO::kAnalogIn, 0, 5000);  // Invalid Group
  imager.SetReadbackVoltage(types::IO::kAnalogOut, invalid_channel, 5000);
  imager.SetReadbackVoltage(types::IO::kAnalogOut, 0, invalid_voltage);
  EXPECT_EQ(imager.GetReadbackVoltage(types::IO::kAnalogOut, 0), 0);
}

}  // namespace test
}  // namespace hardware_pruefadapter