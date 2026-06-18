/**
 * @file esp_async_server_adapter.cpp
 * @brief Implementierung des ESPAsyncWebServer Adapters.
 */

#include "esp_async_server_adapter.h"

namespace hardware_pruefadapter {
namespace adapters {

EspAsyncServerAdapter::EspAsyncServerAdapter(std::uint16_t port)
    : server_(std::make_unique<AsyncWebServer>(port)) {}

types::ErrorCode EspAsyncServerAdapter::Start() {
  if (!server_) return types::ErrorCode::kErrorInitializeFailure;

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods",
                                       "GET, PUT, POST, OPTIONS");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

  server_->onNotFound([](AsyncWebServerRequest* request) {
    if (request->method() == HTTP_OPTIONS) {
      request->send(200);
    } else {
      request->send(404, "text/plain", "Not found");
    }
  });

  server_->begin();
  return types::ErrorCode::kSuccess;
}

void EspAsyncServerAdapter::RegisterStaticRoute(const char* path,
                                                const char* content,
                                                const char* content_type) {
  if (server_ == nullptr) return;

  server_->on(path, HTTP_GET,
              [content, content_type](AsyncWebServerRequest* request) {
                request->send(200, content_type, (const uint8_t*)content,
                                strlen(content));
              });
}

void EspAsyncServerAdapter::RegisterGetRoute(const char* path,
                                             interfaces::RouteHandler handler) {
  if (server_ == nullptr) return;

  server_->on(path, HTTP_GET, [handler](AsyncWebServerRequest* request) {
    if (request == nullptr) return;

    std::string response_content = handler();

    if (response_content.find("<!DOCTYPE html>") != std::string::npos) {
      request->send(200, "text/html", response_content.c_str());
    } else {
      request->send(200, "application/json", response_content.c_str());
    }
  });
}

void EspAsyncServerAdapter::RegisterPutRoute(
    const char* path, interfaces::RouteWithBodyHandler handler) {
  if (server_ == nullptr) return;

  server_->on(path, HTTP_OPTIONS,
              [](AsyncWebServerRequest* request) { request->send(200); });

  server_->on(
      path, HTTP_PUT,
      [](AsyncWebServerRequest* request) {
        if (request->_tempObject != nullptr) {
          std::string* response_str =
              static_cast<std::string*>(request->_tempObject);
          request->send(200, "application/json", response_str->c_str());
          delete response_str;
          request->_tempObject = nullptr;
        } else {
          request->send(400, "application/json",
                        "{\"error\":\"Bad Request / No Body\"}");
        }
      },
      nullptr,
      [handler](AsyncWebServerRequest* request, uint8_t* data, size_t len,
                size_t index, size_t total) {
        std::string* body_str;

        if (index == 0) {
          body_str = new std::string();
          request->_tempObject = body_str;
        } else {
          body_str = static_cast<std::string*>(request->_tempObject);
        }

        if (body_str != nullptr) {
          body_str->append(reinterpret_cast<const char*>(data), len);
        }

        if (index + len == total && body_str != nullptr) {
          std::string response = handler(*body_str);
          *body_str = response;
        }
      });
}

void EspAsyncServerAdapter::RegisterSseRoute(const char* path) {
  if (server_ == nullptr) return;

  events_ = std::make_unique<AsyncEventSource>(path);

  events_->onConnect([](AsyncEventSourceClient* client) {
    client->send("SSE Verbindung hergestellt", "sys", millis(), 1000);
  });

  server_->addHandler(events_.get());
}

void EspAsyncServerAdapter::SendSseEvent(const char* event_name,
                                         const char* data) {
  if (events_ != nullptr) {
    events_->send(data, event_name, millis());
  }
}

}  // namespace adapters
}  // namespace hardware_pruefadapter