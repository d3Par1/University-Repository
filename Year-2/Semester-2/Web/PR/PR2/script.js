/**
 * Насосна станція водопостачання — Моніторинг
 * Практична робота №2 · JavaScript. Динамічний контент.
 * Варіант 15
 */

// ==================== Конфігурація параметрів ====================

const PARAMS = [
    {
        id: 'pressure',
        name: 'Тиск на виході',
        unit: 'атм',
        icon: '🔵',
        min: 3,
        max: 6,
        normal: [4, 5.5],
        decimals: 2
    },
    {
        id: 'productivity',
        name: 'Продуктивність',
        unit: 'м³/год',
        icon: '🌊',
        min: 50,
        max: 200,
        normal: [80, 150],
        decimals: 1
    },
    {
        id: 'power',
        name: 'Споживана потужність',
        unit: 'кВт',
        icon: '⚡',
        min: 0,
        max: 100,
        normal: [30, 80],
        decimals: 1
    },
    {
        id: 'waterLevel',
        name: 'Рівень води',
        unit: 'м',
        icon: '📏',
        min: 0,
        max: 10,
        normal: [3, 8],
        decimals: 2
    }
];

// ==================== Стан додатку ====================

let autoInterval = null;
let isAutoEnabled = false;
let measurementLog = [];
const MAX_LOG_ENTRIES = 50;
const MAX_CHART_POINTS = 20;

// Об'єкти Chart.js
let charts = {};

// ==================== Допоміжні функції ====================

/**
 * Генерує випадкове число у заданому діапазоні з вказаною точністю.
 * @param {number} min - мінімальне значення
 * @param {number} max - максимальне значення
 * @param {number} decimals - кількість знаків після коми
 * @returns {number} випадкове число
 */
function getRandom(min, max, decimals) {
    const value = Math.random() * (max - min) + min;
    return parseFloat(value.toFixed(decimals));
}

/**
 * Визначає статус параметра на основі його значення та меж норми.
 * @param {number} value - поточне значення
 * @param {Object} param - об'єкт параметра з полями min, max, normal
 * @returns {string} 'normal' | 'warning' | 'danger'
 */
function checkStatus(value, param) {
    if (value >= param.normal[0] && value <= param.normal[1]) {
        return 'normal';
    }
    if (value >= param.min && value <= param.max) {
        return 'warning';
    }
    return 'danger';
}

/**
 * Повертає текстовий опис статусу українською.
 * @param {string} status - статус ('normal', 'warning', 'danger')
 * @returns {string} текст статусу
 */
function getStatusText(status) {
    switch (status) {
        case 'normal':  return 'Норма';
        case 'warning': return 'Попередження';
        case 'danger':  return 'Критично';
        default:        return '—';
    }
}

/**
 * Форматує поточну дату та час у форматі HH:MM:SS.
 * @returns {string} відформатований час
 */
function formatTimestamp() {
    return new Date().toLocaleTimeString('uk-UA', {
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit'
    });
}

/**
 * Форматує дату для журналу: DD.MM.YYYY HH:MM:SS.
 * @returns {string}
 */
function formatFullTimestamp() {
    const now = new Date();
    return now.toLocaleDateString('uk-UA') + ' ' + now.toLocaleTimeString('uk-UA');
}

// ==================== Генерація карток (DOM) ====================

/**
 * Створює HTML-картки для кожного параметра моніторингу.
 */
function buildParamCards() {
    const grid = document.getElementById('paramsGrid');
    grid.innerHTML = '';

    PARAMS.forEach((param, idx) => {
        const col = document.createElement('div');
        col.className = 'col-md-6 col-xl-3';
        col.innerHTML = `
            <div class="card param-card h-100" id="card-${param.id}" aria-label="${param.name}">
                <div class="card-body text-center p-4">
                    <div class="param-icon">${param.icon}</div>
                    <h5 class="card-title mt-2">${param.name}</h5>
                    <div class="big-number" id="val-${param.id}">0</div>
                    <span class="unit">${param.unit}</span>
                    <div class="mt-2">
                        <span class="status-dot" id="dot-${param.id}"></span>
                        <span class="status-text" id="txt-${param.id}">—</span>
                    </div>
                    <div class="range-bar" title="Діапазон: ${param.min}–${param.max} ${param.unit}">
                        <div class="range-fill" id="bar-${param.id}" style="width: 0%"></div>
                    </div>
                    <small class="text-muted d-block mt-1">
                        Норма: ${param.normal[0]}–${param.normal[1]} ${param.unit}
                    </small>
                </div>
            </div>
        `;
        grid.appendChild(col);
    });
}

// ==================== Оновлення параметрів ====================

/**
 * Генерує нові значення всіх параметрів та оновлює DOM.
 */
function generateSensorData() {
    const data = {};
    PARAMS.forEach(param => {
        data[param.id] = getRandom(param.min, param.max, param.decimals);
    });
    return data;
}

/**
 * Оновлює відображення одного параметра в DOM.
 * @param {Object} param - конфігурація параметра
 * @param {number} value - нове значення
 */
function updateParamDisplay(param, value) {
    const status = checkStatus(value, param);
    const card = document.getElementById('card-' + param.id);
    const valEl = document.getElementById('val-' + param.id);
    const dotEl = document.getElementById('dot-' + param.id);
    const txtEl = document.getElementById('txt-' + param.id);
    const barEl = document.getElementById('bar-' + param.id);

    // Оновлення значення
    valEl.textContent = value;

    // Оновлення статусу картки
    card.classList.remove('status-normal', 'status-warning', 'status-danger');
    card.classList.add('status-' + status);

    // Текст статусу
    txtEl.textContent = getStatusText(status);

    // Прогрес-бар
    const percent = ((value - param.min) / (param.max - param.min)) * 100;
    barEl.style.width = Math.min(100, Math.max(0, percent)) + '%';
    barEl.classList.remove('range-normal', 'range-warning', 'range-danger');
    barEl.classList.add('range-' + status);
}

/**
 * Оновлює додаткові параметри (насоси, обертання, температура).
 */
function updateExtraParams() {
    // Кількість працюючих насосів (1–4)
    const activePumps = getRandom(1, 4, 0);
    document.getElementById('activePumps').textContent = activePumps;

    for (let i = 1; i <= 4; i++) {
        const dot = document.getElementById('pump' + i);
        if (i <= activePumps) {
            dot.classList.add('active');
        } else {
            dot.classList.remove('active');
        }
    }

    // Частота обертання (1000–3000 об/хв)
    const rotation = getRandom(1000, 3000, 0);
    document.getElementById('rotationFreq').textContent = rotation;
    const rotPercent = ((rotation - 1000) / 2000) * 100;
    const rotBar = document.getElementById('rotationBar');
    rotBar.style.width = rotPercent + '%';

    if (rotPercent < 60) {
        rotBar.className = 'progress-bar progress-bar-striped progress-bar-animated bg-success';
    } else if (rotPercent < 85) {
        rotBar.className = 'progress-bar progress-bar-striped progress-bar-animated bg-warning';
    } else {
        rotBar.className = 'progress-bar progress-bar-striped progress-bar-animated bg-danger';
    }

    // Температура двигуна (40–120 °C, норма 50–90)
    const motorTemp = getRandom(40, 120, 0);
    const motorTempEl = document.getElementById('motorTemp');
    const motorStatusEl = document.getElementById('motorTempStatus');
    motorTempEl.textContent = motorTemp;

    if (motorTemp >= 50 && motorTemp <= 90) {
        motorTempEl.style.color = 'var(--status-normal)';
        motorStatusEl.textContent = '✅ Норма';
        motorStatusEl.style.color = 'var(--status-normal)';
    } else if (motorTemp > 90 && motorTemp <= 110) {
        motorTempEl.style.color = 'var(--status-warning)';
        motorStatusEl.textContent = '⚠️ Підвищена';
        motorStatusEl.style.color = 'var(--status-warning)';
    } else if (motorTemp > 110) {
        motorTempEl.style.color = 'var(--status-danger)';
        motorStatusEl.textContent = '🔴 Критично!';
        motorStatusEl.style.color = 'var(--status-danger)';
    } else {
        motorTempEl.style.color = 'var(--status-warning)';
        motorStatusEl.textContent = '⚠️ Холодний пуск';
        motorStatusEl.style.color = 'var(--status-warning)';
    }
}

/**
 * Головна функція оновлення — генерує дані, оновлює DOM, графіки, журнал.
 */
function updateAll() {
    const data = generateSensorData();
    const timestamp = formatTimestamp();

    // Оновлення кожного параметра
    let worstStatus = 'normal';
    PARAMS.forEach(param => {
        const value = data[param.id];
        updateParamDisplay(param, value);
        const status = checkStatus(value, param);
        if (status === 'danger') worstStatus = 'danger';
        else if (status === 'warning' && worstStatus !== 'danger') worstStatus = 'warning';
    });

    // Оновлення додаткових параметрів
    updateExtraParams();

    // Оновлення часу
    document.getElementById('lastUpdate').textContent = timestamp;

    // Оновлення загального статусу системи
    const systemBadge = document.getElementById('systemStatus');
    if (worstStatus === 'normal') {
        systemBadge.textContent = '✅ Система в нормі';
        systemBadge.className = 'badge bg-success fs-6';
    } else if (worstStatus === 'warning') {
        systemBadge.textContent = '⚠️ Є попередження';
        systemBadge.className = 'badge bg-warning text-dark fs-6';
    } else {
        systemBadge.textContent = '🔴 Критичний стан!';
        systemBadge.className = 'badge bg-danger fs-6';
    }

    // Оновлення графіків
    updateCharts(timestamp, data);

    // Запис у журнал
    addLogEntry(timestamp, data, worstStatus);
}

// ==================== Автооновлення ====================

/**
 * Перемикає автоматичне оновлення даних (вмикає/вимикає таймер).
 */
function toggleAutoUpdate() {
    const btn = document.getElementById('autoUpdateBtn');
    const statusBadge = document.getElementById('autoStatus');
    const intervalMs = parseInt(document.getElementById('intervalSelect').value);

    if (!isAutoEnabled) {
        autoInterval = setInterval(updateAll, intervalMs);
        isAutoEnabled = true;
        btn.textContent = '⏸️ Зупинити';
        btn.className = 'btn btn-danger btn-lg';
        statusBadge.textContent = 'Авто: увімк. (' + (intervalMs / 1000) + ' сек)';
        statusBadge.className = 'badge bg-success';
    } else {
        clearInterval(autoInterval);
        autoInterval = null;
        isAutoEnabled = false;
        btn.textContent = '▶️ Автооновлення';
        btn.className = 'btn btn-outline-light btn-lg';
        statusBadge.textContent = 'Авто: вимк.';
        statusBadge.className = 'badge bg-secondary';
    }
}

// ==================== Графіки (Chart.js) ====================

/**
 * Ініціалізує всі графіки Chart.js.
 */
function initCharts() {
    const chartConfigs = [
        { id: 'pressureChart',     label: 'Тиск (атм)',        color: '#0077b6', paramId: 'pressure' },
        { id: 'productivityChart', label: 'Продукт. (м³/год)', color: '#00b4d8', paramId: 'productivity' },
        { id: 'powerChart',        label: 'Потужність (кВт)',   color: '#f39c12', paramId: 'power' },
        { id: 'waterChart',        label: 'Рівень води (м)',    color: '#2ecc71', paramId: 'waterLevel' }
    ];

    chartConfigs.forEach(cfg => {
        const ctx = document.getElementById(cfg.id).getContext('2d');
        charts[cfg.paramId] = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: cfg.label,
                    data: [],
                    borderColor: cfg.color,
                    backgroundColor: cfg.color + '20',
                    fill: true,
                    tension: 0.3,
                    pointRadius: 3,
                    pointHoverRadius: 5
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                animation: { duration: 400 },
                scales: {
                    y: {
                        beginAtZero: false,
                        grid: { color: '#e9ecef' }
                    },
                    x: {
                        grid: { display: false },
                        ticks: { maxTicksLimit: 8 }
                    }
                },
                plugins: {
                    legend: { display: false }
                }
            }
        });
    });
}

/**
 * Оновлює графіки новими даними.
 * @param {string} timestamp - мітка часу
 * @param {Object} data - об'єкт з даними параметрів
 */
function updateCharts(timestamp, data) {
    PARAMS.forEach(param => {
        const chart = charts[param.id];
        if (!chart) return;

        chart.data.labels.push(timestamp);
        chart.data.datasets[0].data.push(data[param.id]);

        // Обмеження кількості точок
        if (chart.data.labels.length > MAX_CHART_POINTS) {
            chart.data.labels.shift();
            chart.data.datasets[0].data.shift();
        }

        chart.update();
    });
}

// ==================== Журнал вимірювань ====================

/**
 * Додає запис до журналу вимірювань.
 * @param {string} timestamp - мітка часу
 * @param {Object} data - дані параметрів
 * @param {string} overallStatus - загальний статус
 */
function addLogEntry(timestamp, data, overallStatus) {
    const entry = {
        time: timestamp,
        pressure: data.pressure,
        productivity: data.productivity,
        power: data.power,
        waterLevel: data.waterLevel,
        status: overallStatus
    };

    measurementLog.unshift(entry);

    // Обмеження розміру журналу
    if (measurementLog.length > MAX_LOG_ENTRIES) {
        measurementLog.pop();
    }

    // Збереження в localStorage
    saveToLocalStorage();

    // Оновлення DOM таблиці
    renderLog();
}

/**
 * Відображає журнал вимірювань у таблиці.
 */
function renderLog() {
    const tbody = document.getElementById('logBody');
    const countEl = document.getElementById('logCount');

    tbody.innerHTML = '';
    countEl.textContent = measurementLog.length;

    measurementLog.forEach(entry => {
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td>${entry.time}</td>
            <td>${entry.pressure}</td>
            <td>${entry.productivity}</td>
            <td>${entry.power}</td>
            <td>${entry.waterLevel}</td>
            <td><span class="log-status-badge ${entry.status}">${getStatusText(entry.status)}</span></td>
        `;
        tbody.appendChild(tr);
    });
}

/**
 * Очищає журнал вимірювань.
 */
function clearLog() {
    measurementLog = [];
    localStorage.removeItem('pumpStationLog');
    renderLog();
}

// ==================== localStorage ====================

/**
 * Зберігає журнал у localStorage.
 */
function saveToLocalStorage() {
    try {
        localStorage.setItem('pumpStationLog', JSON.stringify(measurementLog));
    } catch (e) {
        // localStorage може бути недоступний
    }
}

/**
 * Завантажує журнал із localStorage.
 */
function loadFromLocalStorage() {
    try {
        const saved = localStorage.getItem('pumpStationLog');
        if (saved) {
            measurementLog = JSON.parse(saved);
            renderLog();
        }
    } catch (e) {
        measurementLog = [];
    }
}

// ==================== Експорт CSV ====================

/**
 * Експортує журнал вимірювань у файл CSV.
 */
function exportToCSV() {
    if (measurementLog.length === 0) {
        alert('Журнал порожній. Немає даних для експорту.');
        return;
    }

    const headers = ['Час', 'Тиск (атм)', 'Продуктивність (м³/год)', 'Потужність (кВт)', 'Рівень води (м)', 'Стан'];
    const rows = measurementLog.map(e => [
        e.time,
        e.pressure,
        e.productivity,
        e.power,
        e.waterLevel,
        getStatusText(e.status)
    ]);

    let csv = '\uFEFF'; // BOM для коректного відображення кирилиці
    csv += headers.join(';') + '\n';
    rows.forEach(row => {
        csv += row.join(';') + '\n';
    });

    const blob = new Blob([csv], { type: 'text/csv;charset=utf-8;' });
    const url = URL.createObjectURL(blob);
    const link = document.createElement('a');
    link.href = url;
    link.download = 'pump_station_log_' + new Date().toISOString().slice(0, 10) + '.csv';
    link.click();
    URL.revokeObjectURL(url);
}

// ==================== Годинник ====================

/**
 * Оновлює годинник у заголовку щосекунди.
 */
function updateClock() {
    document.getElementById('clockBadge').textContent = formatTimestamp();
}

// ==================== Ініціалізація ====================

document.addEventListener('DOMContentLoaded', function () {
    // Побудова карток
    buildParamCards();

    // Ініціалізація графіків
    initCharts();

    // Завантаження збережених даних
    loadFromLocalStorage();

    // Перше оновлення
    updateAll();

    // Годинник
    updateClock();
    setInterval(updateClock, 1000);

    // Обробники подій
    document.getElementById('updateBtn').addEventListener('click', updateAll);
    document.getElementById('autoUpdateBtn').addEventListener('click', toggleAutoUpdate);
    document.getElementById('exportCsvBtn').addEventListener('click', exportToCSV);
    document.getElementById('clearLogBtn').addEventListener('click', clearLog);

    // При зміні інтервалу — перезапуск таймера, якщо увімкнено
    document.getElementById('intervalSelect').addEventListener('change', function () {
        if (isAutoEnabled) {
            clearInterval(autoInterval);
            const intervalMs = parseInt(this.value);
            autoInterval = setInterval(updateAll, intervalMs);
            document.getElementById('autoStatus').textContent =
                'Авто: увімк. (' + (intervalMs / 1000) + ' сек)';
        }
    });
});
