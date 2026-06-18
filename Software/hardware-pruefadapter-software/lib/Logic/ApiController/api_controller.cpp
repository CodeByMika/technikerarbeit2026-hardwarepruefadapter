/**
 * @file api_controller.cpp
 * @brief Implementierung der API-Logik.
 */

#include "api_controller.h"

#include <ArduinoJson.h>

#include "../Services/error_manager.h"
#include "../Services/mailbox_service.h"
#include "../Services/process_imager_service.h"
#include "../Services/serial_streamer.h"
#include "../Web/web_page_index.h"

namespace hardware_pruefadapter {
namespace logic {

ApiController::ApiController(interfaces::IWebServer* web_server,
                             const core::SystemContext& ctx)
    : web_server_(web_server), ctx_(ctx) {}

ApiController::~ApiController() { core::Logger::SetOnLogCallback(nullptr); }

void ApiController::RegisterRoutes() {
  if (web_server_ == nullptr) {
    return;
  }

  web_server_->RegisterStaticRoute("/", core::kIndexHtml, "text/html");

  web_server_->RegisterGetRoute(
      "/api/v1/system/status",
      [this]() -> std::string { return HandleGetSystemStatus(); });

  web_server_->RegisterGetRoute("/api/v1/system/logs", [this]() -> std::string {
    return HandleGetLogs();
  });

  web_server_->RegisterSseRoute("/api/v1/system/logs/stream");

  core::Logger::SetOnLogCallback([this](const std::string& log_line) {
    if (web_server_) web_server_->SendSseEvent("log", log_line.c_str());
  });

  web_server_->RegisterGetRoute("/api/v1/io",
                                [this]() { return HandleGetIoAll(); });
  web_server_->RegisterGetRoute("/api/v1/i2c/devices",
                                [this]() { return HandleGetI2cDevices(); });

  for (std::uint8_t i = 0; i < config::kNumChannels; i++) {
    std::string path_digital_ref_in =
        "/api/v1/io/digital/input/" + std::to_string(i) + "/reference";
    web_server_->RegisterPutRoute(
        path_digital_ref_in.c_str(), [this, i](const std::string& body) {
          return HandlePutDigitalReference(i, types::IO::kInput, body);
        });

    std::string path_digital_ref_out =
        "/api/v1/io/digital/output/" + std::to_string(i) + "/reference";
    web_server_->RegisterPutRoute(
        path_digital_ref_out.c_str(), [this, i](const std::string& body) {
          return HandlePutDigitalReference(i, types::IO::kOutput, body);
        });

    std::string path_analog_put =
        "/api/v1/io/analog/output/" + std::to_string(i);
    web_server_->RegisterPutRoute(path_analog_put.c_str(),
                                  [this, i](const std::string& body) {
                                    return HandlePutAnalogOutput(i, body);
                                  });

    std::string path_digital_put =
        "/api/v1/io/digital/output/" + std::to_string(i);
    web_server_->RegisterPutRoute(path_digital_put.c_str(),
                                  [this, i](const std::string& body) {
                                    return HandlePutDigitalOutput(i, body);
                                  });

    std::string path_analog_in = "/api/v1/io/analog/input/" + std::to_string(i);
    web_server_->RegisterGetRoute(path_analog_in.c_str(), [this, i]() {
      return HandleGetAnalog(i, types::IO::kInput);
    });

    std::string path_analog_out =
        "/api/v1/io/analog/output/" + std::to_string(i);
    web_server_->RegisterGetRoute(path_analog_out.c_str(), [this, i]() {
      return HandleGetAnalog(i, types::IO::kOutput);
    });

    std::string path_digital_in =
        "/api/v1/io/digital/input/" + std::to_string(i);
    web_server_->RegisterGetRoute(path_digital_in.c_str(), [this, i]() {
      return HandleGetDigital(i, types::IO::kInput);
    });

    std::string path_digital_out =
        "/api/v1/io/digital/output/" + std::to_string(i);
    web_server_->RegisterGetRoute(path_digital_out.c_str(), [this, i]() {
      return HandleGetDigital(i, types::IO::kOutput);
    });
  }

  web_server_->RegisterPutRoute(
      "/api/v1/serial/tx",
      [this](const std::string& body) { return HandlePutSerialTx(body); });

  web_server_->RegisterPutRoute(
      "/api/v1/serial/config",
      [this](const std::string& body) { return HandlePutSerialConfig(body); });

  ctx_.logic.serial_streamer->SetRxCallback(
      [this](types::Serial::Interface uart_num, const std::uint8_t* data,
             std::size_t length) {
        if (!web_server_) return;

        std::string source = types::Serial::ToString(uart_num);
        if (source == "Unknown") return;

        std::string payload(reinterpret_cast<const char*>(data), length);

        JsonDocument doc;
        doc["source"] = source;
        doc["data"] = payload;

        std::string json_out;
        serializeJson(doc, json_out);

        web_server_->SendSseEvent("serial", json_out.c_str());
      });
}

std::string ApiController::HandleGetSystemStatus() {
  types::SystemStatusDto status_data =
      logic::DiagnosticsService::BuildSystemStatusDto(
          *ctx_.logic.error_manager);

  return status_data.toJsonString();
}

std::string ApiController::HandleGetLogs() {
  types::LogsResponseDto logs_dto;

  core::Logger::ProcessLogs(
      [&logs_dto](const std::string& log_line) { logs_dto.addLog(log_line); });

  return logs_dto.toJsonString();
}

std::string ApiController::HandleGetIoAll() {
  types::IoAllResponseDto response;
  for (std::uint8_t i = 0; i < config::kNumChannels; i++) {
    response.analog_in[i] =
        ctx_.logic.process_imager->GetVoltage(types::IO::kAnalogIn, i);
    response.analog_out[i] =
        ctx_.logic.process_imager->GetReadbackVoltage(types::IO::kAnalogOut, i);
    response.digital_in[i].value =
        ctx_.logic.process_imager->GetVoltage(types::IO::kDigitalIn, i);
    response.digital_in[i].state =
        ctx_.logic.process_imager->GetDigitalState(types::IO::kDigitalIn, i);
    response.digital_out[i].value =
        ctx_.logic.process_imager->GetReadbackVoltage(types::IO::kDigitalOut,
                                                      i);
    response.digital_out[i].state =
        ctx_.logic.process_imager->GetDigitalState(types::IO::kDigitalOut, i);
  }
  return response.toJsonString();
}

std::string ApiController::HandleGetAnalog(std::uint8_t channel,
                                           types::IO::Direction direction) {
  types::IO::Group group = (direction == types::IO::kInput)
                               ? types::IO::kAnalogIn
                               : types::IO::kAnalogOut;
  std::uint16_t voltage =
      (direction == types::IO::kInput)
          ? ctx_.logic.process_imager->GetVoltage(group, channel)
          : ctx_.logic.process_imager->GetVoltage(group, channel);
  types::AnalogResponseDto response;
  response.direction = (direction == types::IO::kInput) ? "input" : "output";
  response.channel = channel;
  response.value = voltage;
  return response.toJsonString();
}

std::string ApiController::HandleGetDigital(std::uint8_t channel,
                                            types::IO::Direction direction) {
  types::IO::Group group = (direction == types::IO::kInput)
                               ? types::IO::kDigitalIn
                               : types::IO::kDigitalOut;
  std::uint16_t voltage =
      (direction == types::IO::kInput)
          ? ctx_.logic.process_imager->GetVoltage(group, channel)
          : ctx_.logic.process_imager->GetVoltage(group, channel);
  types::DigitalResponseDto response;
  response.direction = (direction == types::IO::kInput) ? "input" : "output";
  response.channel = channel;
  response.value = voltage;
  response.state = ctx_.logic.process_imager->GetDigitalState(group, channel);
  return response.toJsonString();
}

std::string ApiController::HandleGetI2cDevices() {
  types::I2CDeviceListDto response;
  types::I2cDeviceList list =
      logic::DiagnosticsService::GetConfiguredI2cDevices();

  for (std::uint8_t i = 0; i < list.count; i++) {
    response.addDevice(list.devices[i].address, list.devices[i].bus,
                       list.devices[i].name);
  }
  return response.toJsonString();
}

std::string ApiController::HandlePutAnalogOutput(std::uint8_t channel,
                                                 const std::string& body) {
  types::AnalogOutputRequestDto request;

  if (!request.fromJson(body)) {
    LOG_ERROR("API Error (Analog Put): Falsches JSON Format");
    types::ErrorModelDto error("Falsches JSON Format", "N/A");
    return error.toJsonString();
  }

  if (request.value > config::kMaxSystemVoltageMv) {
    LOG_ERROR("API Error: Value ueberschreitet %d mV",
              config::kMaxSystemVoltageMv);
    types::ErrorModelDto error("Value exceeds maximum voltage", "N/A");
    return error.toJsonString();
  }

  ctx_.logic.mailbox->SetAnalogTarget(channel, request.value);

  return "{\"status\":\"success\"}";
}

std::string ApiController::HandlePutDigitalOutput(std::uint8_t channel,
                                                  const std::string& body) {
  types::DigitalOutputRequestDto request;

  if (!request.fromJson(body)) {
    LOG_ERROR("API Error (Digital Put): Falsches JSON Format");
    types::ErrorModelDto error("Falsches JSON Format", "N/A");
    return error.toJsonString();
  }

  if (request.state > 1) {
    LOG_ERROR("API Error (Digital Put): State überschreitet 0 oder 1 status");
    types::ErrorModelDto error("State überschreitet 0 oder 1 status", "N/A");
    return error.toJsonString();
  }

  ctx_.logic.mailbox->SetDigitalTarget(channel, request.state);

  return "{\"status\":\"success\"}";
}

std::string ApiController::HandlePutDigitalReference(
    std::uint8_t channel, types::IO::Direction direction,
    const std::string& body) {
  types::DigitalReferenceRequestDto request;
  LOG_INFO(
      "Received Digital Reference API Request: Channel %d, Direction %s, Body: "
      "%s",
      channel, (direction == types::IO::kInput) ? "Input" : "Output",
      body.c_str());

  if (!request.fromJson(body)) {
    LOG_ERROR("API Error (Digital Reference): Falsches JSON Format");
    types::ErrorModelDto error("Falsches JSON Format", "N/A");
    return error.toJsonString();
  }

  if (request.voltage_mv != 1800 && request.voltage_mv != 3300 &&
      request.voltage_mv != 5000 && request.voltage_mv != 12000 &&
      request.voltage_mv != 24000) {
    LOG_ERROR("API Error: Ungueltige Spannung");
    types::ErrorModelDto error("Ungueltige Spannung.", "N/A");
    return error.toJsonString();
  }

  ctx_.logic.mailbox->SetDigitalReference(direction, channel,
                                          request.voltage_mv);

  return "{\"status\":\"success\"}";
}

std::string ApiController::HandlePutSerialTx(const std::string& body) {
  types::SerialTxRequestDto request;

  if (!request.fromJson(body)) {
    LOG_ERROR("API Error (Serial TX): Falsches JSON Format");
    return types::ErrorModelDto("Falsches JSON Format", "N/A").toJsonString();
  }

  types::Serial::Interface uart_num;
  if (request.interface_name == "UART")
    uart_num = types::Serial::kUart;
  else if (request.interface_name == "RS485")
    uart_num = types::Serial::kRs485;
  else if (request.interface_name == "RS232")
    uart_num = types::Serial::kRs232;
  else {
    return types::ErrorModelDto("Unbekanntes Interface", "N/A").toJsonString();
  }

  ctx_.logic.mailbox->AddSerialTx(uart_num, request.payload);
  return "{\"status\":\"success\"}";
}

std::string ApiController::HandlePutSerialConfig(const std::string& body) {
  types::SerialConfigRequestDto request;

  if (!request.fromJson(body)) {
    LOG_ERROR("API Error (Serial Config): Falsches JSON Format");
    return types::ErrorModelDto("Falsches JSON Format", "N/A").toJsonString();
  }

  types::Serial::Interface uart_num;
  if (request.interface_name == "UART")
    uart_num = types::Serial::kUart;
  else if (request.interface_name == "RS485")
    uart_num = types::Serial::kRs485;
  else if (request.interface_name == "RS232")
    uart_num = types::Serial::kRs232;
  else {
    return types::ErrorModelDto("Unbekanntes Interface", "N/A").toJsonString();
  }

  ctx_.logic.mailbox->SetSerialConfig(uart_num, request.baudrate);
  return "{\"status\":\"success\"}";
}

}  // namespace logic
}  // namespace hardware_pruefadapter