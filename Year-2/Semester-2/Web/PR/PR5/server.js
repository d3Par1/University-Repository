// Сервер моніторингу біогазової електростанції — Варіант 15
// REST API + WebSocket для real-time оновлень
// Симуляція мезофільної ферментації (оптимум 38-40°C)

const express = require('express');
const http = require('http');
const fs = require('fs');
const path = require('path');
const WebSocket = require('ws');

const app = express();
const PORT = process.env.PORT || 3000;
const DATA_FILE = path.join(__dirname, 'data', 'biogas-plants.json');

const TICK_INTERVAL_MS = 2000;
const HISTORY_LIMIT = 24;
const OPTIMAL_TEMP = 39;          // оптимальна температура мезофільної ферментації, °C
const CH4_HEAT_VALUE = 6;         // кВт·год теплової енергії на 1 м³ метану
const CHP_EFFICIENCY = 0.40;      // електричний ККД когенераційної установки

const stats = { requests: 0, byMethod: {}, startedAt: new Date().toISOString() };

app.use(express.json({ limit: '64kb' }));
app.use(express.static(path.join(__dirname, 'public')));

app.use((req, res, next) => {
    stats.requests += 1;
    stats.byMethod[req.method] = (stats.byMethod[req.method] || 0) + 1;
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Methods', 'GET,POST,OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
    if (req.method === 'OPTIONS') return res.sendStatus(204);
    console.log(`[${new Date().toISOString()}] ${req.method} ${req.url}`);
    next();
});

// Завантаження конфігурації станцій з JSON
function loadBaseConfig() {
    try {
        return JSON.parse(fs.readFileSync(DATA_FILE, 'utf-8'));
    } catch (e) {
        console.error('Помилка читання конфігурації:', e.message);
        return [];
    }
}

const baseConfig = loadBaseConfig();

// Початковий стан симуляції — кожна станція в номінальному режимі
const liveState = baseConfig.map(p => ({
    id: p.id,
    fermenterTemp: 38.5 + Math.random() * 1.5,           // 38.5-40°C
    methaneContent: 56 + Math.random() * 4,              // 56-60%
    co2Content: 38 + Math.random() * 3,                  // 38-41%
    h2sContent: 0.025 + Math.random() * 0.015,           // 0.025-0.040%
    otherGases: 1.5 + Math.random() * 1.0,               // 1.5-2.5% (NH3, H2O, N2)
    biogasFlow: 0,
    electricPower: 0,
    substrateLoading: +(p.maxPower * 0.018).toFixed(2),  // т/добу, ~1.8% від встановленої потужності в кВт
    energyToday: 0,
    efficiency: 0,
    status: 'running'
}));

// Розрахунок електричної потужності з потоку біогазу
// P_ел [кВт] = Q_газ [м³/год] × (CH4 % / 100) × CH4_HEAT_VALUE × CHP_EFFICIENCY
function calcElectricPower(biogasFlow, methaneContent) {
    return +(biogasFlow * (methaneContent / 100) * CH4_HEAT_VALUE * CHP_EFFICIENCY).toFixed(1);
}

// Зворотній розрахунок: потрібний потік газу для бажаної потужності
function calcRequiredFlow(targetPower, methaneContent) {
    return +(targetPower / ((methaneContent / 100) * CH4_HEAT_VALUE * CHP_EFFICIENCY)).toFixed(1);
}

// Один такт симуляції — оновлення live state з малими дрейфами
function tick() {
    liveState.forEach((s, idx) => {
        const cfg = baseConfig[idx];

        // Температура: повільний дрейф у вузькому діапазоні навколо оптимуму
        const tempDrift = (Math.random() - 0.5) * 0.15;
        s.fermenterTemp = +(s.fermenterTemp + tempDrift).toFixed(2);
        if (s.fermenterTemp < 37.0) s.fermenterTemp += 0.05;
        if (s.fermenterTemp > 41.5) s.fermenterTemp -= 0.05;

        // Склад газу: метан 54-62%, CO2 36-43%
        s.methaneContent = +(s.methaneContent + (Math.random() - 0.5) * 0.4).toFixed(1);
        if (s.methaneContent < 53) s.methaneContent = 53 + Math.random();
        if (s.methaneContent > 63) s.methaneContent = 63 - Math.random();

        s.h2sContent = +Math.max(0.015, Math.min(0.05, s.h2sContent + (Math.random() - 0.5) * 0.005)).toFixed(3);
        s.otherGases = +Math.max(1.0, Math.min(3.0, s.otherGases + (Math.random() - 0.5) * 0.1)).toFixed(2);
        s.co2Content = +(100 - s.methaneContent - s.h2sContent - s.otherGases).toFixed(1);

        // Ефективність ферментації — функція відхилення від оптимальної температури
        const tempDeviation = Math.abs(s.fermenterTemp - OPTIMAL_TEMP);
        s.efficiency = +Math.max(0.55, 1 - tempDeviation * 0.08).toFixed(3);

        // Потік біогазу — пропорційний завантаженню субстрату та ефективності
        const targetFlow = calcRequiredFlow(cfg.maxPower * 0.85, s.methaneContent) * s.efficiency;
        s.biogasFlow = +(targetFlow + (Math.random() - 0.5) * 20).toFixed(1);

        // Електрична потужність
        s.electricPower = calcElectricPower(s.biogasFlow, s.methaneContent);

        // Накопичення добової генерації (тік 2с → коефіцієнт 2/3600)
        s.energyToday = +(s.energyToday + s.electricPower * (TICK_INTERVAL_MS / 3600000)).toFixed(2);

        // Статус: warning якщо температура поза оптимумом
        if (s.fermenterTemp < 37 || s.fermenterTemp > 42) s.status = 'warning';
        else if (s.efficiency < 0.7) s.status = 'low_efficiency';
        else s.status = 'running';
    });
}

// Об'єднання конфігурації + live state для зовнішнього API
function getMergedItems() {
    return baseConfig.map((cfg, idx) => ({ ...cfg, ...liveState[idx] }));
}

// Агрегована статистика по всіх станціях
function aggregateStats() {
    const items = getMergedItems();
    const totalBiogasFlow = +items.reduce((s, i) => s + i.biogasFlow, 0).toFixed(1);
    const totalPower = +items.reduce((s, i) => s + i.electricPower, 0).toFixed(1);
    const totalEnergyToday = +items.reduce((s, i) => s + i.energyToday, 0).toFixed(2);
    const totalSubstrate = +items.reduce((s, i) => s + i.substrateLoading, 0).toFixed(1);
    const avgMethane = +(items.reduce((s, i) => s + i.methaneContent, 0) / items.length).toFixed(2);
    const avgCO2 = +(items.reduce((s, i) => s + i.co2Content, 0) / items.length).toFixed(2);
    const avgH2S = +(items.reduce((s, i) => s + i.h2sContent, 0) / items.length).toFixed(3);
    const avgOther = +(items.reduce((s, i) => s + i.otherGases, 0) / items.length).toFixed(2);
    const avgFermenterTemp = +(items.reduce((s, i) => s + i.fermenterTemp, 0) / items.length).toFixed(1);
    const avgEfficiency = +(items.reduce((s, i) => s + i.efficiency, 0) / items.length * 100).toFixed(1);
    const maxInstalled = items.reduce((s, i) => s + i.maxPower, 0);
    const utilization = +(totalPower / maxInstalled * 100).toFixed(1);
    return {
        timestamp: Date.now(),
        plantsCount: items.length,
        totalBiogasFlow,
        totalPower,
        totalEnergyToday,
        totalSubstrate,
        avgMethane,
        avgCO2,
        avgH2S,
        avgOther,
        avgFermenterTemp,
        avgEfficiency,
        utilization
    };
}

// Кільцевий буфер 24-годинної історії (одна точка на годину)
const history = [];

function pushHistoryPoint() {
    const snap = aggregateStats();
    history.push({
        timestamp: snap.timestamp,
        biogasFlow: snap.totalBiogasFlow,
        power: snap.totalPower,
        methane: snap.avgMethane,
        temperature: snap.avgFermenterTemp
    });
    if (history.length > HISTORY_LIMIT) history.shift();
}

// Передзаповнення історії — імітація останніх 24 годин
function seedHistory() {
    const now = Date.now();
    for (let i = HISTORY_LIMIT - 1; i >= 0; i--) {
        // Невеликі коливання для реалістичної історії
        const tempVar = (Math.random() - 0.5) * 1.0;
        const ch4Var = (Math.random() - 0.5) * 3;
        const methane = 58 + ch4Var;
        const flowBase = baseConfig.reduce((s, c) => s + calcRequiredFlow(c.maxPower * 0.82, methane), 0);
        const flow = +(flowBase * (0.92 + Math.random() * 0.16)).toFixed(1);
        const power = +(flow * (methane / 100) * CH4_HEAT_VALUE * CHP_EFFICIENCY).toFixed(1);
        history.push({
            timestamp: now - i * 3600000,
            biogasFlow: flow,
            power,
            methane: +methane.toFixed(2),
            temperature: +(39 + tempVar).toFixed(2)
        });
    }
}

seedHistory();

// ─── REST endpoints ─────────────────────────────────────────────────────────

app.get('/api/biogas-plants', (req, res) => {
    res.json(getMergedItems());
});

app.get('/api/biogas-plants/stats', (req, res) => {
    res.json({ ...aggregateStats(), api: stats });
});

app.get('/api/biogas-plants/:id', (req, res) => {
    const id = parseInt(req.params.id);
    const item = getMergedItems().find(i => i.id === id);
    if (!item) return res.status(404).json({ error: 'Станцію не знайдено', id });
    res.json(item);
});

app.get('/api/history', (req, res) => {
    res.json(history);
});

app.get('/api/status', (req, res) => {
    res.json({
        status: 'online',
        uptime: process.uptime(),
        wsClients: wss ? wss.clients.size : 0,
        requests: stats.requests,
        byMethod: stats.byMethod,
        startedAt: stats.startedAt
    });
});

// 404 handler
app.use((req, res) => {
    res.status(404).json({ error: 'Маршрут не знайдено', method: req.method, path: req.path });
});

app.use((err, req, res, next) => {
    console.error('Server error:', err.message);
    res.status(500).json({ error: 'Внутрішня помилка сервера', details: err.message });
});

// ─── HTTP + WebSocket сервери на одному порту ───────────────────────────────

const server = http.createServer(app);
const wss = new WebSocket.Server({ server, path: '/ws' });

function broadcast(obj) {
    const msg = JSON.stringify(obj);
    wss.clients.forEach(client => {
        if (client.readyState === WebSocket.OPEN) client.send(msg);
    });
}

wss.on('connection', (ws) => {
    console.log(`WebSocket клієнт підключився (всього: ${wss.clients.size})`);
    // Початковий snapshot — поточний стан + 24-год історія
    ws.send(JSON.stringify({
        type: 'snapshot',
        stats: aggregateStats(),
        items: getMergedItems(),
        history
    }));
    ws.on('close', () => {
        console.log(`WebSocket клієнт відключився (залишилось: ${wss.clients.size})`);
    });
    ws.on('error', (err) => console.error('WS error:', err.message));
});

// Tick loop — кожні 2с оновлюємо стан і шлемо всім клієнтам
let tickCount = 0;
setInterval(() => {
    tick();
    tickCount++;
    if (tickCount % 1800 === 0) pushHistoryPoint();   // одна точка історії на годину (1800 тіків × 2с)
    broadcast({ type: 'tick', stats: aggregateStats(), items: getMergedItems() });
}, TICK_INTERVAL_MS);

server.listen(PORT, () => {
    console.log(`Біогазовий моніторинг запущено на http://localhost:${PORT}`);
    console.log(`WebSocket endpoint: ws://localhost:${PORT}/ws`);
    console.log('REST endpoints:');
    console.log('  GET /api/biogas-plants');
    console.log('  GET /api/biogas-plants/stats');
    console.log('  GET /api/biogas-plants/:id');
    console.log('  GET /api/history');
    console.log('  GET /api/status');
});
