// Рендер 2 перших сторінок PDF як PNG через Chrome headless (Puppeteer-core)
const fs = require('fs');
const path = require('path');
const puppeteer = require('puppeteer-core');

const CHROME = 'C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe';
const PDF = path.join(__dirname, 'Звіт_ПР5_preview.pdf');
const OUT = path.join(__dirname, 'screenshots');

(async () => {
    const browser = await puppeteer.launch({ executablePath: CHROME, headless: 'new' });
    const page = await browser.newPage();
    await page.setViewport({ width: 1240, height: 1755 });
    // Chrome PDF viewer URL — page=N для конкретної сторінки
    await page.goto(`file:///${PDF.replace(/\\/g, '/')}#page=1&toolbar=0&navpanes=0&scrollbar=0`,
        { waitUntil: 'networkidle0', timeout: 20000 });
    await new Promise(r => setTimeout(r, 3500));
    await page.screenshot({ path: path.join(OUT, '_preview_page1.png') });
    console.log('✓ page1 saved');

    await page.goto(`file:///${PDF.replace(/\\/g, '/')}#page=2&toolbar=0&navpanes=0&scrollbar=0`,
        { waitUntil: 'networkidle0', timeout: 20000 });
    await new Promise(r => setTimeout(r, 3500));
    await page.screenshot({ path: path.join(OUT, '_preview_page2.png') });
    console.log('✓ page2 saved');

    await browser.close();
})().catch(e => { console.error(e.message); process.exit(1); });
