/**
 * @file api_types.h
 * @brief Datentransfer-Objekte (DTOs) für die API.
 * Entspricht den Schemas aus der openapi.yml.
 */

#ifndef HARDWARE_PRUEFADAPTER_INCLUDE_API_TYPES_H_
#define HARDWARE_PRUEFADAPTER_INCLUDE_API_TYPES_H_

#include <ArduinoJson.h>

#include <cstdint>
#include <string>

#include "error_code.h"
#include "system_config.h"

namespace hardware_pruefadapter {
namespace types {

// =============================================================================
// SYSTEM STATUS & LOGS
// =============================================================================

/**
 * @brief Schema: SystemStatus
 * Siehe openapi.yml components/schemas/SystemStatus
 */
/**
 * @brief Schema: SystemStatus
 */
struct SystemStatusDto {
  std::uint32_t uptime_ms = 0;
  std::uint32_t heap_free = 0;
  std::string status = "running";
  std::uint8_t error_count = 0;
  std::string errors[config::kMaxCycleErrors];

  // Serialisiert das Struct in einen JSON-String
  std::string toJsonString() const {
    JsonDocument doc;
    doc["uptime_ms"] = uptime_ms;
    doc["heap_free"] = heap_free;
    doc["status"] = status;
    doc["error_count"] = error_count;

    JsonArray error_array = doc["errors"].to<JsonArray>();
    for (int i = 0; i < error_count; i++) {
      error_array.add(errors[i]);
    }

    std::string out;
    serializeJson(doc, out);
    return out;
  }
};

/**
 * @brief Schema: LogsResponse
 */
struct LogsResponseDto {
  JsonDocument doc;

  LogsResponseDto() {
    doc["logs"].to<JsonArray>();  // Initialisiert das leere Array
  }

  void addLog(const std::string& log_line) { doc["logs"].add(log_line); }

  std::string toJsonString() const {
    std::string out;
    serializeJson(doc, out);
    return out;
  }
};

// =============================================================================
// I2C DEVICES
// =============================================================================

/**
 * @brief Schema: I2CDeviceList (enthält I2CDevice)
 */
struct I2CDeviceListDto {
  JsonDocument doc;

  I2CDeviceListDto() {
    doc.to<JsonArray>();  // Laut OpenAPI ist die Response direkt ein Array
  }

  void addDevice(const std::string& address, std::uint8_t bus,
                 const std::string& name) {
    JsonObject obj = doc.add<JsonObject>();
    obj["address"] = address;
    obj["bus"] = bus;
    obj["name"] = name;
  }

  std::string toJsonString() const {
    std::string out;
    serializeJson(doc, out);
    return out;
  }
};

// =============================================================================
// I/O RESPONSES (GET / Ausgabe an Client)
// =============================================================================

/**
 * @brief Schema: IoAllResponse
 */
struct IoAllResponseDto {
  std::uint16_t analog_in[config::kNumChannels]{0};
  std::uint16_t analog_out[config::kNumChannels]{0};

  struct DigitalData {
    std::int8_t state = 0;
    std::uint16_t value = 0;
  };

  DigitalData digital_in[config::kNumChannels];
  DigitalData digital_out[config::kNumChannels];

  std::string toJsonString() const {
    JsonDocument doc;

    JsonArray a_in = doc["analog_in"].to<JsonArray>();
    JsonArray a_out = doc["analog_out"].to<JsonArray>();
    for(int i = 0; i < config::kNumChannels; i++) {
      a_in.add(analog_in[i]);
      a_out.add(analog_out[i]);
    }

    JsonArray d_in = doc["digital_in"].to<JsonArray>();
    JsonArray d_out = doc["digital_out"].to<JsonArray>();
    for(int i = 0; i < config::kNumChannels; i++) {
      JsonObject din_obj = d_in.add<JsonObject>();
      din_obj["state"] = digital_in[i].state;
      din_obj["value"] = digital_in[i].value;

      JsonObject dout_obj = d_out.add<JsonObject>();
      dout_obj["state"] = digital_out[i].state;
      dout_obj["value"] = digital_out[i].value;
    }

    std::string out;
    serializeJson(doc, out);
    return out;
  }
};

/**
 * @brief Schema: AnalogResponse
 */
struct AnalogResponseDto {
  std::string direction;  // "input" oder "output"
  std::uint8_t channel = 0;
  std::uint16_t value = 0;

  std::string toJsonString() const {
    JsonDocument doc;
    doc["technologyType"] = "analog";
    doc["direction"] = direction;
    doc["channel"] = channel;
    doc["value"] = value;
    doc["unit"] = "mV";

    std::string out;
    serializeJson(doc, out);
    return out;
  }
};

/**
 * @brief Schema: AnalogList
 */
struct AnalogListDto {
  JsonDocument doc;

  AnalogListDto() { doc.to<JsonArray>(); }

  void addChannel(const AnalogResponseDto& ch) {
    JsonObject obj = doc.add<JsonObject>();
    obj["technologyType"] = "analog";
    obj["direction"] = ch.direction;
    obj["channel"] = ch.channel;
    obj["value"] = ch.value;
    obj["unit"] = "mV";
  }

  std::string toJsonString() const {
    std::string out;
    serializeJson(doc, out);
    return out;
  }
};

/**
 * @brief Schema: DigitalResponse
 */
struct DigitalResponseDto {
  std::string direction;  // "input" oder "output"
  std::uint8_t channel = 0;
  std::int8_t state = 0;
  std::uint16_t value = 0;

  std::string toJsonString() const {
    JsonDocument doc;
    doc["technologyType"] = "digital";
    doc["direction"] = direction;
    doc["channel"] = channel;
    doc["state"] = state;
    doc["value"] = value;
    doc["unit"] = "mV";

    std::string out;
    serializeJson(doc, out);
    return out;
  }
};

/**
 * @brief Schema: DigitalList
 */
struct DigitalListDto {
  JsonDocument doc;

  DigitalListDto() { doc.to<JsonArray>(); }

  void addChannel(const DigitalResponseDto& ch) {
    JsonObject obj = doc.add<JsonObject>();
    obj["technologyType"] = "digital";
    obj["direction"] = ch.direction;
    obj["channel"] = ch.channel;
    obj["state"] = ch.state;
    obj["value"] = ch.value;
    obj["unit"] = "mV";
  }

  std::string toJsonString() const {
    std::string out;
    serializeJson(doc, out);
    return out;
  }
};

// =============================================================================
// I/O REQUESTS (PUT / Eingabe vom Client)
// =============================================================================

/**
 * @brief Schema: AnalogOutputRequest
 */
struct AnalogOutputRequestDto {
  std::uint16_t value = 0;

  // Parst einen einkommenden JSON String
  bool fromJson(const std::string& payload) {
    JsonDocument doc;
    if (deserializeJson(doc, payload)) return false;  // Parse Error
    if (doc["value"].isNull()) return false;          // Feld fehlt

    value = doc["value"].as<std::uint16_t>();
    return true;
  }
};

/**
 * @brief Schema: DigitalOutputRequest
 */
struct DigitalOutputRequestDto {
  bool state = false;

  bool fromJson(const std::string& payload) {
    JsonDocument doc;
    if (deserializeJson(doc, payload)) return false;
    if (doc["state"].isNull()) return false;

    state = doc["state"].as<bool>();
    return true;
  }
};

/**
 * @brief Schema: DigitalReferenceRequest
 */
struct DigitalReferenceRequestDto {
  std::uint16_t voltage_mv = 0;

  bool fromJson(const std::string& payload) {
    JsonDocument doc;
    if (deserializeJson(doc, payload)) return false;
    if (doc["voltage_mv"].isNull()) return false;
    
    voltage_mv = doc["voltage_mv"].as<std::uint16_t>();
    return true;
  }
};

// =============================================================================
// Serial Data Transfair Objekt
// =============================================================================

/**
 * @brief Schema: SerialTxRequest
 */
struct SerialTxRequestDto {
  std::string interface_name;
  std::string payload;

  bool fromJson(const std::string& json_payload) {
    JsonDocument doc;
    if (deserializeJson(doc, json_payload)) return false;
    if (doc["interface"].isNull() || doc["payload"].isNull()) return false;

    interface_name = doc["interface"].as<std::string>();
    payload = doc["payload"].as<std::string>();
    return true;
  }
};

/**
 * @brief Schema: SerialConfigRequest
 */
struct SerialConfigRequestDto {
  std::string interface_name;
  std::uint32_t baudrate = 115200;

  bool fromJson(const std::string& json_payload) {
    JsonDocument doc;
    if (deserializeJson(doc, json_payload)) return false;
    if (doc["interface"].isNull() || doc["baudrate"].isNull()) return false;

    interface_name = doc["interface"].as<std::string>();
    baudrate = doc["baudrate"].as<std::uint32_t>();
    return true;
  }
};

// =============================================================================
// ERROR HANDLING (Fehlermeldungen nach außen)
// =============================================================================

/**
 * @brief Schema: ErrorModel (inklusive ErrorDetail)
 */
struct ErrorModelDto {
  JsonDocument doc;

  ErrorModelDto(const std::string& message, const std::string& timestamp) {
    doc["message"] = message;
    doc["timestamp"] = timestamp;
    doc["details"].to<JsonArray>();  // Optionales Detail-Array vorbereiten
  }

  void addDetail(const std::string& field, const std::string& issue) {
    JsonObject obj = doc["details"].add<JsonObject>();
    obj["field"] = field;
    obj["issue"] = issue;
  }

  std::string toJsonString() const {
    std::string out;
    serializeJson(doc, out);
    return out;
  }
};

}  // namespace types
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_INCLUDE_API_TYPES_H_