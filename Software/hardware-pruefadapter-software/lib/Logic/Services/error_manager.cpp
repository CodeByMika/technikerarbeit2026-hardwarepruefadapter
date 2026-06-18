/**
 * @file error_manager.cpp
 * @brief Implementierung des ErrorManagers.
 */

#include "error_manager.h"

namespace hardware_pruefadapter {
namespace logic {

void ErrorManager::AddCycleError(const std::string& context,
                                 types::ErrorCode error) {
  if (error == types::ErrorCode::kSuccess) return;

  std::lock_guard<std::mutex> lock(error_mutex_);

  // Dopplungen überspringen
  for (int i = 0; i < shadow_cycle_error_count_; ++i) {
    if (shadow_cycle_errors_[i].code == error &&
        shadow_cycle_errors_[i].context == context) {
      return;
    }
  }

  // Neuer Fehler?
  bool is_new = true;
  for (int i = 0; i < cycle_error_count_; ++i) {
    if (cycle_errors_[i].code == error && cycle_errors_[i].context == context) {
      is_new = false;
      break;
    }
  }

  // Nur neue Fehler werden in die Konsole/Web-Terminal geloggt
  if (is_new) {
    LOG_WARNING("[%s] %s", context.c_str(),
                types::ErrorCodeToString(error).data());
  }

  // Fehler in den Schatten-Speicher eintragen
  if (shadow_cycle_error_count_ < config::kMaxCycleErrors) {
    shadow_cycle_errors_[shadow_cycle_error_count_] = {context, error};
    shadow_cycle_error_count_++;

    if (shadow_cycle_error_count_ == config::kMaxCycleErrors) {
      LOG_WARNING("Maximale Anzahl von Fehlern in diesem Zyklus erreicht.");
    }
  }
}

void ErrorManager::ResolveErrors() {
  std::lock_guard<std::mutex> lock(error_mutex_);

  // Überprüfung welche Fehler noch vorhanden sind
  for (int i = 0; i < cycle_error_count_; ++i) {
    bool still_active = false;

    for (int j = 0; j < shadow_cycle_error_count_; ++j) {
      if (cycle_errors_[i].code == shadow_cycle_errors_[j].code &&
          cycle_errors_[i].context == shadow_cycle_errors_[j].context) {
        still_active = true;
        break;
      }
    }

    // Wenn der Fehler verschwunden ist, loggen wir eine Erfolgsmeldung
    if (!still_active) {
      LOG("[Fehler behoben] [%s] %s", cycle_errors_[i].context.c_str(),
          types::ErrorCodeToString(cycle_errors_[i].code).data());
    }
  }

  // Schatten-Kopie übertragen
  cycle_error_count_ = shadow_cycle_error_count_;

  for (std::uint8_t i = 0; i < config::kMaxCycleErrors; ++i) {
    if (i < cycle_error_count_) {
      cycle_errors_[i] = shadow_cycle_errors_[i];
    } else {
      // Restliche Plätze zurücksetzen
      cycle_errors_[i].code = types::ErrorCode::kSuccess;
      cycle_errors_[i].context.clear();
    }
  }
}

uint8_t ErrorManager::GetErrorCount() const {
  std::lock_guard<std::mutex> lock(error_mutex_);
  return cycle_error_count_;
}

void ErrorManager::FillErrorStatus(types::SystemStatusDto& dto) const {
  std::lock_guard<std::mutex> lock(error_mutex_);
  dto.error_count = cycle_error_count_;

  for (int i = 0; i < cycle_error_count_; i++) {
    dto.errors[i] =
        "[" + cycle_errors_[i].context + "] " +
        std::string(types::ErrorCodeToString(cycle_errors_[i].code));
  }
}

void ErrorManager::ClearShadowErrors() {
  std::lock_guard<std::mutex> lock(error_mutex_);

  shadow_cycle_error_count_ = 0;
  for (int i = 0; i < config::kMaxCycleErrors; i++) {
    shadow_cycle_errors_[i].code = types::ErrorCode::kSuccess;
    shadow_cycle_errors_[i].context.clear();
  }
}

}  // namespace logic
}  // namespace hardware_pruefadapter