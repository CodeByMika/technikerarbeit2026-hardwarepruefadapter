/**
 * @file main.cpp
 * @brief Entry Point: Hier werden die Business-Logik und
 * Hardware-Initialisierung verknüpft.
 *
 * Architektur:
 * 1. Setup (Hardware & Software Komponenten instanziieren)
 * 2. Wiring (Abhängigkeiten injizieren via Constructor Injection)
 * 3. Loop (Zyklische Ausführung)
 */

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

// --- Core Logic ---
#include "ApiController/api_controller.h"
#include "SystemController/system_controller.h"
#include "system_config.h"
#include "system_context.h"
#include "system_types.h"

// --- Services ---
#include "../Logic/Services/error_manager.h"
#include "../Logic/Services/led_manager_service.h"
#include "../Logic/Services/mailbox_service.h"
#include "../Logic/Services/process_imager_service.h"
#include "../Logic/Services/serial_streamer.h"

// --- Utils ---
#include "../Logic/Utils/nonblocking_delay_utils.h"

// --- Interfaces ---
#include "../Interfaces/analog_to_digital_interface.h"
#include "../Interfaces/digital_to_analog_interface.h"
#include "../Interfaces/led_interface.h"
#include "../Interfaces/web_server_interface.h"

// --- Platform Abstraction ---
#include "platform_factory.h"

// Namespaces
namespace app = ::hardware_pruefadapter;

// =============================================================================
// ================================ LOGIC LAYER ================================
// =============================================================================

// --- Globale Instanzen ---
app::core::SystemContext sys_ctx;

std::unique_ptr<app::interfaces::IWebServer> web_server;
std::unique_ptr<app::core::SystemController> system_controller;
std::unique_ptr<app::logic::ApiController> api_controller;

// --- Serivce-Instanzen ---
std::unique_ptr<app::logic::ErrorManager> error_manager;
std::unique_ptr<app::logic::ProcessImager> process_imager;
std::unique_ptr<app::logic::Mailbox> mailbox;
std::unique_ptr<app::logic::SerialStreamer> serial_streamer;
std::unique_ptr<app::logic::LedManager> led_manager;

// --- Utility-Instanzen ---
app::utils::NonBlockingDelay loop_delay(1000);

// --- Hardware-Treiber ---
std::unique_ptr<app::interfaces::IAnalogToDigitalConverter> adc_analog_in;
std::unique_ptr<app::interfaces::IAnalogToDigitalConverter> adc_analog_out;
std::unique_ptr<app::interfaces::IAnalogToDigitalConverter> adc_digital_in;
std::unique_ptr<app::interfaces::IAnalogToDigitalConverter> adc_digital_out;
std::unique_ptr<app::interfaces::IDigitalToAnalogConverter> dac_digital_out;
std::unique_ptr<app::interfaces::ILedDriver> led_driver_1;
std::unique_ptr<app::interfaces::ILedDriver> led_driver_2;
std::unique_ptr<app::interfaces::ILedDriver> led_driver_3;
std::unique_ptr<app::interfaces::ISerialInterface> serial_0;
std::unique_ptr<app::interfaces::ISerialInterface> serial_1;
std::unique_ptr<app::interfaces::ISerialInterface> serial_2;

/**
 * @brief Initialisiert die Plattform, erstellt Hardware-Treiber und startet
 * Systemdienste.
 */
void SystemSetup() {
  // 1. Plattform initialisieren
  app::platform::Init();

  // 2. Hardware erstellen
  web_server = app::platform::CreateWebServer();
  adc_analog_in = app::platform::CreateAdc();
  adc_analog_out = app::platform::CreateAdc();
  adc_digital_in = app::platform::CreateAdc();
  adc_digital_out = app::platform::CreateAdc();
  dac_digital_out = app::platform::CreateDac();
  led_driver_1 = app::platform::CreateLedDriver();
  led_driver_2 = app::platform::CreateLedDriver();
  led_driver_3 = app::platform::CreateLedDriver();
  serial_0 = app::platform::CreateSerial(app::types::Serial::kUart);
  serial_1 = app::platform::CreateSerial(app::types::Serial::kRs485);
  serial_2 = app::platform::CreateSerial(app::types::Serial::kRs232);

  // 3. Wiring
  sys_ctx.frontend.web_server = web_server.get();

  sys_ctx.converter.adc[app::types::IO::kAnalogIn] = adc_analog_in.get();
  sys_ctx.converter.adc[app::types::IO::kDigitalIn] = adc_digital_in.get();
  sys_ctx.converter.adc[app::types::IO::kAnalogOut] = adc_analog_out.get();
  sys_ctx.converter.adc[app::types::IO::kDigitalOut] = adc_digital_out.get();

  sys_ctx.converter.dac[0] = dac_digital_out.get();

  sys_ctx.ui.led_drivers[0] = led_driver_1.get();
  sys_ctx.ui.led_drivers[1] = led_driver_2.get();
  sys_ctx.ui.led_drivers[2] = led_driver_3.get();

  sys_ctx.communication.serial[0] = serial_0.get();
  sys_ctx.communication.serial[1] = serial_1.get();
  sys_ctx.communication.serial[2] = serial_2.get();

  // =========================================================
  // 4. LOGIK-SERVICES ERSTELLEN & VERKABELN
  // =========================================================

  // 4.1 Basis-Services ohne Abhängigkeiten (Level 1)
  error_manager = std::make_unique<app::logic::ErrorManager>();
  process_imager = std::make_unique<app::logic::ProcessImager>();
  mailbox = std::make_unique<app::logic::Mailbox>();

  sys_ctx.logic.error_manager = error_manager.get();
  sys_ctx.logic.process_imager = process_imager.get();
  sys_ctx.logic.mailbox = mailbox.get();

  // 4.2 Höherwertige Services (Level 2 - brauchen den Context)
  serial_streamer = std::make_unique<app::logic::SerialStreamer>(sys_ctx);
  led_manager = std::make_unique<app::logic::LedManager>(sys_ctx);

  sys_ctx.logic.serial_streamer = serial_streamer.get();
  sys_ctx.logic.led_manager = led_manager.get();

  // =========================================================
  // 5. CONTROLLER (Level 3 - Orchestratoren)
  // =========================================================
  system_controller = std::make_unique<app::core::SystemController>(sys_ctx);

  api_controller =
      std::make_unique<app::logic::ApiController>(web_server.get(), sys_ctx);

  // 6. Starten
  api_controller->RegisterRoutes();
  app::platform::NetworkSetup();

  web_server->Start();
  system_controller->Initialize();
}

/**
 * @brief Prüft non-blocking, ob der Serielle Monitor verbunden ist
 * und auf einen Tastendruck wartet, um die Historie auszugeben.
 */
#ifdef ESP32_ENV
void HandleSerialTerminal() {
  static bool was_connected = false;
  bool is_connected = Serial;

  // Optionale Info, wenn der Monitor gerade frisch verbunden wurde
  if (is_connected && !was_connected) {
    Serial.println("\n\r[SYSTEM] Terminal verbunden. Druecke 'Enter'.");
  }
  was_connected = is_connected;

  if (is_connected && Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      while (Serial.available()) Serial.read();

      app::core::Logger::ProcessLogs([](const std::string& log_line) {
        Serial.println(log_line.c_str());
      });

      Serial.println("=================================================");
    }
  }
}
#endif

/**
 * @brief Führt zyklisch die Systemlogik aus.
 */
void SystemLoop() {
  if (system_controller != nullptr) {
    system_controller->RunSystemLogic();
  }
}

// =============================================================================
// ================== ENTRY LAYER (Technische Einstiegspunkte) =================
// =============================================================================

#ifdef ESP32_ENV
void setup() { SystemSetup(); }
void loop() {
  if (loop_delay.isReady()) {
    SystemLoop();
    HandleSerialTerminal();
  }
}
#else
int main() {
  SystemSetup();

  while (true) {
    SystemLoop();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  return 0;
}
#endif