/**
 * @file serial_interface.h
 * @brief Abstraktes Interface für die serielle Kommunikation.
 *
 * Kapselt UART, RS232 und RS485 Schnittstellen. Definiert Methoden zur
 * blockbasierten Datenübertragung und Hardwarekonfiguration (Baudrate,
 * Parität).
 *
 * @author Techniker_Team_2025_26
 * @date 2026-03-25
 * @version 1.0.0
 * @ingroup Interfaces
 * @copyright Copyright (c) 2026 SOTEC GmBH & Co KG
 */
#ifndef HARDWARE_PRUEFADAPTER_INTERFACES_SERIAL_INTERFACE_H_
#define HARDWARE_PRUEFADAPTER_INTERFACES_SERIAL_INTERFACE_H_

#include <cstddef>
#include <cstdint>

#include "error_code.h"
#include "system_types.h"

namespace hardware_pruefadapter {
namespace interfaces {

/**
 * @class ISerialInterface
 * @brief Schnittstellendefinition für serielle Kommunikationstreiber.
 */
class ISerialInterface {
 public:
  virtual ~ISerialInterface() = default;

  /**
   * @brief Initialisiert die serielle Schnittstelle auf Hardware-Ebene.
   * @param uart_num Hardware-UART-Nummer (z.B. 0, 1, 2 beim ESP32).
   * @param rx_pin GPIO-Pin für den Empfang.
   * @param tx_pin GPIO-Pin für das Senden.
   * @return kSuccess bei Erfolg.
   */
  virtual types::ErrorCode Initialize(types::Serial::Interface uart_num,
                                      std::uint8_t rx_pin,
                                      std::uint8_t tx_pin) = 0;

  virtual bool IsConnected() const = 0;

  virtual types::ErrorCode SetBaudrate(std::uint32_t baudrate) = 0;

  /**
   * @brief Konfiguriert das Datenformat (z.B. 8N1).
   * @param[in] data_bits Anzahl der Datenbits.
   * @param[in] parity Paritätsprüfung.
   * @param[in] stop_bits Anzahl der Stoppbits.
   * @return types::ErrorCode kSuccess bei Erfolg.
   */
  virtual types::ErrorCode SetFormat(types::SerialDataBits data_bits,
                                     types::SerialParity parity,
                                     types::SerialStopBits stop_bits) = 0;

  /**
   * @brief Schaltet zwischen Voll-Duplex und Halb-Duplex um.
   * Bei Halb-Duplex wird der Empfänger während des Sendens hardwareseitig
   * blockiert.
   */
  virtual types::ErrorCode SetDuplexMode(bool full_duplex) = 0;

  /**
   * @brief Aktiviert oder deaktiviert die Unterdrückung des lokalen Echos.
   * Nützlich für RS485-Transceiver, die das TX-Signal auf den RX-Pin spiegeln.
   */
  virtual types::ErrorCode SetEchoSuppression(bool enable) = 0;

  /**
   * @brief Gibt die Anzahl der im RX-Puffer verfügbaren Bytes zurück.
   */
  virtual std::size_t Available() const = 0;

  /**
   * @brief Liest einen Block von Daten aus dem Empfangspuffer.
   * @param buffer Speicherbereich für die Daten.
   * @param max_length Maximale Anzahl an Bytes, die gelesen werden dürfen.
   * @return Die tatsächlich gelesene Anzahl an Bytes.
   */
  virtual std::size_t ReadBlock(std::uint8_t* buffer,
                                std::size_t max_length) = 0;

  /**
   * @brief Schreibt einen Block von Daten in den Sendepuffer (TX).
   * @param data Array mit den zu sendenden Bytes.
   * @param length Anzahl der zu sendenden Bytes.
   * @return Die tatsächlich in den Puffer geschriebene Anzahl an Bytes.
   */
  virtual std::size_t WriteBlock(const void* data, std::size_t length) = 0;

  /**
   * @brief Wartet blockierend, bis alle Daten im TX-Puffer gesendet wurden.
   */
  virtual void FlushTx() = 0;

  /**
   * @brief Verwirft alle ungelesenen Daten im RX-Puffer.
   */
  virtual void ClearRx() = 0;
};

}  // namespace interfaces
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_INTERFACES_SERIAL_INTERFACE_H_