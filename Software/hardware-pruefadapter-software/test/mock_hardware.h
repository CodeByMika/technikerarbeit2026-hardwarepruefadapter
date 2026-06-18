#ifndef MOCK_HARDWARE_H_
#define MOCK_HARDWARE_H_

#include <gmock/gmock.h>

#include <cstdint>

#include "../lib/Interfaces/analog_to_digital_interface.h"
#include "../lib/Interfaces/digital_to_analog_interface.h"
#include "../lib/Interfaces/led_interface.h"
#include "../lib/Interfaces/serial_interface.h"
#include "error_code.h"

namespace hardware_pruefadapter {
namespace test {

/**
 * @brief GoogleMock für den ADC (Analog-Digital-Wandler)
 */
class MockAdc : public interfaces::IAnalogToDigitalConverter {
 public:
  MOCK_METHOD(types::ErrorCode, Initialize,
              (std::uint8_t address, std::uint16_t gain_mv,
               std::uint8_t i2c_bus_nr),
              (override));
  MOCK_METHOD(bool, IsConnected, (), (const, override));
  MOCK_METHOD(bool, IsBusy, (), (const, override));
  MOCK_METHOD(bool, SetMode, (bool mode), (override));
  MOCK_METHOD(bool, SetGain, (std::uint16_t gain_mv), (override));
  MOCK_METHOD(types::ErrorCode, SetDataRate, (std::uint16_t sps), (override));
  MOCK_METHOD(types::ErrorCode, SetDifferentialMode,
              (std::uint8_t ch_low, std::uint8_t ch_high), (override));
  MOCK_METHOD(bool, SetInternExtern, (bool intern_extern), (override));
  MOCK_METHOD(types::ErrorCode, GetValue,
              (std::uint8_t channel, std::int16_t* raw_out), (const, override));
  MOCK_METHOD(bool, GetMode, (), (const, override));
  MOCK_METHOD(std::uint16_t, GetGain, (), (const, override));
  MOCK_METHOD(bool, GetInternExtern, (), (const, override));
  MOCK_METHOD(std::uint16_t, ConvertToVoltage, (std::int16_t raw),
              (const, override));
  types::ErrorCode GetI2cAddress(std::uint8_t* address) const override {
    return types::ErrorCode::kSuccess;
  }
  bool GetOnlineStatus() const override { return true; }
  std::uint8_t GetDifferentialMode() const override { return 0; }
};

/**
 * @brief GoogleMock für den DAC (Digital-Analog-Wandler)
 */
class MockDac : public interfaces::IDigitalToAnalogConverter {
 public:
  MOCK_METHOD(types::ErrorCode, Initialize,
              (std::uint8_t address, std::uint16_t vref_mv,
               std::uint8_t i2c_bus_nr),
              (override));
  MOCK_METHOD(bool, IsConnected, (), (const, override));
  MOCK_METHOD(bool, SetMode, (std::uint8_t mode), (override));
  MOCK_METHOD(types::ErrorCode, SetValue,
              (std::uint8_t channel, std::uint16_t voltage_mv), (override));
  MOCK_METHOD(types::ErrorCode, SetVoltageReference,
              (std::uint16_t voltage_low_mv, std::uint16_t voltage_high_mv),
              (override));
  MOCK_METHOD(types::ErrorCode, GetValue,
              (std::uint8_t channel, std::uint16_t* voltage_mv_in),
              (const, override));
  MOCK_METHOD(types::ErrorCode, GetMaxVoltage, (std::uint16_t* voltage_high_mv),
              (const, override));
  MOCK_METHOD(types::ErrorCode, GetGain, (std::uint16_t* gain_mv),
              (const, override));
  MOCK_METHOD(std::uint8_t, GetMode, (), (const, override));
  MOCK_METHOD(std::uint16_t, ConvertToVoltage, (std::uint16_t raw),
              (const, override));
  MOCK_METHOD(std::uint16_t, ConvertToRaw, (std::uint16_t voltage_mv),
              (const, override));
};

/**
 * @brief GoogleMock für den LED Treiber (LP50xx)
 */
class MockLedDriver : public interfaces::ILedDriver {
 public:
  MOCK_METHOD(types::ErrorCode, Initialize,
              (std::uint8_t address, std::uint8_t config, std::uint8_t chip_nr,
               std::uint8_t i2c_bus_nr),
              (override));
  MOCK_METHOD(types::ErrorCode, SetColor,
              (std::uint8_t channel, types::ColorName color,
               types::AddressType address_type),
              (override));
  MOCK_METHOD(types::ErrorCode, TurnOff, (), (override));
};

/**
 * @brief GoogleMock für die Serielle Schnittstelle
 */
class MockSerial : public interfaces::ISerialInterface {
 public:
  MOCK_METHOD(types::ErrorCode, Initialize,
              (types::Serial::Interface uart_num, std::uint8_t rx_pin,
               std::uint8_t tx_pin),
              (override));
  MOCK_METHOD(bool, IsConnected, (), (const, override));
  MOCK_METHOD(types::ErrorCode, SetBaudrate, (std::uint32_t baudrate),
              (override));
  MOCK_METHOD(types::ErrorCode, SetFormat,
              (types::SerialDataBits data_bits, types::SerialParity parity,
               types::SerialStopBits stop_bits),
              (override));
  MOCK_METHOD(types::ErrorCode, SetDuplexMode, (bool full_duplex), (override));
  MOCK_METHOD(types::ErrorCode, SetEchoSuppression, (bool enable), (override));
  MOCK_METHOD(std::size_t, Available, (), (const, override));
  MOCK_METHOD(std::size_t, ReadBlock,
              (std::uint8_t* buffer, std::size_t max_length), (override));
  MOCK_METHOD(std::size_t, WriteBlock, (const void* data, std::size_t length),
              (override));
  MOCK_METHOD(void, FlushTx, (), (override));
  MOCK_METHOD(void, ClearRx, (), (override));
};

}  // namespace test
}  // namespace hardware_pruefadapter

#endif  // MOCK_HARDWARE_H_