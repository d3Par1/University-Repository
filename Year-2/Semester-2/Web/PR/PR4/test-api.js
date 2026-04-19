// Тестування REST API через fetch (Node.js 18+)
const BASE = 'http://localhost:3000/api/lighting-systems';

async function call(label, url, opts = {}) {
    const res = await fetch(url, opts);
    const data = await res.json();
    console.log(`\n[${label}] ${opts.method || 'GET'} ${url}  →  HTTP ${res.status}`);
    console.log(JSON.stringify(data, null, 2));
    return data;
}

(async () => {
    try {
        await call('1. Список усіх систем', BASE);
        await call('2. Фільтр за режимом auto', BASE + '?mode=auto');
        await call('3. Пошук "Парк"', BASE + '?search=Парк');
        await call('4. Сортування за brightness desc', BASE + '?sort=brightness&order=desc');
        await call('5. Деталі системи id=1', BASE + '/1');
        await call('6. Розклад системи id=2', BASE + '/2/schedule');

        const created = await call('7. POST створення', BASE, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                name: 'Тестовий бульвар',
                lampCount: 50,
                activeLamps: 50,
                brightness: 90,
                mode: 'auto',
                lightLevel: 28
            })
        });

        await call('8. POST control turn_off_all', `${BASE}/${created.id}/control`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ action: 'turn_off_all' })
        });

        await call('9. PATCH brightness=60, mode=manual', `${BASE}/${created.id}`, {
            method: 'PATCH',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ brightness: 60, mode: 'manual' })
        });

        await call('10. PUT brightness=40', `${BASE}/${created.id}/brightness`, {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ brightness: 40 })
        });

        await call('11. Помилка: brightness=150', `${BASE}/${created.id}/brightness`, {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ brightness: 150 })
        });

        await call('12. Помилка: відсутнє name', BASE, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ lampCount: 10 })
        });

        await call('13. 404: неіснуючий id', BASE + '/9999');

        await call('14. DELETE створеної системи', `${BASE}/${created.id}`, { method: 'DELETE' });

        await call('15. Статистика', BASE + '/stats');

        console.log('\nТестування завершено.');
    } catch (e) {
        console.error('Помилка тестування:', e.message);
    }
})();
