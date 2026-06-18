/**
 * @brief Zentrale Steuereinheit der Prüfadapter-Architektur.
 *
 * Der Zentrale Knotenpunkt für das gesamte System.
 * Dieser entkoppelt die Hardware-Treiber von der Anwendungslogik und führt
 * den zyklischen EVA-Prozess (Eingabe - Verarbeitung - Ausgabe) aus.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-03-25
 * @version 2.0.0
 * @ingroup Core
 * @copyright Copyright (c) 2025 SOTEC GmBH & Co KG
 */

#ifndef HARDWARE_PRUEFADAPTER_CORE_SYSTEM_CONTROLLER_H_
#define HARDWARE_PRUEFADAPTER_CORE_SYSTEM_CONTROLLER_H_

//====================System Header===========================
#include <cstdint>
#include <mutex>
#include <string>

//===================Projekt Header===========================
#include "../../Interfaces/analog_to_digital_interface.h"
#include "../../Interfaces/digital_to_analog_interface.h"
#include "../../Interfaces/led_interface.h"
#include "../../Interfaces/serial_interface.h"
#include "../../Interfaces/web_server_interface.h"
#include "../../Platform/platform_factory.h"
#include "../Logic/Services/logger_service.h"
#include "../Services/diagnostics_service.h"
#include "../Services/error_manager.h"
#include "../Services/hardware_bootstrapper.h"
#include "../Services/led_manager_service.h"
#include "../Services/mailbox_service.h"
#include "../Services/process_imager_service.h"
#include "../Services/serial_streamer.h"
#include "../Utils/data_converter_utils.h"
#include "../Utils/moving_average_filter.h"
#include "api_types.h"
#include "error_code.h"
#include "system_config.h"
#include "system_context.h"

namespace hardware_pruefadapter {
namespace core {

/**
 * @class SystemController
 * @brief Implementiert den Ablauf der Prüflogik.
 *
 * Diese Klasse initialisiert die Hardware und steuert den zyklischen Ablauf.
 * Sie hält keine Treiber-Instanzen, sondern nutzt die im Context injizierten
 * Interface-Pointer.
 */
class SystemController {
 public:
  //==================Konstruktor/Destruktor================
  /**
   * @brief Erstellt den Controller und übernimmt den Hardware-Context.
   * @param[in] context Der Container mit allen Hardware-Treibern.
   */
  explicit SystemController(const SystemContext& context);
  ~SystemController() = default;

  /**
   * @brief Startet die Hardware und prüft die Systemintegrität.
   *
   * Führt notwendige Setup-Routinen der Treiber aus (z.B. I2C Init, ADC
   * Konfiguration). Wird einmalig in setup() aufgerufen.
   *
   * @return types::ErrorCode kSuccess oder Fehlercode bei Abbruch.
   */
  types::ErrorCode Initialize();

  /**
   * @brief Führt einen vollständigen Logik-Zyklus aus.
   *
   * Bündelt InputSequence, ProcessSequence und OutputSequence.
   * Wird zyklisch in loop() aufgerufen.
   */
  void RunSystemLogic();

 private:
  //==============Interne Sequenzen==============

  void InputSequence();
  void ProcessSequence();
  void OutputSequence();

  //==============Input Funktionen==============
  void ReadAdcInputs();
  void FetchMailboxTargets();
  void ProcessSerialRx();

  //==============Process Funktionen==============
  void CheckOvervoltageSafety();
  void CheckAndRegulateAnalogOut();
  void CheckDigitalOutSafety();
  void EvaluateSystemErrorState();

  //==============Output Funktionen==============
  void UpdateDacOutputs();
  void UpdateDigitalOutputs();
  void WriteRxDataFromMailbox();
  void SetSerialConfigFromMailbox();

  //==============Member==============
  const SystemContext ctx_;
  logic::SystemHealth health_;

  utils::MovingAverageFilter adc_filters_[config::kAdcCount]
                                         [config::kNumChannels];

  std::uint8_t digital_out_grace_timers_[config::kNumChannels]{0};
  std::uint8_t analog_out_grace_timers_[config::kNumChannels]{0};
};

}  // namespace core
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_CORE_SYSTEM_CONTROLLER_H_