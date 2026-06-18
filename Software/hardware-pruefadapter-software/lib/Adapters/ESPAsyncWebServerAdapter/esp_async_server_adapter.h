/**
 * @file esp_async_server_adapter.h
 * @brief Konkreter Adapter für die ESPAsyncWebServer Bibliothek.
 */
#ifndef HARDWARE_PRUEFADAPTER_ADAPTERS_ESP_ASYNC_SERVER_ADAPTER_H_
#define HARDWARE_PRUEFADAPTER_ADAPTERS_ESP_ASYNC_SERVER_ADAPTER_H_

#include <ESPAsyncWebServer.h>

#include <cstdint>
#include <memory>

#include "../../Interfaces/web_server_interface.h"
#include "error_code.h"
#include "system_config.h"

namespace hardware_pruefadapter {
namespace adapters {

class EspAsyncServerAdapter : public interfaces::IWebServer {
 public:
  explicit EspAsyncServerAdapter(std::uint16_t port);
  ~EspAsyncServerAdapter() override = default;

  types::ErrorCode Start() override;
  void RegisterGetRoute(const char* path,
                        interfaces::RouteHandler handler) override;
  void RegisterPutRoute(const char* path,
                        interfaces::RouteWithBodyHandler handler) override;
  void RegisterStaticRoute(const char* path, const char* content,
                           const char* content_type) override;
  void RegisterSseRoute(const char* path) override;
  void SendSseEvent(const char* event_name, const char* data) override;

 private:
  std::unique_ptr<AsyncWebServer> server_;
  std::unique_ptr<AsyncEventSource> events_;
};

}  // namespace adapters
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_ADAPTERS_ESP_ASYNC_SERVER_ADAPTER_H_