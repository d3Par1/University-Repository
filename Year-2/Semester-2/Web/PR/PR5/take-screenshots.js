// take-screenshots.js — генерація скріншотів дашборду через Puppeteer-core
// Використовує локально встановлений Chrome (не завантажує власний Chromium).
// Запуск: 1) `node server.js` у одному терміналі  2) `node take-screenshots.js` у іншому

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

const URL = 'http://localhost:3000/';
const OUT = path.join(__dirname, 'screenshots');
if (!fs.existsSync(OUT)) fs.mkdirSync(OUT, { recursive: true });

function findChrome() {
    for (const p of CHROME_CANDIDATES) if (fs.existsSync(p)) return p;
    throw new Error('Chrome або Edge не знайдено');
}

async function shootElement(page, selector, file) {
    const el = await page.$(selector);
    if (!el) { console.warn(`[!] селектор не знайдено: ${selector}`); return; }
    await el.screenshot({ path: path.join(OUT, file) });
    console.log(`✓ ${file}`);
}

async function shootViewport(page, file, fullPage = true) {
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
        // ─── Desktop ─────────────────────────────────────────────
        const page = await browser.newPage();
        await page.setViewport({ width: 1920, height: 1080, deviceScaleFactor: 1 });
        await page.goto(URL, { waitUntil: 'networkidle0', timeout: 15000 });

        // Чекаємо поки WebSocket доставить snapshot та намалює gauges
        await page.waitForSelector('#temp-gauges .gauge', { timeout: 8000 });
        await new Promise(r => setTimeout(r, 2500));      // дати tick прийти й Chart.js доанімувати

        await shootViewport(page, 'desktop.png', true);

        // ─── Окремі секції (element-precise) ─────────────────────
        await shootElement(page, '#composition-chart',                              'composition.png');
        await shootElement(page, '#temp-gauges',                                    'gauges.png');
        await shootElement(page, 'section.panel:has(.data-table)',                  'table.png');
        await shootElement(page, 'section.chart-row',                               'charts.png');
        await page.close();

        // ─── Mobile ──────────────────────────────────────────────
        const mobile = await browser.newPage();
        await mobile.setViewport({ width: 375, height: 812, deviceScaleFactor: 2, isMobile: true });
        await mobile.goto(URL, { waitUntil: 'networkidle0', timeout: 15000 });
        await mobile.waitForSelector('#temp-gauges .gauge', { timeout: 8000 });
        await new Promise(r => setTimeout(r, 2000));
        await shootViewport(mobile, 'mobile.png', true);
        await mobile.close();

        console.log('\nЗгенеровано всі скріншоти у:', OUT);
    } finally {
        await browser.close();
    }
})().catch(e => {
    console.error('Помилка:', e.message);
    process.exit(1);
});
