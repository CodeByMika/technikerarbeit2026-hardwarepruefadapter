#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstring>
#include <string>
#include <thread>

#include "../lib/Logic/Services/error_manager.h"
#include "../lib/Logic/Services/led_manager_service.h"
#include "../lib/Logic/Services/mailbox_service.h"
#include "../lib/Logic/Services/process_imager_service.h"
#include "../lib/Logic/Services/serial_streamer.h"
#include "../lib/Logic/SystemController/system_controller.h"
#include "../lib/Platform/simulated_devices.h"
#include "../mock_hardware.h"
#include "error_code.h"
#include "system_config.h"
#include "system_context.h"

using ::testing::_;
using ::testing::Ne;
using ::testing::Return;

namespace hardware_pruefadapter {
namespace test {

class SystemControllerTest : public ::testing::Test {
 protected:
  core::SystemContext ctx;
  core::SystemController* controller = nullptr;

  std::unique_ptr<logic::ErrorManager> error_manager;
  std::unique_ptr<logic::ProcessImager> process_imager;
  std::unique_ptr<logic::Mailbox> mailbox;
  std::unique_ptr<logic::SerialStreamer> serial_streamer;
  std::unique_ptr<logic::LedManager> led_manager;

  ::testing::NiceMock<MockAdc> mock_adc;
  ::testing::NiceMock<MockDac> mock_dac;
  ::testing::NiceMock<MockLedDriver> mock_led;
  ::testing::NiceMock<MockSerial> mock_serial;

  // =========================================================
  // HELPER FUNKTIONEN FÜR HARDWARE-MAPPING
  // =========================================================
  std::uint8_t HwDacCh(std::uint8_t logical_channel) {
    return 3 - logical_channel;
  }
  std::uint16_t HwDacMv(std::uint16_t logical_voltage_mv) {
    return logical_voltage_mv / 5;
  }
  std::uint8_t HwAdcCh(types::IO::Group group, std::uint8_t logical_channel) {
    if (group == types::IO::kDigitalOut) {
      const std::uint8_t map[4] = {2, 1, 3, 0};
      return map[logical_channel];
    }
    return logical_channel;
  }
  std::uint8_t HwLedDriver(std::uint8_t map_index) {
    return config::kLedMap[map_index].driver_index;
  }
  std::uint8_t HwLedCh(std::uint8_t map_index) {
    return config::kLedMap[map_index].channel;
  }

  void SetUp() override {
    platform::SharedSimState::Reset();

    for (int i = 0; i < config::kAdcCount; i++)
      ctx.converter.adc[i] = &mock_adc;
    for (int i = 0; i < config::kDacCount; i++)
      ctx.converter.dac[i] = &mock_dac;
    for (int i = 0; i < config::kLedDriverCount; i++)
      ctx.ui.led_drivers[i] = &mock_led;
    for (int i = 0; i < config::kSerialDriverCount; i++)
      ctx.communication.serial[i] = &mock_serial;

    error_manager = std::make_unique<logic::ErrorManager>();
    process_imager = std::make_unique<logic::ProcessImager>();
    mailbox = std::make_unique<logic::Mailbox>();

    ctx.logic.error_manager = error_manager.get();
    ctx.logic.process_imager = process_imager.get();
    ctx.logic.mailbox = mailbox.get();

    serial_streamer = std::make_unique<logic::SerialStreamer>(ctx);
    led_manager = std::make_unique<logic::LedManager>(ctx);

    ctx.logic.serial_streamer = serial_streamer.get();
    ctx.logic.led_manager = led_manager.get();

    controller = new core::SystemController(ctx);
  }

  void TearDown() override { delete controller; }
};

// -------------------------------------------------------------------
// TEST 1: Ein perfekter Start
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, InitializationSucceedsWithValidHardware) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .Times(config::kAdcCount)
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .Times(config::kDacCount)
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .Times(config::kLedDriverCount)
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .Times(config::kSerialDriverCount)
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  types::ErrorCode status = controller->Initialize();
  EXPECT_EQ(status, types::ErrorCode::kSuccess);
}

// -------------------------------------------------------------------
// TEST 2-4: Fehlerhaftes Setup (Nullpointer)
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, InitializationFailsWhenPointerIsNullAdc) {
  ctx.converter.adc[static_cast<std::uint8_t>(types::IO::kAnalogIn)] = nullptr;
  delete controller;
  controller = new core::SystemController(ctx);

  types::ErrorCode status = controller->Initialize();
  EXPECT_EQ(status, types::ErrorCode::kErrorInitWarning);
}

TEST_F(SystemControllerTest, InitializationFailsWhenPointerIsNullDac) {
  ctx.converter.dac[0] = nullptr;
  delete controller;
  controller = new core::SystemController(ctx);

  types::ErrorCode status = controller->Initialize();
  EXPECT_EQ(status, types::ErrorCode::kErrorInitWarning);
}

TEST_F(SystemControllerTest, InitializationFailsWhenPointerIsNullLed) {
  ctx.ui.led_drivers[1] = nullptr;
  delete controller;
  controller = new core::SystemController(ctx);

  types::ErrorCode status = controller->Initialize();
  EXPECT_EQ(status, types::ErrorCode::kErrorInitWarning);
}

// -------------------------------------------------------------------
// TEST 5: Datenfluss (EVA-Prinzip) für Analog Out
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, AnalogOutputTargetIsProcessedAndSentToDac) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(12000));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  EXPECT_CALL(mock_dac, SetValue(HwDacCh(2), HwDacMv(12000)))
      .Times(1)
      .WillOnce(Return(types::ErrorCode::kSuccess));

  ctx.logic.mailbox->SetAnalogTarget(2, 12000);
  controller->RunSystemLogic();

  EXPECT_EQ(ctx.logic.process_imager->GetVoltage(types::IO::kAnalogOut, 2),
            12000);
}

// -------------------------------------------------------------------
// TEST 6: Hardware Error Handling (Kabel abgerissen)
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, HardwareErrorIsLoggedProperly) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kErrorI2cCommunication));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  controller->RunSystemLogic();

  types::SystemStatusDto status =
      logic::DiagnosticsService::BuildSystemStatusDto(*ctx.logic.error_manager);

  EXPECT_GT(status.error_count, 0);

  std::string first_error = status.errors[0];
  EXPECT_TRUE(first_error.find("I2C") != std::string::npos);
}

// -------------------------------------------------------------------
// TEST 7: Logik-Prüfung (20% / 80% Regel für Digital Inputs)
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, DigitalInputThresholdRule) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  ctx.logic.process_imager->SetDigitalReference(types::IO::kInput, 0, 24000);

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(4000));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  for (int i = 0; i < config::kAdcFilterSamples; i++)
    controller->RunSystemLogic();

  EXPECT_EQ(ctx.logic.process_imager->GetDigitalState(types::IO::kDigitalIn, 0),
            0);
}

// -------------------------------------------------------------------
// TEST 8: Digital Input Threshold Rule - HIGH (> 80%)
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, DigitalInputThresholdRuleHigh) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  ctx.logic.process_imager->SetDigitalReference(types::IO::kInput, 1, 5000);

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(4500));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  for (int i = 0; i < config::kAdcFilterSamples; i++)
    controller->RunSystemLogic();

  EXPECT_EQ(ctx.logic.process_imager->GetDigitalState(types::IO::kDigitalIn, 1),
            1);
}

// -------------------------------------------------------------------
// TEST 9: Digital Input Threshold Rule - FLOAT (Ungültiger Bereich)
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, DigitalInputThresholdRuleFloat) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  ctx.logic.process_imager->SetDigitalReference(types::IO::kInput, 2, 24000);

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(12000));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  for (int i = 0; i < config::kAdcFilterSamples; i++)
    controller->RunSystemLogic();

  EXPECT_EQ(ctx.logic.process_imager->GetDigitalState(types::IO::kDigitalIn, 2),
            -1);
}

// -------------------------------------------------------------------
// TEST 10: Moving Average Filter
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, AnalogInputMovingAverageFiltersNoise) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  std::uint16_t current_simulated_mv = 5000;
  EXPECT_CALL(mock_adc, ConvertToVoltage(_))
      .WillRepeatedly(::testing::Invoke(
          [&](std::int16_t) { return current_simulated_mv; }));

  for (int i = 0; i < 10; i++) controller->RunSystemLogic();
  EXPECT_EQ(ctx.logic.process_imager->GetVoltage(types::IO::kAnalogIn, 0),
            5000);

  current_simulated_mv = 15000;
  controller->RunSystemLogic();

  EXPECT_EQ(ctx.logic.process_imager->GetVoltage(types::IO::kAnalogIn, 0),
            6000);

  current_simulated_mv = 5000;
  for (int i = 0; i < 10; i++) controller->RunSystemLogic();
  EXPECT_EQ(ctx.logic.process_imager->GetVoltage(types::IO::kAnalogIn, 0),
            5000);
}

// -------------------------------------------------------------------
// TEST 11: Auto-Recovery
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, ErrorAutoRecoveryWorks) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  types::ErrorCode dynamic_adc_status =
      types::ErrorCode::kErrorI2cCommunication;

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(::testing::Invoke(
          [&](std::uint8_t, std::int16_t*) { return dynamic_adc_status; }));

  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(0));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  controller->RunSystemLogic();

  types::SystemStatusDto status1 =
      logic::DiagnosticsService::BuildSystemStatusDto(*ctx.logic.error_manager);
  EXPECT_GT(status1.error_count, 0);

  dynamic_adc_status = types::ErrorCode::kSuccess;
  controller->RunSystemLogic();

  types::SystemStatusDto status2 =
      logic::DiagnosticsService::BuildSystemStatusDto(*ctx.logic.error_manager);
  EXPECT_EQ(status2.error_count, 0);
}

// -------------------------------------------------------------------
// TEST 12: Sicherheits-Readback
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, AnalogOutputReadbackIsProcessedCorrectly) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(11500));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  for (int i = 0; i < config::kAdcFilterSamples; i++)
    controller->RunSystemLogic();

  EXPECT_EQ(
      ctx.logic.process_imager->GetReadbackVoltage(types::IO::kAnalogOut, 0),
      11500);
}

// -------------------------------------------------------------------
// TEST 13: Serielle Datenverarbeitung bündelt Pakete korrekt
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, SerialRxBuffersAndTriggersCallback) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  bool callback_fired = false;
  std::string received_payload = "";

  ctx.logic.serial_streamer->SetRxCallback(
      [&](types::Serial::Interface uart_num, const std::uint8_t* data,
          std::size_t len) {
        callback_fired = true;
        received_payload =
            std::string(reinterpret_cast<const char*>(data), len);
      });

  EXPECT_CALL(mock_serial, Available())
      .WillOnce(Return(5))
      .WillRepeatedly(Return(0));

  EXPECT_CALL(mock_serial, ReadBlock(_, _))
      .WillOnce(
          ::testing::Invoke([](std::uint8_t* buffer, std::size_t max_len) {
            const char* test_data = "Hallo";
            std::memcpy(buffer, test_data, 5);
            return 5;
          }));

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(0));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  controller->RunSystemLogic();
  EXPECT_FALSE(callback_fired);

  std::this_thread::sleep_for(
      std::chrono::milliseconds(config::kSerialAlmostFullBuffer + 10));

  controller->RunSystemLogic();

  EXPECT_TRUE(callback_fired);
  EXPECT_EQ(received_payload, "Hallo");
}

// -------------------------------------------------------------------
// TEST 14: ReadAdcInputs - Erfolgreiches Mappen
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, ReadAdcInputs_ValidDataIsMappedCorrectly) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(5000));

  for (int i = 0; i < config::kAdcFilterSamples; i++) {
    controller->RunSystemLogic();
  }

  EXPECT_EQ(ctx.logic.process_imager->GetVoltage(types::IO::kAnalogIn, 0),
            5000);
  EXPECT_EQ(
      ctx.logic.process_imager->GetReadbackVoltage(types::IO::kAnalogOut, 0),
      5000);
}

// -------------------------------------------------------------------
// TEST 15: ReadAdcInputs - Hardware-Fehler
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, ReadAdcInputs_I2cErrorResetsFilterAndValue) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kErrorI2cCommunication));
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(0));

  controller->RunSystemLogic();

  EXPECT_EQ(ctx.logic.process_imager->GetVoltage(types::IO::kAnalogIn, 0), 0);
  EXPECT_GT(ctx.logic.error_manager->GetErrorCount(), 0);
}

// -------------------------------------------------------------------
// TEST 16: CheckAndRegulateAnalogOut - P-Regler arbeitet korrekt (Hybrid Logik)
// -------------------------------------------------------------------
TEST_F(SystemControllerTest,
       ProcessSequence_CheckAndRegulateAnalogOut_RegulatesVoltage) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  // Puffer füllen
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(9500));
  for (int i = 0; i < config::kAdcFilterSamples; i++) {
    controller->RunSystemLogic();
  }

  ::testing::Mock::VerifyAndClearExpectations(&mock_dac);

  const std::uint8_t logical_channel = 0;
  const std::uint16_t target_mv = 10000;

  std::uint16_t sc_diff =
      (target_mv * config::kAnalogErrorThresholdPercent) / 100;
  if (sc_diff < config::kAnalogControlToleranceMv + 500) {
    sc_diff = config::kAnalogControlToleranceMv + 500;
  }

  const std::uint16_t diff_mv =
      config::kAnalogControlToleranceMv +
      ((sc_diff - config::kAnalogControlToleranceMv) / 2);

  const std::uint16_t actual_mv = target_mv - diff_mv;
  const std::uint16_t step_mv = diff_mv / 2;
  const std::uint16_t regulated_target_mv = target_mv + step_mv;

  // Erwartung 1: Initiales Schreiben des Soll-Werts (ohne Regler)
  EXPECT_CALL(mock_dac, SetValue(HwDacCh(logical_channel), HwDacMv(target_mv)))
      .Times(1)
      .WillOnce(Return(types::ErrorCode::kSuccess));

  // Erwartung 2: Nach der Schonfrist greift der P-Regler ein.
  EXPECT_CALL(mock_dac,
              SetValue(HwDacCh(logical_channel), HwDacMv(regulated_target_mv)))
      .Times(1)
      .WillOnce(Return(types::ErrorCode::kSuccess));

  // Ignoriere Aufrufe für andere Kanäle
  EXPECT_CALL(mock_dac, SetValue(::testing::Ne(HwDacCh(logical_channel)), _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  // Act: Zielwert setzen
  ctx.logic.mailbox->SetAnalogTarget(logical_channel, target_mv);
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(actual_mv));

  for (int i = 0; i <= config::kOutputGracePeriodCycles; i++) {
    controller->RunSystemLogic();
  }
}

// -------------------------------------------------------------------
// TEST 17: CheckAndRegulateAnalogOut - Gnadenloser Kurzschlussschutz
// -------------------------------------------------------------------
TEST_F(SystemControllerTest,
       ProcessSequence_CheckAndRegulateAnalogOut_ShortCircuitProtection) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(0));
  for (int i = 0; i < config::kAdcFilterSamples; i++) {
    controller->RunSystemLogic();
  }

  ctx.logic.mailbox->SetAnalogTarget(1, 12000);

  bool safety_triggered = false;
  int max_cycles = config::kOutputGracePeriodCycles + 5;

  for (int i = 0; i < max_cycles; i++) {
    controller->RunSystemLogic();
    if (ctx.logic.process_imager->GetVoltage(types::IO::kAnalogOut, 1) == 0) {
      safety_triggered = true;
      break;
    }
  }

  EXPECT_TRUE(safety_triggered);
  EXPECT_GT(ctx.logic.error_manager->GetErrorCount(), 0);
}

// -------------------------------------------------------------------
// TEST 18: CheckDigitalOutSafety - Einbruch der Referenzspannung
// -------------------------------------------------------------------
TEST_F(SystemControllerTest,
       ProcessSequence_CheckDigitalOutSafety_DetectsVoltageDrop) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(5000));
  for (int i = 0; i < config::kAdcFilterSamples; i++) {
    controller->RunSystemLogic();
  }

  ctx.logic.mailbox->SetDigitalReference(types::IO::kOutput, 0, 24000);
  ctx.logic.mailbox->SetDigitalTarget(0, true);

  bool safety_triggered = false;
  int max_cycles = config::kOutputGracePeriodCycles + 5;

  for (int i = 0; i < max_cycles; i++) {
    controller->RunSystemLogic();
    if (ctx.logic.process_imager->GetVoltage(types::IO::kDigitalOut, 0) == 0) {
      safety_triggered = true;
      break;
    }
  }

  EXPECT_TRUE(safety_triggered);
  EXPECT_GT(ctx.logic.error_manager->GetErrorCount(), 0);
}

// -------------------------------------------------------------------
// TEST 19: UpdateDacOutputs - Hardware Error
// -------------------------------------------------------------------
TEST_F(SystemControllerTest,
       OutputSequence_UpdateDacOutputs_FailSafeOnHardwareError) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(12000));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  for (int i = 0; i < config::kAdcFilterSamples; i++)
    controller->RunSystemLogic();

  ctx.logic.mailbox->SetAnalogTarget(0, 12000);

  EXPECT_CALL(mock_dac, SetValue(HwDacCh(0), HwDacMv(12000)))
      .WillOnce(Return(types::ErrorCode::kErrorI2cCommunication));

  controller->RunSystemLogic();

  EXPECT_EQ(ctx.logic.process_imager->GetVoltage(types::IO::kAnalogOut, 0), 0);
  EXPECT_GT(ctx.logic.error_manager->GetErrorCount(), 0);
  EXPECT_FALSE(platform::SharedSimState::digital_out_state[0]);
}

// -------------------------------------------------------------------
// TEST 20: UpdateHardwareLeds - FLOAT Fehler wird Rot
// -------------------------------------------------------------------
TEST_F(SystemControllerTest,
       OutputSequence_UpdateHardwareLeds_FloatStateIsRed) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  std::uint8_t logical_channel = 1;
  types::IO::Group group = types::IO::kDigitalIn;
  std::uint32_t map_idx = logical_channel + (group * types::IO::kIoGroupCount);
  std::uint8_t hw_channel = HwLedCh(map_idx);

  ctx.logic.process_imager->SetDigitalReference(types::IO::kInput,
                                                logical_channel, 24000);

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(12000));

  EXPECT_CALL(mock_led, SetColor(hw_channel, types::ColorName::kRed, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(Ne(hw_channel), _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  for (int i = 0; i < config::kAdcFilterSamples; i++)
    controller->RunSystemLogic();
}

// -------------------------------------------------------------------
// TEST 21: UpdateSystemLed - System Led wird Rot bei Fehlern
// -------------------------------------------------------------------
TEST_F(SystemControllerTest, OutputSequence_UpdateSystemLed_ReactsToErrors) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(0));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  std::uint8_t hw_system_channel = HwLedCh(types::kSystem);

  EXPECT_CALL(mock_led,
              SetColor(hw_system_channel, types::ColorName::kGreen, _))
      .WillOnce(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(hw_system_channel, types::ColorName::kRed, _))
      .WillOnce(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(Ne(hw_system_channel), _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  controller->RunSystemLogic();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kErrorI2cCommunication));

  controller->RunSystemLogic();
  controller->RunSystemLogic();
}

// -------------------------------------------------------------------
// TEST 22: CheckAndRegulateAnalogOut - P-Regler kappt bei Maximalspannung
// -------------------------------------------------------------------
TEST_F(SystemControllerTest,
       ProcessSequence_CheckAndRegulateAnalogOut_CapsAtMaximumVoltage) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  const std::uint8_t logical_channel = 0;
  const std::uint16_t target_mv = config::kMaxSystemVoltageMv;

  std::uint16_t sc_diff =
      (target_mv * config::kAnalogErrorThresholdPercent) / 100;
  if (sc_diff < config::kAnalogControlToleranceMv + 500) {
    sc_diff = config::kAnalogControlToleranceMv + 500;
  }
  const std::uint16_t diff_mv =
      config::kAnalogControlToleranceMv +
      ((sc_diff - config::kAnalogControlToleranceMv) / 2);
  const std::uint16_t actual_mv = target_mv - diff_mv;

  EXPECT_CALL(mock_adc, ConvertToVoltage(_))
      .WillRepeatedly(
          Return(((target_mv * config::kAnalogErrorThresholdPercent) / 100) +
                 target_mv + 100));
  for (int i = 0; i < config::kAdcFilterSamples; i++) {
    controller->RunSystemLogic();
  }

  ::testing::Mock::VerifyAndClearExpectations(&mock_dac);

  // Erwartung 1: Im 1. Zyklus wird der maximale Soll-Wert geschrieben
  // Erwartung 2: Der P-Regler will nach der Schonfrist addieren, wird aber vom
  // Limit wieder auf Maximum gekappt!
  EXPECT_CALL(mock_dac, SetValue(HwDacCh(logical_channel),
                                 HwDacMv(config::kMaxSystemVoltageMv)))
      .Times(2)
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  EXPECT_CALL(mock_dac, SetValue(::testing::Ne(HwDacCh(logical_channel)), _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  // Act: Zielwert auf das target setzen
  ctx.logic.mailbox->SetAnalogTarget(logical_channel, target_mv);
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(actual_mv));

  // Führe ersten Zyklus aus, damit der Zielwert durch den Sync in das
  // Active-Image übernommen wird
  controller->RunSystemLogic();

  // JETZT simulieren wir einen bereits sehr hochgeregelten Zustand des DACs
  // (z.B. 23.8V) Dies muss NACH dem 1. Zyklus passieren, da SyncUpdateToActive
  // es sonst überschreibt!
  ctx.logic.process_imager->SetAdjustedDacVoltage(
      0, config::kMaxSystemVoltageMv - 200);

  // Die restliche Schonfrist + 1 Zyklus für den Regler abwarten
  for (int i = 0; i < config::kOutputGracePeriodCycles; i++) {
    controller->RunSystemLogic();
  }
}

// -------------------------------------------------------------------
// TEST 23: CheckAndRegulateAnalogOut - P-Regler kappt bei Minimalspannung
// (0V)
// -------------------------------------------------------------------
TEST_F(SystemControllerTest,
       ProcessSequence_CheckAndRegulateAnalogOut_CapsAtMinimumVoltage) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  // Setze den Ist-Wert extrem zu hoch (2000mV), um die P-Regler Totzone zu
  // überwinden
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(2000));
  for (int i = 0; i < config::kAdcFilterSamples; i++) {
    controller->RunSystemLogic();
  }

  ::testing::Mock::VerifyAndClearExpectations(&mock_dac);

  // Erwartung 1: Initialer Aufruf auf 100mV (direkt aus der Mailbox)
  EXPECT_CALL(mock_dac, SetValue(HwDacCh(0), HwDacMv(100)))
      .Times(1)
      .WillOnce(Return(types::ErrorCode::kSuccess));

  // Erwartung 2: Nach der Schonfrist rechnet der Regler:
  // (100 Soll - 2000 Ist) = -1900. Step = -950.
  // Base = 100. New = 100 - 950 = -850 -> Das ist < 0, also Kappung bei 0V
  EXPECT_CALL(mock_dac, SetValue(HwDacCh(0), HwDacMv(0)))
      .Times(1)
      .WillOnce(Return(types::ErrorCode::kSuccess));

  EXPECT_CALL(mock_dac, SetValue(::testing::Ne(HwDacCh(0)), _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  // Act: Zielwert auf 100mV setzen
  ctx.logic.mailbox->SetAnalogTarget(0, 100);
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(2200));

  for (int i = 0; i <= config::kOutputGracePeriodCycles; i++) {
    controller->RunSystemLogic();
  }
}

// -------------------------------------------------------------------
// TEST 24: CheckDigitalOutSafety - Grace Period verhindert sofortiges
// Abschalten
// -------------------------------------------------------------------
TEST_F(SystemControllerTest,
       ProcessSequence_CheckDigitalOutSafety_WaitsForGracePeriod) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(0));

  const std::uint8_t logical_channel = 0;
  const std::uint16_t ref_mv = config::kWarningVoltageMv;

  ctx.logic.mailbox->SetDigitalReference(types::IO::kOutput, logical_channel,
                                         ref_mv);
  controller->RunSystemLogic();

  ctx.logic.mailbox->SetDigitalTarget(logical_channel, true);
  controller->RunSystemLogic();

  EXPECT_EQ(ctx.logic.process_imager->GetVoltage(types::IO::kDigitalOut,
                                                 logical_channel),
            1);
  EXPECT_EQ(ctx.logic.error_manager->GetErrorCount(), 0);

  // Zyklen  innerhalb der Schonfrist ausführen
  int safe_cycles = config::kOutputGracePeriodCycles - 1;
  for (int i = 0; i < safe_cycles; i++) {
    controller->RunSystemLogic();
  }

  // Darf immer noch nicht abgeschaltet haben
  EXPECT_EQ(ctx.logic.process_imager->GetVoltage(types::IO::kDigitalOut,
                                                 logical_channel),
            1);
  EXPECT_EQ(ctx.logic.error_manager->GetErrorCount(), 0);

  // Schonfrist überziehen
  bool safety_triggered = false;
  for (int i = 0; i < 5; i++) {
    controller->RunSystemLogic();
    if (ctx.logic.process_imager->GetVoltage(types::IO::kDigitalOut,
                                             logical_channel) == 0) {
      safety_triggered = true;
      break;
    }
  }

  EXPECT_TRUE(safety_triggered);
  EXPECT_GT(ctx.logic.error_manager->GetErrorCount(), 0);
}

// -------------------------------------------------------------------
// TEST 25: CheckDigitalOutSafety - Erfolgreiches Schalten (Happy Path)
// -------------------------------------------------------------------
TEST_F(SystemControllerTest,
       ProcessSequence_CheckDigitalOutSafety_PassesIfHardwareSwitchesInTime) {
  EXPECT_CALL(mock_adc, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, Initialize(_, _, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_serial, Initialize(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  controller->Initialize();

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  const std::uint8_t logical_channel = 0;
  const std::uint16_t ref_mv = config::kWarningVoltageMv;

  ctx.logic.mailbox->SetDigitalReference(types::IO::kOutput, logical_channel,
                                         ref_mv);
  controller->RunSystemLogic();

  ctx.logic.mailbox->SetDigitalTarget(logical_channel, true);

  // ADC liest am Anfang noch 0V
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(0));

  // Führe ein paar Zyklen aus, aber bleibe sicher innerhalb der Schonfrist
  int partial_grace = config::kOutputGracePeriodCycles / 2;
  for (int i = 0; i < partial_grace; i++) {
    controller->RunSystemLogic();
  }

  EXPECT_EQ(ctx.logic.process_imager->GetVoltage(types::IO::kDigitalOut,
                                                 logical_channel),
            1);
  EXPECT_EQ(ctx.logic.error_manager->GetErrorCount(), 0);

  ::testing::Mock::VerifyAndClearExpectations(&mock_adc);

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  // Nun simuliert der ADC den Hardware-Zustand, dass der Pin erfolgreich
  // geschaltet wurde (Referenzspannung erreicht)
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(ref_mv));

  // Zyklen weiterlaufen lassen, bis die Schonfrist definitiv abgelaufen ist.
  int remaining_grace = config::kOutputGracePeriodCycles + 5;
  for (int i = 0; i < remaining_grace; i++) {
    controller->RunSystemLogic();
  }

  // Ausgang bleibt HIGH und es gibt keinen Fehler!
  EXPECT_EQ(ctx.logic.process_imager->GetVoltage(types::IO::kDigitalOut,
                                                 logical_channel),
            1);
  EXPECT_EQ(ctx.logic.error_manager->GetErrorCount(), 0);
}

}  // namespace test
}  // namespace hardware_pruefadapter