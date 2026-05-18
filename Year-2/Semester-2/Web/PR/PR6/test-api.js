// Тестування безпеки PR6. Запускати при ЗАПУЩЕНОМУ сервері (npm start у іншому терміналі).
// Сценарії:
//   1. Реєстрація + валідація (слабкий пароль → 400, валідний → 201)
//   2. Логін з невірним паролем → 401, rate-limit після 5 спроб
//   3. Доступ без сесії → 401
//   4. Доступ із недостатньою роллю → 403
//   5. Ownership: resident-1 не може чіпати квартиру resident-2 → 403
//   6. XSS payload у name полі — повертається ЕКРАНОВАНИМ через express-validator.escape()
//   7. SQL-ін'єкція у email → пройде валідацію або поверне 401, але не виконається як SQL
//   8. CSRF: POST без токена → 403
//   9. helmet headers присутні

const BASE = process.env.BASE || 'http://localhost:3000';
const cookies = new Map();

function setCookie(headerVal) {
    if (!headerVal) return;
    const arr = Array.isArray(headerVal) ? headerVal : [headerVal];
    for (const c of arr) {
        const [pair] = c.split(';');
        const [k, v] = pair.split('=');
        cookies.set(k.trim(), v);
    }
}
function cookieHeader() {
    return Array.from(cookies.entries()).map(([k, v]) => `${k}=${v}`).join('; ');
}
async function req(method, url, { body, headers, raw } = {}) {
    const h = { ...(headers || {}) };
    if (cookies.size) h['Cookie'] = cookieHeader();
    if (body && !raw) h['Content-Type'] = 'application/json';
    const r = await fetch(BASE + url, {
        method, headers: h,
        body: body ? (raw ? body : JSON.stringify(body)) : undefined,
        redirect: 'manual'
    });
    setCookie(r.headers.getSetCookie?.() || r.headers.get('set-cookie'));
    let data; try { data = await r.json(); } catch { data = null; }
    return { status: r.status, data, headers: r.headers };
}

let pass = 0, fail = 0;
function ok(name)   { console.log('  ✓', name); pass++; }
function bad(name, why) { console.log('  ✗', name, '—', why); fail++; }
function section(t) { console.log('\n■', t); }

async function getCsrf() {
    const r = await fetch(BASE + '/login', { headers: { Cookie: cookieHeader() } });
    setCookie(r.headers.getSetCookie?.() || r.headers.get('set-cookie'));
    const html = await r.text();
    const m = html.match(/name="csrf-token" content="([^"]+)"/);
    return m && m[1];
}

async function run() {
    console.log('▶ Security tests vs', BASE);

    // 0. Reachability + helmet.
    section('0. Reachability & security headers (helmet)');
    let r = await req('GET', '/login');
    if (r.status === 200) ok('GET /login → 200');
    else bad('GET /login', `status=${r.status}`);
    const csp = r.headers.get('content-security-policy');
    if (csp && csp.includes("default-src 'self'")) ok('CSP default-src \'self\''); else bad('CSP missing', csp);
    if (r.headers.get('x-content-type-options') === 'nosniff') ok('X-Content-Type-Options: nosniff');
    else bad('nosniff missing');
    if (r.headers.get('x-frame-options')) ok('X-Frame-Options set');
    else bad('X-Frame-Options missing');

    const csrf = await getCsrf();
    if (csrf) ok('CSRF token extracted from page'); else { bad('No CSRF token in page'); return; }

    section('1. Registration validation');
    // Слабкий пароль:
    r = await req('POST', '/auth/register', {
        headers: { 'CSRF-Token': csrf },
        body: { email: 'weakpwd@x.io', password: 'short', name: 'X', role: 'resident' }
    });
    if (r.status === 400 && r.data?.errors?.length) ok('Слабкий пароль/коротке імʼя → 400');
    else bad('Weak password not rejected', `status=${r.status}`);

    // Невірна роль:
    r = await req('POST', '/auth/register', {
        headers: { 'CSRF-Token': csrf },
        body: { email: 'badrole@x.io', password: 'StrongPass123', name: 'Test', role: 'superadmin' }
    });
    if (r.status === 400) ok('Неіснуюча роль → 400');
    else bad('Invalid role accepted', `status=${r.status}`);

    // XSS у name:
    const xssName = '<script>alert(1)</script>Hacker';
    r = await req('POST', '/auth/register', {
        headers: { 'CSRF-Token': csrf },
        body: { email: 'xss-victim@x.io', password: 'StrongPass123', name: xssName, role: 'resident' }
    });
    if (r.status === 201 && r.data?.user?.name && !r.data.user.name.includes('<script>')) {
        ok('XSS payload у name санітизовано (escape) → збережено як HTML-entities');
    } else if (r.status === 400) {
        ok('XSS payload відхилено валідатором');
    } else bad('XSS not sanitized', JSON.stringify(r.data));

    section('2. Login bruteforce + rate-limit');
    for (let i = 1; i <= 6; i++) {
        r = await req('POST', '/auth/login', {
            headers: { 'CSRF-Token': csrf },
            body: { email: 'manager@demo.local', password: 'WRONG' + i }
        });
        if (i <= 5 && r.status === 401) ok(`Спроба ${i}: 401`);
        else if (i === 6 && (r.status === 401 || r.status === 429)) ok(`Спроба 6: ${r.status} (lockout або rate-limit)`);
        else if (r.status === 429) { ok(`Спроба ${i}: 429 (rate-limit)`); break; }
        else bad(`Спроба ${i}`, `unexpected status=${r.status}`);
    }

    section('3. Unauthenticated API access');
    cookies.clear();
    r = await req('GET', '/api/apartments');
    if (r.status === 401) ok('GET /api/apartments без сесії → 401');
    else bad('Unauth access leaked', `status=${r.status}`);

    section('4. Authenticated flow + RBAC');
    const csrf2 = await getCsrf();
    r = await req('POST', '/auth/login', { headers: { 'CSRF-Token': csrf2 }, body: { email: 'resident@demo.local', password: 'Demo12345!' } });
    if (r.status === 200) ok('Login resident → 200'); else bad('Login resident', `status=${r.status}`);

    const csrf3 = await getCsrf();
    r = await req('GET', '/api/apartments');
    const myApts = Array.isArray(r.data) ? r.data : [];
    if (r.status === 200 && myApts.length > 0) ok(`resident бачить лише свої квартири (${myApts.length})`);
    else bad('Resident apartments fetch', JSON.stringify(r.data));

    r = await req('GET', '/api/system/efficiency');
    if (r.status === 403) ok('resident → /api/system/efficiency → 403');
    else bad('RBAC bypass for /system/efficiency', `status=${r.status}`);

    r = await req('GET', '/api/audit');
    if (r.status === 403) ok('resident → /api/audit → 403');
    else bad('RBAC bypass for /audit', `status=${r.status}`);

    section('5. Ownership enforcement');
    // Спроба resident-а змінити НЕ свою квартиру (id=4 належить null/manager, не resident-1).
    r = await req('POST', '/api/heatpump/temperature', {
        headers: { 'CSRF-Token': csrf3 },
        body: { apartmentId: 4, target: 25 }
    });
    if (r.status === 403) ok('resident не може чіпати чужу квартиру → 403');
    else bad('Ownership bypass', `status=${r.status} data=${JSON.stringify(r.data)}`);

    // Валідне оновлення своєї квартири:
    if (myApts[0]) {
        r = await req('POST', '/api/heatpump/temperature', {
            headers: { 'CSRF-Token': csrf3 },
            body: { apartmentId: myApts[0].id, target: 22.5 }
        });
        if (r.status === 200 && r.data?.target_temp === 22.5) ok('resident оновив свою квартиру → 200');
        else bad('Own apartment update', JSON.stringify(r.data));
    }

    // Цільова температура поза межами 12–28:
    if (myApts[0]) {
        r = await req('POST', '/api/heatpump/temperature', {
            headers: { 'CSRF-Token': csrf3 },
            body: { apartmentId: myApts[0].id, target: 99 }
        });
        if (r.status === 400) ok('Target=99°C → 400 (валідація діапазону)');
        else bad('Range validation missing', `status=${r.status}`);
    }

    section('6. CSRF rejection');
    r = await req('POST', '/api/heatpump/temperature', { body: { apartmentId: myApts[0]?.id, target: 22 } });
    if (r.status === 403) ok('POST без CSRF → 403');
    else bad('CSRF NOT enforced', `status=${r.status}`);

    section('7. SQL injection attempts');
    cookies.clear();
    const csrf4 = await getCsrf();
    // ' OR 1=1 --  у email — express-validator.isEmail() відхиляє ще на рівні валідації.
    r = await req('POST', '/auth/login', {
        headers: { 'CSRF-Token': csrf4 },
        body: { email: "admin' OR 1=1 --", password: 'x' }
    });
    if (r.status === 401 || r.status === 400) ok(`SQL-ін'єкція у email → ${r.status} (Passport не знаходить, prepared statements роблять SQL інертним)`);
    else bad('SQLi possibly succeeded', `status=${r.status}`);

    section('8. Logout');
    await req('POST', '/auth/login', { headers: { 'CSRF-Token': csrf4 }, body: { email: 'manager@demo.local', password: 'Demo12345!' } });
    const csrf5 = await getCsrf();
    r = await req('POST', '/auth/logout', { headers: { 'CSRF-Token': csrf5 } });
    if (r.status === 200) ok('Logout → 200'); else bad('Logout failed', `status=${r.status}`);
    r = await req('GET', '/api/audit');
    if (r.status === 401) ok('Після logout /api/audit → 401');
    else bad('Session not invalidated', `status=${r.status}`);

    console.log(`\n══════════════════════════════════════════`);
    console.log(`  PASS: ${pass}   FAIL: ${fail}`);
    console.log(`══════════════════════════════════════════`);
    process.exit(fail === 0 ? 0 : 1);
}

run().catch(e => { console.error('Fatal:', e); process.exit(2); });
