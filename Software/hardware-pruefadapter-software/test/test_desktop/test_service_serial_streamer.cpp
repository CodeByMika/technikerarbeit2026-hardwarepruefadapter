#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <cstring>  // ACHTUNG: Wichtig für std::memcpy!
#include <thread>

#include "../lib/Logic/Services/serial_streamer.h"
#include "../mock_hardware.h"
#include "system_config.h"

using ::testing::_;
using ::testing::Return;

namespace hardware_pruefadapter {
namespace test {

// -------------------------------------------------------------------
// TEST 1: Senden auf blockierte (offline) Ports wird verworfen
// -------------------------------------------------------------------
TEST(SerialStreamerTest, WriteTx_IgnoredIfHealthMaskIsZero) {
  core::SystemContext ctx;

  ::testing::StrictMock<MockSerial> mock_serial;
  ctx.communication.serial[types::Serial::kUart] = &mock_serial;

  logic::SerialStreamer streamer(ctx);
  streamer.SetHealthMask(0);

  streamer.WriteTx(types::Serial::kUart, "Test");
}

// -------------------------------------------------------------------
// TEST 2: Senden auf aktive Ports leitet an die Hardware weiter
// -------------------------------------------------------------------
TEST(SerialStreamerTest, WriteTx_CalledIfPortIsHealthy) {
  core::SystemContext ctx;
  ::testing::NiceMock<MockSerial> mock_serial;
  ctx.communication.serial[types::Serial::kUart] = &mock_serial;

  logic::SerialStreamer streamer(ctx);
  streamer.SetHealthMask(1 << types::Serial::kUart);

  EXPECT_CALL(mock_serial, WriteBlock(_, 4)).Times(1).WillOnce(Return(4));
  streamer.WriteTx(types::Serial::kUart, "Test");
}

// -------------------------------------------------------------------
// TEST 3: Baudraten-Änderungen bei ungültigen Werten verwerfen
// -------------------------------------------------------------------
TEST(SerialStreamerTest, SetBaudrate_IgnoredForInvalidPorts) {
  core::SystemContext ctx;
  ::testing::StrictMock<MockSerial> mock_serial;
  ctx.communication.serial[types::Serial::kUart] = &mock_serial;

  logic::SerialStreamer streamer(ctx);
  streamer.SetHealthMask(1 << types::Serial::kUart);

  // Ungültige Baudrate
  streamer.SetBaudrate(types::Serial::kUart, config::kMaxSerialBaudRate + 1000);

  // Ungültiger Port (Cast ist für den Out-Of-Bounds Test zwingend nötig)
  streamer.SetBaudrate(
      static_cast<types::Serial::Interface>(config::kSerialDriverCount + 1),
      9600);
}

// -------------------------------------------------------------------
// TEST 4: Empfangen von Daten triggert den Callback (Timeouts)
// -------------------------------------------------------------------
TEST(SerialStreamerTest, ProcessReadRx_TriggersCallbackOnTimeout) {
  core::SystemContext ctx;
  ::testing::NiceMock<MockSerial> mock_serial;
  ctx.communication.serial[types::Serial::kUart] = &mock_serial;

  logic::SerialStreamer streamer(ctx);
  streamer.SetHealthMask(1 << types::Serial::kUart);

  bool callback_fired = false;
  std::string received_payload = "";

  streamer.SetRxCallback([&](types::Serial::Interface uart_num,
                             const std::uint8_t* data, std::size_t len) {
    callback_fired = true;
    received_payload = std::string(reinterpret_cast<const char*>(data), len);
  });

  EXPECT_CALL(mock_serial, Available())
      .WillOnce(Return(5))
      .WillRepeatedly(Return(0));

  EXPECT_CALL(mock_serial, ReadBlock(_, _))
      .WillOnce(
          ::testing::Invoke([](std::uint8_t* buffer, std::size_t max_len) {
            const char* test_data = "Hello";
            std::memcpy(buffer, test_data, 5);
            return 5;
          }));

  streamer.ProcessReadRx();
  EXPECT_FALSE(callback_fired);

  std::this_thread::sleep_for(
      std::chrono::milliseconds(config::kSerialAlmostFullBuffer + 10));

  streamer.ProcessReadRx();

  EXPECT_TRUE(callback_fired);
  EXPECT_EQ(received_payload, "Hello");
}

}  // namespace test
}  // namespace hardware_pruefadapter