/**
 * @file serial_adapter.cpp
 * @brief Implementierung des SerialAdapters.
 */

#include "serial_adapter.h"

namespace hardware_pruefadapter {
namespace adapters {

SerialAdapter::SerialAdapter()
    : serial_(nullptr),
      is_connected_(false),
      is_full_duplex_(true),
      echo_suppression_(false),
      current_baudrate_(115200),
      current_config_(SERIAL_8N1),
      rx_pin_(255),
      tx_pin_(255) {}

SerialAdapter::~SerialAdapter() {
  if (serial_ != nullptr) {
    serial_->end();
    delete serial_;
    serial_ = nullptr;
  }
}

types::ErrorCode SerialAdapter::Initialize(types::Serial::Interface uart_num,
                                           std::uint8_t rx_pin,
                                           std::uint8_t tx_pin) {
  if (uart_num >= types::Serial::kInterfaceCount) {
    return types::ErrorCode::kErrorInvalidChannel;
  }

  if (serial_ != nullptr) {
    serial_->end();
    delete serial_;
  }

  // ESP32 HardwareSerial erwartet einen uint8_t (0, 1 oder 2)
  serial_ = new HardwareSerial(static_cast<std::uint8_t>(uart_num));

  if (rx_pin <= 46 && tx_pin <= 46) {
    this->rx_pin_ = rx_pin;
    this->tx_pin_ = tx_pin;
  } else {
    return types::ErrorCode::kErrorInvalidPinConfig;
  }

  serial_->begin(current_baudrate_, current_config_, rx_pin_, tx_pin_);
  this->is_connected_ = true;
  return types::ErrorCode::kSuccess;
}

bool SerialAdapter::IsConnected() const {
  return this->is_connected_ && (serial_ != nullptr);
}

types::ErrorCode SerialAdapter::SetBaudrate(std::uint32_t baudrate) {
  if (!IsConnected()) {
    return types::ErrorCode::kErrorInitializeFailure;
  }

  current_baudrate_ = baudrate;
  serial_->updateBaudRate(baudrate);
  return types::ErrorCode::kSuccess;
}

types::ErrorCode SerialAdapter::SetFormat(types::SerialDataBits data_bits,
                                          types::SerialParity parity,
                                          types::SerialStopBits stop_bits) {
  if (!IsConnected()) {
    return types::ErrorCode::kErrorInitializeFailure;
  }

  current_config_ = GetEspSerialConfig(data_bits, parity, stop_bits);
  serial_->begin(current_baudrate_, current_config_, rx_pin_, tx_pin_);
  return types::ErrorCode::kSuccess;
}

types::ErrorCode SerialAdapter::SetDuplexMode(bool full_duplex) {
  is_full_duplex_ = full_duplex;
  return types::ErrorCode::kSuccess;
}

std::size_t SerialAdapter::Available() const {
  if (!IsConnected()) return 0;
  return serial_->available();
}

types::ErrorCode SerialAdapter::SetEchoSuppression(bool enable) {
  echo_suppression_ = enable;
  return types::ErrorCode::kSuccess;
}

std::size_t SerialAdapter::ReadBlock(std::uint8_t* buffer,
                                     std::size_t max_length) {
  if (!IsConnected() || buffer == nullptr || max_length == 0) return 0;

  std::size_t bytes_available = serial_->available();
  if (bytes_available == 0) return 0;

  std::size_t bytes_to_read =
      (bytes_available < max_length) ? bytes_available : max_length;

  return serial_->readBytes(buffer, bytes_to_read);
}

std::size_t SerialAdapter::WriteBlock(const void* data, std::size_t length) {
  if (!IsConnected() || data == nullptr || length == 0) return 0;

  if (!is_full_duplex_) {
    ClearRx();
  }

  const uint8_t* byte_data = static_cast<const uint8_t*>(data);
  std::size_t written = serial_->write(byte_data, length);

  if (!is_full_duplex_ || echo_suppression_) {
    FlushTx();
  }

  if (echo_suppression_) {
    ClearRx();
  }

  return written;
}

void SerialAdapter::FlushTx() {
  if (IsConnected()) {
    serial_->flush();
  }
}

void SerialAdapter::ClearRx() {
  if (IsConnected()) {
    while (serial_->available() > 0) {
      serial_->read();
    }
  }
}

uint32_t SerialAdapter::GetEspSerialConfig(
    types::SerialDataBits data_bits, types::SerialParity parity,
    types::SerialStopBits stop_bits) const {
  if (data_bits == types::SerialDataBits::kEight) {
    switch (parity) {
      case types::SerialParity::kNone:
        return (stop_bits == types::SerialStopBits::kOne) ? SERIAL_8N1
                                                          : SERIAL_8N2;
      case types::SerialParity::kEven:
        return (stop_bits == types::SerialStopBits::kOne) ? SERIAL_8E1
                                                          : SERIAL_8E2;
      case types::SerialParity::kOdd:
        return (stop_bits == types::SerialStopBits::kOne) ? SERIAL_8O1
                                                          : SERIAL_8O2;
    }
  } else if (data_bits == types::SerialDataBits::kSeven) {
    switch (parity) {
      case types::SerialParity::kNone:
        return (stop_bits == types::SerialStopBits::kOne) ? SERIAL_7N1
                                                          : SERIAL_7N2;
      case types::SerialParity::kEven:
        return (stop_bits == types::SerialStopBits::kOne) ? SERIAL_7E1
                                                          : SERIAL_7E2;
      case types::SerialParity::kOdd:
        return (stop_bits == types::SerialStopBits::kOne) ? SERIAL_7O1
                                                          : SERIAL_7O2;
    }
  } else if (data_bits == types::SerialDataBits::kSix) {
    switch (parity) {
      case types::SerialParity::kNone:
        return (stop_bits == types::SerialStopBits::kOne) ? SERIAL_6N1
                                                          : SERIAL_6N2;
      case types::SerialParity::kEven:
        return (stop_bits == types::SerialStopBits::kOne) ? SERIAL_6E1
                                                          : SERIAL_6E2;
      case types::SerialParity::kOdd:
        return (stop_bits == types::SerialStopBits::kOne) ? SERIAL_6O1
                                                          : SERIAL_6O2;
    }
  } else if (data_bits == types::SerialDataBits::kFive) {
    switch (parity) {
      case types::SerialParity::kNone:
        return (stop_bits == types::SerialStopBits::kOne) ? SERIAL_5N1
                                                          : SERIAL_5N2;
      case types::SerialParity::kEven:
        return (stop_bits == types::SerialStopBits::kOne) ? SERIAL_5E1
                                                          : SERIAL_5E2;
      case types::SerialParity::kOdd:
        return (stop_bits == types::SerialStopBits::kOne) ? SERIAL_5O1
                                                          : SERIAL_5O2;
    }
  }
  return SERIAL_8N1;
}

}  // namespace adapters
}  // namespace hardware_pruefadapter