/**
 * @file led_manager_service.cpp
 * @brief Implementierung des LED Managers.
 */

#include "led_manager_service.h"

#include "logger_service.h"

namespace hardware_pruefadapter {
namespace logic {

LedManager::LedManager(const core::SystemContext& ctx) : ctx_(ctx) {}

void LedManager::SetHealthMask(std::uint8_t led_init_success_mask) {
  health_mask_ = led_init_success_mask;
}

void LedManager::UpdateHardwareLeds() {
  for (uint8_t j = 0; j < types::IO::kIoGroupCount; j++) {
    for (uint8_t i = 0; i < config::kNumChannels; i++) {
      std::uint32_t bit_pos = i + (j * types::IO::kIoGroupCount);

      if (ctx_.logic.process_imager->HasChangeFlagMask(1UL << bit_pos)) {
        uint8_t driver_idx = config::kLedMap[bit_pos].driver_index;
        uint8_t driver_ch = config::kLedMap[bit_pos].channel;

        if (ctx_.ui.led_drivers[driver_idx] != nullptr &&
            (health_mask_ & (1 << driver_idx)) != 0) {
          bool is_digital =
              (j == types::IO::kDigitalIn || j == types::IO::kDigitalOut);
          std::int8_t state = 0;

          if (is_digital) {
            if (j == types::IO::kDigitalOut) {
              state = ctx_.logic.process_imager->GetVoltage(types::IO::Group(j),
                                                            i) > 0
                          ? 1
                          : 0;
            } else {
              state = ctx_.logic.process_imager->GetDigitalState(
                  types::IO::Group(j), i);
            }
          }

          types::ColorName color = utils::data_converter::DetermineIoLedColor(
              ctx_.logic.process_imager->GetVoltage(types::IO::Group(j), i),
              state, is_digital);

          types::ErrorCode status =
              ctx_.ui.led_drivers[driver_idx]->SetColor(driver_ch, color);

          if (status != types::ErrorCode::kSuccess) {
            ctx_.logic.error_manager->AddCycleError(
                "LED-Treiber " + std::to_string(driver_idx) + "Ch " +
                    std::to_string(driver_ch) + " [SetColor]",
                status);
          }
          // else {
          //   types::RgbColor rgb = types::GetRgbFromColorName(color);
          //   LOG("[LED] Treiber %d Ch %d wurde auf %s gesetzt. R:%d G:%dB:%d",
          //       driver_idx, driver_ch,
          //       types::ColorNameToString(color).data(), rgb.red, rgb.green,
          //       rgb.blue);
          // }
        }
      }
    }
  }
}

void LedManager::UpdateSystemLed() {
  if (ctx_.logic.process_imager->HasChangeFlagMask(types::kLedSystem)) {
    uint8_t driver_idx = config::kLedMap[types::kSystem].driver_index;
    uint8_t driver_ch = config::kLedMap[types::kSystem].channel;

    if (ctx_.ui.led_drivers[driver_idx] != nullptr &&
        (health_mask_ & (1 << driver_idx)) != 0) {
      types::ColorName color = (ctx_.logic.error_manager->GetErrorCount() > 0)
                                   ? types::ColorName::kRed
                                   : types::ColorName::kGreen;
      types::ErrorCode led_status =
          ctx_.ui.led_drivers[driver_idx]->SetColor(driver_ch, color);

      if (led_status != types::ErrorCode::kSuccess) {
        ctx_.logic.error_manager->AddCycleError(
            "LED-Treiber " + std::to_string(driver_idx + 1) + "Ch " +
                std::to_string(driver_ch + 1) + " [SetColor]",
            led_status);
      } else {
        LOG("[LED] Treiber %d Ch %d wurde auf %s gesetzt.", driver_idx,
            driver_ch, types::ColorNameToString(color).data());
      }
    }
  }
}

void LedManager::TurnAllLedsOff() {
  for (int i = 0; i < config::kLedCount; i++) {
    uint8_t driver_idx = config::kLedMap[i].driver_index;
    uint8_t driver_ch = config::kLedMap[i].channel;

    if (ctx_.ui.led_drivers[driver_idx] != nullptr &&
        (health_mask_ & (1 << driver_idx)) != 0) {
      ctx_.ui.led_drivers[driver_idx]->SetColor(driver_ch,
                                                types::ColorName::kBlack);
    }
  }
  LOG_INFO("[LED] Alle LEDs wurden abgeschaltet.");
}

void LedManager::AllLedsOn(types::ColorName color) {
  for (int i = 0; i < config::kLedCount; i++) {
    uint8_t driver_idx = config::kLedMap[i].driver_index;
    uint8_t driver_ch = config::kLedMap[i].channel;

    if (ctx_.ui.led_drivers[driver_idx] != nullptr &&
        (health_mask_ & (1 << driver_idx)) != 0) {
      ctx_.ui.led_drivers[driver_idx]->SetColor(driver_ch, color);
    }
  }
  LOG_INFO("[LED] Alle LEDs wurden auf %s gesetzt.",
           types::ColorNameToString(color).data());
}

}  // namespace logic
}  // namespace hardware_pruefadapter