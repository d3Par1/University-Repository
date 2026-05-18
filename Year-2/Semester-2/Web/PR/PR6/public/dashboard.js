// Dashboard: fetch + polling. Жодного innerHTML з користувацьким вводом — лише textContent.
const csrf = document.querySelector('meta[name="csrf-token"]').content;
const tpl = document.getElementById('apt-card-tpl');
const grid = document.getElementById('apartments-grid');

async function getJSON(url) {
    const r = await fetch(url, { credentials: 'same-origin' });
    if (!r.ok) throw new Error(r.status);
    return r.json();
}
async function postJSON(url, body) {
    const r = await fetch(url, {
        method: 'POST',
        credentials: 'same-origin',
        headers: { 'Content-Type': 'application/json', 'CSRF-Token': csrf },
        body: JSON.stringify(body)
    });
    return { ok: r.ok, status: r.status, data: await r.json().catch(() => ({})) };
}

function renderApartment(a) {
    const node = tpl.content.firstElementChild.cloneNode(true);
    node.querySelector('[data-num]').textContent = a.number;
    node.querySelector('[data-floor]').textContent = 'пов. ' + a.floor;
    node.querySelector('[data-area]').textContent = a.area_m2;
    node.querySelector('[data-cop]').textContent = a.cop;
    node.querySelector('[data-current]').textContent = a.current_temp.toFixed(1) + '°';
    node.querySelector('[data-target]').textContent = a.target_temp.toFixed(1);
    node.querySelector('[data-power]').textContent = a.power_kw.toFixed(2);

    const input = node.querySelector('[data-temp-input]');
    const btn   = node.querySelector('[data-temp-btn]');
    const msg   = node.querySelector('[data-msg]');
    input.value = a.target_temp;

    btn.addEventListener('click', async () => {
        msg.textContent = '';
        const target = parseFloat(input.value);
        const r = await postJSON('/api/heatpump/temperature', { apartmentId: a.id, target });
        if (r.ok) {
            msg.className = 'small mt-1 text-success';
            msg.textContent = '✓ збережено';
            refreshApartments();
        } else {
            msg.className = 'small mt-1 text-danger';
            msg.textContent = '✗ ' + (r.data.error || r.data.errors?.[0]?.msg || ('HTTP ' + r.status));
        }
    });
    return node;
}

async function refreshApartments() {
    const data = await getJSON('/api/apartments');
    grid.innerHTML = '';
    for (const a of data) grid.appendChild(renderApartment(a));
    if (data.length === 0) {
        const empty = document.createElement('div');
        empty.className = 'col-12 text-secondary small';
        empty.textContent = 'Немає закріплених за вами квартир. Зверніться до менеджера.';
        grid.appendChild(empty);
    }
}

// Manager-секція.
async function refreshManager() {
    const eff = document.getElementById('efficiency-cards');
    if (!eff) return;
    const data = await getJSON('/api/system/efficiency').catch(() => null);
    if (!data) return;
    eff.innerHTML = '';
    const cards = [
        ['Квартир',        data.apartments_count],
        ['Площа, м²',      data.total_area_m2],
        ['Електр. кВт',    data.total_electrical_kw],
        ['Тепл. кВт (оц.)', data.thermal_kw_est],
        ['Серед. COP',     data.avg_cop]
    ];
    for (const [k, v] of cards) {
        const c = document.createElement('div');
        c.className = 'col-md-3 col-lg-2';
        c.innerHTML = '<div class="card bg-dark border-info-subtle h-100"><div class="card-body py-3">' +
                      '<div class="small text-secondary"></div><div class="h4 mb-0 text-info"></div></div></div>';
        c.querySelector('.text-secondary').textContent = k;
        c.querySelector('.h4').textContent = v;
        eff.appendChild(c);
    }
    const audit = await getJSON('/api/audit').catch(() => []);
    const body = document.getElementById('audit-body');
    body.innerHTML = '';
    for (const row of audit) {
        const tr = document.createElement('tr');
        for (const v of [
            new Date(row.at).toLocaleString(),
            row.user_id ?? '-',
            row.action,
            row.ip || '-',
            row.detail || ''
        ]) {
            const td = document.createElement('td');
            td.textContent = v; // textContent → XSS impossible
            tr.appendChild(td);
        }
        body.appendChild(tr);
    }
}

// Technician-секція.
async function refreshMaintenance() {
    const body = document.getElementById('maintenance-body');
    if (!body) return;
    const data = await getJSON('/api/maintenance').catch(() => []);
    body.innerHTML = '';
    for (const m of data) {
        const tr = document.createElement('tr');
        for (const v of [
            new Date(m.scheduled_for).toLocaleString(),
            m.apt_number || '-',
            m.title,
            m.notes || '',
            m.status
        ]) {
            const td = document.createElement('td');
            td.textContent = v;
            tr.appendChild(td);
        }
        body.appendChild(tr);
    }
}

const mForm = document.getElementById('maintenance-form');
if (mForm) {
    const feedback = document.getElementById('maintenance-feedback');
    mForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        const fd = new FormData(mForm);
        const payload = {
            apartmentId: fd.get('apartmentId') || null,
            title: fd.get('title'),
            notes: fd.get('notes'),
            scheduledFor: new Date(fd.get('scheduledFor')).toISOString()
        };
        const r = await postJSON('/api/maintenance/schedule', payload);
        feedback.className = r.ok ? 'small text-success' : 'small text-danger';
        feedback.textContent = r.ok ? '✓ заплановано' : ('✗ ' + (r.data.error || r.data.errors?.[0]?.msg));
        if (r.ok) {
            mForm.reset();
            refreshMaintenance();
        }
    });
}

document.getElementById('btn-logout').addEventListener('click', async () => {
    await postJSON('/auth/logout', {});
    location.href = '/login';
});

(async function init() {
    await refreshApartments();
    await refreshManager();
    await refreshMaintenance();
    setInterval(async () => {
        await refreshApartments();
        if (document.getElementById('efficiency-cards')) await refreshManager();
    }, 5000);
})();
