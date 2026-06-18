# PlatformIO interne Funktion -> durch platformio.ini ausgeführt
try:
    Import("env")
    # Sicherer Weg um den Projektpfad in PlatformIO zu laden
    base_dir = env.subst("$PROJECT_DIR")
except NameError:
    import os
    base_dir = os.getcwd()
# -----------------------------------------------------------------------------

import os
import sys

web_dir = os.path.join(base_dir, "lib", "Referenz-Web")
header_file = os.path.join(base_dir, "lib", "Logic", "Web", "web_page_index.h")

# Liste aller benötigten Dateien
files_to_read = {
    "main": os.path.join(web_dir, "index.html"),
    "css": os.path.join(web_dir, "style.css"),
    "js": os.path.join(web_dir, "app.js"),
    "log_terminal": os.path.join(web_dir, "log_terminal.html"),
    "tab_analog": os.path.join(web_dir, "tabs", "tab_analog.html"),
    "tab_digital": os.path.join(web_dir, "tabs", "tab_digital.html"),
    "tab_diagnose": os.path.join(web_dir, "tabs", "tab_diagnose.html"),
    "tab_terminal": os.path.join(web_dir, "tabs", "tab_terminal.html")
}

def generate_header():
    print("\033[96m[UI Bundler]\033[0m Starte Bundling der Web-Dateien...")
    contents = {}
    
    # 1. Lese alle Dateien ein und prüfe, ob sie existieren
    for key, filepath in files_to_read.items():
        if not os.path.exists(filepath):
            print(f"\033[91m[FEHLER]\033[0m Datei nicht gefunden: {filepath}")
            print("\033[93mBitte ueberpruefe den Dateinamen und Pfad!\033[0m")
            sys.exit(1) # Bricht den PlatformIO Build ab
        
        try:
            with open(filepath, "r", encoding="utf-8") as f:
                contents[key] = f.read()
        except Exception as e:
            print(f"\033[91m[FEHLER]\033[0m Konnte {filepath} nicht lesen: {e}")
            sys.exit(1)

    try:
        # 2. Ersetze die Platzhalter im Haupt-HTML
        html = contents["main"]
        html = html.replace("/* === INJECT_CSS === */", contents["css"])
        html = html.replace("/* === INJECT_JS === */", contents["js"])
        html = html.replace("/* === INJECT_ANALOG === */", contents["tab_analog"])
        html = html.replace("/* === INJECT_DIGITAL === */", contents["tab_digital"])
        html = html.replace("/* === INJECT_DIAGNOSE === */", contents["tab_diagnose"])
        html = html.replace("/* === INJECT_TERMINAL === */", contents["tab_terminal"])
        html = html.replace("/* === INJECT_LOG === */", contents["log_terminal"])

        # 3. Baue das C++ Gerüst drumherum
        header_content = """/**
 * @file web_page_index.h
 * @brief AUTO-GENERATED FILE. DO NOT EDIT DIRECTLY!
 * * Diese Datei wird beim Kompilieren automatisch aus den Modulen in lib/Referenz-Web/ generiert.
 */

#ifndef HARDWARE_PRUEFADAPTER_CORE_WEB_PAGE_INDEX_H_
#define HARDWARE_PRUEFADAPTER_CORE_WEB_PAGE_INDEX_H_

namespace hardware_pruefadapter {
namespace core {

// Das gebuendelte Frontend (HTML, CSS, JS)
const char* kIndexHtml = R"html(""" + html + """)html";

}  // namespace core
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_CORE_WEB_PAGE_INDEX_H_
"""
        # 4. Schreibe die fertige C++ Datei
        with open(header_file, "w", encoding="utf-8") as f:
            f.write(header_content)
            
        print("\033[92m[UI Bundler]\033[0m Erfolgreich gebuendelt!")

    except Exception as e:
        print(f"\033[91m[FEHLER]\033[0m Fehler beim Zusammenbauen: {e}")
        sys.exit(1)

# Skript ausfuehren
generate_header()