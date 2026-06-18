# Einrichtung der Build-Umgebung (WSL2 & PlatformIO)

Diese Anleitung beschreibt die Einrichtung der Toolchain unter Windows 11 mittels WSL2 (Windows Subsystem for Linux). Da WSL2 standardmäßig keine USB-Geräte erkennt, nutzen wir `usbipd-win`, um den ESP32 an die Linux-Umgebung durchzureichen. 
Ziel ist es, das Projekt am Ende nahtlos mittels `make` und `platformio` kompilieren, flashen und überwachen zu können.

---

## 1. WSL2 installieren (Windows Seite)

Falls noch kein Linux-Subsystem installiert ist:

1. Öffne die **PowerShell** als Administrator.
2. Führe folgenden Befehl aus:
   ```powershell
   wsl --install
   ```

3. Starte den Computer neu.
4. Nach dem Neustart öffnet sich ein Terminal. Richte dort deinen Linux-Benutzernamen und dein Passwort ein (Standardmäßig wird Ubuntu installiert).

---

## 2. USB-Unterstützung für WSL2 einrichten (`usbipd-win`)

Damit das Linux-System den per USB angeschlossenen ESP32 (COM-Port) sehen und flashen kann, muss geprüft werden ob das Open-Source-Tool namens `usbipd-win` auf Windows installiert ist.
```powershell
PS C:\Users\USERNAME> usbipd
Required command was not provided.

usbipd-win 5.3.0

Description:
  Shares locally connected USB devices to other machines, including Hyper-V guests and WSL 2.

Usage:
  usbipd [command] [options]

Options:
  -?, -h, --help  Show help and usage information
  --version       Show version information

Commands:
  attach   Attach a USB device to a client
  bind     Bind device
  detach   Detach a USB device from a client
  license  Display license information
  list     List USB devices
  policy   Manage policy rules
  server   Run the server on the console
  state    Output state in JSON
  unbind   Unbind device
```

### 2.2 ESP32 an WSL durchreichen

Schließe nun den ESP32 per USB an deinen Windows-PC an.

1. Öffne die **PowerShell als Administrator**!
2. Liste alle angeschlossenen USB-Geräte auf:
```powershell
usbipd list
```

*Suche in der Liste nach dem ESP32 (oft als "USB JTAG/serial debug unit" oder "CP210x" gelistet). Merke dir die Hardware-ID, meistens ist das beim ESP32-S3 `303a:1001`.*

3. **Gerät für Sharing binden** (Muss nur beim allerersten Mal pro Gerät gemacht werden):
```powershell
usbipd bind --hardware-id 303a:1001
```
*(Ersetze `303a:1001` falls dein ESP32 eine andere ID hat).*

4. **Gerät an WSL anhängen (Attach)**:
Öffne parallel dein Linux-Terminal (z.B. Ubuntu) im Hintergrund. Führe dann in der **Admin-PowerShell** aus:
```powershell
usbipd attach --wsl --hardware-id 303a:1001
```

> **Wichtig:** Schritt 4 (`attach`) musst du jedes Mal wiederholen, wenn du den ESP32 vom USB-Kabel trennst und wieder einsteckst, ein Reset gemacht wird oder WSL neu startest!


## 3. Toolchain installieren (Linux Seite)

Öffne nun dein WSL-Terminal (z.B. "Ubuntu" im Startmenü). Um sicherzugehen, dass der ESP32 erkannt wird, kannst du `lsusb` eingeben. Der ESP32 sollte nun in der Liste auftauchen.

### Schritt 3.1: Systempakete vorbereiten

Aktualisiere die Paketquellen und installiere die für USB und Python benötigten Grundpakete:

```bash
sudo apt update && sudo apt install python3-venv linux-tools-virtual hwdata curl
```

### Schritt 3.2: PlatformIO Core über das Installer-Skript installieren

PlatformIO empfiehlt offiziell die Nutzung des eigenen Installer-Skripts. Dieses richtet PlatformIO sicher in einer komplett isolierten, virtuellen Python-Umgebung ein.

1. Lade das Skript herunter:
```bash
curl -fsSL -o get-platformio.py [https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py](https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py)
```


2. Führe das Skript aus:
```bash
python3 get-platformio.py
```



### Schritt 3.3: Pfad-Konfiguration (Shell Commands)

Damit der Befehl `pio` im Terminal global verfügbar ist, muss das neu erstellte Verzeichnis zur Systemvariablen `PATH` hinzugefügt werden.

Führe folgenden Befehl aus, um PlatformIO zu deiner `~/.bashrc` hinzuzufügen:

```bash
echo 'export PATH="$PATH:$HOME/.platformio/penv/bin"' >> ~/.bashrc
```

Lade die Konfiguration danach neu:

```bash
source ~/.bashrc
```

### Schritt 3.4: Installation verifizieren

Prüfe, ob PlatformIO korrekt erkannt wird:

```bash
pio --version
```

Erwartete Ausgabe: `PlatformIO Core, version x.x.x`

---

## 4. Projekt kompilieren und flashen

Navigiere in deinem WSL-Terminal in das Verzeichnis dieses Projekts (wo das `Makefile` liegt). Du bist nun bereit, die Firmware zu bauen.

Das Projekt enthält ein `Makefile`, das die PlatformIO-Befehle abstrahiert. Nutze folgende Befehle:

| Befehl | Beschreibung |
| --- | --- |
| `make build` / `make` | Kompiliert die Firmware (`firmware.bin`) für den ESP32, ohne zu flashen. |
| `make flash` | Kompiliert und flasht die Firmware auf den ESP32. Setzt voraus, dass `usbipd attach` ausgeführt wurde! |
| `make monitor` | Startet den seriellen Monitor (Standardmäßig 115200 Baud). Beenden mit `Strg + C`. |
| `make clean` | Löscht alle temporären Build-Dateien (.pio Ordner). |
| `make native` | Baut das Projekt als PC-Simulation (ohne ESP32) und führt es direkt unter Linux aus. |
| `make test` | Führt die Unit-Tests des Projekts aus und speichert die Ergebnisse in `test_output.txt`. |

> **Troubleshooting Berechtigungen (Linux):**
> Sollte WSL beim Flashen Probleme mit Zugriffsrechten auf den USB-Port (`/dev/ttyACM0` oder `/dev/ttyUSB0`) haben, füge deinen Linux-User der `dialout`-Gruppe hinzu:
> `sudo usermod -aG dialout $USER`
> *(Danach das WSL-Terminal einmal komplett schließen und neu öffnen).*