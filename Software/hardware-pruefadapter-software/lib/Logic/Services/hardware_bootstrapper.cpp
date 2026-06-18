/**
 * @file hardware_bootstrapper.cpp
 * @brief Implementierung des Hardware-Bootstrappers.
 */

#include "hardware_bootstrapper.h"

namespace hardware_pruefadapter {
namespace logic {

SystemHealth HardwareBootstrapper::Boot(const core::SystemContext& ctx,
                                        ErrorManager& error_manager) {
  SystemHealth health;
  types::ErrorCode status;

  health = AdcInit(ctx, error_manager, health);

  health = DacInit(ctx, error_manager, health);

  health = LedInit(ctx, error_manager, health);

  health = SerialInit(ctx, error_manager, health);

  health = HealthCheck(health);

  return health;
}

SystemHealth HardwareBootstrapper::AdcInit(const core::SystemContext& ctx,
                         ErrorManager& error_manager, SystemHealth health) {
  types::ErrorCode status;
  for (std::uint8_t i = 0; i < config::kAdcCount; i++) {
    if (ctx.converter.adc[i] != nullptr) {
      if (config::kDebugMode)
        LOG_INFO("ADC Chip %d im Context gefunden.", i + 1);

      uint8_t i2c_address = 0;
      if (i == 0)
        i2c_address = config::kAdcI2cAddressAnalogIn;
      else if (i == 1)
        i2c_address = config::kAdcI2cAddressDigitalIn;
      else if (i == 2)
        i2c_address = config::kAdcI2cAddressReverseOutAnalog;
      else if (i == 3)
        i2c_address = config::kAdcI2cAddressReverseOutDigital;

      status = ctx.converter.adc[i]->Initialize(i2c_address, config::kGainADC,
                                                config::kI2cBus1);
      if (status != types::ErrorCode::kSuccess) {
        error_manager.AddCycleError("ADC " + std::to_string(i + 1) + " [Init]",
                                    status);
      } else {
        LOG_INFO("ADC Chip %d Initialisierung erfolgreich.", i + 1);
        health.adc_init_success |= 1 << i;
      }
    } else {
      LOG_WARNING("ADC Chip %d fehlt im Context!", i + 1);
      error_manager.AddCycleError("ADC " + std::to_string(i + 1) + " [Init]",
                                  types::ErrorCode::kErrorNullptrFailure);
    }
  }

  return health;
}

SystemHealth HardwareBootstrapper::DacInit(const core::SystemContext& ctx,
                         ErrorManager& error_manager, SystemHealth health) {
  types::ErrorCode status;

  for (std::uint8_t i = 0; i < config::kDacCount; i++) {
    if (ctx.converter.dac[i] != nullptr) {
      if (config::kDebugMode)
        LOG_INFO("DAC Digital-Out %d im Context gefunden.", i + 1);

      uint8_t i2c_address = 0;
      if (i == 0) i2c_address = config::kDacI2cAddressAnalogOut;

      status = ctx.converter.dac[i]->Initialize(
          i2c_address, config::kDacReferenceMv, config::kI2cBus0);

      if (status != types::ErrorCode::kSuccess) {
        error_manager.AddCycleError("DAC " + std::to_string(i + 1) + " [Init]",
                                    status);
      } else {
        LOG_INFO("DAC %d Initialisierung erfolgreich.", i + 1);
        health.dac_init_success |= 1 << i;
      }
    } else {
      LOG_ERROR("DAC %d fehlt im Context!", i + 1);
      error_manager.AddCycleError("DAC " + std::to_string(i + 1) + " [Init]",
                                  types::ErrorCode::kErrorNullptrFailure);
    }
  }

  return health;
}

SystemHealth HardwareBootstrapper::LedInit(const core::SystemContext& ctx,
                         ErrorManager& error_manager, SystemHealth health) {
  types::ErrorCode status;

  for (std::uint8_t i = 0; i < config::kLedDriverCount; i++) {
    if (ctx.ui.led_drivers[i] != nullptr) {
      uint8_t i2c_address = 0xFF;
      if (i == 0)
        i2c_address = config::kLedDriver1I2cAddress;
      else if (i == 1)
        i2c_address = config::kLedDriver2I2cAddress;
      else if (i == 2)
        i2c_address = config::kLedDriver3I2cAddress;

      status = ctx.ui.led_drivers[i]->Initialize(
          i2c_address, config::kInitialLedConfig, i, config::kI2cBus0);

      if (status != types::ErrorCode::kSuccess) {
        error_manager.AddCycleError(
            "LED-Treiber " + std::to_string(i + 1) + " [Init]", status);
      } else {
        LOG_INFO("LED Driver %d initialisiert.", i + 1);
        health.led_init_success |= 1 << i;
      }
    } else {
      LOG_WARNING("LED Driver %d fehlt im Context!", i + 1);
      error_manager.AddCycleError(
          "LED-Treiber " + std::to_string(i + 1) + " [Init]",
          types::ErrorCode::kErrorNullptrFailure);
    }
  }

  return health;
}

SystemHealth HardwareBootstrapper::SerialInit(const core::SystemContext& ctx,
                            ErrorManager& error_manager, SystemHealth health) {
  types::ErrorCode status;

  for (std::uint8_t i = 0; i < config::kSerialDriverCount; i++) {
    if (ctx.communication.serial[i] != nullptr) {
      types::Serial::Interface iface = static_cast<types::Serial::Interface>(i);

      if (config::kDebugMode)
        LOG_INFO("Serial Interface %s im Context gefunden.",
                 types::Serial::ToString(iface));

      std::uint8_t rx_pin = 0, tx_pin = 0;
      bool is_full_duplex = true;

      if (iface == types::Serial::kUart) {
        rx_pin = config::kGPIOPinRxUART0;
        tx_pin = config::kGPIOPinTxUART0;
        is_full_duplex = true;
        status = types::ErrorCode::kSuccess;
      } else if (iface == types::Serial::kRs485) {
        rx_pin = config::kGPIOPinRxUART1;
        tx_pin = config::kGPIOPinTxUART1;
        is_full_duplex = false;
        status = types::ErrorCode::kSuccess;
      } else if (iface == types::Serial::kRs232) {
        rx_pin = config::kGPIOPinRxUART2;
        tx_pin = config::kGPIOPinTxUART2;
        is_full_duplex = true;
        status = types::ErrorCode::kSuccess;
      } else {
        status = types::ErrorCode::kErrorInitializeFailure;
      }

      if (status == types::ErrorCode::kSuccess) {
        status = ctx.communication.serial[i]->Initialize(iface, rx_pin, tx_pin);

        if (status == types::ErrorCode::kSuccess) {
          ctx.communication.serial[i]->SetBaudrate(config::kSerialBaudRate);
          ctx.communication.serial[i]->SetFormat(types::SerialDataBits::kEight,
                                                 types::SerialParity::kNone,
                                                 types::SerialStopBits::kOne);
          ctx.communication.serial[i]->SetDuplexMode(is_full_duplex);

          LOG_INFO("Serial Interface %s erfolgreich initialisiert.",
                   types::Serial::ToString(iface));
          health.serial_init_success |= (1 << i);
        } else {
          error_manager.AddCycleError(std::string("Serial ") +
                                          types::Serial::ToString(iface) +
                                          " [Init]",
                                      status);
        }
      }
    } else {
      LOG_WARNING("Serial Interface %d fehlt im Context!", i);
    }
  }

  return health;
}

SystemHealth HardwareBootstrapper::HealthCheck(SystemHealth health) {
  bool system_healthy = true;
  if (health.adc_init_success != config::kExpectedAdcInitMask) {
    LOG_WARNING("Nicht alle konfigurierten ADC-Kanäle initialisiert!");
    system_healthy = false;
  }
  if (health.dac_init_success != config::kExpectedDacInitMask) {
    LOG_WARNING("Nicht alle konfigurierten DAC-Kanäle initialisiert!");
    system_healthy = false;
  }
  if (health.led_init_success != config::kExpectedLedInitMask) {
    LOG_WARNING("Nicht alle konfigurierten LED-Treiber initialisiert!");
    system_healthy = false;
  }
  if (health.serial_init_success != config::kExpectedSerialInitMask) {
    LOG_WARNING("Nicht alle konfigurierten Serial-Treiber initialisiert!");
    system_healthy = false;
  }

  health.is_healthy = system_healthy;
  if (system_healthy) {
    LOG("[SYSTEM] Initialisierung erfolgreich.\n\n");
  } else {
    LOG("[SYSTEM] Initialisierung mit WARNUNGEN abgeschlossen (Eingeschränkter "
        "Modus).\n\n");
  }

  return health;
}

}  // namespace logic
}  // namespace hardware_pruefadapter