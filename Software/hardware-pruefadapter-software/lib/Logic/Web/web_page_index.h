/**
 * @file web_page_index.h
 * @brief AUTO-GENERATED FILE. DO NOT EDIT DIRECTLY!
 * * Diese Datei wird beim Kompilieren automatisch aus den Modulen in lib/Referenz-Web/ generiert.
 */

#ifndef HARDWARE_PRUEFADAPTER_CORE_WEB_PAGE_INDEX_H_
#define HARDWARE_PRUEFADAPTER_CORE_WEB_PAGE_INDEX_H_

namespace hardware_pruefadapter {
namespace core {

// Das gebuendelte Frontend (HTML, CSS, JS)
const char* kIndexHtml = R"html(<!DOCTYPE html>
<html lang="de">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Prozesssteuerung Dashboard</title>
    <style>
        /* --- KONFIGURATION & VARIABLEN --- */
:root {
    --bg-dark: #2b2b40;
    --bg-card: #3e3e5e;
    --bg-navigation: #232336;
    --text-main: #ffffff;
    --text-info: #aaaaaa;
    --text-resolve: #00d2ff;
    --accent: #436EB2;
    --success: #28a745;
    --warning: #ffcc00;
    --danger: #dc3545;
    --float: #555;
    --toggle-off: #ccc;
    --terminal-bg: #1e1e2e;
    --terminal-text: #00ff9d;
    --terminal-tx: #ffcc00;
}

/* ==========================================================================
   1. GRUNDLAYOUT & STRUKTUR
   ========================================================================== */
body {
    margin: 0;
    font-family: "Source Sans Pro", "Helvetica Neue", "Calibri", arial, sans-serif;
    background-color: var(--bg-dark);
    color: var(--text-main);
    display: flex;
    flex-direction: column;
    height: 100vh;
    font-size: 14px;
}

header {
    background-color: var(--accent);
    padding: 5px 20px;
    text-align: center;
    box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
    z-index: 10;
}

header h1 {
    margin: 5px 0;
    font-size: 1.3rem;
}

footer {
    background-color: #1a1a2e;
    text-align: center;
    padding: 5px;
    font-size: 0.85rem;
    margin-top: auto;
    color: var(--text-info);
    border-top: 1px solid rgba(255, 255, 255, 0.1);
}

footer.simulation {
    background-color: var(--warning);
    color: black;
    font-weight: bold;
}

.main-container {
    display: flex;
    flex: 1;
    overflow: hidden;
}

.content {
    flex: 1;
    padding: 0 15px 15px 15px;
    overflow-y: auto;
    scroll-behavior: smooth;
    display: flex;
    flex-direction: column;
}

.navigation {
    width: 180px;
    background-color: var(--bg-navigation);
    padding: 10px 0;
    display: flex;
    flex-direction: column;
    z-index: 5;
}

.navigation a {
    color: var(--text-info);
    text-decoration: none;
    padding: 12px 15px;
    border-bottom: 1px solid rgba(255, 255, 255, 0.05);
    cursor: pointer;
    transition: all 0.2s;
}

.navigation a:hover {
    background-color: rgba(67, 110, 178, 0.3);
    color: var(--text-main);
}

.navigation a.active {
    background-color: var(--accent);
    color: white;
    font-weight: bold;
    border-left: 4px solid white;
}

/* ==========================================================================
   2. KOMPONENTEN: LAYOUT & CONTAINER
   ========================================================================== */
.tab-pane {
    display: block;
    margin-bottom: 20px;
    animation: fadeIn 0.3s ease-in-out;
}

@keyframes fadeIn {
    from {
        opacity: 0;
        transform: translateY(5px);
    }

    to {
        opacity: 1;
        transform: translateY(0);
    }
}

.persistent-pane {
    padding-top: 5px;
}

#persistent-system {
    position: sticky;
    top: 0;
    margin-top: 0;
    padding-top: 10px;
    padding-bottom: 10px;
    background-color: var(--bg-dark);
    z-index: 50;
    border-bottom: 1px solid rgba(255, 255, 255, 0.1);
}

.card-section {
    display: flex;
    flex-wrap: wrap;
    gap: 20px;
}

.card {
    background-color: var(--bg-card);
    border-radius: 6px;
    padding: 10px;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    flex: 1;
    min-width: 180px;
    display: flex;
    flex-direction: column;
    gap: 5px;
}

.card .value-display {
    margin-top: auto;
}

.status-items {
    display: flex;
    flex-wrap: wrap;
    gap: 15px;
    align-items: center;
}

/* ==========================================================================
   3. KOMPONENTEN: TYPOGRAFIE & TEXT
   ========================================================================== */
.section-title {
    border-bottom: 2px solid var(--accent);
    padding-bottom: 3px;
    margin-bottom: 10px;
    margin-top: 10px;
    font-size: 1.1rem;
}

#persistent-system .section-title {
    margin-top: 0;
}

.card h3 {
    margin: 0 0 5px 0;
    font-size: 0.95rem;
}

.control-label {
    font-size: 0.8rem;
    color: var(--text-info);
}

.card>.control-label {
    margin-top: 10px;
    display: block;
}

.input-group .control-label {
    margin: 0;
    display: inline;
    font-size: 0.9rem;
}

.value-display {
    font-size: 1.4em;
    font-weight: bold;
    font-variant-numeric: tabular-nums;
    display: flex;
    align-items: center;
    gap: 15px;
}

/* ==========================================================================
   4. KOMPONENTEN: CONTROLS (INPUTS, BUTTONS)
   ========================================================================== */
.control-input,
.control-select {
    background-color: var(--bg-navigation);
    color: white;
    border: 1px solid #555;
    padding: 4px 6px;
    border-radius: 4px;
    font-size: 0.85rem;
    box-sizing: border-box;
    width: 100%;
}

.input-group {
    display: flex;
    gap: 5px;
    align-items: center;
    flex-wrap: wrap;
}

.input-group .control-input {
    width: 70px;
}

input[type="range"] {
    accent-color: var(--accent);
    cursor: pointer;
    margin: 5px 0;
}

.btn {
    background-color: var(--accent);
    color: white;
    border: none;
    padding: 4px 8px;
    border-radius: 4px;
    cursor: pointer;
    transition: 0.2s;
}

.btn:hover {
    opacity: 0.9;
}

.btn-secondary {
    background-color: #555;
    color: white;
    border: none;
    padding: 4px 8px;
    border-radius: 4px;
    cursor: pointer;
    transition: 0.2s;
}

.btn-secondary:hover {
    background-color: #777;
}

/* ==========================================================================
   5. KOMPONENTEN: INDIKATOREN (LEDS, PROGRESS, BADGES)
   ========================================================================== */
.led {
    width: 20px;
    height: 20px;
    border-radius: 50%;
    display: inline-block;
    transition: 0.3s;
    margin-right: 15px;
}

.led.off {
    background-color: #555;
    box-shadow: none;
}

.led.on {
    background-color: var(--success);
    box-shadow: 0 0 8px var(--success);
}

.led.fail {
    background-color: var(--danger);
    box-shadow: 0 0 8px var(--danger);
}

progress {
    width: 100%;
    height: 10px;
    accent-color: var(--success);
    margin-top: 2px;
}

.progress-track {
    width: 100%;
    height: 10px;
    background-color: rgba(0, 0, 0, 0.3);
    border-radius: 5px;
    margin-top: 5px;
    overflow: hidden;
    box-shadow: inset 0 1px 3px rgba(0, 0, 0, 0.5);
}

.progress-fill {
    height: 100%;
    width: 0%;
    background-color: var(--success);
    transition: width 0.3s ease-out, background-color 0.3s ease;
}

.progress-fill.error {
    background-color: var(--danger);
}

progress.error {
    accent-color: var(--danger);
}

progress.error::-webkit-progress-value {
    background-color: var(--danger);
}

progress.error::-moz-progress-bar {
    background-color: var(--danger);
}

.status-badge {
    display: inline-block;
    padding: 4px 10px;
    border-radius: 3px;
    font-size: 0.7em;
    font-weight: bold;
    text-transform: uppercase;
}

.status-badge.high {
    background-color: var(--success);
    color: white;
}

.status-badge.low {
    background-color: var(--float);
    color: white;
}

.status-badge.error {
    background-color: var(--danger);
    color: white;
}

.error-list {
    margin: 5px 0 0 20px;
    padding: 0;
    list-style-type: square;
    color: var(--danger);
    font-size: 0.8rem;
    max-height: 40px;
    overflow-y: auto;
    display: none;
}

.error-list.active {
    display: block;
}

.error-list li {
    margin-bottom: 2px;
}

/* ==========================================================================
   6. KOMPONENTEN: SPEZIAL (SWITCH & TERMINAL)
   ========================================================================== */
.digital-out-card {
    flex-direction: row !important;
    padding: 0 !important;
    overflow: hidden;
}

.digital-out-content {
    flex: 1;
    padding: 10px;
    display: flex;
    flex-direction: column;
    gap: 5px;
}

.switch-vertical-container {
    width: 50px;
    background-color: rgba(0, 0, 0, 0.15);
    border-left: 1px solid rgba(255, 255, 255, 0.05);
    display: flex;
    justify-content: center;
    align-items: center;
    padding: 10px 0;
}

.switch-vertical {
    position: relative;
    display: inline-block;
    width: 24px;
    height: 100%;
    min-height: 70px;
}

.switch-vertical input {
    opacity: 0;
    width: 0;
    height: 0;
}

.slider-vertical {
    position: absolute;
    cursor: pointer;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background-color: var(--toggle-off);
    transition: .4s;
    border-radius: 24px;
}

.slider-vertical:before {
    position: absolute;
    content: "";
    height: 18px;
    width: 18px;
    left: 3px;
    bottom: 3px;
    background-color: white;
    transition: .4s;
    border-radius: 50%;
    box-shadow: 0 2px 5px rgba(0, 0, 0, 0.3);
}

.switch-vertical input:checked+.slider-vertical {
    background-color: var(--success);
}

.switch-vertical input:checked+.slider-vertical:before {
    bottom: calc(100% - 21px);
}

.toolbar {
    background: rgba(0, 0, 0, 0.2);
    padding: 5px 10px;
    border-radius: 4px;
    display: flex;
    flex-wrap: wrap;
    gap: 15px;
    align-items: center;
}

.toolbar-group {
    display: flex;
    align-items: center;
    gap: 5px;
}

.toolbar-group .control-select {
    width: auto;
}

.toolbar-actions {
    margin-left: auto;
    display: flex;
    gap: 10px;
}

.terminal-log {
    background-color: var(--terminal-bg);
    color: var(--terminal-text);
    font-family: 'Courier New', monospace;
    height: 120px;
    overflow-y: auto;
    padding: 5px;
    border-radius: 4px;
    border: 1px solid #555;
    font-size: 0.85rem;
    white-space: pre-wrap;
}

.log-entry-rx {
    color: var(--terminal-text);
}

.log-entry-tx {
    color: var(--terminal-tx);
}

.log-entry-sys {
    color: var(--text-info);
    font-style: italic;
}

.log-entry-resolve {
    color: var(--text-resolve);
    font-weight: bold;
}

.log-time {
    color: #6e6e8e;
    margin-right: 8px;
    font-size: 0.85em;
}

.log-info {
    color: #a6e22e;
}

.log-warn {
    color: var(--warning);
}

.log-error {
    color: var(--danger);
    font-weight: bold;
}

/* ==========================================================================
   7. RESPONSIVE (MOBILE)
   ========================================================================== */
@media (max-width: 768px) {
    .main-container {
        flex-direction: column;
    }

    .navigation {
        width: 100%;
        flex-direction: row;
        overflow-x: auto;
        padding: 0;
        box-shadow: 0 2px 5px rgba(0, 0, 0, 0.3);
    }

    .navigation a {
        flex: 1;
        text-align: center;
        border: none;
        border-bottom: 3px solid transparent;
        font-size: 0.9rem;
        padding: 10px;
    }

    .navigation a.active {
        border-left: none;
        border-bottom: 3px solid white;
    }

    .input-group {
        flex-direction: column;
        align-items: stretch;
    }

    .tab-pane {
        display: none;
        margin-bottom: 0;
    }

    .tab-pane.active {
        display: block;
    }
}

/* ==========================================================================
   8. TABELLEN (DIAGNOSE)
   ========================================================================== */
.styled-table {
    width: 100%;
    border-collapse: collapse;
    font-size: 0.9em;
    text-align: left;
    margin-top: 10px;
}

.styled-table thead tr {
    background-color: rgba(0, 0, 0, 0.2);
    color: var(--text-info);
}

.styled-table th,
.styled-table td {
    padding: 10px 12px;
    border-bottom: 1px solid rgba(255, 255, 255, 0.05);
}

.styled-table tbody tr:hover {
    background-color: rgba(255, 255, 255, 0.05);
}

/* ==========================================================================
   9. ANIMATIONEN & TRENDS (PHASE 3)
   ========================================================================== */

.trend-arrow {
    font-size: 0.85em;
    margin-left: 8px;
    display: inline-block;
    transition: transform 0.3s, color 0.3s;
}

.trend-up {
    color: var(--warning);
    transform: translateY(-2px);
}

.trend-down {
    color: var(--text-resolve);
    transform: translateY(2px);
}

.trend-stable {
    color: var(--text-info);
    opacity: 0.5;
}
    </style>
</head>

<body>

    <header>
        <h1>Hardware Prüfadapter</h1>
    </header>

    <div class="main-container">
        <nav class="navigation">
            <a id="nav-analog" class="tab-link active" onclick="switchTab('analog')">Analog I/O</a>
            <a id="nav-digital" class="tab-link" onclick="switchTab('digital')">Digital I/O</a>
            <a id="nav-diagnose" class="tab-link" onclick="switchTab('diagnose')">Diagnose</a>
        </nav>

        <main class="content">
            <div id="persistent-system" class="persistent-pane">
                <h2 class="section-title">System Status</h2>
                <div class="card-section">
                    <div class="card">
                        <h3>Allgemein</h3>
                        <div class="status-items">
                            <div class="system-status"><span class="led off" id="system-fail-led"></span>System bereit
                            </div>
                            <div class="system-status">
                                <span class="led off" id="system-error-led"></span><span id="system-error-text">System
                                    Fehler (0)</span>
                            </div>
                        </div>
                        <ul id="system-error-list" class="error-list"></ul>
                    </div>
                    <div class="card">
                        <h3>Diagnose</h3>
                        <div class="status-items">
                            <div class="system-fail"><span class="led off" id="analog-fail-led"></span>Analog</div>
                            <div class="system-fail"><span class="led off" id="digital-fail-led"></span>Digital</div>
                            <div class="system-fail"><span class="led off" id="comm-fail-led"></span>Interface</div>
                        </div>
                    </div>
                </div>
            </div>

            <div id="tab-analog" class="tab-pane">
    <h2 class="section-title">Analog I/O Steuerung</h2>
    <h3>Eingänge (0-24V)</h3>
    <div class="card-section">
        <div class="card">
            <h3>Eingang 1</h3>
            <div class="value-display">
                <span id="analog-in-1-value">0.00</span><span class="control-label">V</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill" id="analog-in-1-bar" data-max="30"></div>
            </div>
        </div>
        <div class="card">
            <h3>Eingang 2</h3>
            <div class="value-display">
                <span id="analog-in-2-value">0.00</span><span class="control-label">V</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill" id="analog-in-2-bar" data-max="30"></div>
            </div>
        </div>
        <div class="card">
            <h3>Eingang 3</h3>
            <div class="value-display">
                <span id="analog-in-3-value">0.00</span><span class="control-label">V</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill" id="analog-in-3-bar" data-max="30"></div>
            </div>
        </div>
        <div class="card">
            <h3>Eingang 4</h3>
            <div class="value-display">
                <span id="analog-in-4-value">0.00</span><span class="control-label">V</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill" id="analog-in-4-bar" data-max="30"></div>
            </div>
        </div>
    </div>

    <h3>Ausgänge</h3>
    <div class="card-section">
        <div class="card">
            <h3>Ausgang 1</h3>
            <input type="range" id="analog-out-1-slider" min="0" max="24" step="0.1" value="0">
            <div class="input-group">
                <input type="number" id="analog-out-1-number" value="0.00" step="0.1" class="control-input">
                <span class="control-label">V</span>
                <button id="analog-out-1-btn-set" class="btn">Set</button>
            </div>
            <label class="control-label">Ist-Wert (Readback):</label>
            <div class="value-display">
                <span id="analog-out-1-readback-value">0.00</span><span class="control-label">V</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill" id="analog-out-1-bar-visual" data-max="30"></div>
            </div>
        </div>
        <div class="card">
            <h3>Ausgang 2</h3>
            <input type="range" id="analog-out-2-slider" min="0" max="24" step="0.1" value="0">
            <div class="input-group">
                <input type="number" id="analog-out-2-number" value="0.00" step="0.1" class="control-input">
                <span class="control-label">V</span>
                <button id="analog-out-2-btn-set" class="btn">Set</button>
            </div>
            <label class="control-label">Ist-Wert (Readback):</label>
            <div class="value-display">
                <span id="analog-out-2-readback-value">0.00</span><span class="control-label">V</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill" id="analog-out-2-bar-visual" data-max="30"></div>
            </div>
        </div>
        <div class="card">
            <h3>Ausgang 3</h3>
            <input type="range" id="analog-out-3-slider" min="0" max="24" step="0.1" value="0">
            <div class="input-group">
                <input type="number" id="analog-out-3-number" value="0.00" step="0.1" class="control-input">
                <span class="control-label">V</span>
                <button id="analog-out-3-btn-set" class="btn">Set</button>
            </div>
            <label class="control-label">Ist-Wert (Readback):</label>
            <div class="value-display">
                <span id="analog-out-3-readback-value">0.00</span><span class="control-label">V</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill" id="analog-out-3-bar-visual" data-max="30"></div>
            </div>
        </div>
        <div class="card">
            <h3>Ausgang 4</h3>
            <input type="range" id="analog-out-4-slider" min="0" max="24" step="0.1" value="0">
            <div class="input-group">
                <input type="number" id="analog-out-4-number" value="0.00" step="0.1" class="control-input">
                <span class="control-label">V</span>
                <button id="analog-out-4-btn-set" class="btn">Set</button>
            </div>
            <label class="control-label">Ist-Wert (Readback):</label>
            <div class="value-display">
                <span id="analog-out-4-readback-value">0.00</span><span class="control-label">V</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill" id="analog-out-4-bar-visual" data-max="30"></div>
            </div>
        </div>
    </div>
</div>

            <div id="tab-digital" class="tab-pane">
    <h2 class="section-title">Digital I/O Steuerung</h2>
    <h3>Eingänge (0-24V)</h3>
    <div class="card-section">
        <div class="card">
            <h3>Eingang 1</h3>
            <label class="control-label">Referenzspannung:</label>
            <select id="digital-in-ref-1" class="control-select">
                <option value="1800">1.8 V (TTL)</option>
                <option value="3300">3.3 V (CMOS)</option>
                <option value="5000">5.0 V (TTL)</option>
                <option value="12000">12.0 V</option>
                <option value="24000" selected>24.0 V (SPS)</option>
            </select>
            <div class="value-display">
                <span><span id="digital-in-1-value">0.00</span> <span class="control-label">V</span></span>
                <span id="digital-in-1-badge" class="status-badge low">LOW</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill" id="digital-in-1-bar" data-max="30"></div>
            </div>
        </div>
        <div class="card">
            <h3>Eingang 2</h3>
            <label class="control-label">Referenzspannung:</label>
            <select id="digital-in-ref-2" class="control-select">
                <option value="1800">1.8 V (TTL)</option>
                <option value="3300">3.3 V (CMOS)</option>
                <option value="5000">5.0 V (TTL)</option>
                <option value="12000">12.0 V</option>
                <option value="24000" selected>24.0 V (SPS)</option>
            </select>
            <div class="value-display">
                <span><span id="digital-in-2-value">0.00</span> <span class="control-label">V</span></span>
                <span id="digital-in-2-badge" class="status-badge low">LOW</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill" id="digital-in-2-bar" data-max="30"></div>
            </div>
        </div>
        <div class="card">
            <h3>Eingang 3</h3>
            <label class="control-label">Referenzspannung:</label>
            <select id="digital-in-ref-3" class="control-select">
                <option value="1800">1.8 V (TTL)</option>
                <option value="3300">3.3 V (CMOS)</option>
                <option value="5000">5.0 V (TTL)</option>
                <option value="12000">12.0 V</option>
                <option value="24000" selected>24.0 V (SPS)</option>
            </select>
            <div class="value-display">
                <span><span id="digital-in-3-value">0.00</span> <span class="control-label">V</span></span>
                <span id="digital-in-3-badge" class="status-badge low">LOW</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill" id="digital-in-3-bar" data-max="30"></div>
            </div>
        </div>
        <div class="card">
            <h3>Eingang 4</h3>
            <label class="control-label">Referenzspannung:</label>
            <select id="digital-in-ref-4" class="control-select">
                <option value="1800">1.8 V (TTL)</option>
                <option value="3300">3.3 V (CMOS)</option>
                <option value="5000">5.0 V (TTL)</option>
                <option value="12000">12.0 V</option>
                <option value="24000" selected>24.0 V (SPS)</option>
            </select>
            <div class="value-display">
                <span><span id="digital-in-4-value">0.00</span> <span class="control-label">V</span></span>
                <span id="digital-in-4-badge" class="status-badge low">LOW</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill" id="digital-in-4-bar" data-max="30"></div>
            </div>
        </div>
    </div>

    <h3>Ausgänge</h3>
    <div class="card-section">
        <div class="card digital-out-card">
            <div class="digital-out-content">
                <h3>Ausgang 1</h3>
                <label class="control-label">Referenzspannung:</label>
                <select id="digital-out-ref-1" class="control-select">
                    <option value="1800">1.8 V (TTL)</option>
                    <option value="3300">3.3 V (CMOS)</option>
                    <option value="5000">5.0 V (TTL)</option>
                    <option value="12000">12.0 V</option>
                    <option value="24000" selected>24.0 V (SPS)</option>
                </select>
                <div class="value-display">
                    <span><span id="digital-out-1-value">0.00</span> <span class="control-label">V</span></span>
                    <span id="digital-out-1-badge" class="status-badge low">LOW</span>
                    <span class="led off" id="digital-out-1-led"></span>
                </div>
                <div class="progress-track">
                    <div class="progress-fill" id="digital-out-1-bar" data-max="30"></div>
                </div>
            </div>
            <div class="switch-vertical-container">
                <label class="switch-vertical">
                    <input type="checkbox" id="digital-out-1-toggle">
                    <span class="slider-vertical"></span>
                </label>
            </div>
        </div>
        <div class="card digital-out-card">
            <div class="digital-out-content">
                <h3>Ausgang 2</h3>
                <label class="control-label">Referenzspannung:</label>
                <select id="digital-out-ref-2" class="control-select">
                    <option value="1800">1.8 V (TTL)</option>
                    <option value="3300">3.3 V (CMOS)</option>
                    <option value="5000">5.0 V (TTL)</option>
                    <option value="12000">12.0 V</option>
                    <option value="24000" selected>24.0 V (SPS)</option>
                </select>
                <div class="value-display">
                    <span><span id="digital-out-2-value">0.00</span> <span class="control-label">V</span></span>
                    <span id="digital-out-2-badge" class="status-badge low">LOW</span>
                    <span class="led off" id="digital-out-2-led"></span>
                </div>
                <div class="progress-track">
                    <div class="progress-fill" id="digital-out-2-bar" data-max="30"></div>
                </div>
            </div>
            <div class="switch-vertical-container">
                <label class="switch-vertical">
                    <input type="checkbox" id="digital-out-2-toggle">
                    <span class="slider-vertical"></span>
                </label>
            </div>
        </div>
        <div class="card digital-out-card">
            <div class="digital-out-content">
                <h3>Ausgang 3</h3>
                <label class="control-label">Referenzspannung:</label>
                <select id="digital-out-ref-3" class="control-select">
                    <option value="1800">1.8 V (TTL)</option>
                    <option value="3300">3.3 V (CMOS)</option>
                    <option value="5000">5.0 V (TTL)</option>
                    <option value="12000">12.0 V</option>
                    <option value="24000" selected>24.0 V (SPS)</option>
                </select>
                <div class="value-display">
                    <span><span id="digital-out-3-value">0.00</span> <span class="control-label">V</span></span>
                    <span id="digital-out-3-badge" class="status-badge low">LOW</span>
                    <span class="led off" id="digital-out-3-led"></span>
                </div>
                <div class="progress-track">
                    <div class="progress-fill" id="digital-out-3-bar" data-max="30"></div>
                </div>
            </div>
            <div class="switch-vertical-container">
                <label class="switch-vertical">
                    <input type="checkbox" id="digital-out-3-toggle">
                    <span class="slider-vertical"></span>
                </label>
            </div>
        </div>
        <div class="card digital-out-card">
            <div class="digital-out-content">
                <h3>Ausgang 4</h3>
                <label class="control-label">Referenzspannung:</label>
                <select id="digital-out-ref-4" class="control-select">
                    <option value="1800">1.8 V (TTL)</option>
                    <option value="3300">3.3 V (CMOS)</option>
                    <option value="5000">5.0 V (TTL)</option>
                    <option value="12000">12.0 V</option>
                    <option value="24000" selected>24.0 V (SPS)</option>
                </select>
                <div class="value-display">
                    <span><span id="digital-out-4-value">0.00</span> <span class="control-label">V</span></span>
                    <span id="digital-out-4-badge" class="status-badge low">LOW</span>
                    <span class="led off" id="digital-out-4-led"></span>
                </div>
                <div class="progress-track">
                    <div class="progress-fill" id="digital-out-4-bar" data-max="30"></div>
                </div>
            </div>
            <div class="switch-vertical-container">
                <label class="switch-vertical">
                    <input type="checkbox" id="digital-out-4-toggle">
                    <span class="slider-vertical"></span>
                </label>
            </div>
        </div>
    </div>
</div>

            <div id="tab-diagnose" class="tab-pane">
    <h2 class="section-title">System Diagnose & I2C Scanner</h2>
    <div class="card-section">
        <div class="card">
            <h3>I2C Bus Übersicht</h3>
            <table class="styled-table" id="i2c-device-table">
                <thead>
                    <tr>
                        <th>I2C Adresse</th>
                        <th>Bus</th>
                        <th>Geräte-Typ</th>
                        <th>Status</th>
                    </tr>
                </thead>
                <tbody id="i2c-device-tbody">
                    <tr>
                        <td colspan="4">Warte auf Daten...</td>
                    </tr>
                </tbody>
            </table>
        </div>
    </div>
</div>

            <div id="tab-terminal" class="tab-pane">
    <h2 class="section-title">Interaktives Terminal (Serial)</h2>
    <div class="card-section">
        <div class="card">
            <div class="toolbar">
                <div class="toolbar-group">
                    <label class="control-label">Interface:</label>
                    <select id="comm-interface-select" class="control-select">
                        <option value="ALL" selected>Alle Interfaces</option>
                        <option value="UART">UART</option>
                        <option value="RS232">RS232</option>
                        <option value="RS485">RS485</option>
                    </select>
                </div>
                <div class="toolbar-group">
                    <label class="control-label">New Liner:</label>
                    <select id="term-line-ending" class="control-select" style="width: 70px;">
                        <option value="auto" selected>Auto</option>
                        <option value="null">None</option>
                        <option value="\n">LF</option>
                        <option value="\r">CR</option>
                        <option value="\r\n">CRLF</option>
                    </select>
                </div>
                <div class="toolbar-group">
                    <label class="control-label">Baud:</label>
                    <select id="comm-baud-select" class="control-select">
                        <option value="9600">9600</option>
                        <option value="115200" selected>115200</option>
                    </select>
                </div>
                <div class="toolbar-group">
                    <label class="control-label">Ziel Interface:</label>
                    <select id="term-tx-target" class="control-select" style="width: 80px;">
                        <option value="UART">UART</option>
                        <option value="RS232">RS232</option>
                        <option value="RS485">RS485</option>
                    </select>
                </div>
                <div class="toolbar-actions">
                    <button class="btn" id="term-apply-config-btn">Config anwenden</button>
                    <button class="btn-secondary" id="term-export-btn">Export</button>
                    <button class="btn-secondary" id="term-clear-btn">Clear</button>
                    <label style="display:flex; align-items:center; gap:5px; font-size:0.8rem;">
                        <input type="checkbox" id="term-autoscroll" checked> Autoscroll
                    </label>
                </div>
            </div>

            <div id="comm-log" class="terminal-log"></div>

            <div class="toolbar" style="margin-top: 10px;">
                <div class="input-group" style="flex: 1; flex-wrap: nowrap;">
                    <input type="text" id="term-input" class="control-input" style="width: 100%;" placeholder="Befehl eingeben...">
                </div>
                <button class="btn" id="term-send-btn">Senden</button>
            </div>
        </div>
    </div>
</div>

            <div id="persistent-comm" class="persistent-pane">
    <h2 class="section-title">Debug Terminal</h2>
    <div class="card-section">
        <div class="card">
            <div class="toolbar">
                <div class="toolbar-actions">
                    <button class="btn-secondary" id="term-clear-btn">Clear Local</button>
                    <button class="btn" id="term-reload-btn">Force Sync</button>
                </div>
            </div>

            <div id="comm-log" class="terminal-log">
            </div>
        </div>
    </div>
</div>



        </main>
    </div>

    <footer id="sys-footer">
        System Status: Verbinde... | Uptime: 0h 0m 0s
    </footer>

    <script>
        /**
 * Prozesssteuerung Dashboard Logik
 */

const CONFIG = {
    channelCount: 4,
    refreshInterval: 5000,
    ioRefreshInterval: 1000
};

const State = {
    internalUptimeMs: 0,
    initialIoSyncDone: false,
    isSimulationMode: false,
    errorCount: 0,
    lastData: null,
    interactionLocks: {}
};

// ==========================================
// 1. API SERVICE
// ==========================================
class ApiService {
    static async fetchWithTimeout(url, options = {}, timeoutMs = 3000) {
        const controller = new AbortController();
        const id = setTimeout(() => controller.abort(), timeoutMs);

        try {
            const response = await fetch(url, { ...options, signal: controller.signal });
            clearTimeout(id);

            // BOUNCER-Logik: Server meldet Überlast -> Fehler werfen, UI ignoriert den Zyklus
            if (response.status === 503) {
                throw new Error("503 Service Unavailable (Overload)");
            }

            if (!response.ok) throw new Error(`HTTP Error ${response.status}`);
            return await response.json();

        } catch (e) {
            clearTimeout(id);
            throw e;
        }
    }

    static async get(url) {
        return this.fetchWithTimeout(url);
    }

    static async put(url, payload) {
        return this.fetchWithTimeout(url, {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });
    }

    static async fetchSystemStatus() { return this.get('/api/v1/system/status'); }
    static async fetchI2cDevices() { return this.get('/api/v1/i2c/devices'); }
    static async fetchIoAll() { return this.get('/api/v1/io'); }

    static async setAnalogOut(channel, voltage_mv) {
        return this.put(`/api/v1/io/analog/output/${channel}`, { value: voltage_mv });
    }
    static async setDigitalOut(channel, stateBool) {
        return this.put(`/api/v1/io/digital/output/${channel}`, { state: stateBool });
    }
    static async setDigitalRef(channel, direction, voltage_mv) {
        const dirStr = direction === 0 ? 'input' : 'output';
        return this.put(`/api/v1/io/digital/${dirStr}/${channel}/reference`, { voltage_mv: voltage_mv });
    }
    static async setSerialConfig(interfaceName, baudrate) {
        return this.put('/api/v1/serial/config', { interface: interfaceName, baudrate: parseInt(baudrate) });
    }
    static async sendSerialTx(interfaceName, payloadStr) {
        return this.put('/api/v1/serial/tx', { interface: interfaceName, payload: payloadStr });
    }
}

// ==========================================
// 2. SYSTEM LOGGER (Debug Terminal)
// ==========================================
class SystemLogger {
    constructor() {
        this.storedLogs = [];
        this.maxLogs = 2000;
        this.container = null;
    }

    init() {
        this.container = document.querySelector('#persistent-comm .terminal-log');
        this.storedLogs = JSON.parse(sessionStorage.getItem('system_logs') || '[]');

        if (this.container) this.container.innerHTML = '';

        if (this.storedLogs.length === 0) {
            this.pushToStorage("[SYS] System Frontend geladen (Warte auf Daten...)");
        } else {
            this.storedLogs.forEach(log => this.renderLine(log));
            this.scrollToBottom();
        }

        this.setupButtons();
        this.fetchHistory();
    }

    renderLine(logString) {
        if (!this.container) return;
        const div = document.createElement('div');
        const logRegex = /^(\[\d{2}:\d{2}:\d{2}\.\d{3}\])\s(\[[^\]]+\])\s(.*)$/;
        const match = logString.match(logRegex);

        if (match) {
            const [, timeStr, levelStr, msgStr] = match;
            let cssClass = 'log-info';
            if (levelStr === '[WARN]') cssClass = 'log-warn';
            if (levelStr === '[ERROR]') cssClass = 'log-error';
            if (levelStr === '[TX]') cssClass = 'log-entry-tx';
            if (levelStr === '[RX]') cssClass = 'log-entry-rx';
            if (levelStr === '[SYS]') cssClass = 'log-entry-sys';
            if (levelStr === '[Fehler behoben]') cssClass = 'log-entry-resolve';

            div.className = cssClass;
            div.innerHTML = `<span class="log-time">${timeStr}</span><span>${levelStr}</span> ${msgStr}`;
        } else {
            div.className = 'log-info';
            div.textContent = logString;
        }

        this.container.appendChild(div);
        this.scrollToBottomIfNeeded();
    }

    scrollToBottom() {
        if (this.container) this.container.scrollTop = this.container.scrollHeight;
    }

    scrollToBottomIfNeeded() {
        if (!this.container) return;
        const isScrolledToBottom = this.container.scrollHeight - this.container.clientHeight <= this.container.scrollTop + 50;
        if (isScrolledToBottom) this.scrollToBottom();
    }

    pushToStorage(logString) {
        if (!this.storedLogs.includes(logString)) {
            this.storedLogs.push(logString);
            if (this.storedLogs.length > this.maxLogs) this.storedLogs.shift();
            sessionStorage.setItem('system_logs', JSON.stringify(this.storedLogs));
            this.renderLine(logString);
        }
    }

    async fetchHistory() {
        try {
            const data = await ApiService.get('/api/v1/system/logs');
            if (data.logs && Array.isArray(data.logs)) {
                data.logs.forEach(log => { if (log.trim() !== "") this.pushToStorage(log); });
            }
        } catch (e) { console.warn("Log Fetch fehlgeschlagen."); }
    }

    setupButtons() {
        const clearBtn = document.querySelector('#persistent-comm #term-clear-btn');
        const reloadBtn = document.querySelector('#persistent-comm #term-reload-btn');
        if (clearBtn) {
            clearBtn.addEventListener('click', () => {
                sessionStorage.removeItem('system_logs');
                this.storedLogs = [];
                if (this.container) this.container.innerHTML = '';
            });
        }
        if (reloadBtn) {
            reloadBtn.addEventListener('click', () => {
                sessionStorage.removeItem('system_logs');
                location.reload();
            });
        }
    }
}

// ==========================================
// 3. SERIAL TERMINAL (UART/RS232/RS485)
// ==========================================
class SerialTerminal {
    constructor() {
        this.history = [];
        this.maxHistoryCount = 500;

        this.interfaceBaudrates = {
            "UART": "115200",
            "RS232": "115200",
            "RS485": "115200"
        };

        this.activeLines = {};
        this.pendingNewline = {};
        this.lastChar = {};
        this.escapePending = {};
        this.container = null;
    }

    init() {
        this.container = document.querySelector('#tab-terminal .terminal-log');
        this.interfaceViewSelect = document.getElementById('comm-interface-select');
        this.baudSelect = document.getElementById('comm-baud-select');
        this.applyConfigBtn = document.getElementById('term-apply-config-btn');

        this.exportBtn = document.getElementById('term-export-btn');
        this.clearBtn = document.getElementById('term-clear-btn');
        this.autoScrollCheck = document.getElementById('term-autoscroll');

        this.inputField = document.getElementById('term-input');
        this.txTargetSelect = document.getElementById('term-tx-target');
        this.lineEndingSelect = document.getElementById('term-line-ending');
        this.sendBtn = document.getElementById('term-send-btn');

        this.setupEvents();

        const savedHistory = sessionStorage.getItem('serial_terminal_history');
        if (savedHistory) {
            try {
                this.history = JSON.parse(savedHistory);
                this.replayHistory();
                this.appendLog("SYS", "Terminal-Historie wiederhergestellt.", "sys", null, false);
            } catch (e) {
                this.history = [];
            }
        } else {
            this.appendLog("SYS", "Interaktives Terminal bereit.", "sys", null, true);
        }
    }

    setupEvents() {
        if (!this.applyConfigBtn) return;

        this.applyConfigBtn.addEventListener('click', () => this.applyConfig());

        this.txTargetSelect.addEventListener('change', () => {
            const target = this.txTargetSelect.value;
            if (this.interfaceBaudrates[target]) {
                this.baudSelect.value = this.interfaceBaudrates[target];
            }
        });

        this.exportBtn.addEventListener('click', () => this.exportLogToFile());

        this.clearBtn.addEventListener('click', () => {
            if (this.container) this.container.innerHTML = '';
            this.activeLines = {};
            this.pendingNewline = {};
            this.history = [];
            sessionStorage.removeItem('serial_terminal_history');
        });

        this.sendBtn.addEventListener('click', () => this.sendData());
        this.inputField.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') this.sendData();
        });

        this.interfaceViewSelect.addEventListener('change', () => this.applyViewFilter());
        this.lineEndingSelect.addEventListener('change', () => this.replayHistory());
    }

    async applyConfig() {
        const targetInterface = this.txTargetSelect.value;
        const baudrate = this.baudSelect.value;
        try {
            await ApiService.setSerialConfig(targetInterface, baudrate);
            this.interfaceBaudrates[targetInterface] = baudrate;
            this.appendLog("SYS", `Konfiguration angewendet: ${targetInterface} @ ${baudrate} Baud`, "sys");
        } catch (e) {
            this.appendLog("SYS", `Fehler bei Konfiguration: ${e.message}`, "error");
        }
    }

    // ================== HISTORIE & SPEICHER ==================

    pushToHistory(item) {
        this.history.push(item);
        if (this.history.length > this.maxHistoryCount) {
            this.history.shift();
        }
        sessionStorage.setItem('serial_terminal_history', JSON.stringify(this.history));
    }

    replayHistory() {
        if (!this.container) return;

        this.container.innerHTML = '';
        this.activeLines = {};
        this.pendingNewline = {};
        this.lastChar = {};
        this.escapePending = {};

        this.history.forEach(item => {
            if (item.typeClass === 'rx') {
                this.processRxChunk(item.source, item.payload, item.timeStr);
            } else {
                this.appendLog(item.source, item.payload, item.typeClass, item.timeStr, false);
            }
        });

        if (this.autoScrollCheck && this.autoScrollCheck.checked) {
            this.container.scrollTop = this.container.scrollHeight;
        }
    }

    exportLogToFile() {
        let textContent = "Hardware Pruefadapter - Terminal Log Export\n";
        textContent += "===========================================\n\n";

        this.history.forEach(item => {
            const prefix = item.typeClass === 'tx' ? '->' : (item.typeClass === 'rx' ? '<-' : '  ');
            let cleanPayload = item.payload.replace(/\r/g, '[CR]').replace(/\n/g, '[LF]\n');
            if (item.typeClass !== 'rx' && item.typeClass !== 'tx') {
                cleanPayload = item.payload;
            }
            textContent += `[${item.timeStr}] [${item.source}] ${prefix} ${cleanPayload}`;
            if (!textContent.endsWith('\n')) textContent += '\n';
        });

        const blob = new Blob([textContent], { type: 'text/plain' });
        const a = document.createElement('a');
        a.href = URL.createObjectURL(blob);
        a.download = `terminal_export_${new Date().getTime()}.txt`;
        a.click();
    }

    // ================== ANSICHT & FILTER ==================

    applyViewFilter() {
        if (!this.container) return;
        const filter = this.interfaceViewSelect.value;
        const entries = this.container.querySelectorAll('div');
        entries.forEach(entry => this.applyViewFilterToElement(entry, filter));
        if (this.autoScrollCheck && this.autoScrollCheck.checked) this.container.scrollTop = this.container.scrollHeight;
    }

    applyViewFilterToElement(entry, filter) {
        if (!filter) filter = this.interfaceViewSelect.value;
        const source = entry.dataset.source;
        if (!source || source === "SYS" || filter === "ALL" || filter === source) {
            entry.style.display = "";
        } else {
            entry.style.display = "none";
        }
    }

    getCurrentTimeStr() {
        const now = new Date();
        return `${String(now.getHours()).padStart(2, '0')}:${String(now.getMinutes()).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}.${String(now.getMilliseconds()).padStart(3, '0')}`;
    }

    createNewLogLine(source, typeClass, timeStr) {
        if (!timeStr) timeStr = this.getCurrentTimeStr();
        const div = document.createElement('div');
        div.dataset.source = source;

        let css = 'log-info';
        if (typeClass === 'rx') css = 'log-entry-rx';
        if (typeClass === 'tx') css = 'log-entry-tx';
        if (typeClass === 'sys') css = 'log-entry-sys';
        if (typeClass === 'error') css = 'log-error';
        div.className = css;

        const prefix = typeClass === 'tx' ? '▶' : (typeClass === 'rx' ? '◀' : '');
        div.innerHTML = `<span class="log-time">[${timeStr}]</span> <span style="color:var(--text-info);">[${source}]</span> ${prefix} <span class="log-payload"></span>`;
        return div;
    }

    appendChunk(source, htmlChunk, closeLine, timeStr) {
        if (!this.container || htmlChunk === "") return;

        let lineDiv = this.activeLines[source];
        if (!lineDiv) {
            lineDiv = this.createNewLogLine(source, 'rx', timeStr);
            this.activeLines[source] = lineDiv;
            this.container.appendChild(lineDiv);
            this.applyViewFilterToElement(lineDiv);
        }

        let payloadSpan = lineDiv.querySelector('.log-payload');
        if (payloadSpan) payloadSpan.innerHTML += htmlChunk;

        if (closeLine) this.activeLines[source] = null;

        if (this.autoScrollCheck && this.autoScrollCheck.checked && lineDiv.style.display !== "none") {
            this.container.scrollTop = this.container.scrollHeight;
        }
    }

    // ================== PARSER LOGIK ==================

    handleRx(source, dataChunk) {
        const timeStr = this.getCurrentTimeStr();
        this.pushToHistory({ timeStr, source, typeClass: 'rx', payload: dataChunk });
        this.processRxChunk(source, dataChunk, timeStr);
    }

    processRxChunk(source, dataChunk, timeStr) {
        const separatorMode = this.lineEndingSelect.value;

        if (this.pendingNewline[source] === undefined) this.pendingNewline[source] = false;
        if (this.lastChar[source] === undefined) this.lastChar[source] = '';
        if (this.escapePending[source] === undefined) this.escapePending[source] = false;

        let htmlChunk = "";

        for (let i = 0; i < dataChunk.length; i++) {
            let char = dataChunk[i];

            if (this.escapePending[source]) {
                if (char === 'n') char = '\n';
                else if (char === 'r') char = '\r';
                else htmlChunk += '\\';
                this.escapePending[source] = false;
            } else if (char === '\\') {
                this.escapePending[source] = true;
                continue;
            }

            let displayStr = char;

            if (char === '\r') displayStr = '<span style="color: #777; font-size: 0.9em; font-weight: bold;">\\r</span>';
            else if (char === '\n') displayStr = '<span style="color: #777; font-size: 0.9em; font-weight: bold;">\\n</span>';
            else displayStr = char.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");

            if (this.pendingNewline[source]) {
                if (separatorMode === 'auto' && this.lastChar[source] === '\r' && char === '\n') {
                    htmlChunk += displayStr;
                    this.appendChunk(source, htmlChunk, true, timeStr);
                    htmlChunk = "";
                    this.pendingNewline[source] = false;
                    this.lastChar[source] = char;
                    continue;
                } else {
                    this.appendChunk(source, htmlChunk, true, timeStr);
                    htmlChunk = "";
                    this.pendingNewline[source] = false;
                }
            }

            htmlChunk += displayStr;

            if (separatorMode === '\\r' && char === '\r') this.pendingNewline[source] = true;
            else if (separatorMode === '\\n' && char === '\n') this.pendingNewline[source] = true;
            else if (separatorMode === '\\r\\n' && this.lastChar[source] === '\r' && char === '\n') this.pendingNewline[source] = true;
            else if (separatorMode === 'auto' && (char === '\r' || char === '\n')) this.pendingNewline[source] = true;

            this.lastChar[source] = char;
        }

        if (htmlChunk.length > 0) {
            this.appendChunk(source, htmlChunk, false, timeStr);
        }
    }

    async sendData() {
        let payload = this.inputField.value;
        if (!payload) return;

        payload = payload.replace(/\\r/g, '\r').replace(/\\n/g, '\n');
        const targetInterface = this.txTargetSelect.value;

        try {
            await ApiService.sendSerialTx(targetInterface, payload);
            this.appendLog(targetInterface, payload, 'tx', null, true);
            this.inputField.value = '';
        } catch (e) {
            this.appendLog(targetInterface, `TX Fehler: ${e.message}`, 'error', null, true);
        }
    }

    appendLog(source, message, typeClass, timeStr = null, saveToHistory = true) {
        if (!this.container) return;
        if (!timeStr) timeStr = this.getCurrentTimeStr();

        if (saveToHistory) {
            this.pushToHistory({ timeStr, source, typeClass, payload: message });
        }

        if (this.activeLines[source] && typeClass !== 'rx') {
            this.activeLines[source] = null;
        }

        const lineDiv = this.createNewLogLine(source, typeClass, timeStr);

        let safeMsg = message.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
        safeMsg = safeMsg.replace(/\r/g, '<span style="color: #777; font-size: 0.9em; font-weight: bold;">\\r</span>')
            .replace(/\n/g, '<span style="color: #777; font-size: 0.9em; font-weight: bold;">\\n</span>');

        lineDiv.querySelector('.log-payload').innerHTML = safeMsg;
        this.container.appendChild(lineDiv);
        this.applyViewFilterToElement(lineDiv);

        if (this.autoScrollCheck && this.autoScrollCheck.checked && lineDiv.style.display !== "none") {
            this.container.scrollTop = this.container.scrollHeight;
        }
    }
}

const SystemLogInstance = new SystemLogger();
const SerialTermInstance = new SerialTerminal();

// ==========================================
// 4. UI MANAGER (DOM Updates)
// ==========================================
class UIManager {
    static formatUptime(ms) {
        const totalSeconds = Math.floor(ms / 1000);
        const hours = Math.floor(totalSeconds / 3600);
        const minutes = Math.floor((totalSeconds % 3600) / 60);
        const seconds = totalSeconds % 60;
        return `${hours}h ${minutes}m ${seconds}s`;
    }

    static updateFooter() {
        const footer = document.getElementById('sys-footer');
        if (footer) {
            const modeText = State.isSimulationMode ? 'Simulation' : 'Online';
            footer.innerText = `System Status: ${modeText} | Uptime: ${this.formatUptime(State.internalUptimeMs)}`;
            footer.className = State.isSimulationMode ? 'simulation' : '';
        }
    }

    static updateSystemStatus(data) {
        const statusLed = document.getElementById('system-fail-led');
        const commLed = document.getElementById('comm-fail-led');

        if (statusLed) statusLed.className = "led on";

        if (data.uptime_ms !== undefined) {
            if (State.internalUptimeMs > 10000 && data.uptime_ms < (State.internalUptimeMs - 5000)) {
                SystemLogInstance.pushToStorage("[SYS] Backend Neustart erkannt! Resynchronisiere UI...");
                State.initialIoSyncDone = false;
                AppController.syncIoData();
            }
            State.internalUptimeMs = data.uptime_ms;
        }

        const errorLed = document.getElementById('system-error-led');
        const errorText = document.getElementById('system-error-text');
        const errorList = document.getElementById('system-error-list');

        let hasSerialError = false;

        if (errorLed && errorText && errorList) {
            State.errorCount = data.error_count || 0;
            if (State.errorCount === 0) {
                errorLed.className = "led off";
                errorText.innerText = "System Fehler (0)";
                errorList.classList.remove('active');
                errorList.innerHTML = '';
            } else {
                errorLed.className = "led fail";
                errorText.innerText = `System Fehler (${State.errorCount})`;
                errorList.innerHTML = '';

                if (data.errors && Array.isArray(data.errors)) {
                    data.errors.forEach(errString => {
                        if (errString.includes("Serial") || errString.includes("UART")) hasSerialError = true;

                        const li = document.createElement('li');
                        li.innerText = errString;
                        errorList.appendChild(li);
                    });
                }
                errorList.classList.add('active');
            }
        }

        if (commLed) commLed.className = hasSerialError ? "led fail" : "led on";

        if (data.status !== undefined) {
            State.isSimulationMode = (data.status === 'simulation');
        }

        this.updateFooter();
    }

    static setSystemOffline() {
        const statusLed = document.getElementById('system-fail-led');
        if (statusLed) statusLed.className = "led fail";
        const aLed = document.getElementById('analog-fail-led');
        if (aLed) aLed.className = "led fail";
        const dLed = document.getElementById('digital-fail-led');
        if (dLed) dLed.className = "led fail";
        const commLed = document.getElementById('comm-fail-led');
        if (commLed) commLed.className = "led fail";
    }

    static checkVoltageWarning(id, voltageVolts) {
        const el = document.getElementById(id);
        if (!el) return;

        if (parseFloat(voltageVolts) > 24) {
            el.style.color = "var(--warning)";
            if (!el.innerHTML.includes("⚠")) {
                el.innerHTML += ' <span style="font-size:0.7em; color:var(--warning);" title="Zu Hoch Aufpassen">⚠</span>';
            }
        } else {
            el.style.color = "";
        }
    }

    static updateIoData(data) {
        const last = State.lastData;

        for (let i = 0; i < CONFIG.channelCount; i++) {
            let ch = i + 1;

            // ANALOG IN
            let aInVolts = (data.analog_in[i] / 1000).toFixed(2);
            let aInLast = last ? last.analog_in[i] : null;
            this.setElementTextWithTrend(`analog-in-${ch}-value`, aInVolts, data.analog_in[i], aInLast);
            this.setElementValue(`analog-in-${ch}-bar`, aInVolts, 30);
            this.checkVoltageWarning(`analog-in-${ch}-value`, aInVolts);

            // DIGITAL IN
            let dInVolts = data.digital_in[i].value / 1000;
            let dInState = data.digital_in[i].state;
            let dInLast = last ? last.digital_in[i].value : null;
            let refSelectDIn = document.getElementById(`digital-in-ref-${ch}`);
            this.setElementTextWithTrend(`digital-in-${ch}-value`, dInVolts.toFixed(2), data.digital_in[i].value, dInLast);
            let dInMax = refSelectDIn ? (parseInt(refSelectDIn.value) / 1000) : 30;
            this.setElementValue(`digital-in-${ch}-bar`, dInVolts, dInMax);
            this.checkVoltageWarning(`digital-in-${ch}-value`, dInVolts);

            let barDIn = document.getElementById(`digital-in-${ch}-bar`);
            if (barDIn) {
                if (dInState === -1) barDIn.classList.add('error');
                else barDIn.classList.remove('error');
            }
            this.updateDigitalBadge(`digital-in-${ch}-badge`, dInState);

            // ANALOG OUT
            let aOutVolts = data.analog_out[i] / 1000;
            let aOutLast = last ? last.analog_out[i] : null;
            this.setElementValue(`analog-out-${ch}-bar-visual`, aOutVolts, 30);
            this.setElementTextWithTrend(`analog-out-${ch}-readback-value`, aOutVolts.toFixed(2), data.analog_out[i], aOutLast);
            this.checkVoltageWarning(`analog-out-${ch}-readback-value`, aOutVolts);

            let aLockTime = State.interactionLocks[`analog-out-${ch}`] || 0;
            let aIsLocked = (Date.now() - aLockTime) < 3000;

            let slider = document.getElementById(`analog-out-${ch}-slider`);
            let numberInput = document.getElementById(`analog-out-${ch}-number`);
            if (slider && numberInput) {
                let isFocused = (document.activeElement === slider || document.activeElement === numberInput);
                let currentSliderVal = parseFloat(slider.value) || 0;

                if (!State.initialIoSyncDone || (!isFocused && !aIsLocked && Math.abs(currentSliderVal - aOutVolts) > 1.0)) {
                    let roundedVolts = Math.round(aOutVolts * 10) / 10;
                    this.setElementValue(`analog-out-${ch}-slider`, roundedVolts);
                    this.setElementValue(`analog-out-${ch}-number`, roundedVolts.toFixed(2));
                }
            }

            // DIGITAL OUT
            let dOutVolts = data.digital_out[i].value / 1000;
            let dOutState = data.digital_out[i].state;
            let dOutLast = last ? last.digital_out[i].value : null;
            let refSelectDOut = document.getElementById(`digital-out-ref-${ch}`);
            this.setElementTextWithTrend(`digital-out-${ch}-value`, dOutVolts.toFixed(2), data.digital_out[i].value, dOutLast);
            let dOutMax = refSelectDOut ? (parseInt(refSelectDOut.value) / 1000) : 30;
            this.setElementValue(`digital-out-${ch}-bar`, dOutVolts, dOutMax);
            this.checkVoltageWarning(`digital-out-${ch}-value`, dOutVolts);

            let barDOut = document.getElementById(`digital-out-${ch}-bar`);
            if (barDOut) {
                if (dOutState === -1) barDOut.classList.add('error');
                else barDOut.classList.remove('error');
            }
            this.updateDigitalBadge(`digital-out-${ch}-badge`, dOutState);
            let ledDOut = document.getElementById(`digital-out-${ch}-led`);
            if (ledDOut) {
                ledDOut.className = dOutState === 1 ? "led on" : "led off";
            }

            // Digital Lock Logik
            let dLockTime = State.interactionLocks[`digital-out-${ch}`] || 0;
            let dIsLocked = (Date.now() - dLockTime) < 3000;

            let toggle = document.getElementById(`digital-out-${ch}-toggle`);
            let shouldBeChecked = (dOutState === 1 || dOutState === -1);

            if (toggle && !dIsLocked && toggle.checked !== shouldBeChecked) {
                toggle.checked = shouldBeChecked;
            }
        }

        State.initialIoSyncDone = true;
        State.lastData = data;

        const aLed = document.getElementById('analog-fail-led');
        const dLed = document.getElementById('digital-fail-led');
        if (aLed) aLed.className = "led on";
        if (dLed) dLed.className = "led on";
    }

    static setElementTextWithTrend(id, text, currentVal, lastVal) {
        const el = document.getElementById(id);
        if (!el) return;
        let trendHtml = '';
        if (lastVal !== undefined && lastVal !== null) {
            let diff = currentVal - lastVal;
            if (diff > 5) trendHtml = '<span class="trend-arrow trend-up">↑</span>';
            else if (diff < -5) trendHtml = '<span class="trend-arrow trend-down">↓</span>';
            else trendHtml = '<span class="trend-arrow trend-stable">~</span>';
        }
        el.innerHTML = `${text}${trendHtml}`;
    }

    static setElementValue(id, val, max = null) {
        const el = document.getElementById(id);
        if (!el) return;
        if (el.tagName === 'INPUT') {
            if (max !== null) el.max = max;
            el.value = val;
        }
        else if (el.classList.contains('progress-fill')) {
            if (max !== null) el.dataset.max = max;
            let currentMax = parseFloat(el.dataset.max) || 24;
            let percent = (val / currentMax) * 100;
            if (percent > 100) percent = 100;
            if (percent < 0) percent = 0;
            el.style.width = `${percent}%`;
        }
    }

    static updateDigitalBadge(id, state) {
        const badge = document.getElementById(id);
        if (!badge) return;
        if (state === 1) { badge.innerText = "HIGH"; badge.className = "status-badge high"; }
        else if (state === 0) { badge.innerText = "LOW"; badge.className = "status-badge low"; }
        else { badge.innerText = "FLOAT"; badge.className = "status-badge error"; }
    }

    static initControls() {
        for (let i = 1; i <= CONFIG.channelCount; i++) {
            this.setupAnalogControl(i);
            this.setupDigitalControl(i);
            this.setupReferenceControl(i);
        }
    }

    static setupAnalogControl(ch) {
        const slider = document.getElementById(`analog-out-${ch}-slider`);
        const input = document.getElementById(`analog-out-${ch}-number`);
        const btn = document.getElementById(`analog-out-${ch}-btn-set`);

        if (!slider || !input || !btn) return;
        slider.addEventListener('input', () => {
            State.interactionLocks[`analog-out-${ch}`] = Date.now();
            input.value = parseFloat(slider.value).toFixed(2);
        });
        input.addEventListener('input', () => {
            State.interactionLocks[`analog-out-${ch}`] = Date.now();
            slider.value = parseFloat(input.value) || 0;
        });
        btn.addEventListener('click', async () => {
            State.interactionLocks[`analog-out-${ch}`] = Date.now();
            const valV = parseFloat(input.value) || 0;
            const valMv = Math.round(valV * 1000);
            try {
                await ApiService.setAnalogOut(ch - 1, valMv);
                SystemLogInstance.pushToStorage(`[TX] Analog Out ${ch} gesetzt auf: ${valV.toFixed(2)} V`);
                setTimeout(() => AppController.syncIoData(), 100);
            } catch (error) { SystemLogInstance.pushToStorage(`[RX] Analog Out ${ch} fehler: ${error.message}`); }
        });
    }

    static setupDigitalControl(ch) {
        const toggle = document.getElementById(`digital-out-${ch}-toggle`);
        if (!toggle) return;
        toggle.addEventListener('change', async () => {
            State.interactionLocks[`digital-out-${ch}`] = Date.now();
            const stateBool = toggle.checked;
            try {
                await ApiService.setDigitalOut(ch - 1, stateBool);
                SystemLogInstance.pushToStorage(`[TX] Digital Out ${ch} gesetzt auf: ${stateBool ? 'ON' : 'OFF'}`);
                setTimeout(() => AppController.syncIoData(), 100);
            } catch (error) {
                toggle.checked = !stateBool;
            }
        });
    }

    static setupReferenceControl(ch) {
        const selectIn = document.getElementById(`digital-in-ref-${ch}`);
        const selectOut = document.getElementById(`digital-out-ref-${ch}`);

        if (selectIn) {
            selectIn.addEventListener('change', async () => {
                const valMv = parseInt(selectIn.value, 10);
                try {
                    await ApiService.setDigitalRef(ch - 1, 0, valMv);
                    SystemLogInstance.pushToStorage(`[TX] Ref Digital IN ${ch} auf ${valMv} mV gesetzt`);
                    setTimeout(() => AppController.syncIoData(), 100);
                } catch (error) { }
            });
        }

        if (selectOut) {
            selectOut.addEventListener('change', async () => {
                const valMv = parseInt(selectOut.value, 10);
                try {
                    await ApiService.setDigitalRef(ch - 1, 1, valMv);
                    SystemLogInstance.pushToStorage(`[TX] Ref Digital OUT ${ch} auf ${valMv} mV gesetzt`);
                    setTimeout(() => AppController.syncIoData(), 100);
                } catch (error) { }
            });
        }
    }

    static updateI2cTable(devicesArray) {
        const tbody = document.getElementById('i2c-device-tbody');
        if (!tbody) return;
        if (!devicesArray || devicesArray.length === 0) {
            tbody.innerHTML = '<tr><td colspan="4">Keine I2C Geräte gefunden.</td></tr>';
            return;
        }
        tbody.innerHTML = '';
        devicesArray.forEach(dev => {
            const tr = document.createElement('tr');
            tr.innerHTML = `<td>${dev.address}</td><td>Wire ${dev.bus}</td><td>${dev.name}</td><td><span class="status-badge high">OK</span></td>`;
            tbody.appendChild(tr);
        });
    }
}

// ==========================================
// 5. APP CONTROLLER (Orchestrierung)
// ==========================================
class AppController {
    static evtSource = null;

    static initSSEStream() {
        if (this.evtSource) this.evtSource.close();

        this.evtSource = new EventSource('/api/v1/system/logs/stream');

        this.evtSource.addEventListener('log', (e) => {
            if (e.data && e.data.trim() !== "") SystemLogInstance.pushToStorage(e.data);
        });

        this.evtSource.addEventListener('serial', (e) => {
            if (e.data) {
                try {
                    const parsed = JSON.parse(e.data);
                    SerialTermInstance.handleRx(parsed.source, parsed.data);
                } catch (err) { console.error("SSE Serial JSON Parse Error:", err); }
            }
        });
    }

    static async syncSystemStatus() {
        try {
            const data = await ApiService.fetchSystemStatus();
            UIManager.updateSystemStatus(data);
            const i2cData = await ApiService.fetchI2cDevices();
            UIManager.updateI2cTable(i2cData);
        } catch (err) {
            // Fallback auf Offline Anzeige bei HTTP Timeout oder 503
            UIManager.setSystemOffline();
        }
    }

    static async syncIoData() {
        const sysLed = document.getElementById('system-fail-led');
        if (!sysLed || sysLed.className.includes('fail')) return;
        try {
            const data = await ApiService.fetchIoAll();
            UIManager.updateIoData(data);
        } catch (err) {
            UIManager.setSystemOffline();
        }
    }

    static init() {
        SystemLogInstance.init();
        SerialTermInstance.init();

        this.initSSEStream();
        UIManager.initControls();

        this.syncSystemStatus();
        this.syncIoData();

        setInterval(() => this.syncSystemStatus(), CONFIG.refreshInterval);
        setInterval(() => this.syncIoData(), CONFIG.ioRefreshInterval);

        setInterval(() => {
            State.internalUptimeMs += 1000;
            UIManager.updateFooter();
        }, 1000);
    }
}

// Tab Navigation
window.switchTab = function (tabName) {
    document.querySelectorAll('.tab-link').forEach(link => link.classList.remove('active'));
    const activeLink = document.getElementById(`nav-${tabName}`);
    if (activeLink) activeLink.classList.add('active');

    if (window.innerWidth <= 768) {
        document.querySelectorAll('.tab-pane').forEach(pane => pane.classList.remove('active'));
        const activeTab = document.getElementById(`tab-${tabName}`);
        if (activeTab) activeTab.classList.add('active');
    } else {
        const target = document.getElementById(`tab-${tabName}`);
        if (target) target.scrollIntoView({ behavior: 'smooth', block: 'start' });
    }
};

document.addEventListener('DOMContentLoaded', () => {
    AppController.init();
});
    </script>

</body>

</html>)html";

}  // namespace core
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_CORE_WEB_PAGE_INDEX_H_
