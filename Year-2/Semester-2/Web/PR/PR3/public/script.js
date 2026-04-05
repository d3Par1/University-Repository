/**
 * Реєстр резервних джерел живлення (ДГУ)
 * Практична робота №3 · HTML-форми та серверна обробка (Node.js + Express)
 * Варіант 15 — Клієнтський JavaScript
 */

// ==================== Елементи DOM ====================

const form = document.getElementById('generatorForm');
const alertContainer = document.getElementById('alertContainer');
const generatorsList = document.getElementById('generatorsList');
const resultCount = document.getElementById('resultCount');
const formTitle = document.getElementById('formTitle');
const submitBtn = document.getElementById('submitBtn');
const cancelEditBtn = document.getElementById('cancelEditBtn');
const editIdInput = document.getElementById('editId');

// Поля форми
const powerInput = document.getElementById('power');
const fuelConsumptionInput = document.getElementById('fuelConsumption');
const consumptionHint = document.getElementById('consumptionHint');

// Фільтри
const searchInput = document.getElementById('searchInput');
const filterType = document.getElementById('filterType');
const filterStatus = document.getElementById('filterStatus');

// ==================== Довідники ====================

const GENERATOR_TYPES = {
    'diesel':      'Дизельна ГУ',
    'gasoline':    'Бензинова ГУ',
    'gas':         'Газова ГУ',
    'gas-turbine': 'Газотурбінна',
    'portable':    'Портативна'
};

const FUEL_TYPES = {
    'diesel-fuel':  'Дизельне паливо',
    'gasoline-fuel':'Бензин АІ-95',
    'natural-gas':  'Природний газ',
    'propane':      'Пропан-бутан'
};

const STATUS_NAMES = {
    'operational':        'Справний',
    'needs-maintenance':  'Потребує ТО',
    'faulty':             'Несправний',
    'decommissioned':     'Виведений з експл.'
};

// Коефіцієнти витрати палива (л/кВА·год) для автоматичного розрахунку
const FUEL_RATE = {
    'diesel-fuel':   0.25,
    'gasoline-fuel':  0.35,
    'natural-gas':   0.30,
    'propane':       0.32
};

// ==================== Ініціалізація ====================

document.addEventListener('DOMContentLoaded', () => {
    loadGenerators();
    loadStats();

    // Автоматичний розрахунок витрати палива при зміні потужності або типу палива
    powerInput.addEventListener('input', autoCalcConsumption);
    document.getElementById('fuelType').addEventListener('change', autoCalcConsumption);

    // Фільтрація з debounce для пошуку
    let searchTimeout;
    searchInput.addEventListener('input', () => {
        clearTimeout(searchTimeout);
        searchTimeout = setTimeout(loadGenerators, 300);
    });
    filterType.addEventListener('change', loadGenerators);
    filterStatus.addEventListener('change', loadGenerators);
});

// ==================== Робота з формою ====================

/**
 * Автоматично розраховує приблизну витрату палива.
 */
function autoCalcConsumption() {
    const power = parseFloat(powerInput.value);
    const fuelType = document.getElementById('fuelType').value;

    if (power > 0 && FUEL_RATE[fuelType]) {
        const estimated = Math.round(power * FUEL_RATE[fuelType] * 10) / 10;
        fuelConsumptionInput.value = estimated;
        consumptionHint.textContent = `Розрахунок: ${power} кВА × ${FUEL_RATE[fuelType]} = ~${estimated} л/год`;
        consumptionHint.style.color = '#16a34a';
    } else {
        consumptionHint.textContent = 'Розрахунок: ~0.25 л/кВА·год';
        consumptionHint.style.color = '';
    }
}

/**
 * Обробка відправки форми (реєстрація або оновлення).
 */
form.addEventListener('submit', async (e) => {
    e.preventDefault();

    // Клієнтська валідація Bootstrap
    if (!form.checkValidity()) {
        form.classList.add('was-validated');
        return;
    }

    const formData = new FormData(form);
    const data = Object.fromEntries(formData);
    const editId = editIdInput.value;

    try {
        const url = editId ? `/api/generators/${editId}` : '/api/generators';
        const method = editId ? 'PUT' : 'POST';

        const response = await fetch(url, {
            method,
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        });

        const result = await response.json();

        if (result.success) {
            showAlert('success', result.message);
            form.reset();
            form.classList.remove('was-validated');
            cancelEdit();
            loadGenerators();
            loadStats();
        } else {
            const errorMsg = result.errors
                ? result.errors.join('; ')
                : result.message;
            showAlert('danger', errorMsg);
        }
    } catch (error) {
        showAlert('danger', "Помилка з'єднання з сервером");
        console.error('Error:', error);
    }
});

// ==================== Завантаження даних ====================

/**
 * Завантажує список ДГУ з сервера з урахуванням фільтрів.
 */
async function loadGenerators() {
    try {
        const params = new URLSearchParams();
        if (searchInput.value) params.set('search', searchInput.value);
        if (filterType.value)  params.set('type', filterType.value);
        if (filterStatus.value) params.set('status', filterStatus.value);

        const response = await fetch('/api/generators?' + params.toString());
        const generators = await response.json();

        displayGenerators(generators);
        resultCount.textContent = generators.length;
    } catch (error) {
        generatorsList.innerHTML = '<p class="text-danger text-center py-4">Помилка завантаження даних</p>';
        console.error('Error:', error);
    }
}

/**
 * Завантажує та відображає статистику.
 */
async function loadStats() {
    try {
        const response = await fetch('/api/generators/stats');
        const stats = await response.json();

        document.getElementById('statTotal').textContent = stats.total;
        document.getElementById('statPower').textContent = stats.totalPower;
        document.getElementById('statConsumption').textContent = stats.avgConsumption;
        document.getElementById('statMaintenance').textContent =
            (stats.byStatus['needs-maintenance'] || 0) + (stats.byStatus['faulty'] || 0);
    } catch (error) {
        console.error('Error loading stats:', error);
    }
}

// ==================== Відображення ====================

/**
 * Відображає список ДГУ у вигляді карток.
 * @param {Array} generators - масив записів ДГУ
 */
function displayGenerators(generators) {
    if (generators.length === 0) {
        generatorsList.innerHTML = '<p class="text-muted text-center py-4">Немає зареєстрованих ДГУ</p>';
        return;
    }

    const today = new Date();
    today.setHours(0, 0, 0, 0);
    const soon = new Date(today);
    soon.setDate(soon.getDate() + 14); // 14 днів — попередження

    generatorsList.innerHTML = generators.map(g => {
        // Визначення статусу ТО
        let maintenanceClass = '';
        let maintenanceNote = '';
        if (g.nextMaintenance) {
            const nextDate = new Date(g.nextMaintenance);
            if (nextDate < today) {
                maintenanceClass = 'maintenance-overdue';
                maintenanceNote = '<span class="text-danger fw-bold small">&#128308; ТО прострочено!</span>';
            } else if (nextDate <= soon) {
                maintenanceClass = 'maintenance-soon';
                maintenanceNote = '<span class="text-warning fw-bold small">&#9888; ТО найближчим часом</span>';
            }
        }

        return `
            <div class="generator-card ${maintenanceClass}">
                <div class="d-flex justify-content-between align-items-start">
                    <h5>${escapeHtml(g.objectName)}</h5>
                    <span class="status-badge ${g.status}">${STATUS_NAMES[g.status] || g.status}</span>
                </div>
                <p><span class="label">Тип:</span> ${GENERATOR_TYPES[g.generatorType] || g.generatorType}</p>
                <p><span class="label">Потужність:</span> ${g.power} кВА</p>
                <p><span class="label">Паливо:</span> ${FUEL_TYPES[g.fuelType] || g.fuelType} · Витрата: ${g.fuelConsumption} л/год</p>
                <p><span class="label">Адреса:</span> ${escapeHtml(g.address)}</p>
                ${g.lastMaintenance ? `<p><span class="label">Останнє ТО:</span> ${formatDate(g.lastMaintenance)}</p>` : ''}
                ${g.nextMaintenance ? `<p><span class="label">Наступне ТО:</span> ${formatDate(g.nextMaintenance)} ${maintenanceNote}</p>` : ''}
                ${g.notes ? `<p><span class="label">Примітки:</span> ${escapeHtml(g.notes)}</p>` : ''}
                <p class="text-muted small mb-0">Зареєстровано: ${formatDateTime(g.registrationDate)}</p>
                <div class="card-actions">
                    <button class="btn btn-sm btn-outline-primary" onclick="editGenerator('${g.id}')">Редагувати</button>
                    <button class="btn btn-sm btn-outline-danger" onclick="deleteGenerator('${g.id}')">Видалити</button>
                </div>
            </div>
        `;
    }).join('');
}

// ==================== CRUD операції ====================

/**
 * Заповнює форму даними ДГУ для редагування.
 * @param {string} id - ідентифікатор ДГУ
 */
async function editGenerator(id) {
    try {
        const response = await fetch('/api/generators');
        const generators = await response.json();
        const g = generators.find(item => item.id === id);

        if (!g) {
            showAlert('danger', 'ДГУ не знайдено');
            return;
        }

        // Заповнення форми
        editIdInput.value = g.id;
        document.getElementById('objectName').value = g.objectName;
        document.getElementById('generatorType').value = g.generatorType;
        document.getElementById('power').value = g.power;
        document.getElementById('fuelType').value = g.fuelType;
        document.getElementById('address').value = g.address;
        document.getElementById('fuelConsumption').value = g.fuelConsumption;
        document.getElementById('lastMaintenance').value = g.lastMaintenance || '';
        document.getElementById('nextMaintenance').value = g.nextMaintenance || '';
        document.getElementById('status').value = g.status;
        document.getElementById('notes').value = g.notes || '';

        // Зміна UI
        formTitle.textContent = 'Редагування ДГУ';
        submitBtn.textContent = 'Зберегти зміни';
        cancelEditBtn.style.display = 'inline-block';

        // Прокрутка до форми
        document.getElementById('form-section').scrollIntoView({ behavior: 'smooth' });
    } catch (error) {
        showAlert('danger', 'Помилка завантаження даних');
    }
}

/**
 * Скасовує режим редагування.
 */
function cancelEdit() {
    editIdInput.value = '';
    formTitle.textContent = 'Реєстрація нового ДГУ';
    submitBtn.textContent = 'Зареєструвати';
    cancelEditBtn.style.display = 'none';
    form.classList.remove('was-validated');
}

/**
 * Видаляє ДГУ за ідентифікатором.
 * @param {string} id - ідентифікатор ДГУ
 */
async function deleteGenerator(id) {
    if (!confirm('Ви впевнені, що хочете видалити цей ДГУ з реєстру?')) {
        return;
    }

    try {
        const response = await fetch(`/api/generators/${id}`, { method: 'DELETE' });
        const result = await response.json();

        if (result.success) {
            showAlert('success', result.message);
            loadGenerators();
            loadStats();
        } else {
            showAlert('danger', result.message);
        }
    } catch (error) {
        showAlert('danger', "Помилка з'єднання з сервером");
    }
}

// ==================== Фільтри ====================

/**
 * Скидає всі фільтри та перезавантажує список.
 */
function resetFilters() {
    searchInput.value = '';
    filterType.value = '';
    filterStatus.value = '';
    loadGenerators();
}

// ==================== Допоміжні функції ====================

/**
 * Показує повідомлення (alert) з автоматичним зникненням.
 * @param {string} type - 'success' | 'danger' | 'warning'
 * @param {string} text - текст повідомлення
 */
function showAlert(type, text) {
    const alert = document.createElement('div');
    alert.className = `alert alert-${type} alert-dismissible fade show`;
    alert.innerHTML = `
        ${text}
        <button type="button" class="btn-close" data-bs-dismiss="alert"></button>
    `;
    alertContainer.prepend(alert);

    setTimeout(() => {
        alert.classList.remove('show');
        setTimeout(() => alert.remove(), 300);
    }, 4000);
}

/**
 * Екранує HTML-символи для запобігання XSS.
 * @param {string} str - рядок для екранування
 * @returns {string} безпечний рядок
 */
function escapeHtml(str) {
    const div = document.createElement('div');
    div.textContent = str;
    return div.innerHTML;
}

/**
 * Форматує ISO-дату у DD.MM.YYYY.
 * @param {string} isoDate - дата ISO
 * @returns {string}
 */
function formatDate(isoDate) {
    if (!isoDate) return '—';
    const d = new Date(isoDate);
    return d.toLocaleDateString('uk-UA');
}

/**
 * Форматує ISO datetime у DD.MM.YYYY HH:MM.
 * @param {string} isoDateTime - дата-час ISO
 * @returns {string}
 */
function formatDateTime(isoDateTime) {
    if (!isoDateTime) return '—';
    const d = new Date(isoDateTime);
    return d.toLocaleDateString('uk-UA') + ' ' + d.toLocaleTimeString('uk-UA', {
        hour: '2-digit',
        minute: '2-digit'
    });
}
