/**
 * @file system_config.h
 * @brief Die Steuerzentrale für alle festen Einstellungen.
 *
 * Hier legen wir alle Zentralen Einstellungen und Werte fest.
 * Konfigurationen müssen nur hier geändert werden.
 *
 * @author Techniker_Team_2025_26
 * @date 2025-12-18
 * @version 1.0.0
 * @ingroup Core
 * @copyright Copyright (c) 2025 SOTEC GmBH & Co KG
 */

#ifndef HARDWARE_PRUEFADAPTER_CORE_SYSTEM_CONFIG_H_
#define HARDWARE_PRUEFADAPTER_CORE_SYSTEM_CONFIG_H_

//====================System Header===========================
#include <cstdint>

#include "system_types.h"

namespace hardware_pruefadapter {
namespace config {

/**
 * @name Kommunikation
 * @brief Einstellungen für Datenaustausch (Seriel, I2C, Web, WLAN).
 * @{
 */

/** @brief Geschwindigkeit der seriellen Konsole (Bits pro Sekunde). */
inline constexpr std::uint32_t kSerialBaudRate = 115200U;

/** @brief Maximale Geschwindigkeit der seriellen Konsole (Bits pro Sekunde). */
inline constexpr std::uint32_t kMaxSerialBaudRate = 115200U;

/** @brief Maximale Puffergröße */
inline constexpr std::uint16_t kSerialBufferSize = 256U;

/** @brief buffer_almost_full erreicht */
inline constexpr std::uint8_t kSerialAlmostFullBuffer = 50U;

// --- 1. I2C Bus Konfiguration ---

/** @brief Daten-Leitung (SDA) für den 1. I2C Bus (SDA42). */
inline constexpr std::uint8_t kI2c0SdaPin = 10U;

/** @brief Takt-Leitung (SCL) für den 1. I2C Bus (SCL41). */
inline constexpr std::uint8_t kI2c0SclPin = 9U;

/** @brief Bus-Identifikator für den 1. I2C Bus. (SDA42/SCL41) */
inline constexpr std::uint8_t kI2cBus0 = 0U;

// --- 2. I2C Bus Konfiguration ---

/** @brief Daten-Leitung (SDA) für den 2. I2C Bus (SDA15). */
inline constexpr std::uint8_t kI2c1SdaPin = 16U;

/** @brief Takt-Leitung (SCL) für den 2. I2C Bus (SCL16). */
inline constexpr std::uint8_t kI2c1SclPin = 15U;

/** @brief Bus-Identifikator für den 2. I2C Bus. (SDA15/SCL16) */
inline constexpr std::uint8_t kI2cBus1 = 1U;

// --- I2C Geschwindigkeiten ---

/** @brief Standard I2C Geschwindigkeit (100 kHz). */
inline constexpr std::uint32_t kI2cFrequency = 100000U;

/** @brief Schnelle I2C Geschwindigkeit (400 kHz). */
inline constexpr std::uint32_t kI2cFrequencyFast = 400000U;

/** @brief Maximale Anzahl verwalteter I2C-Geräte für die API-Liste. */
inline constexpr std::uint8_t kMaxI2cDevices = 10U;

// --- WebServer Konfiguration ---

/** @brief TCP Port des Webservers (PC-Simulation: 8080, ESP32: 80). */
#ifdef NATIVE_ENV
inline constexpr std::uint16_t kWebServerPort = 8080U;
#else
inline constexpr std::uint16_t kWebServerPort = 80U;
#endif

/** @brief Maximale Anzahl gleichzeitiger Web-Clients. */
inline constexpr std::uint8_t kWebServerMaxClients = 4U;

/** @brief Reconection Time für SSE in Millisekunden. */
inline constexpr std::uint16_t kSseReconectionTime = 1000U;

// --- WLAN Konfiguration ---

/** @brief WLAN SSID (Name des Netzwerks). */
inline const char* kSsid = "Hardware-Pruefadapter";

/** @brief WLAN Passwort. */
inline const char* kPassword = "";

/** @} */  // Ende Gruppe Kommunikation

/**
 * @name HardwareAdressen
 * @brief I2C Adressen der angeschlossenen Chips auf dem Board.
 * @{
 */

/** @brief Adresse des ADC für Analog Input. */
inline constexpr std::uint8_t kAdcI2cAddressAnalogIn = 0x49U;
/** @brief Adresse des ADC für Digital Input. */
inline constexpr std::uint8_t kAdcI2cAddressDigitalIn = 0x48U;
/** @brief Adresse des ADC für Reverse reading Output Analog. */
inline constexpr std::uint8_t kAdcI2cAddressReverseOutAnalog = 0x4AU;
/** @brief Adresse des ADC für Reverse reading Output Digital. */
inline constexpr std::uint8_t kAdcI2cAddressReverseOutDigital = 0x4BU;

/** @brief Adresse des DAC für Analog Output. */
inline constexpr std::uint8_t kDacI2cAddressAnalogOut = 0x4CU;

/** @brief Adresse des 1. LED Treibers. */
inline constexpr std::uint8_t kLedDriver1I2cAddress = 0x28U;
/** @brief Adresse des 2. LED Treibers. */
inline constexpr std::uint8_t kLedDriver2I2cAddress = 0x29U;
/** @brief Adresse des 3. LED Treibers. */
inline constexpr std::uint8_t kLedDriver3I2cAddress = 0x2AU;
/** @brief Broadcast-Adresse für alle LED Treiber gleichzeitig. */
inline constexpr std::uint8_t kLedDriverBroadcastAddress = 0x3CU;

/** @} */  // Ende Gruppe HardwareAdressen

/**
 * @name Kalibrierung und Eigenschaften
 * @brief Physikalische Eigenschaften der Hardware-Komponenten.
 * @{
 */

/** @brief Verstärkungsfaktor des ADC. */
inline constexpr std::uint16_t kGainADC = 4096;
/** @brief Anzahl der Kanäle pro Wandler. */
inline constexpr std::uint8_t kNumChannels = 4;
/** @brief Maximale Bit-Auflösung. */
inline constexpr std::uint16_t kMaximumBitResolution = 32768U;

/** @brief Noise Gate: Alles unter diesem Wert wird als 0V gewertet (in mV). */
inline constexpr std::uint16_t kNoiseGateMv = 90U;
/** @brief Maximale Systemspannung in Millivolt (Skala bis Wert). */
inline constexpr std::uint16_t kMaxSystemVoltageMv = 25000U;
/** @brief Warnschwelle in Millivolt (ab 24V). */
inline constexpr std::uint16_t kWarningVoltageMv = 24500U;

/** @brief Schwelle für Digital LOW in Prozent (<= 20% = LOW). */
inline constexpr std::uint8_t kLogicLowThresholdPercent = 20U;
/** @brief Schwelle für Digital HIGH in Prozent (>= 80% = HIGH). */
inline constexpr std::uint8_t kLogicHighThresholdPercent = 80U;
/** @brief Anzahl der Messungen für den gleitenden Mittelwert. */
inline constexpr std::uint8_t kAdcFilterSamples = 10U;
/** @brief Schonfrist für Ausgänge, um den ADC-Filter zu füllen. */
inline constexpr std::uint8_t kOutputGracePeriodCycles =
    kAdcFilterSamples + 10U;

/** @brief Erwartete Initialisierungs-Masken */
inline constexpr std::uint8_t kExpectedAdcInitMask =
    types::kChip1Init | types::kChip2Init | types::kChip3Init |
    types::kChip4Init;
inline constexpr std::uint8_t kExpectedDacInitMask = types::kChip1Init;
inline constexpr std::uint8_t kExpectedLedInitMask =
    types::kChip1Init | types::kChip2Init | types::kChip3Init;
inline constexpr std::uint8_t kExpectedSerialInitMask =
    types::kChip1Init | types::kChip2Init | types::kChip3Init;

inline constexpr std::uint8_t kAdcCount = 4U;
inline constexpr std::uint8_t kDacCount = 1U;
inline constexpr std::uint8_t kLedDriverCount = 3U;
inline constexpr std::uint8_t kSerialDriverCount = 3U;

inline constexpr std::uint16_t kAnalogControlToleranceMv = 500U;
inline constexpr std::uint8_t kAnalogErrorThresholdPercent = 10U;
inline constexpr std::uint16_t kDacReferenceMv = 5000U;

/** @brief GPIO Pins für Low Voltage (1.8V, 3.3V, 5V) */
inline constexpr std::uint8_t kDigitalOutPinsLowVolt[4] = {39, 40, 41, 42};
/** @brief GPIO Pins für High Voltage (12V, 24V) */
inline constexpr std::uint8_t kDigitalOutPinsHighVolt[4] = {12, 11, 8, 38};

/** @} */  // Ende Kalibrierung und Eigenschaften

/** @brief RX-Pin für UART0 (UART) */
inline constexpr std::uint8_t kGPIOPinRxUART0 = 7;
/** @brief TX-Pin für UART0 (UART) */
inline constexpr std::uint8_t kGPIOPinTxUART0 = 6;
/** @brief RX-Pin für UART1 (RS485) */
inline constexpr std::uint8_t kGPIOPinRxUART1 = 18;
/** @brief TX-Pin für UART1 (RS485) */
inline constexpr std::uint8_t kGPIOPinTxUART1 = 17;
/** @brief RX-Pin für UART2 (RS232)  */
inline constexpr std::uint8_t kGPIOPinRxUART2 = 14;
/** @brief TX-Pin für UART2 (RS232)  */
inline constexpr std::uint8_t kGPIOPinTxUART2 = 13;

/**
 * @name Initiale LED Treiber Konfiguration
 * @brief Einstellungen und Mapping für die LED Treiber.
 * @{
 */

/**
 * @brief Struktur für die Zuordnung von virtuellen LEDs zu physischen Treibern.
 */
struct LedMapping {
  uint8_t driver_index;  ///< 0 = Treiber 1, 1 = Treiber 2, 2 = Treiber 3
  uint8_t channel;       ///< Hardwarekanal auf dem Treiber (0 bis 5)
};

/**
 * @brief Die "Lookup Table": Welcher Index (0-16) gehört wohin?
 * Treiber haben je 6 Kanäle (0-5).
 */
inline constexpr LedMapping kLedMap[] = {
    // Index 0-3: Analog In (An.Ein-A bis An.Ein-D)
    {2, 2},  // D1001 (An.Ein-A) -> RGB_01 -> IC1003, Ch 2 (OUT6-8)
    {0, 5},  // D1005 (An.Ein-B) -> RGB_02 -> IC1001, Ch 5 (OUT15-17)
    {0, 2},  // D1009 (An.Ein-C) -> RGB_03 -> IC1001, Ch 2 (OUT6-8)
    {0, 1},  // D1013 (An.Ein-D) -> RGB_04 -> IC1001, Ch 1 (OUT3-5)

    // Index 4-7: Digital In (Dig.Ein-A bis Dig.Ein-D)
    {1, 3},  // D1002 (Dig.Ein-A) -> RGB_05 -> IC1002, Ch 3 (OUT9-11)
    {1, 4},  // D1006 (Dig.Ein-B) -> RGB_06 -> IC1002, Ch 4 (OUT12-14)
    {1, 5},  // D1010 (Dig.Ein-C) -> RGB_07 -> IC1002, Ch 5 (OUT15-17)
    {2, 4},  // D1014 (Dig.Ein-D) -> RGB_08 -> IC1003, Ch 4 (OUT12-14)

    // Index 8-11: Analog Out (An.Aus-A bis An.Aus-D)
    {2, 1},  // D1003 (An.Aus-A) -> RGB_09 -> IC1003, Ch 1 (OUT3-5)
    {0, 4},  // D1007 (An.Aus-B) -> RGB_10 -> IC1001, Ch 4 (OUT12-14)
    {0, 3},  // D1011 (An.Aus-C) -> RGB_11 -> IC1001, Ch 3 (OUT9-11)
    {0, 0},  // D1015 (An.Aus-D) -> RGB_12 -> IC1001, Ch 0 (OUT0-2)

    // Index 12-15: Digital Out (Dig.Aus-A bis Dig.Aus-D)
    {1, 2},  // D1004 (Dig.Aus-A) -> RGB_13 -> IC1002, Ch 2 (OUT6-8)
    {1, 1},  // D1008 (Dig.Aus-B) -> RGB_14 -> IC1002, Ch 1 (OUT3-5)
    {1, 0},  // D1012 (Dig.Aus-C) -> RGB_15 -> IC1002, Ch 0 (OUT0-2)
    {2, 5},  // D1016 (Dig.Aus-D) -> RGB_16 -> IC1003, Ch 5 (OUT15-17)

    // Index 16: System (Status)
    {2, 3}  // D1017 (Status)    -> RGB_17 -> IC1003, Ch 3 (OUT9-11)
};

/** @brief Bitmaske für alle Analog-In LEDs. */
inline constexpr std::uint32_t kGroupAnalogIn =
    types::kAnalogIn1 | types::kAnalogIn2 | types::kAnalogIn3 |
    types::kAnalogIn4;
/** @brief Bitmaske für alle Digital-In LEDs. */
inline constexpr std::uint32_t kGroupDigitalIn =
    types::kDigitalIn1 | types::kDigitalIn2 | types::kDigitalIn3 |
    types::kDigitalIn4;
/** @brief Bitmaske für alle Analog-Out LEDs. */
inline constexpr std::uint32_t kGroupAnalogOut =
    types::kAnalogOut1 | types::kAnalogOut2 | types::kAnalogOut3 |
    types::kAnalogOut4;
/** @brief Bitmaske für alle Digital-Out LEDs. */
inline constexpr std::uint32_t kGroupDigitalOut =
    types::kDigitalOut1 | types::kDigitalOut2 | types::kDigitalOut3 |
    types::kDigitalOut4;
/** @brief Bitmaske für System-LEDs. */
inline constexpr std::uint32_t kGroupLedSystem =
    types::kLedSystem | types::kRGBOnBoard;
/** @brief Bitmaske für alle Ein- und Ausgangs-LEDs. */
inline constexpr std::uint32_t kGroupChannelAll =
    kGroupAnalogIn | kGroupDigitalIn | kGroupAnalogOut | kGroupDigitalOut;

/** @brief Gesamtanzahl der verwalteten LEDs. */
inline constexpr int kLedCount = 17;

/** @brief Initiale Konfigurationsmaske für die LP50xx Chips. */
inline constexpr std::uint8_t kInitialLedConfig =
    types::kLedGlobalOn | types::kMaxCurrent35mA | types::kPwmDitheringOff |
    types::kAutoIncOn | types::kPowerSaveOff | types::kLogScaleOff;

/** @} */  // Ende Gruppe LED Treiber Konfiguration

/**
 * @name Logging
 * @brief Konfiguration des zentralen Log-Services.
 * @{
 */
/** @brief Maximale Anzahl der im RAM gespeicherten Log-Einträge bis das
 * Web-Terminal gestartet ist. */
inline constexpr std::uint8_t kMaxLogLines = 100U;
/** @brief Maximale Länge einer einzelnen Log-Nachricht in Zeichen. */
inline constexpr std::uint16_t kMaxLogLength = 128U;

/** @} */  // Ende Gruppe Logging

/**
 * @name TestKonfiguration
 * @brief Schalter für Simulation vs. echte Hardware und Fehler-Toleranzen.
 * @{
 */

/** @brief Maximale Anzahl gespeicherter Fehler pro Zyklus. */
inline constexpr uint8_t kMaxCycleErrors = 20U;

/** @brief Schalter für den Debug-Modus (Aktiviert detaillierte Logs). */
inline constexpr bool kDebugMode = true;

/** @} */  // Ende Gruppe Test Konfiguration

}  // namespace config
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_CORE_SYSTEM_CONFIG_H_