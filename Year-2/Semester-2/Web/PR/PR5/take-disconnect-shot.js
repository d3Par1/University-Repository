// Окремий скріншот стану «Офлайн» — підключаємось, чекаємо snapshot,
// потім форсовано закриваємо WebSocket з боку клієнта і фіксуємо UI.
const fs = require('fs');
const path = require('path');
const puppeteer = require('puppeteer-core');

const CHROME = 'C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe';
const URL = 'http://localhost:3000/';
const OUT = path.join(__dirname, 'screenshots');

(async () => {
    const browser = await puppeteer.launch({ executablePath: CHROME, headless: 'new' });
    const page = await browser.newPage();
    await page.setViewport({ width: 1920, height: 1080 });
    await page.goto(URL, { waitUntil: 'networkidle0' });
    await page.waitForSelector('#temp-gauges .gauge', { timeout: 8000 });
    await new Promise(r => setTimeout(r, 2000));

    // Підмінюємо connectWS, щоб не реконектився, і закриваємо поточний WS
    await page.evaluate(() => {
        window.connectWS = () => {};
        if (typeof ws !== 'undefined' && ws) ws.close();
    });

    // Чекаємо поки спрацює onclose → setWsStatus(false)
    await page.waitForFunction(
        () => document.getElementById('ws-dot').classList.contains('ws-offline'),
        { timeout: 5000 }
    );
    await new Promise(r => setTimeout(r, 800));

    await page.screenshot({ path: path.join(OUT, 'disconnect.png'), fullPage: false });
    console.log('✓ disconnect.png');
    await browser.close();
})().catch(e => { console.error(e.message); process.exit(1); });
