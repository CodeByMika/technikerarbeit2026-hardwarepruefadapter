/**
 * @file simulated_devices.h
 * @brief Implementierung von simulierten Hardware-Geräten für die PC-Umgebung.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-03-25
 * @version 2.1.0
 * @ingroup Platform
 */

#ifndef HARDWARE_PRUEFADAPTER_PLATFORM_SIMULATED_DEVICES_H_
#define HARDWARE_PRUEFADAPTER_PLATFORM_SIMULATED_DEVICES_H_

#include <cmath>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "../Interfaces/analog_to_digital_interface.h"
#include "../Interfaces/digital_to_analog_interface.h"
#include "../Interfaces/led_interface.h"
#include "../Logic/Services/logger_service.h"
#include "error_code.h"
#include "system_config.h"
#include "system_types.h"

namespace hardware_pruefadapter {
namespace platform {

struct SharedSimState {
  static inline std::uint16_t dac_voltage[4] = {0, 0, 0, 0};
  static inline bool digital_out_state[4] = {false, false, false, false};
  static inline std::uint16_t digital_out_ref_mv[4] = {
      config::kMaxSystemVoltageMv, config::kMaxSystemVoltageMv,
      config::kMaxSystemVoltageMv, config::kMaxSystemVoltageMv};

  static void Reset() {
    for (int i = 0; i < 4; i++) {
      dac_voltage[i] = 0;
      digital_out_state[i] = false;
      digital_out_ref_mv[i] = config::kMaxSystemVoltageMv;
    }
  }
};

class SimulatedAdc : public interfaces::IAnalogToDigitalConverter {
 public:
  types::ErrorCode Initialize(std::uint8_t address, std::uint16_t gain_mv,
                              std::uint8_t i2c_bus_nr) override {
    address_ = address;
    return types::ErrorCode::kSuccess;
  }

  bool IsConnected() const override { return true; }
  bool IsBusy() const override { return false; }
  bool SetMode(bool mode) override { return true; }
  bool SetGain(std::uint16_t gain_mv) override { return true; }

  types::ErrorCode SetDataRate(std::uint16_t sps) override {
    return types::ErrorCode::kSuccess;
  }

  types::ErrorCode SetDifferentialMode(std::uint8_t ch_low,
                                       std::uint8_t ch_high) override {
    return types::ErrorCode::kSuccess;
  }

  bool SetInternExtern(bool intern_extern) override { return true; }

  types::ErrorCode GetValue(std::uint8_t channel,
                            std::int16_t* raw_out) const override {
    uint32_t voltage_mv = 0;

    if (address_ == config::kAdcI2cAddressReverseOutAnalog) {
      // SystemController Mappt HW Channel: i=0->2, i=1->1, i=2->3, i=3->0
      std::uint8_t dac_hw_channel = 3 - channel;
      voltage_mv = SharedSimState::dac_voltage[dac_hw_channel] * 5;

    } else if (address_ == config::kAdcI2cAddressReverseOutDigital) {
      // SystemController Mappt HW Channel: i=0->2, i=1->1, i=2->3, i=3->0
      std::uint8_t logical_channel = 0;
      if (channel == 2)
        logical_channel = 0;
      else if (channel == 1)
        logical_channel = 1;
      else if (channel == 3)
        logical_channel = 2;
      else if (channel == 0)
        logical_channel = 3;

      voltage_mv = SharedSimState::digital_out_state[logical_channel]
                       ? SharedSimState::digital_out_ref_mv[logical_channel]
                       : 0;
    } else {
      // Dummy Werte für Analog In / Digital In (1V, 2V, 3V, 4V)
      voltage_mv = 1000 * (channel + 1);
    }

    *raw_out = static_cast<std::int16_t>(voltage_mv);
    return types::ErrorCode::kSuccess;
  }

  bool GetMode() const override { return true; }
  std::uint16_t GetGain() const override { return config::kGainADC; }
  bool GetInternExtern() const override { return false; }

  std::uint16_t ConvertToVoltage(std::int16_t raw) const override {
    return static_cast<std::uint16_t>(raw);
  }

  types::ErrorCode GetI2cAddress(std::uint8_t* address) const override {
    if (address != nullptr) *address = address_;
    return types::ErrorCode::kSuccess;
  }

  bool GetOnlineStatus() const override { return true; }

  std::uint8_t GetDifferentialMode() const override { return 0; }

 private:
  std::uint8_t address_ = 0;
};

class SimulatedDac : public interfaces::IDigitalToAnalogConverter {
 public:
  types::ErrorCode Initialize(std::uint8_t address, std::uint16_t vref_mv,
                              std::uint8_t i2c_bus_nr) override {
    return types::ErrorCode::kSuccess;
  }

  bool IsConnected() const override { return true; }
  bool SetMode(std::uint8_t mode) override { return true; }

  types::ErrorCode SetValue(std::uint8_t channel,
                            std::uint16_t voltage_mv) override {
    SharedSimState::dac_voltage[channel] = voltage_mv;

    // core::Logger::LogInfo("[SIM DAC] HW-Channel %d -> %d mV", channel,
    //                       voltage_mv);

    return types::ErrorCode::kSuccess;
  }

  types::ErrorCode SetVoltageReference(std::uint16_t vref_low_mv,
                                       std::uint16_t vref_high_mv) override {
    return types::ErrorCode::kSuccess;
  }

  types::ErrorCode GetValue(std::uint8_t channel,
                            std::uint16_t* out) const override {
    *out = SharedSimState::dac_voltage[channel];
    return types::ErrorCode::kSuccess;
  }

  types::ErrorCode GetMaxVoltage(std::uint16_t* out) const override {
    *out = config::kDacReferenceMv;
    return types::ErrorCode::kSuccess;
  }

  types::ErrorCode GetGain(std::uint16_t* out) const override {
    *out = 1;
    return types::ErrorCode::kSuccess;
  }

  std::uint8_t GetMode() const override { return 0; }

  std::uint16_t ConvertToVoltage(std::uint16_t raw) const override {
    return raw;
  }

  std::uint16_t ConvertToRaw(std::uint16_t val) const override { return val; }
};

class SimulatedLedDriver : public interfaces::ILedDriver {
 public:
  types::ErrorCode Initialize(std::uint8_t address, std::uint8_t config,
                              std::uint8_t chip_nr,
                              std::uint8_t i2c_bus_nr) override {
    chip_nr_ = chip_nr;
    core::Logger::LogInfo(
        "[SIM LED] Driver %d initialized (Address 0x%d, Bus %d)", chip_nr + 1,
        address, i2c_bus_nr);
    return types::ErrorCode::kSuccess;
  }

  types::ErrorCode SetColor(std::uint8_t channel, types::ColorName color,
                            types::AddressType address_type) override {
    // Ermittle den logischen Namen der LED aus der system_config.h Map
    std::string led_name = "Unbekannte LED";
    for (int i = 0; i < config::kLedCount; i++) {
      if (config::kLedMap[i].driver_index == chip_nr_ &&
          config::kLedMap[i].channel == channel) {
        if (i >= 0 && i <= 3)
          led_name = "Analog In " + std::to_string(i + 1);
        else if (i >= 4 && i <= 7)
          led_name = "Digital In " + std::to_string(i - 3);
        else if (i >= 8 && i <= 11)
          led_name = "Analog Out " + std::to_string(i - 7);
        else if (i >= 12 && i <= 15)
          led_name = "Digital Out " + std::to_string(i - 11);
        else if (i == 16)
          led_name = "System Status";
        break;
      }
    }

    core::Logger::LogInfo("[SIM LED] %s (Driver %d, Ch %d) auf %s gesetzt",
                          led_name.c_str(), chip_nr_ + 1, channel,
                          types::ColorNameToString(color).data());
    return types::ErrorCode::kSuccess;
  }

  types::ErrorCode TurnOff() override {
    core::Logger::LogInfo("[SIM LED] Alle LEDs auf Driver %d abgeschaltet",
                          chip_nr_ + 1);
    return types::ErrorCode::kSuccess;
  }

 private:
  std::uint8_t chip_nr_ = 0;
};

class SimulatedSerial : public interfaces::ISerialInterface {
 public:
  explicit SimulatedSerial(types::Serial::Interface uart_num)
      : uart_num_(uart_num), running_(true) {
    if (uart_num_ == types::Serial::kUart) {
      input_thread_ = std::thread([this]() {
        while (running_) {
          int c = std::cin.get();
          if (c != EOF && running_) {
            std::lock_guard<std::mutex> lock(rx_mutex_);
            rx_queue_.push(static_cast<uint8_t>(c));
          }
        }
      });
      input_thread_.detach();
    }
  }

  ~SimulatedSerial() override { running_ = false; }

  types::ErrorCode Initialize(types::Serial::Interface uart_num,
                              std::uint8_t rx_pin,
                              std::uint8_t tx_pin) override {
    is_connected_ = true;
    return types::ErrorCode::kSuccess;
  }

  bool IsConnected() const override { return is_connected_; }

  types::ErrorCode SetBaudrate(std::uint32_t baudrate) override {
    return types::ErrorCode::kSuccess;
  }

  types::ErrorCode SetFormat(types::SerialDataBits data_bits,
                             types::SerialParity parity,
                             types::SerialStopBits stop_bits) override {
    return types::ErrorCode::kSuccess;
  }

  types::ErrorCode SetDuplexMode(bool full_duplex) override {
    return types::ErrorCode::kSuccess;
  }

  types::ErrorCode SetEchoSuppression(bool enable) override {
    return types::ErrorCode::kSuccess;
  }

  std::size_t Available() const override {
    std::lock_guard<std::mutex> lock(rx_mutex_);
    return rx_queue_.size();
  }

  std::size_t ReadBlock(std::uint8_t* buffer, std::size_t max_length) override {
    std::lock_guard<std::mutex> lock(rx_mutex_);
    std::size_t count = 0;
    while (count < max_length && !rx_queue_.empty()) {
      buffer[count++] = rx_queue_.front();
      rx_queue_.pop();
    }
    return count;
  }

  std::size_t WriteBlock(const void* data, std::size_t length) override {
    std::string msg(static_cast<const char*>(data), length);
    std::cout << "\n\033[93m[SIM " << types::Serial::ToString(uart_num_)
              << " TX]\033[0m " << msg << std::endl;
    return length;
  }

  void FlushTx() override {}

  void ClearRx() override {
    std::lock_guard<std::mutex> lock(rx_mutex_);
    while (!rx_queue_.empty()) rx_queue_.pop();
  }

 private:
  types::Serial::Interface uart_num_;
  bool is_connected_ = false;
  bool running_ = false;

  std::queue<uint8_t> rx_queue_;
  mutable std::mutex rx_mutex_;
  std::thread input_thread_;
};

}  // namespace platform
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_PLATFORM_SIMULATED_DEVICES_H_