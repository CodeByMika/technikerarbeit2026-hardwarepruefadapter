/**
 * @file lp50xx_adapter.h
 * @brief Konkreter Adapter für den LP50xx LED-Treiber.
 */
#ifndef HARDWARE_PRUEFADAPTER_ADAPTERS_LP50XX_ADAPTER_H_
#define HARDWARE_PRUEFADAPTER_ADAPTERS_LP50XX_ADAPTER_H_

#include <LP50XX.h>

#include <cstdint>

#include "../Interfaces/led_interface.h"
#include "color_types.h"
#include "error_code.h"

namespace hardware_pruefadapter {
namespace adapters {

class Lp50xxAdapter : public interfaces::ILedDriver {
 public:
  Lp50xxAdapter();
  ~Lp50xxAdapter() override = default;

  types::ErrorCode Initialize(std::uint8_t address, std::uint8_t config,
                              std::uint8_t chip_nr,
                              std::uint8_t i2c_bus_nr) override;
  types::ErrorCode SetColor(std::uint8_t channel, types::ColorName color,
                            types::AddressType address_type) override;
  types::ErrorCode TurnOff() override;

 private:
  LP50XX led_driver_;
  std::uint8_t i2c_device_address_;
  std::uint8_t chip_nr_;
};

}  // namespace adapters
}  // namespace hardware_pruefadapter
#endif  // HARDWARE_PRUEFADAPTER_ADAPTERS_LP50XX_ADAPTER_H_