#ifdef NATIVE_ENV

#include <chrono>
#include <iostream>
#include <thread>

#include "../Test/NativeWebServerAdapter/native_web_server_adapter.h"
#include "platform_factory.h"
#include "simulated_devices.h"
#include "system_config.h"

namespace hardware_pruefadapter {
namespace platform {

void Init() {
  std::cout << "[PLATFORM] NATIVE Environment (Simulation) gestartet." << std::endl;
}

void NetworkSetup() {
  std::cout << "[PLATFORM] Simulation Network ready at localhost." << std::endl;
}

std::unique_ptr<interfaces::IWebServer> CreateWebServer() {
  return std::make_unique<adapters::NativeWebServerAdapter>(config::kWebServerPort);
}

std::unique_ptr<interfaces::IAnalogToDigitalConverter> CreateAdc() {
  return std::make_unique<SimulatedAdc>();
}

std::unique_ptr<interfaces::IDigitalToAnalogConverter> CreateDac() {
  return std::make_unique<SimulatedDac>();
}

std::unique_ptr<interfaces::ILedDriver> CreateLedDriver() { 
  return std::make_unique<SimulatedLedDriver>(); 
}

std::unique_ptr<interfaces::ISerialInterface> CreateSerial(types::Serial::Interface uart_num) {
  return std::make_unique<SimulatedSerial>(uart_num);
}

void SetDigitalOutPin(std::uint8_t channel, bool state, std::uint16_t ref_mv) {
  SharedSimState::digital_out_state[channel] = state;
}

void SetSimulatedReferenceVoltage(types::IO::Direction direction, std::uint8_t channel,
                                  std::uint16_t voltage_mv) {
  // 1 = Output
  if (direction == types::IO::kOutput) {
    SharedSimState::digital_out_ref_mv[channel] = voltage_mv;
  }
}

}  // namespace platform
}  // namespace hardware_pruefadapter

#endif