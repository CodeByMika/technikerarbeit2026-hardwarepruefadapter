/**
 * @file logger_service.h
 * @brief Zentraler Logging-Service mit Ringpuffer für das Web-Terminal.
 * 
 * @author Techniker_Team_2025_26
 * @date 2026-03-25
 * @version 1.0.0
 * @ingroup Services
 */

#ifndef HARDWARE_PRUEFADAPTER_CORE_LOGGER_SERVICE_H_
#define HARDWARE_PRUEFADAPTER_CORE_LOGGER_SERVICE_H_

#include <cstdarg>
#include <cstdio>
#include <functional>
#include <mutex>
#include <string>

#ifdef ESP32_ENV
#include <Arduino.h>
#else
#include <chrono>
#include <iostream>
#endif

#include "system_config.h"

namespace hardware_pruefadapter {
namespace core {

using LogCallback = std::function<void(const std::string&)>;

class Logger {
 public:

  // --- Logging Funktionen ---
  /** @brief Loggt eine Nachricht ohne Präfix. */
  static void Log(const char* format, ...);

  /** @brief Loggt eine Informations-Nachricht mit Präfix [INFO]. */
  static void LogInfo(const char* format, ...);

  /** @brief Loggt eine Warnung mit Präfix [WARN]. */
  static void LogWarning(const char* format, ...);

  /** @brief Loggt einen Fehler mit Präfix [ERROR]. */
  static void LogError(const char* format, ...);

  // --- Helfer ---

  /**
   * @brief Führt eine übergebene Funktion für jeden gespeicherten Log aus.
   * @param callback Die Funktion, die ausgeführt wird (z.B. ein Lambda)
   */
  template <typename Callable>
  static void ProcessLogs(Callable callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::size_t start_index = (count_ == config::kMaxLogLines) ? head_ : 0;

    for (std::size_t i = 0; i < count_; i++) {
      std::size_t index = (start_index + i) % config::kMaxLogLines;
      callback(buffer_[index]);
    }
  }

  static void SetOnLogCallback(LogCallback callback);

  static void ClearLogs();

 private:
  static void Logging(const char* prefix, const char* format, va_list args);
  static void LoggingString(const char* prefix, const char* message);

  // --- Ringpuffer Variablen ---
  static std::string buffer_[config::kMaxLogLines];
  static std::size_t head_;   // Zeigt auf den nächsten freien Slot
  static std::size_t count_;  // Wie viele Logs wir aktuell haben
  static std::mutex mutex_;   // Verhindert Zugriffs-Crashs (Thread-Safety)
  static LogCallback on_log_callback_;
};

}  // namespace core
}  // namespace hardware_pruefadapter

// =======================================================================
// GLOBAL LOGGING TEMPLATES
// =======================================================================

template <typename... Args>
inline void LOG(const char* msg, Args... args) {
  hardware_pruefadapter::core::Logger::Log(msg, args...);
}

template <typename... Args>
inline void LOG_INFO(const char* msg, Args... args) {
  hardware_pruefadapter::core::Logger::LogInfo(msg, args...);
}

template <typename... Args>
inline void LOG_ERROR(const char* msg, Args... args) {
  hardware_pruefadapter::core::Logger::LogError(msg, args...);
}

template <typename... Args>
inline void LOG_WARNING(const char* msg, Args... args) {
  hardware_pruefadapter::core::Logger::LogWarning(msg, args...);
}

#endif  // HARDWARE_PRUEFADAPTER_CORE_LOGGER_SERVICE_H_