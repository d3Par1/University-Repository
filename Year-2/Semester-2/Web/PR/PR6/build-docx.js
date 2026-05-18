// Конвертер ZVIT_PR6.md → zvit-temp.html → .docx (через Word COM).
// Імітує форматування Звіт_ПР5.docx: 14pt Times New Roman, bold-inline-параграфи замість Word-headings,
// right-aligned блок Виконав/Перевірив, inline-styles на кожному <p> (Word ігнорує CSS-класи при HTML→DOCX).

const fs = require('fs');
const path = require('path');

const PROJECT = __dirname;
const MD_PATH = path.join(PROJECT, 'ZVIT_PR6.md');
const HTML_PATH = path.join(PROJECT, 'zvit-temp.html');
const SCREENSHOTS = path.join(PROJECT, 'screenshots');

const imageByFigure = {
    1: 'register.png',
    2: 'login-error.png',
    3: 'dashboard-resident.png',
    4: 'dashboard-manager.png',
    5: 'test-output.png',
    6: 'dashboard-manager.png'   // повторний скрін для секції CSRF/rate-limit — показує аудит-журнал
};

const ST = {
    body:        'font-family: \'Times New Roman\', serif; font-size: 14pt; text-align: justify; margin: 4pt 0;',
    titleCenter: 'font-family: \'Times New Roman\', serif; font-size: 14pt; text-align: center; margin: 4pt 0;',
    rightAlign:  'font-family: \'Times New Roman\', serif; font-size: 14pt; text-align: right; margin: 4pt 0;',
    centerBold:  'font-family: \'Times New Roman\', serif; font-size: 14pt; text-align: center; font-weight: bold; margin: 8pt 0;',
    figureImg:   'text-align: center; margin: 12pt 0 4pt 0;',
    figureCap:   'font-family: \'Times New Roman\', serif; font-size: 14pt; text-align: center; font-style: italic; margin: 0 0 12pt 0;',
    code:        'font-family: Consolas, \'Courier New\', monospace; font-size: 11pt; background: #f7f7f7; padding: 6pt 8pt; border: 1px solid #d0d0d0; white-space: pre-wrap; margin: 6pt 0;'
};

const md = fs.readFileSync(MD_PATH, 'utf-8');
const lines = md.split(/\r?\n/);

let html = '';
let inCode = false, codeBuf = [];
let inTable = false, tableBuf = [];
let listStack = [];
let inTitleBlock = true;

const escapeHtml = s => s.replace(/[<>&]/g, c => ({ '<': '&lt;', '>': '&gt;', '&': '&amp;' }[c]));

function inline(s) {
    s = escapeHtml(s);
    s = s.replace(/\*\*([^*]+)\*\*/g, '<b>$1</b>');
    s = s.replace(/\*([^*]+)\*/g, '<i>$1</i>');
    s = s.replace(/`([^`]+)`/g, '<span style="font-family: Consolas, monospace; font-size: 12pt;">$1</span>');
    return s;
}

function closeLists() {
    while (listStack.length) html += `</${listStack.pop()}>\n`;
}

function flushTable() {
    if (!tableBuf.length) return;
    const rows = tableBuf.map(l => l.replace(/^\||\|$/g, '').split('|').map(c => c.trim()));
    const isSep = r => r.every(c => /^[-:\s]+$/.test(c));
    const data = rows.filter(r => !isSep(r));
    if (data.length) {
        const tdStyle = 'border: 1px solid #000; padding: 4pt 6pt; font-family: \'Times New Roman\', serif; font-size: 12pt; vertical-align: top;';
        const thStyle = tdStyle + ' background: #e8e8e8; font-weight: bold;';
        html += '<table style="border-collapse: collapse; margin: 6pt 0; width: 100%;">\n<tr>';
        html += data[0].map(c => `<td style="${thStyle}"><b>${inline(c)}</b></td>`).join('');
        html += '</tr>\n';
        for (let i = 1; i < data.length; i++) {
            html += '<tr>' + data[i].map(c => `<td style="${tdStyle}">${inline(c)}</td>`).join('') + '</tr>\n';
        }
        html += '</table>\n';
    }
    tableBuf = [];
    inTable = false;
}

for (let i = 0; i < lines.length; i++) {
    const line = lines[i];

    if (line.startsWith('```')) {
        if (inCode) {
            html += `<p style="${ST.code}">` + codeBuf.map(escapeHtml).join('<br/>') + '</p>\n';
            codeBuf = []; inCode = false;
        } else {
            closeLists(); flushTable();
            inCode = true;
        }
        continue;
    }
    if (inCode) { codeBuf.push(line); continue; }

    if (/^\|.*\|$/.test(line)) {
        closeLists();
        if (!inTable) inTable = true;
        tableBuf.push(line);
        continue;
    } else if (inTable) {
        flushTable();
    }

    if (/^---+$/.test(line)) {
        closeLists();
        html += '<br clear="all" style="page-break-before: always;"/>\n';
        inTitleBlock = false;
        continue;
    }

    let m;
    if (m = line.match(/^#\s+(.+)$/)) {
        closeLists();
        html += `<p style="${ST.centerBold}"><b>${inline(m[1])}</b></p>\n`;
        continue;
    }
    if (m = line.match(/^##\s+(.+)$/)) {
        closeLists();
        html += `<p style="${ST.centerBold}"><b>${inline(m[1])}</b></p>\n`;
        continue;
    }
    if (m = line.match(/^###?#?\s+(.+)$/)) {
        closeLists();
        html += `<p style="${ST.body}"><b>${inline(m[1])}</b></p>\n`;
        continue;
    }

    if (m = line.match(/^\*Рис\.\s+(\d+)\.\s*(.+)\*$/)) {
        closeLists();
        const fig = parseInt(m[1]);
        const cap = m[2];
        const imgFile = imageByFigure[fig];
        if (imgFile) {
            const imgBuf = fs.readFileSync(path.join(SCREENSHOTS, imgFile));
            const dataUri = `data:image/png;base64,${imgBuf.toString('base64')}`;
            html += `<p style="${ST.figureImg}"><img src="${dataUri}" alt="${escapeHtml(cap)}" style="max-width: 16cm; height: auto;"/></p>\n`;
        }
        html += `<p style="${ST.figureCap}"><i>Рис. ${fig}. ${escapeHtml(cap)}</i></p>\n`;
        continue;
    }

    if (m = line.match(/^>\s?(.*)$/)) {
        closeLists();
        const content = m[1];
        if (content.trim() === '') {
            html += `<p style="${ST.rightAlign}">&nbsp;</p>\n`;
        } else {
            html += `<p style="${ST.rightAlign}">${inline(content)}</p>\n`;
        }
        continue;
    }

    if (m = line.match(/^(\d+)\.\s+(.+)$/)) {
        if (listStack[listStack.length - 1] !== 'ol') {
            closeLists();
            html += '<ol style="font-family: \'Times New Roman\', serif; font-size: 14pt;">\n';
            listStack.push('ol');
        }
        html += `<li style="margin: 2pt 0;">${inline(m[2])}</li>\n`;
        continue;
    }

    if (m = line.match(/^[-*]\s+(.+)$/)) {
        if (listStack[listStack.length - 1] !== 'ul') {
            closeLists();
            html += '<ul style="font-family: \'Times New Roman\', serif; font-size: 14pt;">\n';
            listStack.push('ul');
        }
        html += `<li style="margin: 2pt 0;">${inline(m[1])}</li>\n`;
        continue;
    }

    if (line.trim() === '') {
        closeLists();
        if (inTitleBlock) {
            html += `<p style="${ST.titleCenter}">&nbsp;</p>\n`;
        }
        continue;
    }

    closeLists();
    if (inTitleBlock) {
        html += `<p style="${ST.titleCenter}">${inline(line)}</p>\n`;
    } else {
        html += `<p style="${ST.body}">${inline(line)}</p>\n`;
    }
}
closeLists();
flushTable();

const fullHtml = `<!DOCTYPE html>
<html><head>
<meta charset="utf-8">
<title>Звіт з практичної роботи №6</title>
</head>
<body style="font-family: 'Times New Roman', serif; font-size: 14pt;">
${html}
</body></html>`;

const buf = Buffer.concat([Buffer.from([0xEF, 0xBB, 0xBF]), Buffer.from(fullHtml, 'utf-8')]);
fs.writeFileSync(HTML_PATH, buf);
console.log('✓ HTML створено:', HTML_PATH);
