/**
 * @file serial_streamer.cpp
 * @brief Implementierung des Serial Streamers.
 */

#include "serial_streamer.h"

namespace hardware_pruefadapter {
namespace logic {

SerialStreamer::SerialStreamer(const core::SystemContext& ctx) : ctx_(ctx) {}

void SerialStreamer::SetHealthMask(std::uint8_t serial_init_success_mask) {
  health_mask_ = serial_init_success_mask;
}

void SerialStreamer::SetRxCallback(SerialRxCallback cb) {
  on_serial_rx_callback_ = cb;
}

void SerialStreamer::WriteTx(types::Serial::Interface uart_num,
                             const std::string& payload) {
  if (uart_num >= types::Serial::kInterfaceCount || payload.empty()) return;

  if (ctx_.communication.serial[uart_num] != nullptr &&
      (health_mask_ & (1 << uart_num))) {
    ctx_.communication.serial[uart_num]->WriteBlock(payload.data(),
                                                    payload.size());
  }
}

void SerialStreamer::SetBaudrate(types::Serial::Interface uart_num,
                                 std::uint32_t baudrate) {
  if (uart_num >= types::Serial::kInterfaceCount ||
      baudrate > config::kMaxSerialBaudRate)
    return;

  if (ctx_.communication.serial[uart_num] != nullptr &&
      (health_mask_ & (1 << uart_num))) {
    ctx_.communication.serial[uart_num]->SetBaudrate(baudrate);
    LOG_INFO("[SYSTEM] Baudrate von %s auf %d geaendert.",
             types::Serial::ToString(uart_num), baudrate);
  }
}

void SerialStreamer::ProcessReadRx() {
  for (std::uint8_t i = 0; i < config::kSerialDriverCount; i++) {
    if (ctx_.communication.serial[i] != nullptr && (health_mask_ & (1 << i))) {
      std::size_t available = ctx_.communication.serial[i]->Available();

      if (available > 0) {
        std::size_t space_left =
            sizeof(rx_buffers_[i].data) - rx_buffers_[i].length;
        std::size_t to_read = (available < space_left) ? available : space_left;

        if (to_read > 0) {
          std::size_t bytes_read = ctx_.communication.serial[i]->ReadBlock(
              &rx_buffers_[i].data[rx_buffers_[i].length], to_read);

          rx_buffers_[i].length += bytes_read;
          rx_buffers_[i].last_rx_time = DiagnosticsService::GetSystemUptimeMs();
        }
      }

      if (rx_buffers_[i].length > 0) {
        bool buffer_almost_full =
            (rx_buffers_[i].length >=
             (config::kSerialBufferSize - config::kSerialAlmostFullBuffer));
        bool timeout_reached =
            ((DiagnosticsService::GetSystemUptimeMs() -
              rx_buffers_[i].last_rx_time) > config::kSerialAlmostFullBuffer);

        if (buffer_almost_full || timeout_reached) {
          if (on_serial_rx_callback_) {
            on_serial_rx_callback_(static_cast<types::Serial::Interface>(i),
                                   rx_buffers_[i].data, rx_buffers_[i].length);
          }

          for (uint8_t j = 0; j < rx_buffers_[i].length; j++) {
            rx_buffers_[i].data[j] = 0;
          }
          rx_buffers_[i].length = 0;
        }
      }
    }
  }
}

}  // namespace logic
}  // namespace hardware_pruefadapter