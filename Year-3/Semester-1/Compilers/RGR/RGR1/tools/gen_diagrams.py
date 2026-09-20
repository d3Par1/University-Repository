"""Generate Wirth syntax diagrams (SVG + PNG) for every rule of grammar/krok.ebnf.

The diagrams are produced from the grammar file only, so the EBNF text and the
diagrams cannot diverge (checklist item 4.2.6).

Usage:  python tools/gen_diagrams.py [--svg-only]
PNG rendering uses headless Microsoft Edge (or Chrome via $BROWSER).
"""
import os
import re
import shutil
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

import railroad as rr

import ebnf
from check_grammar import GRAMMAR

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / "diagrams"
BROWSER_CANDIDATES = [
    os.environ.get("BROWSER", ""),
    r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe",
    r"C:\Program Files\Google\Chrome\Application\chrome.exe",
    "msedge", "google-chrome", "chromium",
]
SHOW_QUOTED = {" ", r"\t", r"\n", r"\r"}

CSS = """
svg.railroad-diagram { background-color: #ffffff; }
svg.railroad-diagram path { stroke-width: 1.4; stroke: #000; fill: none; }
svg.railroad-diagram text { font: 14px Consolas, 'DejaVu Sans Mono', monospace; text-anchor: middle; white-space: pre; fill: #000; }
svg.railroad-diagram text.comment { font: italic 12px Consolas, monospace; }
svg.railroad-diagram rect { stroke-width: 1.4; stroke: #000; fill: #fff; }
svg.railroad-diagram g.non-terminal text { font-style: italic; }
"""


def build(node):
    if isinstance(node, ebnf.Term):
        return rr.Terminal(f"'{node.text}'" if node.text in SHOW_QUOTED else node.text)
    if isinstance(node, ebnf.NonTerm):
        return rr.NonTerminal(node.name)
    if isinstance(node, ebnf.Special):
        return rr.Comment(f"? {node.text} ?")
    if isinstance(node, ebnf.Seq):
        items = list(node.items)
        out = []
        i = 0
        while i < len(items):
            cur = items[i]
            nxt = items[i + 1] if i + 1 < len(items) else None
            # A { A }  ->  loop;   A { sep A }  ->  loop with separator
            if isinstance(nxt, ebnf.Rep):
                body = nxt.item
                if body == cur:
                    out.append(rr.OneOrMore(build(cur)))
                    i += 2
                    continue
                if isinstance(body, ebnf.Seq) and len(body.items) == 2 and body.items[1] == cur:
                    out.append(rr.OneOrMore(build(cur), build(body.items[0])))
                    i += 2
                    continue
            out.append(build(cur))
            i += 1
        return out[0] if len(out) == 1 else rr.Sequence(*out)
    if isinstance(node, ebnf.Alt):
        return rr.Choice(0, *(build(i) for i in node.items))
    if isinstance(node, ebnf.Opt):
        return rr.Optional(build(node.item))
    if isinstance(node, ebnf.Rep):
        return rr.ZeroOrMore(build(node.item))
    raise TypeError(node)


def svg_for(rule: ebnf.Rule) -> tuple[str, float, float]:
    d = rr.Diagram(build(rule.body), type="simple")
    parts: list[str] = []
    d.writeStandalone(parts.append, css=CSS)
    svg = "".join(parts)
    w = float(re.search(r'width="([\d.]+)"', svg).group(1))
    h = float(re.search(r'height="([\d.]+)"', svg).group(1))
    return svg, w, h


def browser():
    for c in BROWSER_CANDIDATES:
        if c and (Path(c).exists() or shutil.which(c)):
            return c
    return None


def render_png(exe, name, svg, w, h):
    title_h = 26
    width, height = int(w) + 16, int(h) + title_h + 4
    html = (f"<!doctype html><meta charset='utf-8'><body style='margin:0;background:#fff'>"
            f"<div style=\"font:bold 15px Consolas,monospace;padding:6px 8px 0;height:{title_h - 6}px\">{name}</div>"
            f"<div style='padding-left:8px'>{svg}</div></body>")
    with tempfile.TemporaryDirectory() as tmp:
        page = Path(tmp) / f"{name}.html"
        page.write_text(html, encoding="utf-8")
        subprocess.run([exe, "--headless=new", "--disable-gpu", "--hide-scrollbars",
                        "--force-device-scale-factor=2", f"--user-data-dir={tmp}\\profile",
                        f"--window-size={width},{height}", f"--screenshot={OUT / (name + '.png')}",
                        page.as_uri()], check=True, capture_output=True, timeout=120)


def main():
    g = ebnf.load(GRAMMAR)
    OUT.mkdir(exist_ok=True)
    jobs = []
    for rule in g.rules.values():
        svg, w, h = svg_for(rule)
        (OUT / f"{rule.name}.svg").write_text(svg, encoding="utf-8")
        jobs.append((rule.name, svg, w, h))
    print(f"{len(jobs)} SVG diagrams written to {OUT}")
    if "--svg-only" in sys.argv:
        return
    exe = browser()
    if not exe:
        sys.exit("no Chromium-based browser found for PNG rendering (set $BROWSER)")
    with ThreadPoolExecutor(max_workers=6) as pool:
        list(pool.map(lambda j: render_png(exe, *j), jobs))
    print(f"{len(jobs)} PNG diagrams rendered with {Path(exe).name}")


if __name__ == "__main__":
    main()
