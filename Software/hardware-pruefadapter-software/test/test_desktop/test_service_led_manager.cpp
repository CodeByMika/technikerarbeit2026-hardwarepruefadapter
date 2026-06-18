#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../lib/Logic/Services/error_manager.h"
#include "../lib/Logic/Services/led_manager_service.h"
#include "../lib/Logic/Services/process_imager_service.h"
#include "../mock_hardware.h"
#include "system_config.h"
#include "system_context.h"

using ::testing::_;
using ::testing::Ne;
using ::testing::Return;

namespace hardware_pruefadapter {
namespace test {

// =========================================================
// HELPER FUNKTIONEN FÜR HARDWARE-MAPPING (Voll Dynamisch)
// =========================================================
inline std::uint8_t HwLedDriver(std::uint32_t map_index) {
  return config::kLedMap[map_index].driver_index;
}
inline std::uint8_t HwLedCh(std::uint32_t map_index) {
  return config::kLedMap[map_index].channel;
}

// -------------------------------------------------------------------
// TEST 1: Ignorieren von Signalen bei inaktiven Hardware-Treibern
// -------------------------------------------------------------------
TEST(LedManagerTest, IgnoresUpdatesIfHealthMaskIsZero) {
  core::SystemContext ctx;
  ::testing::StrictMock<MockLedDriver> mock_led;

  std::uint32_t map_idx = 0;  // Test Kanal Index
  ctx.ui.led_drivers[HwLedDriver(map_idx)] = &mock_led;

  logic::ProcessImager imager;
  logic::ErrorManager em;
  ctx.logic.process_imager = &imager;
  ctx.logic.error_manager = &em;
  logic::LedManager manager(ctx);

  // Arrange: Treiber als Offline (0) markieren
  manager.SetHealthMask(0);  // Treiber ist OFFLINE

  // Act: Simulierte Änderung im Prozessabbild anlegen
  imager.AddSystemChangeFlagMask(1UL << map_idx);
  manager.UpdateHardwareLeds();

  // Assert: StrictMock garantiert, dass SetColor nie ausgeführt wurde
}

// -------------------------------------------------------------------
// TEST 2: Korrekte Farbzuweisung für digitale Ein- und Ausgänge
// -------------------------------------------------------------------
TEST(LedManagerTest, SetsCorrectColorsForDigitalIo) {
  core::SystemContext ctx;
  ::testing::NiceMock<MockLedDriver> mock_led;

  std::uint8_t logical_channel = 0;
  std::uint32_t map_idx =
      logical_channel + (types::IO::kDigitalIn * types::IO::kIoGroupCount);
  std::uint8_t hw_driver = HwLedDriver(map_idx);
  std::uint8_t hw_channel = HwLedCh(map_idx);

  ctx.ui.led_drivers[hw_driver] = &mock_led;

  logic::ProcessImager imager;
  logic::ErrorManager em;
  ctx.logic.process_imager = &imager;
  ctx.logic.error_manager = &em;
  logic::LedManager manager(ctx);

  // Arrange: Treiber 1 als Online markieren
  manager.SetHealthMask(1 << hw_driver);

  imager.AddSystemChangeFlagMask(1UL << map_idx);
  imager.SetDigitalReference(types::IO::kInput, logical_channel, 24000);
  imager.SetVoltage(types::IO::kDigitalIn, logical_channel, 24000);

  // Treiber muss auf diesem HW Kanal ein Gruen setzen
  EXPECT_CALL(mock_led, SetColor(hw_channel, types::ColorName::kGreen, _))
      .Times(1)
      .WillOnce(Return(types::ErrorCode::kSuccess));

  // Act: Update der Hardware-LEDs auslösen
  manager.UpdateHardwareLeds();
}

// -------------------------------------------------------------------
// TEST 3: Hardware-LED wechselt zwingend auf Rot bei FLOAT-Zustand
// -------------------------------------------------------------------
TEST(LedManagerTest, SetsRedColorOnFloatState) {
  core::SystemContext ctx;
  ::testing::NiceMock<MockLedDriver> mock_led;

  std::uint8_t logical_channel = 1;
  std::uint32_t map_idx =
      logical_channel + (types::IO::kDigitalIn * types::IO::kIoGroupCount);
  std::uint8_t hw_driver = HwLedDriver(map_idx);
  std::uint8_t hw_channel = HwLedCh(map_idx);

  ctx.ui.led_drivers[hw_driver] = &mock_led;

  logic::ProcessImager imager;
  logic::ErrorManager em;
  ctx.logic.process_imager = &imager;
  ctx.logic.error_manager = &em;
  logic::LedManager manager(ctx);

  manager.SetHealthMask(1 << hw_driver);

  imager.AddSystemChangeFlagMask(1UL << map_idx);
  imager.SetDigitalReference(types::IO::kInput, logical_channel, 24000);
  imager.SetVoltage(types::IO::kDigitalIn, logical_channel,
                    12000);  // 50% = Float

  // Assert: LED muss Rot signalisieren
  EXPECT_CALL(mock_led, SetColor(hw_channel, types::ColorName::kRed, _))
      .Times(1)
      .WillOnce(Return(types::ErrorCode::kSuccess));

  // Act: Update der Hardware-LEDs auslösen
  manager.UpdateHardwareLeds();
}

// -------------------------------------------------------------------
// TEST 4: Globale System-LED reagiert auf ErrorCount
// -------------------------------------------------------------------
TEST(LedManagerTest, UpdatesSystemLedBasedOnErrorCount) {
  core::SystemContext ctx;
  ::testing::NiceMock<MockLedDriver> mock_led;

  std::uint32_t map_idx = types::kSystem;
  std::uint8_t hw_driver = HwLedDriver(map_idx);
  std::uint8_t hw_channel = HwLedCh(map_idx);

  ctx.ui.led_drivers[hw_driver] = &mock_led;

  logic::ProcessImager imager;
  logic::ErrorManager em;
  ctx.logic.process_imager = &imager;
  ctx.logic.error_manager = &em;
  logic::LedManager manager(ctx);

  manager.SetHealthMask(1 << hw_driver);
  imager.AddSystemChangeFlagMask(types::kLedSystem);

  // Act 1: Zustand ohne Fehler (Erwartung: Grün)
  EXPECT_CALL(mock_led, SetColor(hw_channel, types::ColorName::kGreen, _))
      .Times(1)
      .WillOnce(Return(types::ErrorCode::kSuccess));
  manager.UpdateSystemLed();

  // Arrange: Wir simulieren einen Systemfehler
  em.ClearShadowErrors();
  em.AddCycleError("System", types::ErrorCode::kErrorInvalidValue);
  em.ResolveErrors();

  // Act 2: Zustand mit Fehler (Erwartung: Rot)
  EXPECT_CALL(mock_led, SetColor(hw_channel, types::ColorName::kRed, _))
      .Times(1)
      .WillOnce(Return(types::ErrorCode::kSuccess));
  manager.UpdateSystemLed();
}

// -------------------------------------------------------------------
// TEST 5: Hardwareausfälle des LED-Chips werden als Systemfehler protokolliert
// -------------------------------------------------------------------
TEST(LedManagerTest, LogsErrorIfI2cCommunicationFails) {
  core::SystemContext ctx;
  ::testing::NiceMock<MockLedDriver> mock_led;

  std::uint8_t logical_channel = 1;
  std::uint32_t map_idx =
      logical_channel + (types::IO::kAnalogIn * types::IO::kIoGroupCount);
  std::uint8_t hw_driver = HwLedDriver(map_idx);

  ctx.ui.led_drivers[hw_driver] = &mock_led;

  logic::ProcessImager imager;
  logic::ErrorManager em;
  ctx.logic.process_imager = &imager;
  ctx.logic.error_manager = &em;
  logic::LedManager manager(ctx);

  manager.SetHealthMask(1 << hw_driver);
  imager.AddSystemChangeFlagMask(1UL << map_idx);

  // Arrange: Treiber wirft einen Fehler beim Versuch, die Farbe zu setzen
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .Times(1)
      .WillOnce(Return(types::ErrorCode::kErrorI2cCommunication));

  // Act: Update der Hardware-LEDs auslösen
  manager.UpdateHardwareLeds();
  em.ResolveErrors();

  // Assert: ErrorManager muss den Fehler empfangen haben
  EXPECT_GT(em.GetErrorCount(), 0);
}

}  // namespace test
}  // namespace hardware_pruefadapter