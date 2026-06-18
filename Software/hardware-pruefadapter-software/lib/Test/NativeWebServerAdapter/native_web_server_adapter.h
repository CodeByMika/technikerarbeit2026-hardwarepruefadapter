/**
 * @file native_web_server_adapter.h
 * @brief PC-Simulationsadapter für den Webserver (nutzt httplib).
 *
 * @author Techniker_Team_2025_26
 * @date 2026-03-25
 * @version 1.0.0
 */
#ifdef NATIVE_ENV

#ifndef HARDWARE_PRUEFADAPTER_ADAPTERS_NATIVE_WEB_SERVER_ADAPTER_H_
#define HARDWARE_PRUEFADAPTER_ADAPTERS_NATIVE_WEB_SERVER_ADAPTER_H_

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "../../Interfaces/web_server_interface.h"
#include "../../Logic/Services/web_bouncer_service.h"
#include "httplib.h"

namespace hardware_pruefadapter {
namespace adapters {

class NativeWebServerAdapter : public interfaces::IWebServer {
 public:
  explicit NativeWebServerAdapter(int port) : port_(port) {
    server_ = std::make_unique<httplib::Server>();
  }

  types::ErrorCode Start() override {
    server_thread_ =
        std::thread([this]() { server_->listen("0.0.0.0", port_); });
    return types::ErrorCode::kSuccess;
  }

  void RegisterGetRoute(const char* path,
                        interfaces::RouteHandler handler) override {
    server_->Get(
        path, [handler](const httplib::Request&, httplib::Response& res) {
          res.set_header("Access-Control-Allow-Origin", "*");
          res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
          res.set_header("Access-Control-Allow-Headers", "*");

          // --- BOUNCER: Überlastungsschutz ---
          if (!logic::WebBouncer::AcquireSlot()) {
            res.status = 503;
            res.set_content("{\"error\":\"Service Unavailable (Overload)\"}",
                            "application/json");
            return;
          }

          std::string content = handler();
          logic::WebBouncer::ReleaseSlot();

          if (content.find("<!DOCTYPE html>") != std::string::npos) {
            res.set_content(content, "text/html");
          } else {
            res.set_content(content, "application/json");
          }
        });
  }

  void RegisterPutRoute(const char* path,
                        interfaces::RouteWithBodyHandler handler) override {
    server_->Put(path, [handler](const httplib::Request& req,
                                 httplib::Response& res) {
      res.set_header("Access-Control-Allow-Origin", "*");
      res.set_header("Access-Control-Allow-Methods", "GET, PUT, POST, OPTIONS");
      res.set_header("Access-Control-Allow-Headers", "*");

      // --- BOUNCER: Überlastungsschutz ---
      if (!logic::WebBouncer::AcquireSlot()) {
        res.status = 503;
        res.set_content("{\"error\":\"Service Unavailable (Overload)\"}",
                        "application/json");
        return;
      }

      std::string content = handler(req.body);
      logic::WebBouncer::ReleaseSlot();

      res.set_content(content, "application/json");
    });

    server_->Options(path, [](const httplib::Request&, httplib::Response& res) {
      res.set_header("Access-Control-Allow-Origin", "*");
      res.set_header("Access-Control-Allow-Methods", "GET, PUT, POST, OPTIONS");
      res.set_header("Access-Control-Allow-Headers", "Content-Type");
      res.set_content("", "text/plain");
    });
  }

  void RegisterStaticRoute(const char* path, const char* content,
                           const char* content_type) override {
    server_->Get(path, [content, content_type](const httplib::Request&,
                                               httplib::Response& res) {
      res.set_header("Access-Control-Allow-Origin", "*");
      res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
      res.set_header("Access-Control-Allow-Headers", "*");

      if (!logic::WebBouncer::AcquireSlot()) {
        res.status = 503;
        res.set_content("{\"error\":\"Service Unavailable (Overload)\"}",
                        "application/json");
        return;
      }

      res.set_content(content, content_type);
      logic::WebBouncer::ReleaseSlot();
    });
  }

  void RegisterSseRoute(const char* path) override {
    std::cout << "[SIM] Aktive SSE Route registriert auf: " << path
              << std::endl;

    server_->Get(path, [this](const httplib::Request&, httplib::Response& res) {
      // --- BOUNCER: Maximales Stream Limit prüfen ---
      if (!logic::WebBouncer::AllowSseConnection(active_sse_clients_.load())) {
        res.status = 503;
        res.set_content("{\"error\":\"Service Unavailable (Overload)\"}",
                        "application/json");
        return;
      }
      active_sse_clients_++;

      res.set_header("Content-Type", "text/event-stream");
      res.set_header("Cache-Control", "no-cache");
      res.set_header("Connection", "keep-alive");
      res.set_header("Access-Control-Allow-Origin", "*");

      res.set_chunked_content_provider(
          "text/event-stream",
          [this](size_t offset, httplib::DataSink& sink) {
            while (true) {
              std::unique_lock<std::mutex> lock(sse_mutex_);

              if (sse_cv_.wait_for(lock, std::chrono::seconds(2),
                                   [this] { return !sse_queue_.empty(); })) {
                while (!sse_queue_.empty()) {
                  std::string msg = sse_queue_.front();
                  sse_queue_.pop();

                  if (!sink.write(msg.c_str(), msg.size())) return false;
                }
              } else {
                if (!sink.write(": ping\n\n", 8)) return false;
              }
            }
            return true;
          },
          [this](bool) {
            active_sse_clients_--;
          }  // Callback wenn Browser schließt
      );
    });
  }

  void SendSseEvent(const char* event_name, const char* data) override {
    std::lock_guard<std::mutex> lock(sse_mutex_);

    std::string msg = "event: ";
    msg += event_name;
    msg += "\ndata: ";
    msg += data;
    msg += "\n\n";

    sse_queue_.push(msg);
    sse_cv_.notify_all();
  }

 private:
  int port_;
  std::unique_ptr<httplib::Server> server_;
  std::thread server_thread_;

  std::mutex sse_mutex_;
  std::condition_variable sse_cv_;
  std::queue<std::string> sse_queue_;

  std::atomic<size_t> active_sse_clients_{0};
};

}  // namespace adapters
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_ADAPTERS_NATIVE_WEB_SERVER_ADAPTER_H_
#endif  // NATIVE_ENV