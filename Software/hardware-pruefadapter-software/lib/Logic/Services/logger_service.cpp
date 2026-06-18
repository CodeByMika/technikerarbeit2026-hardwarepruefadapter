/**
 * @file logger_service.cpp
 * @brief Implementierung des zentralen Loggers.
 */

#include "logger_service.h"

namespace hardware_pruefadapter {
namespace core {

std::string Logger::buffer_[config::kMaxLogLines];
std::size_t Logger::head_ = 0;
std::size_t Logger::count_ = 0;
std::mutex Logger::mutex_;
LogCallback Logger::on_log_callback_ = nullptr;

void Logger::SetOnLogCallback(LogCallback callback) {
  std::lock_guard<std::mutex> lock(mutex_);
  on_log_callback_ = callback;
}

void Logger::Log(const char* format, ...) {
  va_list args;
  va_start(args, format);
  Logging("", format, args);
  va_end(args);
}

void Logger::LogInfo(const char* format, ...) {
  va_list args;
  va_start(args, format);
  Logging("[INFO] ", format, args);
  va_end(args);
}

void Logger::LogWarning(const char* format, ...) {
  va_list args;
  va_start(args, format);
  Logging("[WARN] ", format, args);
  va_end(args);
}

void Logger::LogError(const char* format, ...) {
  va_list args;
  va_start(args, format);
  Logging("[ERROR] ", format, args);
  va_end(args);
}

void Logger::ClearLogs() {
  std::lock_guard<std::mutex> lock(mutex_);
  for (std::size_t i = 0; i < config::kMaxLogLines; i++) {
    buffer_[i].clear();
  }
  head_ = 0;
  count_ = 0;
}

void Logger::Logging(const char* prefix, const char* format, va_list args) {
  char temp_buffer[config::kMaxLogLength];
  vsnprintf(temp_buffer, sizeof(temp_buffer), format, args);

  LoggingString(prefix, temp_buffer);
}

void Logger::LoggingString(const char* prefix, const char* message) {
  uint32_t total_ms = 0;
#ifdef ESP32_ENV
  total_ms = millis();
#else
  total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now().time_since_epoch())
                 .count();
#endif

  uint32_t ms = total_ms % 1000;
  uint32_t sec = (total_ms / 1000) % 60;
  uint32_t min = (total_ms / 60000) % 60;
  uint32_t hr = (total_ms / 3600000);

  char time_buf[24];
  snprintf(time_buf, sizeof(time_buf), "[%02u:%02u:%02u.%03u] ", hr, min, sec, ms);

  std::string log_message = std::string(time_buf) + std::string(prefix) + std::string(message);

#ifdef ESP32_ENV
  Serial.println(log_message.c_str());
#else
  std::cout << log_message << std::endl;
#endif

  {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_[head_] = log_message;
    head_ = (head_ + 1) % config::kMaxLogLines;
    if (count_ < config::kMaxLogLines) {
      count_++;
    }
  } 
  
  if (on_log_callback_) {
    on_log_callback_(log_message);
  }
}

}  // namespace core
}  // namespace hardware_pruefadapter