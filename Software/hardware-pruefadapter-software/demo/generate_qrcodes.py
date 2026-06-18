import sys
import os
import re

try:
    import qrcode
except ImportError:
    print("Fehler: Das Paket 'qrcode' fehlt. Das Makefile sollte es eigentlich installieren.")
    sys.exit(1)

# Pfad zur C++ Konfigurationsdatei (Relativ zu diesem Skript)
CONFIG_FILE = os.path.join(os.path.dirname(__file__), '../include/system_config.h')
PORT = 3000

def get_wifi_credentials():
    """Liest SSID und Passwort dynamisch aus der system_config.h"""
    ssid = "Hardware-Pruefadapter"  # Fallback
    password = "12345678"           # Fallback
    
    try:
        with open(CONFIG_FILE, 'r', encoding='utf-8') as f:
            content = f.read()
            
            # Suche nach: inline const char* kSsid = "WERT";
            ssid_match = re.search(r'kSsid\s*=\s*"([^"]+)"', content)
            if ssid_match:
                ssid = ssid_match.group(1)
                
            # Suche nach: inline const char* kPassword = "WERT";
            pw_match = re.search(r'kPassword\s*=\s*"([^"]+)"', content)
            if pw_match:
                password = pw_match.group(1)
                
    except Exception as e:
        print(f"\n[WARNUNG] Konnte system_config.h nicht lesen: {e}")
        print("Nutze Standard-Fallbacks für QR-Code.\n")
        
    return ssid, password

def generate():
    # IP-Adresse aus den Argumenten lesen (Standard: 192.168.4.2)
    ip = sys.argv[1] if len(sys.argv) > 1 else "192.168.4.2"
    
    # Lese aktuelle Werte aus dem C++ Backend
    ssid, password = get_wifi_credentials()

    wifi_string = f"WIFI:S:{ssid};T:WPA;P:{password};;"
    url_string = f"http://{ip}:{PORT}"

    print(f"\n[QR] Lese Config... SSID: '{ssid}' | PW: '{password}'")

    print(f"[QR] Generiere WLAN-Login Code...")
    qr_wifi = qrcode.make(wifi_string)
    qr_wifi.save("demo/1_WLAN_Login.png")

    print(f"[QR] Generiere Dashboard-Link Code für URL: {url_string}")
    qr_url = qrcode.make(url_string)
    qr_url.save("demo/2_Dashboard_Link.png")

    print("\n==========================================================")
    print("✅ ERFOLGREICH!")
    print("Die beiden Bilder wurden im Hauptverzeichnis erstellt:")
    print(" -> 1_WLAN_Login.png")
    print(" -> 2_Dashboard_Link.png")
    print("Zieh sie einfach nebeneinander auf deine Präsentationsfolie.")
    print("==========================================================\n")

if __name__ == "__main__":
    generate()