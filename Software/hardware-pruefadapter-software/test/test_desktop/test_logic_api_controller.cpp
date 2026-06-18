#include <ArduinoJson.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "../lib/Logic/ApiController/api_controller.h"
#include "../lib/Logic/Services/error_manager.h"
#include "../lib/Logic/Services/led_manager_service.h"
#include "../lib/Logic/Services/mailbox_service.h"
#include "../lib/Logic/Services/process_imager_service.h"
#include "../lib/Logic/Services/serial_streamer.h"
#include "../lib/Logic/SystemController/system_controller.h"
#include "../lib/Platform/simulated_devices.h"
#include "../mock_hardware.h"
#include "system_config.h"
#include "system_context.h"

using ::testing::_;
using ::testing::Return;

namespace hardware_pruefadapter {
namespace test {

// ===================================================================
// 1. FAKE WEB SERVER (Simuliert den Browser/Netzwerk-Stack)
// ===================================================================
class FakeWebServer : public interfaces::IWebServer {
 public:
  std::map<std::string, interfaces::RouteHandler> get_routes;
  std::map<std::string, interfaces::RouteWithBodyHandler> put_routes;
  std::vector<std::string> sse_routes;
  std::vector<std::pair<std::string, std::string>> sse_events;

  types::ErrorCode Start() override { return types::ErrorCode::kSuccess; }

  void RegisterGetRoute(const char* path,
                        interfaces::RouteHandler handler) override {
    get_routes[path] = handler;
  }

  void RegisterPutRoute(const char* path,
                        interfaces::RouteWithBodyHandler handler) override {
    put_routes[path] = handler;
  }

  void RegisterSseRoute(const char* path) override {
    sse_routes.push_back(path);
  }

  void SendSseEvent(const char* event_name, const char* data) override {
    sse_events.push_back({event_name, data});
  }

  void RegisterStaticRoute(const char* path, const char* content,
                           const char* content_type) override {}
};

// ===================================================================
// 2. TEST FIXTURE (Setup für jeden API Test)
// ===================================================================
class ApiControllerTest : public ::testing::Test {
 protected:
  core::SystemContext ctx;
  core::SystemController* system_controller = nullptr;
  logic::ApiController* api_controller = nullptr;
  FakeWebServer fake_server;

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

    system_controller = new core::SystemController(ctx);
    api_controller = new logic::ApiController(&fake_server, ctx);

    EXPECT_CALL(mock_adc, Initialize(_, _, _))
        .WillRepeatedly(Return(types::ErrorCode::kSuccess));
    EXPECT_CALL(mock_dac, Initialize(_, _, _))
        .WillRepeatedly(Return(types::ErrorCode::kSuccess));
    EXPECT_CALL(mock_led, Initialize(_, _, _, _))
        .WillRepeatedly(Return(types::ErrorCode::kSuccess));
    EXPECT_CALL(mock_serial, Initialize(_, _, _))
        .WillRepeatedly(Return(types::ErrorCode::kSuccess));

    system_controller->Initialize();
    api_controller->RegisterRoutes();
  }

  void TearDown() override {
    delete api_controller;
    delete system_controller;
  }
};

// ===================================================================
// 3. DIE TESTS
// ===================================================================

// -------------------------------------------------------------------
// TEST 1: Start des Webservers
// -------------------------------------------------------------------

TEST_F(ApiControllerTest, RoutesAreSuccessfullyRegistered) {
  EXPECT_TRUE(fake_server.get_routes.find("/api/v1/system/status") !=
              fake_server.get_routes.end());
  EXPECT_TRUE(fake_server.get_routes.find("/api/v1/io") !=
              fake_server.get_routes.end());
  EXPECT_TRUE(fake_server.put_routes.find("/api/v1/io/analog/output/3") !=
              fake_server.put_routes.end());
  EXPECT_EQ(fake_server.sse_routes.size(), 1);
  EXPECT_EQ(fake_server.sse_routes[0], "/api/v1/system/logs/stream");
}

// -------------------------------------------------------------------
// TEST 2: Rückgabewerte sind im richtigen Json Format
// -------------------------------------------------------------------

TEST_F(ApiControllerTest, GetIoAllReturnsCorrectJsonSchema) {
  std::string response = fake_server.get_routes["/api/v1/io"]();
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, response);

  EXPECT_EQ(err.code(), DeserializationError::Ok);
  EXPECT_TRUE(doc["analog_in"].is<JsonArray>());
  EXPECT_EQ(doc["analog_in"].size(), config::kNumChannels);
  EXPECT_EQ(doc["analog_out"].size(), config::kNumChannels);
  EXPECT_EQ(doc["digital_in"].size(), config::kNumChannels);
  EXPECT_EQ(doc["digital_out"].size(), config::kNumChannels);
  EXPECT_FALSE(doc["digital_in"][0]["state"].isNull());
  EXPECT_FALSE(doc["digital_in"][0]["value"].isNull());
}

// -------------------------------------------------------------------
// TEST 3: Analogen Ausgang setzen
// -------------------------------------------------------------------

TEST_F(ApiControllerTest, PutAnalogOutputValidValueSucceeds) {
  std::string payload = "{\"value\": 12000}";
  std::string response =
      fake_server.put_routes["/api/v1/io/analog/output/0"](payload);

  JsonDocument doc;
  deserializeJson(doc, response);
  EXPECT_EQ(doc["status"], "success");

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_adc, ConvertToVoltage(_))
      .WillRepeatedly(Return(12000));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  EXPECT_CALL(mock_dac, SetValue(HwDacCh(0), HwDacMv(12000)))
      .Times(1)
      .WillOnce(Return(types::ErrorCode::kSuccess));

  for (int i = 0; i <= config::kOutputGracePeriodCycles; i++) {
    system_controller->RunSystemLogic();
  }
}

// -------------------------------------------------------------------
// TEST 4: Analogen Ausgang setzen, Spannung überschritten
// -------------------------------------------------------------------

TEST_F(ApiControllerTest, PutAnalogOutputExceedsMaximumFails) {
  // Generiert das JSON dynamisch anhand des Limits
  std::string payload =
      "{\"value\": " + std::to_string(config::kMaxSystemVoltageMv + 1000) + "}";
  std::string response =
      fake_server.put_routes["/api/v1/io/analog/output/0"](payload);

  JsonDocument doc;
  deserializeJson(doc, response);
  EXPECT_FALSE(doc["message"].isNull());
  EXPECT_TRUE(
      std::string(doc["message"].as<const char*>()).find("exceeds maximum") !=
      std::string::npos);
}

// -------------------------------------------------------------------
// TEST 5: Put Request Invalid JSON
// -------------------------------------------------------------------

TEST_F(ApiControllerTest, PutWithInvalidJsonFailsGracefully) {
  std::string payload = "{value: 5000";
  std::string response =
      fake_server.put_routes["/api/v1/io/analog/output/0"](payload);

  JsonDocument doc;
  deserializeJson(doc, response);
  EXPECT_FALSE(doc["message"].isNull());
  EXPECT_TRUE(
      std::string(doc["message"].as<const char*>()).find("Falsches JSON") !=
      std::string::npos);
}

TEST_F(ApiControllerTest, PutDigitalOutputValidValueSucceeds) {
  std::string payload = "{\"state\": true}";
  std::string response =
      fake_server.put_routes["/api/v1/io/digital/output/1"](payload);

  JsonDocument doc;
  deserializeJson(doc, response);
  EXPECT_EQ(doc["status"], "success");
}

TEST_F(ApiControllerTest, PutDigitalReferenceChangesSystemConfig) {
  std::string payload = "{\"voltage_mv\": 5000}";
  std::string response =
      fake_server.put_routes["/api/v1/io/digital/input/2/reference"](payload);

  JsonDocument doc;
  deserializeJson(doc, response);
  EXPECT_EQ(doc["status"], "success");

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_adc, ConvertToVoltage(_))
      .WillRepeatedly(Return(4500));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  system_controller->RunSystemLogic();

  EXPECT_EQ(ctx.logic.process_imager->GetDigitalState(types::IO::kDigitalIn, 2),
            1);
}

TEST_F(ApiControllerTest, PutSerialTxSendsDataToCorrectUart) {
  EXPECT_CALL(mock_serial, WriteBlock(::testing::_, 9))
      .WillOnce(::testing::Invoke([](const void* data, std::size_t len) {
        std::string sent_msg(static_cast<const char*>(data), len);
        EXPECT_EQ(sent_msg, "AT+TEST\r\n");
        return len;
      }));

  EXPECT_CALL(mock_adc, GetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_adc, ConvertToVoltage(_)).WillRepeatedly(Return(0));
  EXPECT_CALL(mock_dac, SetValue(_, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));
  EXPECT_CALL(mock_led, SetColor(_, _, _))
      .WillRepeatedly(Return(types::ErrorCode::kSuccess));

  std::string payload =
      "{\"interface\": \"UART\", \"payload\": \"AT+TEST\\r\\n\"}";
  std::string response = fake_server.put_routes["/api/v1/serial/tx"](payload);

  system_controller->RunSystemLogic();

  JsonDocument doc;
  deserializeJson(doc, response);
  EXPECT_EQ(doc["status"], "success");
}

}  // namespace test
}  // namespace hardware_pruefadapter