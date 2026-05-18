// Рендерить test-output.txt у консольний PNG (для включення у звіт).
const fs = require('fs');
const path = require('path');
const puppeteer = require('puppeteer-core');

const CHROME_CANDIDATES = [
    'C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe',
    'C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe',
    `${process.env.LOCALAPPDATA}\\Google\\Chrome\\Application\\chrome.exe`,
    'C:\\Program Files\\Microsoft\\Edge\\Application\\msedge.exe'
];
function findChrome() {
    for (const p of CHROME_CANDIDATES) if (fs.existsSync(p)) return p;
    throw new Error('Chrome не знайдено');
}

const txt = fs.readFileSync(path.join(__dirname, 'screenshots', 'test-output.txt'), 'utf-8');
const esc = s => s.replace(/[<&]/g, c => ({ '<': '&lt;', '&': '&amp;' }[c]));

// Кольорове розфарбовування ANSI-подібного виводу.
const html = esc(txt)
    .replace(/^▶.*$/gm,  '<span style="color:#7dd3fc;font-weight:600">$&</span>')
    .replace(/^■.*$/gm,  '<span style="color:#fbbf24;font-weight:600">$&</span>')
    .replace(/✓/g,       '<span style="color:#34d399">✓</span>')
    .replace(/✗/g,       '<span style="color:#ef4444">✗</span>')
    .replace(/PASS:\s*\d+/g, m => `<span style="color:#34d399;font-weight:700">${m}</span>`)
    .replace(/FAIL:\s*\d+/g, m => `<span style="color:${/FAIL:\s*0/.test(m) ? '#34d399' : '#ef4444'};font-weight:700">${m}</span>`);

const page = `<!doctype html><meta charset="utf-8">
<style>
    body { margin:0; padding:24px; background:#0b1220; color:#e8eefb;
           font-family: 'Cascadia Code', 'JetBrains Mono', Consolas, monospace;
           font-size: 14px; line-height: 1.45; white-space: pre; }
</style>
<body>${html}</body>`;

(async () => {
    const browser = await puppeteer.launch({ executablePath: findChrome(), headless: 'new' });
    try {
        const p = await browser.newPage();
        await p.setViewport({ width: 900, height: 760, deviceScaleFactor: 2 });
        await p.setContent(page, { waitUntil: 'load' });
        await p.screenshot({ path: path.join(__dirname, 'screenshots', 'test-output.png'), fullPage: true });
        console.log('✓ test-output.png');
    } finally {
        await browser.close();
    }
})().catch(e => { console.error(e); process.exit(1); });
