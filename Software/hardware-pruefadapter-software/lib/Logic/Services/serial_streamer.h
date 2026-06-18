/**
 * @file serial_streamer.h
 * @brief Service zur Verwaltung des asynchronen RX/TX Datenstroms.
 *
 * Kapselt das Puffer-Management, Timeouts und die Event-Auslösung
 * für alle seriellen Hardware-Schnittstellen.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-03-26
 * @version 1.0.0
 * @ingroup Services
 */
#ifndef HARDWARE_PRUEFADAPTER_SERVICES_SERIAL_STREAMER_H_
#define HARDWARE_PRUEFADAPTER_SERVICES_SERIAL_STREAMER_H_

#include <cstdint>
#include <functional>
#include <string>

#include "../../Interfaces/analog_to_digital_interface.h"
#include "../../Interfaces/digital_to_analog_interface.h"
#include "../../Interfaces/led_interface.h"
#include "../../Interfaces/serial_interface.h"
#include "../../Interfaces/web_server_interface.h"
#include "diagnostics_service.h"
#include "logger_service.h"
#include "system_config.h"
#include "system_context.h"
#include "system_types.h"

namespace hardware_pruefadapter {
namespace logic {

using SerialRxCallback =
    std::function<void(types::Serial::Interface uart_num,
                       const std::uint8_t* data, std::size_t length)>;

/**
 * @class SerialStreamer
 * @brief Zuständig für das Lesen (RX) und Schreiben (TX) von seriellen Daten.
 */
class SerialStreamer {
 public:
  explicit SerialStreamer(const core::SystemContext& ctx);
  ~SerialStreamer() = default;

  /** @brief Teilt dem Streamer mit, welche Ports fehlerfrei gestartet sind. */
  void SetHealthMask(std::uint8_t serial_init_success_mask);

  /** @brief Registriert den Callback für vollständig empfangene RX-Pakete. */
  void SetRxCallback(SerialRxCallback cb);

  /** @brief Zyklischer RX-Handler: Liest Bytes und wertet Timeouts aus. */
  void ProcessReadRx();

  /** @brief TX-Handler: Sendet einen String über den gewählten Port. */
  void WriteTx(types::Serial::Interface uart_num, const std::string& payload);

  /** @brief Ändert die Hardware-Baudrate zur Laufzeit. */
  void SetBaudrate(types::Serial::Interface uart_num, std::uint32_t baudrate);

 private:
  const core::SystemContext& ctx_;
  std::uint8_t health_mask_ = 0;

  SerialRxCallback on_serial_rx_callback_ = nullptr;
  types::SerialRxBuffer<config::kSerialBufferSize>
      rx_buffers_[config::kSerialDriverCount];
};

}  // namespace logic
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_SERVICES_SERIAL_STREAMER_H_