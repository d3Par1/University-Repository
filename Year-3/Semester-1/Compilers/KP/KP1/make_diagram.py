# Діаграма станів лексичного аналізатора мови Krok:
#   krok_lex.jff - файл для JFLAP (File -> Open), krok_lex.svg / .png - зображення.
# Усе будується зі словника stf модуля krok_lex, тому діаграма й код не розходяться.
import math, os, subprocess, sys, tempfile
from krok_lex import stf, initState, F, Fstar, Ferror

HERE = os.path.dirname(os.path.abspath(__file__))
R = 22
POS = {0: (80, 600), 1: (330, 90), 2: (580, 90), 3: (330, 200), 5: (580, 200), 102: (800, 130),
       4: (330, 320), 12: (580, 400), 6: (800, 270), 7: (1020, 270), 8: (1240, 160),
       9: (1440, 90), 10: (1440, 240), 103: (1240, 60), 13: (1440, 360), 104: (1020, 420),
       11: (330, 480), 14: (330, 580), 15: (580, 520), 16: (800, 600), 105: (580, 640),
       106: (800, 500), 17: (330, 700), 18: (580, 720), 19: (800, 720),
       30: (800, 900), 31: (1020, 760), 107: (800, 1185), 108: (800, 1120),
       32: (80, 250), 101: (80, 950)}
POS.update({20 + i: (330, 780 + 45 * i) for i in range(10)})
W, H = 1560, 1260


def edges():
    out = {}
    for (src, label), dst in stf.items():
        out.setdefault((src, dst), []).append(label)
    return out


def esc(s):
    return s.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')


def write_jff():
    lines = ['<?xml version="1.0" encoding="UTF-8" standalone="no"?><structure>', '\t<type>fa</type>',
             '\t<automaton>']
    for s, (x, y) in sorted(POS.items()):
        lines.append('\t\t<state id="{0}" name="{0}"><x>{1}.0</x><y>{2}.0</y>{3}{4}</state>'.format(
            s, x, y, '<initial/>' if s == initState else '', '<final/>' if s in F else ''))
    for (src, label), dst in stf.items():
        lines.append('\t\t<transition><from>{0}</from><to>{1}</to><read>{2}</read></transition>'.format(
            src, dst, esc(label)))
    lines += ['\t</automaton>', '</structure>']
    open(os.path.join(HERE, 'krok_lex.jff'), 'w', encoding='utf-8').write('\n'.join(lines))


def write_svg():
    e = edges()
    p = ['<svg xmlns="http://www.w3.org/2000/svg" width="{0}" height="{1}" viewBox="0 0 {0} {1}" '
         'font-family="Arial" font-size="13">'.format(W, H),
         '<defs><marker id="a" markerWidth="10" markerHeight="8" refX="9" refY="4" orient="auto">'
         '<path d="M0,0 L10,4 L0,8 z"/></marker></defs>', '<rect width="100%" height="100%" fill="#fff"/>']
    for (src, dst), labels in e.items():
        text = esc(' '.join(labels) if all(len(l) == 1 for l in labels) else ', '.join(labels))
        x1, y1 = POS[src]
        if src == dst:                                   # петля над станом
            p.append('<path d="M{0},{1} C{2},{3} {4},{3} {5},{1}" fill="none" stroke="#000" '
                     'marker-end="url(#a)"/>'.format(x1 - 10, y1 - R + 2, x1 - 26, y1 - 62, x1 + 26, x1 + 10))
            p.append('<text x="{0}" y="{1}" text-anchor="middle">{2}</text>'.format(x1, y1 - 56, text))
            continue
        x2, y2 = POS[dst]
        d = math.hypot(x2 - x1, y2 - y1)
        ux, uy = (x2 - x1) / d, (y2 - y1) / d
        off = 6 if (dst, src) in e else 0                # зустрічні дуги розводимо
        ox, oy = -uy * off, ux * off
        sx, sy, ex, ey = x1 + ux * R + ox, y1 + uy * R + oy, x2 - ux * R + ox, y2 - uy * R + oy
        p.append('<line x1="{0:.0f}" y1="{1:.0f}" x2="{2:.0f}" y2="{3:.0f}" stroke="#000" '
                 'marker-end="url(#a)"/>'.format(sx, sy, ex, ey))
        t = 0.36 if src != initState else 0.72           # з стану 0 виходить віяло - підпис ближче до цілі
        lx, ly = sx + (ex - sx) * t - uy * 10, sy + (ey - sy) * t + ux * 10 + 4
        p.append('<text x="{0:.0f}" y="{1:.0f}" text-anchor="middle" stroke="#fff" stroke-width="4" '
                 'paint-order="stroke">{2}</text>'.format(lx, ly, text))
    for s, (x, y) in POS.items():
        p.append('<circle cx="{0}" cy="{1}" r="{2}" fill="#ffff99" stroke="#000"/>'.format(x, y, R))
        if s in F:
            p.append('<circle cx="{0}" cy="{1}" r="{2}" fill="none" stroke="#000"/>'.format(x, y, R - 4))
        p.append('<text x="{0}" y="{1}" text-anchor="middle">{2}</text>'.format(x, y + 5, s))
        mark = '*' if s in Fstar else 'ERROR' if s in Ferror else ''
        if mark:
            w = 12 if mark == '*' else 50
            p.append('<rect x="{0}" y="{1}" width="{2}" height="16" fill="#ffff99" stroke="#000"/>'
                     '<text x="{3}" y="{4}" text-anchor="middle">{5}</text>'.format(
                         x - w / 2, y + R, w, x, y + R + 13, mark))
    x, y = POS[initState]
    p.append('<path d="M{0},{1} L{2},{3} L{2},{4} z" fill="none" stroke="#000"/>'.format(x - R, y, x - R - 22, y - 16, y + 16))
    p.append('</svg>')
    svg = '\n'.join(p)
    open(os.path.join(HERE, 'krok_lex.svg'), 'w', encoding='utf-8').write(svg)
    return svg


def render_png(svg):
    exe = r'C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe'
    if not os.path.exists(exe):
        return print('Edge не знайдено - PNG не створено')
    with tempfile.TemporaryDirectory() as tmp:
        page = os.path.join(tmp, 'd.html')
        open(page, 'w', encoding='utf-8').write('<body style="margin:0">' + svg + '</body>')
        subprocess.run([exe, '--headless=new', '--disable-gpu', '--hide-scrollbars',
                        '--force-device-scale-factor=2', '--user-data-dir=' + os.path.join(tmp, 'p'),
                        '--window-size={0},{1}'.format(W, H),
                        '--screenshot=' + os.path.join(HERE, 'krok_lex.png'),
                        'file:///' + page.replace('\\', '/')], check=True, capture_output=True, timeout=120)


if __name__ == '__main__':
    missing = sorted({s for k in stf for s in (k[0], stf[k])} - set(POS))
    if missing:
        sys.exit('немає координат для станів: {0}'.format(missing))
    write_jff()
    render_png(write_svg())
    print('станів: {0}, переходів: {1}'.format(len(POS), len(stf)))
