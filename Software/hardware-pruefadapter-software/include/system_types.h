/**
 * @file system_config.h
 * @brief Die Steuerzentrale für alle festen Einstellungen.
 *
 * Hier legen wir alle Werte fest, die "festverdrahtet" sind.
 * Dazu gehören:
 * - Welche Pinnummern sind verwendet?
 * - Welche Frequenzen und Baudraten werden verwendet?
 * - Wie spreche ich Teilnehmer an? (Adressen)
 *
 * Configurationen müssen nur hier geändert werden.
 *
 * @author Techniker_Team_2025_26
 * @date 2025-12-18
 * @version 1.0.0
 * @ingroup Core
 * @copyright Copyright (c) 2025 SOTEC GmBH & Co KG
 */

#ifndef HARDWARE_PRUEFADAPTER_CORE_SYSTEM_TYPES_H_
#define HARDWARE_PRUEFADAPTER_CORE_SYSTEM_TYPES_H_

#include <cstdint>
#include <string>

namespace hardware_pruefadapter {
namespace types {

enum LedDriverConfigurationType {
  kLedGlobalOn = 0 << 0,
  kLedGlobalOff = 1 << 0,
  kMaxCurrent25mA = 0 << 1,
  kMaxCurrent35mA = 1 << 1,
  kPwmDitheringOff = 0 << 2,
  kPwmDitheringOn = 1 << 2,
  kAutoIncOff = 0 << 3,
  kAutoIncOn = 1 << 3,
  kPowerSaveOff = 0 << 4,
  kPowerSaveOn = 1 << 4,
  kLogScaleOff = 0 << 5,
  kLogScaleOn = 1 << 5
};

enum AddressType { kNormal, kBroadcast };

/**
 * @brief Definiert die I/O Bänke und erlaubt automatische Konvertierung in
 * Array-Indizes.
 */
struct IO {
  enum Group : std::uint8_t {
    kAnalogIn = 0,
    kDigitalIn,
    kAnalogOut,
    kDigitalOut,
    kIoGroupCount
  };

  enum Direction : std::uint8_t { kInput = 0, kOutput };

  // =================================================================
  // CONVERTER FUNKTIONEN
  // =================================================================

  /**
   * @brief Leitet die physische Richtung (IN/OUT) direkt aus der I/O-Gruppe ab.
   */
  static constexpr Direction GetDirection(Group group) {
    return (group == kAnalogIn || group == kDigitalIn) ? kInput : kOutput;
  }

  /**
   * @brief Konvertiert die Richtung in einen lesbaren String (z.B. für Logs).
   */
  static constexpr const char* ToString(Direction direction) {
    return (direction == kInput) ? "IN" : "OUT";
  }

  /**
   * @brief Konvertiert die I/O-Gruppe in einen lesbaren String (Logs/API).
   */
  static constexpr const char* ToString(Group group) {
    switch (group) {
      case kAnalogIn:
        return "AnalogIn";
      case kDigitalIn:
        return "DigitalIn";
      case kAnalogOut:
        return "AnalogOut";
      case kDigitalOut:
        return "DigitalOut";
      default:
        return "Unknown";
    }
  }
};

enum ChannelType {
  kAnalogIn = 0,
  kAnalogIn1 = (1 << 0),
  kAnalogIn2 = (1 << 1),
  kAnalogIn3 = (1 << 2),
  kAnalogIn4 = (1 << 3),

  kDigitalIn = 4,
  kDigitalIn1 = (1 << 4),
  kDigitalIn2 = (1 << 5),
  kDigitalIn3 = (1 << 6),
  kDigitalIn4 = (1 << 7),

  kAnalogOut = 8,
  kAnalogOut1 = (1 << 8),
  kAnalogOut2 = (1 << 9),
  kAnalogOut3 = (1 << 10),
  kAnalogOut4 = (1 << 11),

  kDigitalOut = 12,
  kDigitalOut1 = (1 << 12),
  kDigitalOut2 = (1 << 13),
  kDigitalOut3 = (1 << 14),
  kDigitalOut4 = (1 << 15),

  kSystem = 16,
  kLedSystem = (1 << 16),
  kRGBOnBoard = (1 << 17)
};

enum InitIndex {
  kChip1Init = (1 << 0),
  kChip2Init = (1 << 1),
  kChip3Init = (1 << 2),
  kChip4Init = (1 << 3),
  kChip5Init = (1 << 4),
  kChip6Init = (1 << 5),
  kChip7Init = (1 << 6),
  kChip8Init = (1 << 7)
};

/**
 * @brief Definiert die seriellen Schnittstellen und erlaubt automatische
 * Konvertierung.
 */
struct Serial {
  enum Interface : std::uint8_t {
    kUart = 0,
    kRs485 = 1,
    kRs232 = 2,
    kInterfaceCount
  };

  // =================================================================
  // CONVERTER FUNKTIONEN
  // =================================================================

  /**
   * @brief Konvertiert die Schnittstelle in einen ausführlichen String (z.B.
   * für Logs).
   */
  static constexpr const char* ToString(Interface iface) {
    switch (iface) {
      case kUart:
        return "UART";
      case kRs485:
        return "RS485";
      case kRs232:
        return "RS232";
      default:
        return "Unknown";
    }
  }
};

/** @brief Abstraktion der Parität (Parity) für serielle Daten. */
enum class SerialParity { kNone, kEven, kOdd };

/** @brief Abstraktion der Stoppbits. */
enum class SerialStopBits { kOne, kTwo };

/** @brief Abstraktion der Datenbits (meist 8). */
enum class SerialDataBits { kFive, kSix, kSeven, kEight };

/**
 * @brief Ringpuffer/Speicher für eingehende serielle Daten.
 * @tparam BufferSize Die maximale Anzahl an Bytes, die der Puffer halten kann.
 */
template <std::size_t BufferSize>
struct SerialRxBuffer {
  std::uint8_t data[BufferSize]{0};  // Speicher für den Daten-Chunk
  std::size_t length = 0;            // Aktuelle Anzahl an Bytes im Puffer
  std::uint32_t last_rx_time = 0;  // Zeitstempel des letzten empfangenen Bytes
};

/** @brief Datensatz für ein einzelnes I2C-Gerät */
struct I2cDeviceInfo {
  std::string address;
  std::uint8_t bus;
  std::string name;
};

/** * @brief Liste mehrerer I2C-Geräte (Generische Größe)
 * @tparam MaxSize Die maximale Anzahl an Geräten im Array.
 */
template <std::size_t MaxSize>
struct I2cDeviceList {
  std::uint8_t count = 0;
  I2cDeviceInfo devices[MaxSize];
};

}  // namespace types
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_CORE_SYSTEM_TYPES_H_