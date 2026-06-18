/**
 * @file diagnostics_service.cpp
 * @brief Implementierung des Diagnose-Services.
 */

#include "diagnostics_service.h"

namespace hardware_pruefadapter {
namespace logic {

types::I2cDeviceList<config::kMaxI2cDevices>
DiagnosticsService::GetConfiguredI2cDevices() {
  types::I2cDeviceList<config::kMaxI2cDevices> list;

  list.devices[list.count++] = {
      "0x" + std::to_string(config::kAdcI2cAddressAnalogIn), config::kI2cBus1,
      "ADC Analog In"};
  list.devices[list.count++] = {
      "0x" + std::to_string(config::kAdcI2cAddressDigitalIn), config::kI2cBus1,
      "ADC Digital In"};
  list.devices[list.count++] = {
      "0x" + std::to_string(config::kAdcI2cAddressReverseOutAnalog),
      config::kI2cBus1, "ADC Analog Out (Readback)"};
  list.devices[list.count++] = {
      "0x" + std::to_string(config::kAdcI2cAddressReverseOutDigital),
      config::kI2cBus1, "ADC Digital Out (Readback)"};
  list.devices[list.count++] = {
      "0x" + std::to_string(config::kDacI2cAddressAnalogOut), config::kI2cBus1,
      "DAC Analog Out"};
  list.devices[list.count++] = {
      "0x" + std::to_string(config::kLedDriver1I2cAddress), config::kI2cBus1,
      "LP50xx LED Treiber 1"};
  list.devices[list.count++] = {
      "0x" + std::to_string(config::kLedDriver2I2cAddress), config::kI2cBus1,
      "LP50xx LED Treiber 2"};
  list.devices[list.count++] = {
      "0x" + std::to_string(config::kLedDriver3I2cAddress), config::kI2cBus1,
      "LP50xx LED Treiber 3"};

  return list;
}

std::uint32_t DiagnosticsService::GetSystemUptimeMs() {
#ifdef ESP32_ENV
  return millis();
#else
  using namespace std::chrono;
  static auto start = steady_clock::now();
  return duration_cast<milliseconds>(steady_clock::now() - start).count();
#endif
}

void DiagnosticsService::LogSystemInfo() {
#ifdef ESP32_ENV
  uint8_t core = ESP.getChipCores();
  const char* model = ESP.getChipModel();
  uint8_t revision = ESP.getChipRevision();

  LOG("========== ESP32-S3 ==========");
  LOG("ESP32 Model: %s (Rev %d)", model, revision);
  LOG("ESP32 Core Anzahl: %d", core);

  char ver_buf[64];
  snprintf(ver_buf, sizeof(ver_buf), "ESP32 Core Version: %d.%d.%d",
           ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR,
           ESP_ARDUINO_VERSION_PATCH);
  LOG("%s", ver_buf);

  uint32_t flash_size = ESP.getFlashChipSize();
  LOG("Flash Größe: %u Bytes (%.2f MB)", flash_size,
      flash_size / (1024.0 * 1024.0));

  uint32_t psram_size = ESP.getPsramSize();
  LOG("PSRAM Größe: %u Bytes (%.2f MB)", psram_size,
      psram_size / (1024.0 * 1024.0));
  LOG("==============================\n\n");
#else
  LOG("========== SIMULATION ==========");
  LOG("Platform: Native (WSL/Linux/Windows)");
  LOG("C++ Standard: %ld", __cplusplus);
  LOG("================================\n\n");
#endif
}

types::SystemStatusDto DiagnosticsService::BuildSystemStatusDto(
    const ErrorManager& error_manager) {
  types::SystemStatusDto dto;

  dto.uptime_ms = GetSystemUptimeMs();
  dto.heap_free = GetSystemFreeHeap();

#ifdef NATIVE_ENV
  dto.status = "simulation";
#else
  dto.status = "running";
#endif

  // ErrorManager füllt Thread-sicher die Fehler ein
  error_manager.FillErrorStatus(dto);

  return dto;
}

std::uint32_t DiagnosticsService::GetSystemFreeHeap() {
#ifdef ESP32_ENV
  return ESP.getFreeHeap();
#else
  return 999999;  // Mock für den PC
#endif
}

}  // namespace logic
}  // namespace hardware_pruefadapter