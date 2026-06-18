/**
 * @file system_context.h
 * @brief Der zentrale Verteiler für alle Hardware-Module.
 *
 * Diese Struktur dient als zentraler Zugriffspunkt. Dies dient der
 * Übersichtlichkeit beim Übergeben der Pointer im Hauptprogramm.
 * Die eigentlichen Objekte werden beim Systemstart (in main) erzeugt
 * und hier nur verlinkt.
 *
 * @author Techniker_Team_2025_26
 * @date 2025-12-18
 * @version 1.0.0
 * @ingroup Core
 * @copyright Copyright (c) 2025 SOTEC GmBH & Co KG
 */

#ifndef HARDWARE_PRUEFADAPTER_CORE_SYSTEM_CONTEXT_H_
#define HARDWARE_PRUEFADAPTER_CORE_SYSTEM_CONTEXT_H_

#include "system_config.h"
#include "system_types.h"

// Wir sagen dem Compiler nur: "Es gibt diese Klassen", ohne die Datei zu laden.
namespace hardware_pruefadapter {
namespace adapters {
class EspAsyncServerAdapter;
class NativeWebServerAdapter;
class Ads1x15Adapter;
class SerialAdapter;
}  // namespace adapters

namespace interfaces {
class IAnalogToDigitalConverter;
class IWebServer;
class ILedDriver;
class IDigitalToAnalogConverter;
class ISerialInterface;
}  // namespace interfaces

namespace logic {
class ErrorManager;
class ProcessImager;
class Mailbox;
class SerialStreamer;
class LedManager;
}  // namespace logic

namespace core {

/**
 * @brief Container für alle Hardware-Treiber-Verbindungen.
 *
 * Dient als "Telefonbuch": Wer Hardware braucht, schlägt hier nach.
 * Dies ist ein struct (Datencontainer)
 */
struct SystemContext {
  /**
   * @brief Gruppe für alle Umwandler (Analog/Digital).
   */
  struct Converter {
    /** @brief ADCs für die verschiedenen I/O Bänke (Indiziert über
     * types::IO::Group) */
    interfaces::IAnalogToDigitalConverter* adc[config::kAdcCount]{};

    /** @brief DAC für die analogen Ausgänge */
    interfaces::IDigitalToAnalogConverter* dac[config::kDacCount]{};
  } converter;

  /**
   * @brief Gruppe für Benutzerschnittstellen (LEDs, Weboberfläche).
   */
  struct UI {
    /** @brief Die LED-Treiber-Chips. Zuordnung der LEDs erfolgt über die
     * LedMap. */
    interfaces::ILedDriver* led_drivers[config::kLedDriverCount]{};
  } ui;

  /**
   * @brief Gruppe für serielle Kommunikation (UART, RS485, RS232).
   */
  struct Communication {
    /** @brief Die Hardware-Schnittstellen (0=UART, 1=RS485, 2=RS232). */
    interfaces::ISerialInterface* serial[config::kSerialDriverCount]{};
  } communication;

  /**
   * @brief Gruppe für Statusanzeige und Steuerung.
   */
  struct Frontend {
    interfaces::IWebServer* web_server = nullptr;
  } frontend;

  struct Logic {
    logic::ErrorManager* error_manager = nullptr;
    logic::ProcessImager* process_imager = nullptr;
    logic::Mailbox* mailbox = nullptr;
    logic::SerialStreamer* serial_streamer = nullptr;
    logic::LedManager* led_manager = nullptr;
  } logic;
};

}  // namespace core
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_CORE_SYSTEM_CONTEXT_H_