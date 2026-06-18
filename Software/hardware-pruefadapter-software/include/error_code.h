/**
 * @file error_code.h
 * @brief Liste aller Fehlercodes und deren Übersetzung in Text.
 *
 * Diese Datei ist das "Wörterbuch" für Fehler im System.
 * Sie definiert:
 * 1. Eine Liste mit Nummern für jeden möglichen Fehler
 * 2. Eine Funktion, die aus der Nummer einen lesbaren Text macht.
 *
 * @author Techniker_Team_2025_26
 * @date 2025-12-18
 * @version 1.0.0
 * @ingroup Types
 * @copyright Copyright (c) 2025 SOTEC GmBH & Co KG
 */

#ifndef HARDWARE_PRUEFADAPTER_TYPES_ERROR_CODE_H_
#define HARDWARE_PRUEFADAPTER_TYPES_ERROR_CODE_H_

//====================System Header===========================
#include <string_view>
#include <string>

namespace hardware_pruefadapter {
namespace types {


/**
 * @brief Liste aller definierten Zustände und Fehler.
 */
enum class ErrorCode {
  // ======================GENERELL============================
  kSuccess = 0,          // Operation erfolgreich.
  kErrorProgramming,     // Programmierfehler (default beim Einlesen)
  kErrorNotImplemented,  // Nicht implementierte Funktion
  kErrorNullptrFailure,  // Nullpointer Überprüfung fehlgeschlagen
  kErrorInitWarning,     // Initialisierung mit Warnungen (z.B. Teilweise
                         // Hardware-Fehler)
  kErrorMaxCycle,        // Maximale Anzahl von Fehlern, die in einem Zyklus
                         // gesammelt werden können.

  //========================DEVICE=============================
  kErrorDeviceNotFound,     // Keine Antwort vom Chip, Hardwaredefekt?
  kErrorInitializeFailure,  // Initialiserung oder .begin fehlgeschlagen
  kErrorInvalidPinConfig,   // Falsches Adressformat
  kErrorInvalidChannel,     // Kanal außerhalb des Bereichs
  kErrorInvalidValue,       // Wert außerhalb des Bereichs
  kErrorInvalidFormat,      // Falsche Formatierung/Falsches Zeichen
  kErrorInvalidConfig,      // Ungültige Konfiguration (z.B. Differential Mode +
                            // GetValue)
  kErrorNackReceived,       // I2C NACK erhalten (z.B. falsche Adresse)
  kErrorI2cCommunication,   // Allgemeiner I2C Kommunikationsfehler

  // ======================Status============================
  kStatusDeviceConnected,  // Gerät ist verbunden
  kStatusDeviceBusy,       // Gerät ist beschäftigt
  kStatusDeviceError,      // Gerät hat einen Fehler festgestellt
};

/**
 * @brief Ein Event, das den Fehler und das betroffene Bauteil speichert.
 */
struct ErrorEvent {
  std::string context;
  ErrorCode code;
};

/**
 * @brief Wandelt den ErrorCode in einen lesbaren String um.
 *
 * Diese Funktion wird zur Kompilierzeit in den Speicher geschrieben.
 * Durch den constexpr dateityp kostet das zur Laufzeit keine Rechenleistung,
 * die Fehlercodes sind schon bekannt.
 *
 * @param[in] code Der Fehlercode, der übersetzt werden soll.
 * @return std::string Der dazugehörige Fehlertext.
 */
constexpr std::string_view ErrorCodeToString(ErrorCode code) {
  switch (code) {
    // ======================GENERELL============================
    case ErrorCode::kSuccess:
      return "Success";
    case ErrorCode::kErrorProgramming:
      return "Programmier Fehler";
    case ErrorCode::kErrorNotImplemented:
      return "Nicht implementierte Funktion";
    case ErrorCode::kErrorNullptrFailure:
      return "Nullpointer Überprüfung fehlgeschlagen";
    case ErrorCode::kErrorInitWarning:
      return "Initialisierung fehlgeschlagen (z.B. Teilweise Hardware-Fehler)";
    case ErrorCode::kErrorMaxCycle:
      return "Maximale Anzahl von Fehlern in einem Zyklus erreicht oder "
             "überschritten";

    //========================DEVICE=============================
    case ErrorCode::kErrorDeviceNotFound:
      return "Device nicht gefunden oder antwortet nicht";
    case ErrorCode::kErrorInitializeFailure:
      return "Initialisierung des Objektes fehlgeschlagen.";
    case ErrorCode::kErrorInvalidPinConfig:
      return "Fehlerhafte HEX Adresse";
    case ErrorCode::kErrorInvalidChannel:
      return "Kanalauswahl außerhalb des Bereichs";
    case ErrorCode::kErrorInvalidValue:
      return "Wert außerhalb des Bereichs";
    case ErrorCode::kErrorInvalidFormat:
      return "Ungültiges Zeichen eingegeben";
    case ErrorCode::kErrorInvalidConfig:
      return "Ungültige Konfiguration";
    case ErrorCode::kErrorNackReceived:
      return "Nack Erhalten";
    case ErrorCode::kErrorI2cCommunication:
      return "Allgemeines I2C Kommunikations Problem";

    // ======================Status============================
    case ErrorCode::kStatusDeviceConnected:
      return "running";
    case ErrorCode::kStatusDeviceBusy:
      return "busy";
    case ErrorCode::kStatusDeviceError:
      return "error";

    default:
      return "UNBEKANNTER FEHLERCODE";
  }
}

}  // namespace types
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_TYPES_ERROR_CODE_H_