// Скріншоти всіх ключових станів портала через Puppeteer-core (локальний Chrome).
// Передумова: `node server.js` запущено + БД заСіяна (`node seed.js --reset`).
const fs = require('fs');
const path = require('path');
const puppeteer = require('puppeteer-core');

const CHROME_CANDIDATES = [
    'C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe',
    'C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe',
    `${process.env.LOCALAPPDATA}\\Google\\Chrome\\Application\\chrome.exe`,
    'C:\\Program Files\\Microsoft\\Edge\\Application\\msedge.exe',
    'C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe'
];

const URL = process.env.BASE || 'http://localhost:3000';
const OUT = path.join(__dirname, 'screenshots');
fs.mkdirSync(OUT, { recursive: true });

function findChrome() {
    for (const p of CHROME_CANDIDATES) if (fs.existsSync(p)) return p;
    throw new Error('Chrome або Edge не знайдено');
}

async function loginAs(page, email, password) {
    await page.goto(URL + '/login', { waitUntil: 'networkidle0' });
    await page.type('input[name="email"]', email);
    await page.type('input[name="password"]', password);
    await Promise.all([
        page.waitForNavigation({ waitUntil: 'networkidle0', timeout: 8000 }).catch(() => {}),
        page.click('button[type="submit"]')
    ]);
    // даємо API підвантажитися та симулятору зробити tick
    await new Promise(r => setTimeout(r, 1500));
}

async function shoot(page, file, fullPage = true) {
    await page.screenshot({ path: path.join(OUT, file), fullPage });
    console.log(`✓ ${file}`);
}

(async () => {
    const browser = await puppeteer.launch({
        executablePath: findChrome(),
        headless: 'new',
        args: ['--no-sandbox', '--disable-setuid-sandbox']
    });
    try {
        const page = await browser.newPage();
        await page.setViewport({ width: 1440, height: 900, deviceScaleFactor: 1 });

        // 1. Login page (порожня)
        await page.goto(URL + '/login', { waitUntil: 'networkidle0' });
        await shoot(page, 'login.png', false);

        // 2. Register page з заповненим паролем (показати password strength meter).
        await page.goto(URL + '/register', { waitUntil: 'networkidle0' });
        await page.type('input[name="name"]', 'Тестовий Користувач');
        await page.type('input[name="email"]', 'test-screenshot@x.io');
        await page.type('input[id="pwd"]', 'StrongPass1234!');
        await new Promise(r => setTimeout(r, 400));
        await shoot(page, 'register.png', false);

        // 3. Невірний логін → повідомлення про помилку.
        await page.goto(URL + '/login', { waitUntil: 'networkidle0' });
        await page.type('input[name="email"]', 'manager@demo.local');
        await page.type('input[name="password"]', 'WRONG');
        await page.click('button[type="submit"]');
        await new Promise(r => setTimeout(r, 800));
        await shoot(page, 'login-error.png', false);

        // 4. Resident dashboard — лише свої квартири.
        await loginAs(page, 'resident@demo.local', 'Demo12345!');
        await shoot(page, 'dashboard-resident.png', true);

        // 5. Logout перед іншою роллю.
        await page.evaluate(async () => {
            const csrf = document.querySelector('meta[name="csrf-token"]').content;
            await fetch('/auth/logout', { method: 'POST', credentials: 'same-origin',
                                          headers: { 'Content-Type': 'application/json', 'CSRF-Token': csrf },
                                          body: '{}' });
        });

        // 6. Manager dashboard — всі квартири + efficiency + audit log.
        await loginAs(page, 'manager@demo.local', 'Demo12345!');
        await shoot(page, 'dashboard-manager.png', true);

        console.log('\n✓ Усі скріншоти у', OUT);
    } finally {
        await browser.close();
    }
})().catch(e => { console.error('Fatal:', e); process.exit(1); });
