const API = '/api/lighting-systems';
const dataEl = document.getElementById('current-data');
const countEl = document.getElementById('count');
const respEl = document.getElementById('response');
const badgeEl = document.getElementById('status-badge');
const statusTextEl = document.getElementById('status-text');

async function call(method, url, body) {
    const opts = { method, headers: { 'Content-Type': 'application/json' } };
    if (body) opts.body = JSON.stringify(body);
    const res = await fetch(url, opts);
    let data;
    try { data = await res.json(); } catch { data = { raw: await res.text() }; }
    showResponse(res.status, data);
    await refresh();
    return data;
}

function showResponse(status, data) {
    badgeEl.textContent = `Status: ${status}`;
    badgeEl.classList.toggle('error', status >= 400);
    statusTextEl.textContent = status >= 200 && status < 300 ? 'OK' : status >= 400 ? 'Error' : '';
    respEl.textContent = JSON.stringify(data, null, 2);
}

async function refresh() {
    try {
        const res = await fetch(API);
        const data = await res.json();
        dataEl.textContent = JSON.stringify(data, null, 2);
        countEl.textContent = Array.isArray(data) ? data.length : (data.total ?? '—');
    } catch (e) {
        dataEl.textContent = 'Помилка завантаження: ' + e.message;
    }
}

document.querySelectorAll('.ep[data-method]').forEach(btn => {
    btn.addEventListener('click', () => call(btn.dataset.method, btn.dataset.url));
});

document.getElementById('btn-create').addEventListener('click', () => {
    const name = prompt('Назва системи:', 'Тестовий бульвар');
    if (!name) return;
    const lampCount = parseInt(prompt('Кількість світильників:', '50'));
    const brightness = parseInt(prompt('Яскравість (0-100):', '80'));
    call('POST', API, { name, lampCount, activeLamps: lampCount, brightness, mode: 'auto', lightLevel: 25 });
});

document.getElementById('btn-control').addEventListener('click', () => {
    const id = prompt('ID системи:', '1');
    if (!id) return;
    const action = prompt('Команда (turn_on_all, turn_off_all, turn_on, turn_off):', 'turn_off_all');
    const body = { action };
    if (action === 'turn_on' || action === 'turn_off') body.count = parseInt(prompt('Скільки світильників?', '10'));
    call('POST', `${API}/${id}/control`, body);
});

document.getElementById('btn-brightness').addEventListener('click', () => {
    const id = prompt('ID системи:', '1');
    if (!id) return;
    const brightness = parseInt(prompt('Нова яскравість (0-100):', '60'));
    call('PUT', `${API}/${id}/brightness`, { brightness });
});

document.getElementById('btn-delete').addEventListener('click', () => {
    const id = prompt('ID системи для видалення:', '');
    if (!id) return;
    if (!confirm(`Видалити систему #${id}?`)) return;
    call('DELETE', `${API}/${id}`);
});

refresh();
