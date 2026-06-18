#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../lib/Logic/Services/hardware_bootstrapper.h"
#include "../mock_hardware.h"

using ::testing::_;
using ::testing::Return;

namespace hardware_pruefadapter {
namespace test {

// -------------------------------------------------------------------
// TEST 1: Bootvorgang fängt fehlende Treiber (Nullpointer) sauber ab
// -------------------------------------------------------------------
TEST(HardwareBootstrapperTest, BootFailsGracefullyOnNullptrs) {
  core::SystemContext ctx;
  logic::ErrorManager error_manager;

  auto health = logic::HardwareBootstrapper::Boot(ctx, error_manager);

  // Fehler aus dem Schattenspeicher offiziell übernehmen
  error_manager.ResolveErrors();

  EXPECT_FALSE(health.is_healthy);
  EXPECT_EQ(health.adc_init_success, 0);

  EXPECT_GT(error_manager.GetErrorCount(), 0);
}

// -------------------------------------------------------------------
// TEST 2: Bootvorgang schließt erfolgreich ab, wenn alle Treiber bereit sind
// -------------------------------------------------------------------
TEST(HardwareBootstrapperTest, BootSucceedsWithValidHardwareInfo) {
  core::SystemContext ctx;
  ::testing::NiceMock<MockAdc> mock_adc;
  ::testing::NiceMock<MockDac> mock_dac;
  ::testing::NiceMock<MockLedDriver> mock_led;
  ::testing::NiceMock<MockSerial> mock_serial;

  for (int i = 0; i < config::kAdcCount; i++) ctx.converter.adc[i] = &mock_adc;
  for (int i = 0; i < config::kDacCount; i++) ctx.converter.dac[i] = &mock_dac;
  for (int i = 0; i < config::kLedDriverCount; i++)
    ctx.ui.led_drivers[i] = &mock_led;
  for (int i = 0; i < config::kSerialDriverCount; i++)
    ctx.communication.serial[i] = &mock_serial;

  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  logic::ErrorManager error_manager;
  auto health = logic::HardwareBootstrapper::Boot(ctx, error_manager);

  // Fehler aus dem Schattenspeicher offiziell übernehmen
  error_manager.ResolveErrors();

  EXPECT_TRUE(health.is_healthy);
  EXPECT_EQ(error_manager.GetErrorCount(), 0);

  EXPECT_EQ(health.adc_init_success, config::kExpectedAdcInitMask);
  EXPECT_EQ(health.dac_init_success, config::kExpectedDacInitMask);
  EXPECT_EQ(health.led_init_success, config::kExpectedLedInitMask);
  EXPECT_EQ(health.serial_init_success, config::kExpectedSerialInitMask);
}

// -------------------------------------------------------------------
// TEST 3: Bootvorgang registriert I2C-Fehler (Hardware Defekt)
// -------------------------------------------------------------------
TEST(HardwareBootstrapperTest, BootRegistersHardwareErrors) {
  core::SystemContext ctx;
  ::testing::NiceMock<MockAdc> mock_adc;
  ::testing::NiceMock<MockDac> mock_dac;
  ::testing::NiceMock<MockLedDriver> mock_led;
  ::testing::NiceMock<MockSerial> mock_serial;

  for (int i = 0; i < config::kAdcCount; i++) ctx.converter.adc[i] = &mock_adc;
  for (int i = 0; i < config::kDacCount; i++) ctx.converter.dac[i] = &mock_dac;
  for (int i = 0; i < config::kLedDriverCount; i++)
    ctx.ui.led_drivers[i] = &mock_led;
  for (int i = 0; i < config::kSerialDriverCount; i++)
    ctx.communication.serial[i] = &mock_serial;

  // Simulieren eines defekten I2C Busses beim ersten ADC
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillOnce(Return(types::ErrorCode::kErrorI2cCommunication))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  logic::ErrorManager error_manager;
  auto health = logic::HardwareBootstrapper::Boot(ctx, error_manager);

  // Fehler aus dem Schattenspeicher offiziell übernehmen
  error_manager.ResolveErrors();

  // System startet im Notlauf (Teilweise intakt)
  EXPECT_FALSE(health.is_healthy);
  EXPECT_GT(error_manager.GetErrorCount(), 0);

  // Maske darf NICHT der Expected-Maske entsprechen, da Chip 1 (Bit 0) fehlt
  EXPECT_NE(health.adc_init_success, config::kExpectedAdcInitMask);
}

}  // namespace test
}  // namespace hardware_pruefadapter