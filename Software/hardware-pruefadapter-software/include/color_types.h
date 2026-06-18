/**
 * @file color_types.h
 * @brief Konstrukt der Farben und Liste aller Farbnamen.
 *
 * Diese Datei enthält zwei Dinge:
 * 1. Einen Struct, um Rot-, Grün- und Blau-Werte zu speichern.
 * 2. Eine Liste (Enum) mit verständlichen Namen.
 *
 * @author Techniker_Team_2025_26
 * @date 2025-12-18
 * @version 1.0.0
 * @ingroup Types
 * @copyright Copyright (c) 2025 SOTEC GmBH & Co KG
 */

#ifndef HARDWARE_PRUEFADAPTER_TYPES_COLOR_TYPES_H_
#define HARDWARE_PRUEFADAPTER_TYPES_COLOR_TYPES_H_

//====================System Header===========================
#include <cstdint>
#include <string_view>

namespace hardware_pruefadapter {
namespace types {

/**
 * @brief Das Datenpaket für eine Farbe (Rot, Grün, Blau).
 *
 * Wertebereich von 0..255 je Farbanteil
 */
struct RgbColor {
  std::uint8_t red;    // Rot-Anteil
  std::uint8_t green;  // Grün-Anteil
  std::uint8_t blue;   // Blau-Anteil
};

/**
 * @brief Liste aller erlaubten Farbnamen im Projekt.
 *
 * Im Code können die Namen der Farben verwendet werden, das macht ihn lesbarer
 * und verhindert fehler. Dabei können auch Statusfarben und Sprechende namen
 * verwendet werden.
 */
enum class ColorName {
  //=========Basis-Farben=========
  kBlack,
  kWhite,

  //=========Rot-Töne (Warnung, Fehler, Hitze)=========
  kDarkRed,
  kRed,

  //=========Orange-Töne (Warnung, Aktivität)=========
  kOrangeRed,
  kTomato,
  kCoral,
  kDarkOrange,
  kOrange,

  //=========Gelb-Töne (Info, Wartung)=========
  kGold,
  kYellow,
  kLightYellow,
  kKhaki,
  kDarkKhaki,

  //=========Violett/Lila-Töne (Systemstatus, Boot)=========
  kLavender,
  kViolet,
  kMagenta,
  kMediumOrchid,
  kMediumPurple,
  kPurple,
  kIndigo,

  //=========Grün-Töne (Erfolg, Idle, Verbunden)=========
  kGreen,
  kDarkGreen,
  kYellowGreen,
  kOlive,
  kDarkOliveGreen,

  //=========Blau/Cyan-Töne (Netzwerk)=========
  kAqua,
  kCyan,
  kLightCyan,
  kAquamarine,
  kLightBlue,
  kSkyBlue,
  kRoyalBlue,
  kBlue,
  kMediumBlue,
  kDarkBlue,
  kNavy,
  kMidnightBlue,

  //=========Braun-Töne (Keine eindeutige Zuweisung)=========
  kWheat,
  kPeru,
  kChocolate,
  kBrown,
  kMaroon,

  //=========Status-Farben (Logische Bedeutung)=========
  kStatusOk,             // Standard: Grün
  kStatusWarning,        // Standard: Orange
  kStatusError,          // Standard: Rot
  kStatusIdle,           // Standard: Blau
  kStatusConnecting,     // Standard: Cyan
};

/**
 * @brief Übersetzt einen Farb-Namen in echte RGB-Zahlenwerte.
 *
 * Diese Übersetzung wird durch 'constexpr' komplett während
 * der Kompilierung durchgeführt. Das kostet zur Laufzeit
 * keine Rechenleistung.
 *
 * @param[in] color Der Name aus der Liste ColorName.
 * @return RgbColor Das Paket mit den drei Zahlenwerten.
 */
constexpr RgbColor GetRgbFromColorName(ColorName color) {
  switch (color) {
    //=========Basis-Farben=========
    case ColorName::kBlack:
      return {0, 0, 0};
    case ColorName::kWhite:
      return {255, 255, 255};

    //=========Rot-Töne=========
    case ColorName::kDarkRed:
      return {139, 0, 0};
    case ColorName::kRed:
      return {255, 0, 0};

    //=========Orange-Töne=========
    case ColorName::kOrangeRed:
      return {255, 69, 0};
    case ColorName::kTomato:
      return {255, 99, 71};
    case ColorName::kCoral:
      return {255, 127, 80};
    case ColorName::kDarkOrange:
      return {255, 140, 0};
    case ColorName::kOrange:
      return {255, 165, 0};

    //=========Gelb-Töne=========
    case ColorName::kGold:
      return {255, 215, 0};
    case ColorName::kYellow:
      return {255, 255, 0};
    case ColorName::kLightYellow:
      return {255, 255, 224};
    case ColorName::kKhaki:
      return {240, 230, 140};
    case ColorName::kDarkKhaki:
      return {189, 183, 107};

    //=========Violett/Lila-Töne=========
    case ColorName::kLavender:
      return {230, 230, 250};
    case ColorName::kViolet:
      return {238, 130, 238};
    case ColorName::kMagenta:
      return {255, 0, 255};
    case ColorName::kMediumOrchid:
      return {186, 85, 211};
    case ColorName::kMediumPurple:
      return {147, 112, 219};
    case ColorName::kPurple:
      return {128, 0, 128};
    case ColorName::kIndigo:
      return {75, 0, 130};

    //=========Grün-Töne=========
    case ColorName::kGreen:
      return {0, 128, 0};
    case ColorName::kDarkGreen:
      return {0, 100, 0};
    case ColorName::kYellowGreen:
      return {154, 205, 50};
    case ColorName::kOlive:
      return {128, 128, 0};
    case ColorName::kDarkOliveGreen:
      return {85, 107, 47};

    //=========Blau/Cyan-Töne=========
    case ColorName::kAqua:
      return {0, 255, 255};
    case ColorName::kCyan:
      return {0, 255, 255};
    case ColorName::kLightCyan:
      return {224, 255, 255};
    case ColorName::kAquamarine:
      return {127, 255, 212};
    case ColorName::kLightBlue:
      return {173, 216, 230};
    case ColorName::kSkyBlue:
      return {135, 206, 235};
    case ColorName::kRoyalBlue:
      return {65, 105, 225};
    case ColorName::kBlue:
      return {0, 0, 255};
    case ColorName::kMediumBlue:
      return {0, 0, 205};
    case ColorName::kDarkBlue:
      return {0, 0, 139};
    case ColorName::kNavy:
      return {0, 0, 128};
    case ColorName::kMidnightBlue:
      return {25, 25, 112};

    //=========Braun-Töne=========
    case ColorName::kWheat:
      return {245, 222, 179};
    case ColorName::kPeru:
      return {205, 133, 63};
    case ColorName::kChocolate:
      return {210, 105, 30};
    case ColorName::kBrown:
      return {165, 42, 42};
    case ColorName::kMaroon:
      return {128, 0, 0};

    //=========Status-Farben=========
    case ColorName::kStatusOk:
      return {0, 255, 0};
    case ColorName::kStatusWarning:
      return {255, 165, 0};
    case ColorName::kStatusError:
      return {255, 0, 0};
    case ColorName::kStatusIdle:
      return {0, 0, 255};
    case ColorName::kStatusConnecting:
      return {0, 255, 255};

    default:
      return {0, 0, 0};
  }
}

constexpr std::string_view ColorNameToString(ColorName color) {
  switch (color) {
    //=========Basis-Farben=========
    case ColorName::kBlack:
      return "Schwarz";
    case ColorName::kWhite:
      return "Weiß";

    //=========Rot-Töne (Warnung, Fehler, Hitze)=========
    case ColorName::kDarkRed:
      return "Dunkelrot";
    case ColorName::kRed:
      return "Rot";

    //=========Orange-Töne (Warnung, Aktivität)=========
    case ColorName::kOrangeRed:
      return "Orangerot";
    case ColorName::kTomato:
      return "Tomatenrot";
    case ColorName::kCoral:
      return "Koralle";
    case ColorName::kDarkOrange:
      return "Dunkelorange";
    case ColorName::kOrange:
      return "Orange";

    //=========Gelb-Töne (Info, Wartung)=========
    case ColorName::kGold:
      return "Gold";
    case ColorName::kYellow:
      return "Gelb";
    case ColorName::kLightYellow:
      return "Hellgelb";
    case ColorName::kKhaki:
      return "Khaki";
    case ColorName::kDarkKhaki:
      return "Dunkelkhaki";

    //=========Violett/Lila-Töne (Systemstatus, Boot)=========
    case ColorName::kLavender:
      return "Lavendel";
    case ColorName::kViolet:
      return "Violett";
    case ColorName::kMagenta:
      return "Magenta";
    case ColorName::kMediumOrchid:
      return "Orchidee";
    case ColorName::kMediumPurple:
      return "Mittelviolett";
    case ColorName::kPurple:
      return "Lila";
    case ColorName::kIndigo:
      return "Indigo";

    //=========Grün-Töne (Erfolg, Idle, Verbunden)=========
    case ColorName::kGreen:
      return "Grün";
    case ColorName::kDarkGreen:
      return "Dunkelgrün";
    case ColorName::kYellowGreen:
      return "Gelbgrün";
    case ColorName::kOlive:
      return "Olivgrün";
    case ColorName::kDarkOliveGreen:
      return "Dunkelolivgrün";

    //=========Blau/Cyan-Töne (Netzwerk)=========
    case ColorName::kAqua:
      return "Aqua";
    case ColorName::kCyan:
      return "Cyan";
    case ColorName::kLightCyan:
      return "Hellcyan";
    case ColorName::kAquamarine:
      return "Aquamarin";
    case ColorName::kLightBlue:
      return "Hellblau";
    case ColorName::kSkyBlue:
      return "Himmelblau";
    case ColorName::kRoyalBlue:
      return "Königsblau";
    case ColorName::kBlue:
      return "Blau";
    case ColorName::kMediumBlue:
      return "Mittelblau";
    case ColorName::kDarkBlue:
      return "Dunkelblau";
    case ColorName::kNavy:
      return "Marineblau";
    case ColorName::kMidnightBlue:
      return "Mitternachtsblau";

    //=========Braun-Töne (Keine eindeutige Zuweisung)=========
    case ColorName::kWheat:
      return "Weizen";
    case ColorName::kPeru:
      return "Perubraun";
    case ColorName::kChocolate:
      return "Schokolade";
    case ColorName::kBrown:
      return "Braun";
    case ColorName::kMaroon:
      return "Kastanienbraun";

    //=========Status-Farben (Logische Bedeutung)=========
    case ColorName::kStatusOk:             // Standard: Grün
      return "Status OK";
    case ColorName::kStatusWarning:        // Standard: Orange
      return "Status Warnung";
    case ColorName::kStatusError:          // Standard: Rot
      return "Status Fehler";
    case ColorName::kStatusIdle:           // Standard: Blau
      return "Status Leerlauf";
    case ColorName::kStatusConnecting:     // Standard: Cyan
      return "Status Verbindet";

    default:
      return "UNBEKANNTER FARBCODE";
  }
}

}  // namespace types
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_TYPES_COLOR_TYPES_H_