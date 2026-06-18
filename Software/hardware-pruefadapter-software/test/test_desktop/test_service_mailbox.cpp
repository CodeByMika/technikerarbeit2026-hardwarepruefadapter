#include <gtest/gtest.h>

#include "../lib/Logic/Services/mailbox_service.h"
#include "system_config.h"

namespace hardware_pruefadapter {
namespace test {

// -------------------------------------------------------------------
// TEST 1: Out-Of-Bounds-Schutz und Limits für Analog-Out (Mailbox)
// -------------------------------------------------------------------
TEST(MailboxTest, AnalogTarget_ValidAndInvalidBounds) {
  logic::Mailbox mailbox;

  mailbox.SetAnalogTarget(0, 15000);

  std::uint16_t invalid_voltage =
      static_cast<std::uint16_t>(config::kMaxSystemVoltageMv + 1000);
  mailbox.SetAnalogTarget(1, invalid_voltage);
  mailbox.SetAnalogTarget(config::kNumChannels + 1, 10000);

  auto snap = mailbox.ConsumeIoTargets();

  EXPECT_TRUE(snap.new_analog[0]);
  EXPECT_EQ(snap.analog_mv[0], 15000);

  EXPECT_FALSE(snap.new_analog[1]);
  EXPECT_EQ(snap.analog_mv[1], 0);
}

// -------------------------------------------------------------------
// TEST 2: Out-Of-Bounds-Schutz für Digital-Out (Mailbox)
// -------------------------------------------------------------------
TEST(MailboxTest, DigitalTarget_IgnoresInvalidChannels) {
  logic::Mailbox mailbox;

  mailbox.SetDigitalTarget(0, true);
  mailbox.SetDigitalTarget(config::kNumChannels + 1, true);

  auto snap = mailbox.ConsumeIoTargets();

  EXPECT_TRUE(snap.new_digital[0]);
  EXPECT_TRUE(snap.digital_state[0]);
}

// -------------------------------------------------------------------
// TEST 3: "Last Write Wins" SPS-Logik für I/O Ziele
// -------------------------------------------------------------------
TEST(MailboxTest, AnalogTarget_LastWriteWins) {
  logic::Mailbox mailbox;

  mailbox.SetAnalogTarget(2, 5000);
  mailbox.SetAnalogTarget(2, 10000);

  auto snap = mailbox.ConsumeIoTargets();
  EXPECT_TRUE(snap.new_analog[2]);
  EXPECT_EQ(snap.analog_mv[2], 10000);
}

// -------------------------------------------------------------------
// TEST 4: Serielle Daten (TX) müssen chronologisch verkettet werden
// -------------------------------------------------------------------
TEST(MailboxTest, SerialTx_AppendsMultiplePayloads) {
  logic::Mailbox mailbox;

  mailbox.AddSerialTx(types::Serial::kUart, "AT");
  mailbox.AddSerialTx(types::Serial::kUart, "+RST\n");

  auto snap = mailbox.ConsumeSerialTx();

  EXPECT_TRUE(snap.has_data);
  EXPECT_EQ(snap.payloads[types::Serial::kUart], "AT+RST\n");

  auto snap2 = mailbox.ConsumeSerialTx();
  EXPECT_FALSE(snap2.has_data);
  EXPECT_TRUE(snap2.payloads[types::Serial::kUart].empty());
}

// -------------------------------------------------------------------
// TEST 5: Ungültige serielle Konfigurationen abfangen
// -------------------------------------------------------------------
TEST(MailboxTest, SerialConfig_IgnoresInvalidBaudrates) {
  logic::Mailbox mailbox;

  mailbox.SetSerialConfig(types::Serial::kRs485, 9600);
  mailbox.SetSerialConfig(types::Serial::kRs232,
                          config::kMaxSerialBaudRate + 1000);

  auto snap = mailbox.ConsumeSerialConfig();
  EXPECT_TRUE(snap.new_baudrate[types::Serial::kRs485]);
  EXPECT_EQ(snap.baudrates[types::Serial::kRs485], 9600);

  EXPECT_FALSE(snap.new_baudrate[types::Serial::kRs232]);
}

}  // namespace test
}  // namespace hardware_pruefadapter