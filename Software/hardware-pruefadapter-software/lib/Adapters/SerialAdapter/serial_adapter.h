/**
 * @file serial_adapter.h
 * @brief Konkreter Adapter für serielle Kommunikation (UART, RS232, RS485).
 *
 * Nutzt die ESP32 HardwareSerial-Klasse.
 * Unterstützt Voll-Duplex und Halb-Duplex, Echounterdrückung kann aus und
 * angeschaltet werden.
 *
 * @author Techniker_Team_2026
 * @date 2026-03-21
 * @version 1.0.0
 * @ingroup Adapters
 * @copyright Copyright (c) 2026 SOTEC GmBH & Co KG
 */
#ifndef HARDWARE_PRUEFADAPTER_ADAPTERS_SERIAL_ADAPTER_H_
#define HARDWARE_PRUEFADAPTER_ADAPTERS_SERIAL_ADAPTER_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <HardwareSerial.h>
#include "../../Interfaces/serial_interface.h"

namespace hardware_pruefadapter {
namespace adapters {

class SerialAdapter : public interfaces::ISerialInterface {
 public:
  SerialAdapter();
  ~SerialAdapter() override;

  types::ErrorCode Initialize(types::Serial::Interface uart_num, std::uint8_t rx_pin,
                              std::uint8_t tx_pin) override;
  bool IsConnected() const override;

  types::ErrorCode SetBaudrate(std::uint32_t baudrate) override;
  types::ErrorCode SetFormat(types::SerialDataBits data_bits,
                             types::SerialParity parity,
                             types::SerialStopBits stop_bits) override;
  types::ErrorCode SetDuplexMode(bool full_duplex) override;
  types::ErrorCode SetEchoSuppression(bool enable) override;

  std::size_t Available() const override;
  std::size_t ReadBlock(std::uint8_t* buffer, std::size_t max_length) override;
  std::size_t WriteBlock(const void* data, std::size_t length) override;

  void FlushTx() override;
  void ClearRx() override;

 private:
  uint32_t GetEspSerialConfig(types::SerialDataBits data_bits,
                              types::SerialParity parity,
                              types::SerialStopBits stop_bits) const;

  HardwareSerial* serial_;
  bool is_connected_;
  bool is_full_duplex_;
  bool echo_suppression_;

  std::uint32_t current_baudrate_;
  uint32_t current_config_;
  std::uint8_t rx_pin_;
  std::uint8_t tx_pin_;
};

}  // namespace adapters
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_ADAPTERS_SERIAL_ADAPTER_H_