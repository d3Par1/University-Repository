// Клієнт моніторингу біогазової електростанції — Варіант 15
// REST: початкове завантаження історії (як fallback)
// WebSocket: real-time оновлення кожні 2с (snapshot + tick)

const WS_URL = `ws://${location.host}/ws`;
const REST_HISTORY = '/api/history';
const RECONNECT_DELAY_MS = 5000;

let biogasChart, compositionChart;
let ws = null;

// ─── Charts ────────────────────────────────────────────────────────────────

function initBiogasChart() {
    const ctx = document.getElementById('biogas-chart').getContext('2d');
    biogasChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [
                {
                    label: 'Обсяг біогазу (м³/год)',
                    data: [],
                    borderColor: '#22c55e',
                    backgroundColor: 'rgba(34, 197, 94, 0.15)',
                    fill: true,
                    tension: 0.35,
                    yAxisID: 'y'
                },
                {
                    label: 'Електр. потужність (кВт)',
                    data: [],
                    borderColor: '#f59e0b',
                    backgroundColor: 'rgba(245, 158, 11, 0.10)',
                    fill: false,
                    tension: 0.35,
                    yAxisID: 'y1'
                }
            ]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            interaction: { mode: 'index', intersect: false },
            scales: {
                y:  { type: 'linear', position: 'left',  title: { display: true, text: 'м³/год' }, beginAtZero: true },
                y1: { type: 'linear', position: 'right', title: { display: true, text: 'кВт' },     beginAtZero: true,
                      grid: { drawOnChartArea: false } }
            },
            plugins: { legend: { position: 'bottom' } }
        }
    });
}

function initCompositionChart() {
    const ctx = document.getElementById('composition-chart').getContext('2d');
    compositionChart = new Chart(ctx, {
        type: 'doughnut',
        data: {
            labels: ['CH₄ (метан)', 'CO₂ (вуглекислий газ)', 'H₂S (сірководень)', 'Інші (NH₃, H₂O, N₂)'],
            datasets: [{
                data: [0, 0, 0, 0],
                backgroundColor: ['#22c55e', '#94a3b8', '#ef4444', '#eab308'],
                borderWidth: 2,
                borderColor: '#fff'
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: { position: 'bottom' },
                tooltip: {
                    callbacks: {
                        label: (ctx) => `${ctx.label}: ${ctx.parsed.toFixed(2)}%`
                    }
                }
            }
        }
    });
}

// ─── KPI cards ─────────────────────────────────────────────────────────────

function updateKPI(s) {
    document.getElementById('kpi-flow').textContent      = s.totalBiogasFlow.toFixed(0);
    document.getElementById('kpi-methane').textContent   = s.avgMethane.toFixed(1);
    document.getElementById('kpi-power').textContent     = s.totalPower.toFixed(0);
    document.getElementById('kpi-temp').textContent      = s.avgFermenterTemp.toFixed(1);
    document.getElementById('kpi-substrate').textContent = s.totalSubstrate.toFixed(1);
}

// ─── Composition doughnut update ────────────────────────────────────────────

function updateComposition(s) {
    compositionChart.data.datasets[0].data = [s.avgMethane, s.avgCO2, s.avgH2S, s.avgOther];
    compositionChart.update('none');
}

// ─── Biogas line chart from history snapshot ────────────────────────────────

function setBiogasHistory(history) {
    biogasChart.data.labels = history.map(h => formatHourLabel(h.timestamp));
    biogasChart.data.datasets[0].data = history.map(h => h.biogasFlow);
    biogasChart.data.datasets[1].data = history.map(h => h.power);
    biogasChart.update('none');
}

function pushBiogasPoint(stats) {
    biogasChart.data.labels.push(formatTimeLabel(stats.timestamp));
    biogasChart.data.datasets[0].data.push(stats.totalBiogasFlow);
    biogasChart.data.datasets[1].data.push(stats.totalPower);
    if (biogasChart.data.labels.length > 60) {
        biogasChart.data.labels.shift();
        biogasChart.data.datasets[0].data.shift();
        biogasChart.data.datasets[1].data.shift();
    }
    biogasChart.update('none');
}

// ─── Temperature gauges (custom SVG) ───────────────────────────────────────
// Шкала: 30-45°C, кольорові зони: cold <37, optimal 37-42, overheat >42

function buildGaugeHTML(plant) {
    const t = plant.fermenterTemp;
    const min = 30, max = 45;
    const angle = ((t - min) / (max - min)) * 180 - 90;     // -90° (left) to +90° (right)
    const zone = t < 37 ? 'cold' : t > 42 ? 'overheat' : 'optimal';
    const status = t < 37 ? 'Холодно' : t > 42 ? 'Перегрів' : 'Оптимум';

    return `
    <div class="gauge gauge-${zone}">
        <div class="gauge-title">${plant.name}</div>
        <div class="gauge-svg-wrap">
            <svg viewBox="-50 -50 100 60" class="gauge-svg" aria-hidden="true">
                <!-- Cold zone (30-37°C) -->
                <path d="M -45,0 A 45,45 0 0,1 -28.93,-34.41" stroke="#3b82f6" stroke-width="8" fill="none" />
                <!-- Optimal (37-42) -->
                <path d="M -28.93,-34.41 A 45,45 0 0,1 28.93,-34.41" stroke="#22c55e" stroke-width="8" fill="none" />
                <!-- Overheat (42-45) -->
                <path d="M 28.93,-34.41 A 45,45 0 0,1 45,0" stroke="#ef4444" stroke-width="8" fill="none" />
                <!-- Needle -->
                <line x1="0" y1="0" x2="0" y2="-38" stroke="#1f2937" stroke-width="2.5" stroke-linecap="round"
                      transform="rotate(${angle.toFixed(2)})" />
                <circle cx="0" cy="0" r="3.5" fill="#1f2937" />
            </svg>
        </div>
        <div class="gauge-value">${t.toFixed(2)} °C</div>
        <div class="gauge-status">${status}</div>
    </div>
    `;
}

function updateGauges(items) {
    document.getElementById('temp-gauges').innerHTML = items.map(buildGaugeHTML).join('');
}

// ─── Table render ──────────────────────────────────────────────────────────

function statusBadge(s) {
    if (s === 'warning')        return '<span class="badge bg-warning text-dark">⚠ Увага</span>';
    if (s === 'low_efficiency') return '<span class="badge bg-info text-dark">↓ ККД</span>';
    return '<span class="badge bg-success">● Норма</span>';
}

function updateTable(items) {
    const tbody = document.getElementById('plants-tbody');
    tbody.innerHTML = items.map(p => `
        <tr>
            <td><strong>${p.name}</strong></td>
            <td>${p.region}</td>
            <td class="substrate">${p.substrateType}</td>
            <td class="num">${p.biogasFlow.toFixed(1)}</td>
            <td class="num">${p.methaneContent.toFixed(1)}</td>
            <td class="num">${p.fermenterTemp.toFixed(2)}</td>
            <td class="num"><strong>${p.electricPower.toFixed(0)}</strong></td>
            <td class="num">${(p.efficiency * 100).toFixed(1)}</td>
            <td>${statusBadge(p.status)}</td>
        </tr>
    `).join('');
}

// ─── WebSocket connection lifecycle ────────────────────────────────────────

function setWsStatus(connected) {
    const dot = document.getElementById('ws-dot');
    const txt = document.getElementById('ws-status');
    if (connected) {
        dot.classList.remove('ws-offline');
        dot.classList.add('ws-online');
        txt.textContent = 'Онлайн · оновлення кожні 2с';
    } else {
        dot.classList.remove('ws-online');
        dot.classList.add('ws-offline');
        txt.textContent = 'Офлайн · перепідключення через 5с';
    }
}

function connectWS() {
    setWsStatus(false);
    ws = new WebSocket(WS_URL);

    ws.onopen = () => {
        console.log('[WS] connected');
        setWsStatus(true);
    };

    ws.onmessage = (event) => {
        const msg = JSON.parse(event.data);
        if (msg.type === 'snapshot') {
            // Початкове завантаження: повний стан + історія
            if (msg.history && msg.history.length) setBiogasHistory(msg.history);
            updateKPI(msg.stats);
            updateComposition(msg.stats);
            updateGauges(msg.items);
            updateTable(msg.items);
        } else if (msg.type === 'tick') {
            updateKPI(msg.stats);
            updateComposition(msg.stats);
            updateGauges(msg.items);
            updateTable(msg.items);
            // Інкрементальна точка на графік (для наочного real-time)
            pushBiogasPoint(msg.stats);
        }
    };

    ws.onerror = (err) => {
        console.warn('[WS] error', err);
    };

    ws.onclose = () => {
        console.log('[WS] closed, reconnecting in', RECONNECT_DELAY_MS, 'ms');
        setWsStatus(false);
        setTimeout(connectWS, RECONNECT_DELAY_MS);
    };
}

// ─── REST fallback: load history if WS snapshot didn't arrive ───────────────

async function loadHistoryFallback() {
    try {
        const res = await fetch(REST_HISTORY);
        if (!res.ok) throw new Error(`HTTP ${res.status}`);
        const history = await res.json();
        if (biogasChart.data.labels.length === 0 && history.length) setBiogasHistory(history);
    } catch (e) {
        console.warn('REST history fetch failed:', e.message);
    }
}

// ─── Helpers ───────────────────────────────────────────────────────────────

function formatTimeLabel(ts) {
    return new Date(ts).toLocaleTimeString('uk-UA', { hour: '2-digit', minute: '2-digit', second: '2-digit' });
}

function formatHourLabel(ts) {
    return new Date(ts).toLocaleTimeString('uk-UA', { hour: '2-digit', minute: '2-digit' });
}

// ─── Init ──────────────────────────────────────────────────────────────────

document.addEventListener('DOMContentLoaded', () => {
    initBiogasChart();
    initCompositionChart();
    loadHistoryFallback();   // підстраховка якщо WS snapshot ще не прийшов
    connectWS();
});
