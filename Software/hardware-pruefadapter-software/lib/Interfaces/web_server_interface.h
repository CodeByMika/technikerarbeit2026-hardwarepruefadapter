/**
 * @file web_server_interface.h
 * @brief Abstrakte Schnittstelle für den Webserver.
 *
 * Entkoppelt die spezifische Webserver-Bibliothek (z.B. ESPAsyncWebServer)
 * von der Anwendungs- und Routinglogik.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-01-08
 * @version 1.0.0
 * @ingroup Interfaces
 * @copyright Copyright (c) 2025 SOTEC GmBH & Co KG
 */
#ifndef HARDWARE_PRUEFADAPTER_INTERFACES_WEB_SERVER_INTERFACE_H_
#define HARDWARE_PRUEFADAPTER_INTERFACES_WEB_SERVER_INTERFACE_H_

#include <functional>
#include <string>

#include "error_code.h"

namespace hardware_pruefadapter {
namespace interfaces {

using RouteHandler = std::function<std::string()>;
using RouteWithBodyHandler =
    std::function<std::string(const std::string& body)>;

/**
 * @class IWebServer
 * @brief Basisklasse für Webserver-Adapter.
 */
class IWebServer {
 public:
  virtual ~IWebServer() = default;

  /**
   * @brief Startet den Webserver und öffnet den Port für Verbindungen.
   */
  virtual types::ErrorCode Start() = 0;

  /**
   * @brief Registriert eine HTTP GET Route.
   * @param path Der URL-Pfad (z.B. "/api/status").
   * @param handler Callback-Funktion ohne Body.
   */
  virtual void RegisterGetRoute(const char* path, RouteHandler handler) = 0;

  /**
   * @brief Registriert eine HTTP PUT Route zum Empfangen von Daten.
   * @param path Der URL-Pfad (z.B. "/api/v1/analog/output/1").
   * @param handler Callback-Funktion mit Request-Body.
   */
  virtual void RegisterPutRoute(const char* path,
                                RouteWithBodyHandler handler) = 0;

  /**
   * @brief Registriert eine statische Route, die direkt aus dem Flash (PROGMEM)
   * gesendet wird.
   */
  virtual void RegisterStaticRoute(const char* path, const char* content,
                                   const char* content_type) = 0;

  /**
   * @brief Registriert einen Endpunkt für einen Server-Sent Events (SSE)
   * Stream.
   */
  virtual void RegisterSseRoute(const char* path) = 0;

  /**
   * @brief Sendet eine Nachricht an alle verbundenen SSE-Clients.
   * @param event_name Typ des Events (z.B. "log" oder "serial").
   * @param data Der JSON- oder Text-Payload.
   */
  virtual void SendSseEvent(const char* event_name, const char* data) = 0;
};

}  // namespace interfaces
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_INTERFACES_WEB_SERVER_INTERFACE_H_