# Makefile für Hardware Pruefadapter
# Für PlatformIO Core CLI

# Installation:
# PlatformIO Core installieren
# sudo apt update && sudo apt install pipx
# pipx ensurepath
# pipx install platformio

# Wenn Probleme mit Berechtigungen des GitRepo auftreten:
# sudo chown -R $USER:$USER .

# Standard-Aktion wenn nur 'make' ausgeführt wird (baut für ESP32)
all: build

# Projekt kompilieren (Standardumgebung: ESP32)
build:
	pio run

# ==========================================
# NATIVE (SIMULATION)
# ==========================================

# Kompiliert das Projekt für den PC und startet die Simulation
native:
	pio run -e native
	./.pio/build/native/program

# Führt alle Unit-Tests aus (Terminal: Kurz, Datei: Kurz + Verbose)
test:
	@echo "--- 1. Testlauf: Übersicht für das Terminal ---"
	@echo "================ ÜBERSICHT ================" > test/test_output.txt
	-pio test -e native | tee -a test/test_output.txt
	@echo "\n\n================ VERBOSE DETAILS ================" >> test/test_output.txt
	@echo "--- 2. Testlauf: Sammle detaillierte Logs im Hintergrund ---"
	-pio test -e native -v >> test/test_output.txt 2>&1
	@echo "\nFertig! Die Datei test/test_output.txt enthält nun die Übersicht und alle Details."

# ==========================================
# ESP32
# ==========================================

# Kompiliert nur für den ESP32
esp:
	pio run -e esp32-s3-devkitc-1

# Projekt kompilieren und auf den ESP32 flashen
flash:
	pio run -e esp32-s3-devkitc-1 --target upload

# Seriellen Monitor starten
monitor:
	pio device monitor

# ==========================================
# SYSTEM BEFEHLE
# ==========================================

# Alles bereinigen (löscht alte Build-Dateien)
clean:
	pio run --target clean
	pio run --target clean --environment native

# Angeschlossene Geräte auflisten
list:
	pio device list

# ==========================================
# DEMO-MODUS (Proxy für mehrere User)
# ==========================================

# Startet den Node.js Proxy für Demos
demo:
	@echo "Pruefe Node.js Installation..."
	@which node > /dev/null || (echo "Fehler: Node.js ist nicht installiert. Bitte mit 'sudo apt install nodejs npm' installieren." && exit 1)
	@read -p "Bitte gib die WLAN IP-Adresse des ESP32 ein [Enter fuer 192.168.4.1]: " ip; \
	ip=$${ip:-192.168.4.1}; \
	echo "Starte Demo Proxy Server verbunden mit $$ip..."; \
	cd demo && npm install && node proxy.js $$ip

# Generiert QR-Codes als PNG-Dateien für Präsentationen
qrcode:
	@echo "Pruefe QR-Code Abhaengigkeiten..."
	@python3 -c "import qrcode" 2>/dev/null || (echo "Installiere python3-qrcode..." && sudo apt-get update && sudo apt-get install -y python3-qrcode)
	@read -p "Bitte gib die feste IP deines Laptops ein [Enter fuer 192.168.4.2]: " ip; \
	ip=$${ip:-192.168.4.2}; \
	python3 demo/generate_qrcodes.py $$ip

# Startet den Proxy verbunden mit der lokalen PC-Simulation
nativedemo:
	@echo "Starte Demo Proxy Server verbunden mit lokaler Simulation..."
	@echo "Pruefe Node.js Installation..."
	@which node > /dev/null || (echo "Fehler: Node.js ist nicht installiert. Bitte mit 'sudo apt install nodejs npm' installieren." && exit 1)
	cd demo && npm install && node proxy.js 127.0.0.1

.PHONY: all build native test esp flash monitor clean list demo qrcode nativedemo