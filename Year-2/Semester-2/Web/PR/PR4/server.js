const express = require('express');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;
const DATA_FILE = path.join(__dirname, 'data', 'lighting-systems.json');

const ALLOWED_MODES = ['auto', 'manual', 'schedule'];
const POWER_PER_LAMP_KW = 0.12;

const stats = { requests: 0, byMethod: {}, startedAt: new Date().toISOString() };

app.use(express.json({ limit: '64kb' }));
app.use(express.static(path.join(__dirname, 'public')));

app.use((req, res, next) => {
    stats.requests += 1;
    stats.byMethod[req.method] = (stats.byMethod[req.method] || 0) + 1;
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Methods', 'GET,POST,PUT,PATCH,DELETE,OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
    const ts = new Date().toISOString();
    console.log(`[${ts}] ${req.method} ${req.url}`);
    if (req.method === 'OPTIONS') return res.sendStatus(204);
    next();
});

function readData() {
    try {
        if (!fs.existsSync(DATA_FILE)) return [];
        return JSON.parse(fs.readFileSync(DATA_FILE, 'utf-8'));
    } catch (e) {
        console.error('readData error:', e.message);
        return [];
    }
}

function writeData(items) {
    fs.writeFileSync(DATA_FILE, JSON.stringify(items, null, 2), 'utf-8');
}

function nextId(items) {
    return items.length ? Math.max(...items.map(i => i.id)) + 1 : 1;
}

function recomputePower(item) {
    item.totalPower = +(item.activeLamps * POWER_PER_LAMP_KW * (item.brightness / 100)).toFixed(2);
}

function validateNew(body) {
    const errors = [];
    if (!body.name || String(body.name).trim().length < 2) errors.push("Поле 'name' обов'язкове (мін. 2 символи)");
    if (typeof body.lampCount !== 'number' || body.lampCount < 1) errors.push("'lampCount' має бути числом >= 1");
    if (body.activeLamps !== undefined && (typeof body.activeLamps !== 'number' || body.activeLamps < 0)) errors.push("'activeLamps' має бути числом >= 0");
    if (body.activeLamps !== undefined && body.lampCount !== undefined && body.activeLamps > body.lampCount) errors.push("'activeLamps' не може перевищувати 'lampCount'");
    if (body.brightness !== undefined && (body.brightness < 0 || body.brightness > 100)) errors.push("'brightness' має бути в діапазоні 0..100");
    if (body.mode !== undefined && !ALLOWED_MODES.includes(body.mode)) errors.push(`'mode' має бути одним з: ${ALLOWED_MODES.join(', ')}`);
    return { valid: errors.length === 0, errors };
}

// ─── 1) GET /api/lighting-systems — список з фільтрами, пошуком, сортуванням, пагінацією
app.get('/api/lighting-systems', (req, res) => {
    let items = readData();
    const { mode, search, sort, order, page, limit, minBrightness, maxBrightness } = req.query;

    if (mode) items = items.filter(i => i.mode === mode);
    if (minBrightness !== undefined) items = items.filter(i => i.brightness >= Number(minBrightness));
    if (maxBrightness !== undefined) items = items.filter(i => i.brightness <= Number(maxBrightness));
    if (search) {
        const q = String(search).toLowerCase();
        items = items.filter(i => i.name.toLowerCase().includes(q));
    }

    if (sort) {
        const dir = order === 'desc' ? -1 : 1;
        items.sort((a, b) => (a[sort] > b[sort] ? dir : a[sort] < b[sort] ? -dir : 0));
    }

    const total = items.length;
    if (page && limit) {
        const p = Math.max(1, parseInt(page));
        const l = Math.max(1, parseInt(limit));
        items = items.slice((p - 1) * l, p * l);
        return res.json({ total, page: p, limit: l, data: items });
    }

    res.json(items);
});

// ─── 2) GET /api/lighting-systems/stats — агрегована статистика
app.get('/api/lighting-systems/stats', (req, res) => {
    const items = readData();
    const totalLamps = items.reduce((s, i) => s + i.lampCount, 0);
    const activeLamps = items.reduce((s, i) => s + i.activeLamps, 0);
    const totalPower = +items.reduce((s, i) => s + i.totalPower, 0).toFixed(2);
    const totalEnergy = +items.reduce((s, i) => s + i.energyConsumed, 0).toFixed(2);
    const byMode = {};
    items.forEach(i => { byMode[i.mode] = (byMode[i.mode] || 0) + 1; });
    res.json({
        systemsCount: items.length,
        totalLamps,
        activeLamps,
        totalPower,
        totalEnergy,
        byMode,
        api: stats
    });
});

// ─── 3) GET /api/lighting-systems/:id — деталі однієї системи
app.get('/api/lighting-systems/:id', (req, res) => {
    const id = parseInt(req.params.id);
    const item = readData().find(i => i.id === id);
    if (!item) return res.status(404).json({ error: 'Систему освітлення не знайдено', id: req.params.id });
    res.json(item);
});

// ─── 4) GET /api/lighting-systems/:id/schedule — розклад роботи
app.get('/api/lighting-systems/:id/schedule', (req, res) => {
    const id = parseInt(req.params.id);
    const item = readData().find(i => i.id === id);
    if (!item) return res.status(404).json({ error: 'Систему освітлення не знайдено' });
    res.json({ id: item.id, name: item.name, mode: item.mode, schedule: item.schedule });
});

// ─── 5) POST /api/lighting-systems — створення нової системи
app.post('/api/lighting-systems', (req, res) => {
    const v = validateNew(req.body);
    if (!v.valid) return res.status(400).json({ error: 'Помилка валідації', details: v.errors });

    const items = readData();
    const item = {
        id: nextId(items),
        name: String(req.body.name).trim(),
        lampCount: req.body.lampCount,
        activeLamps: req.body.activeLamps ?? 0,
        totalPower: 0,
        brightness: req.body.brightness ?? 100,
        mode: req.body.mode ?? 'auto',
        energyConsumed: req.body.energyConsumed ?? 0,
        lightLevel: req.body.lightLevel ?? 0,
        schedule: req.body.schedule ?? { on: '18:30', off: '06:15' },
        updatedAt: new Date().toISOString()
    };
    recomputePower(item);
    items.push(item);
    writeData(items);
    res.status(201).json(item);
});

// ─── 6) POST /api/lighting-systems/:id/control — керування світильниками
app.post('/api/lighting-systems/:id/control', (req, res) => {
    const id = parseInt(req.params.id);
    const items = readData();
    const idx = items.findIndex(i => i.id === id);
    if (idx === -1) return res.status(404).json({ error: 'Систему освітлення не знайдено' });

    const { action, count } = req.body;
    const item = items[idx];

    if (action === 'turn_on_all') {
        item.activeLamps = item.lampCount;
    } else if (action === 'turn_off_all') {
        item.activeLamps = 0;
    } else if (action === 'turn_on' && typeof count === 'number') {
        item.activeLamps = Math.min(item.lampCount, item.activeLamps + count);
    } else if (action === 'turn_off' && typeof count === 'number') {
        item.activeLamps = Math.max(0, item.activeLamps - count);
    } else {
        return res.status(400).json({ error: "Невідома команда. Доступні 'action': turn_on_all, turn_off_all, turn_on, turn_off" });
    }

    recomputePower(item);
    item.updatedAt = new Date().toISOString();
    writeData(items);
    res.json({ message: `Команду '${action}' виконано`, system: item });
});

// ─── 7) PUT /api/lighting-systems/:id — повне оновлення
app.put('/api/lighting-systems/:id', (req, res) => {
    const id = parseInt(req.params.id);
    const items = readData();
    const idx = items.findIndex(i => i.id === id);
    if (idx === -1) return res.status(404).json({ error: 'Систему освітлення не знайдено' });

    const v = validateNew(req.body);
    if (!v.valid) return res.status(400).json({ error: 'Помилка валідації', details: v.errors });

    const item = {
        id,
        name: String(req.body.name).trim(),
        lampCount: req.body.lampCount,
        activeLamps: req.body.activeLamps ?? 0,
        totalPower: 0,
        brightness: req.body.brightness ?? 100,
        mode: req.body.mode ?? 'auto',
        energyConsumed: req.body.energyConsumed ?? items[idx].energyConsumed,
        lightLevel: req.body.lightLevel ?? items[idx].lightLevel,
        schedule: req.body.schedule ?? items[idx].schedule,
        updatedAt: new Date().toISOString()
    };
    recomputePower(item);
    items[idx] = item;
    writeData(items);
    res.json(item);
});

// ─── 8) PATCH /api/lighting-systems/:id — часткове оновлення
app.patch('/api/lighting-systems/:id', (req, res) => {
    const id = parseInt(req.params.id);
    const items = readData();
    const idx = items.findIndex(i => i.id === id);
    if (idx === -1) return res.status(404).json({ error: 'Систему освітлення не знайдено' });

    if (req.body.mode !== undefined && !ALLOWED_MODES.includes(req.body.mode))
        return res.status(400).json({ error: `'mode' має бути одним з: ${ALLOWED_MODES.join(', ')}` });
    if (req.body.brightness !== undefined && (req.body.brightness < 0 || req.body.brightness > 100))
        return res.status(400).json({ error: "'brightness' має бути в діапазоні 0..100" });
    if (req.body.activeLamps !== undefined && req.body.activeLamps > items[idx].lampCount)
        return res.status(400).json({ error: "'activeLamps' не може перевищувати 'lampCount'" });

    items[idx] = { ...items[idx], ...req.body, id, updatedAt: new Date().toISOString() };
    recomputePower(items[idx]);
    writeData(items);
    res.json(items[idx]);
});

// ─── 9) PUT /api/lighting-systems/:id/brightness — швидка зміна яскравості
app.put('/api/lighting-systems/:id/brightness', (req, res) => {
    const id = parseInt(req.params.id);
    const items = readData();
    const idx = items.findIndex(i => i.id === id);
    if (idx === -1) return res.status(404).json({ error: 'Систему освітлення не знайдено' });

    const { brightness } = req.body;
    if (typeof brightness !== 'number' || brightness < 0 || brightness > 100)
        return res.status(400).json({ error: "Поле 'brightness' має бути числом 0..100" });

    items[idx].brightness = brightness;
    recomputePower(items[idx]);
    items[idx].updatedAt = new Date().toISOString();
    writeData(items);
    res.json({ message: 'Яскравість оновлено', system: items[idx] });
});

// ─── 10) DELETE /api/lighting-systems/:id — видалення системи
app.delete('/api/lighting-systems/:id', (req, res) => {
    const id = parseInt(req.params.id);
    const items = readData();
    const idx = items.findIndex(i => i.id === id);
    if (idx === -1) return res.status(404).json({ error: 'Систему освітлення не знайдено' });
    const [removed] = items.splice(idx, 1);
    writeData(items);
    res.json({ message: 'Систему освітлення видалено', system: removed });
});

app.use((req, res) => {
    res.status(404).json({ error: 'Маршрут не знайдено', method: req.method, path: req.path });
});

app.use((err, req, res, next) => {
    console.error('Server error:', err.message);
    res.status(500).json({ error: 'Внутрішня помилка сервера', details: err.message });
});

app.listen(PORT, () => {
    console.log(`Smart Street Lighting REST API запущено на http://localhost:${PORT}`);
    console.log('Endpoints:');
    console.log('  GET    /api/lighting-systems');
    console.log('  GET    /api/lighting-systems/stats');
    console.log('  GET    /api/lighting-systems/:id');
    console.log('  GET    /api/lighting-systems/:id/schedule');
    console.log('  POST   /api/lighting-systems');
    console.log('  POST   /api/lighting-systems/:id/control');
    console.log('  PUT    /api/lighting-systems/:id');
    console.log('  PATCH  /api/lighting-systems/:id');
    console.log('  PUT    /api/lighting-systems/:id/brightness');
    console.log('  DELETE /api/lighting-systems/:id');
});
