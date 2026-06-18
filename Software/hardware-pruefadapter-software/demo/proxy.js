const express = require('express');
const { createProxyMiddleware } = require('http-proxy-middleware');
const EventSource = require('eventsource');
const fs = require('fs');
const path = require('path');

// ===========================================================================
// 1. CONFIG AUS C++ BACKEND LESEN (Single Source of Truth)
// ===========================================================================
const configPath = path.join(__dirname, '../include/system_config.h');
let targetPort = 8080; // Fallback

try {
    const configContent = fs.readFileSync(configPath, 'utf8');
    // Sucht per Regex nach: inline constexpr std::uint16_t kWebServerPort = 8080U;
    const portMatch = configContent.match(/kWebServerPort\s*=\s*(\d+)U?/);
    if (portMatch) {
        targetPort = parseInt(portMatch[1], 10);
    }
} catch (e) {
    console.warn("[WARNUNG] Konnte system_config.h nicht lesen. Nutze Port 8080.");
}

// Ziel-IP aus Makefile-Argumenten (127.0.0.1 bei nativedemo, sonst ESP IP)
const targetIp = process.argv[2] || '192.168.4.1';
const ESP_URL = `http://${targetIp}:${targetPort}`;
const PROXY_PORT = 3000; // Der Port, auf dem die Handys der Zuschauer landen

const app = express();

// ===========================================================================
// 2. STATISCHES HTML (Null Last auf dem Backend)
// ===========================================================================
app.get('/', (req, res) => {
    const webDir = path.join(__dirname, '../lib/Referenz-Web');
    
    try {
        let html = fs.readFileSync(path.join(webDir, 'index.html'), 'utf8');
        html = html.replace('/* === INJECT_CSS === */', fs.readFileSync(path.join(webDir, 'style.css'), 'utf8'));
        html = html.replace('/* === INJECT_JS === */', fs.readFileSync(path.join(webDir, 'app.js'), 'utf8'));
        html = html.replace('/* === INJECT_ANALOG === */', fs.readFileSync(path.join(webDir, 'tabs/tab_analog.html'), 'utf8'));
        html = html.replace('/* === INJECT_DIGITAL === */', fs.readFileSync(path.join(webDir, 'tabs/tab_digital.html'), 'utf8'));
        html = html.replace('/* === INJECT_DIAGNOSE === */', fs.readFileSync(path.join(webDir, 'tabs/tab_diagnose.html'), 'utf8'));
        html = html.replace('/* === INJECT_TERMINAL === */', fs.readFileSync(path.join(webDir, 'tabs/tab_terminal.html'), 'utf8'));
        html = html.replace('/* === INJECT_LOG === */', fs.readFileSync(path.join(webDir, 'log_terminal.html'), 'utf8'));
        
        res.send(html);
    } catch (e) {
        res.status(500).send("Fehler beim Laden des Frontends: " + e.message);
    }
});

// ===========================================================================
// 3. SSE FAN-OUT (1 Verbindung zum Backend -> Multiple Verbindungen zu Clients)
// ===========================================================================
let sseClients = [];

app.get('/api/v1/system/logs/stream', (req, res) => {
    res.setHeader('Content-Type', 'text/event-stream');
    res.setHeader('Cache-Control', 'no-cache');
    res.setHeader('Connection', 'keep-alive');
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.flushHeaders();

    sseClients.push(res);
    console.log(`[Proxy] Neuer Zuhörer verbunden (Gesamt: ${sseClients.length})`);

    req.on('close', () => {
        sseClients = sseClients.filter(c => c !== res);
        console.log(`[Proxy] Zuhörer getrennt (Gesamt: ${sseClients.length})`);
    });
});

console.log(`[Proxy] Verbinde SSE Stream mit Backend auf ${ESP_URL}...`);
const es = new EventSource(`${ESP_URL}/api/v1/system/logs/stream`);

es.on('log', (e) => sseClients.forEach(c => c.write(`event: log\ndata: ${e.data}\n\n`)));
es.on('serial', (e) => sseClients.forEach(c => c.write(`event: serial\ndata: ${e.data}\n\n`)));
es.on('sys', (e) => sseClients.forEach(c => c.write(`event: sys\ndata: ${e.data}\n\n`)));

es.onerror = (err) => {
    console.log("[Proxy] SSE Verbindung zum Backend unterbrochen. Versuche Reconnect im Hintergrund...");
};

// ===========================================================================
// 4. API ROUTING (Befehle an das Backend durchreichen)
// ===========================================================================
app.use('/api', createProxyMiddleware({
    target: ESP_URL,
    changeOrigin: true,
    timeout: 5000,
    proxyTimeout: 5000,
    onProxyReq: (proxyReq, req, res) => {
        if (req.method !== 'GET') {
            console.log(`[Proxy] Sende Befehl an Backend: ${req.method} ${req.url}`);
        }
    }
}));

// ===========================================================================
// SERVER START
// ===========================================================================
app.listen(PROXY_PORT, '0.0.0.0', () => {
    console.log(`\n======================================================`);
    console.log(`🚀 DEMO PROXY SERVER LÄUFT!`);
    console.log(`======================================================`);
    console.log(`Die Zuhörer müssen im Browser folgende Adresse öffnen:`);
    console.log(`-> http://<IP-DEINES-Laptops>:${PROXY_PORT}`);
    console.log(`\n(Der Proxy ist verbunden mit dem Backend auf: ${ESP_URL})`);
    console.log(`======================================================\n`);
});