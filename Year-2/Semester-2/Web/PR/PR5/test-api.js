// Тестування REST API біогазового моніторингу (Node.js 18+)
// Використання: запустити сервер `npm start`, потім `npm test` в іншому терміналі

const BASE = 'http://localhost:3000';

async function call(label, url, opts = {}) {
    const res = await fetch(BASE + url, opts);
    let data;
    try { data = await res.json(); } catch { data = { raw: await res.text() }; }
    console.log(`\n[${label}] ${opts.method || 'GET'} ${url}  →  HTTP ${res.status}`);
    console.log(JSON.stringify(data, null, 2).slice(0, 500));
    return data;
}

(async () => {
    try {
        await call('1. Список усіх біогазових станцій',  '/api/biogas-plants');
        await call('2. Агрегована статистика',           '/api/biogas-plants/stats');
        await call('3. Деталі станції id=1',             '/api/biogas-plants/1');
        await call('4. Історія за 24 години',            '/api/history');
        await call('5. Статус сервера + WS-клієнти',     '/api/status');

        await call('6. 404: неіснуючий id',              '/api/biogas-plants/999');
        await call('7. 404: невідомий маршрут',          '/api/unknown');

        console.log('\nТестування REST API завершено успішно.');
    } catch (e) {
        console.error('Помилка тестування:', e.message);
        process.exit(1);
    }
})();
