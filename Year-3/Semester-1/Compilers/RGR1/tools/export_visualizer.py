"""Export grammar/krok.ebnf into the dialects of the three diagram generators listed in [1] 2.2.4.

  krok-visualizer.ebnf  EBNF Visualizer (JKU Linz)      rule ends with '.', quoted terminals
  krok-iso14977.ebnf    EBNF 2 RailRoad                 ISO/IEC 14977: ',' between items, ';' at end
  krok-w3c.ebnf         Railroad Diagram Generator      W3C notation: '::=', '?', '*'

All three are generated from the same grammar file, so the diagrams they draw are the
diagrams in the specification.

Usage:  python tools/export_visualizer.py
"""
from pathlib import Path

import ebnf
from check_grammar import GRAMMAR

OUT = GRAMMAR.parent / "krok-visualizer.ebnf"
OUT_ISO = GRAMMAR.parent / "krok-iso14977.ebnf"
OUT_W3C = GRAMMAR.parent / "krok-w3c.ebnf"
HEADER = """(* Граматика мови Krok для інструменту EBNF Visualizer (dotnet.jku.at/applications/visualizer)
   Згенеровано з grammar/krok.ebnf програмою tools/export_visualizer.py — не редагувати вручну.
   Load Grammar... -> обрати правило у меню Rule -> діаграма Вірта. *)

"""


def quote(text: str) -> str:
    """Terminal as the visualizer wants it: quoted, with the quote char that does not clash."""
    if '"' in text:
        return f"'{text}'"
    return f'"{text}"'


def render(node, top=False) -> str:
    if isinstance(node, ebnf.Term):
        return quote(node.text)
    if isinstance(node, ebnf.NonTerm):
        return node.name
    if isinstance(node, ebnf.Special):
        return quote(f"? {node.text} ?")
    if isinstance(node, ebnf.Seq):
        return " ".join(render(i) for i in node.items)
    if isinstance(node, ebnf.Alt):
        body = " | ".join(render(i) for i in node.items)
        return body if top else f"( {body} )"
    if isinstance(node, ebnf.Opt):
        return f"[ {render(node.item, top=True)} ]"
    if isinstance(node, ebnf.Rep):
        return "{ " + render(node.item, top=True) + " }"
    raise TypeError(node)


def render_iso(node, top=False) -> str:
    """ISO/IEC 14977: concatenation is written with commas."""
    if isinstance(node, ebnf.Term):
        return quote(node.text)
    if isinstance(node, ebnf.NonTerm):
        return node.name
    if isinstance(node, ebnf.Special):
        return f"? {node.text} ?"
    if isinstance(node, ebnf.Seq):
        return ", ".join(render_iso(i) for i in node.items)
    if isinstance(node, ebnf.Alt):
        body = " | ".join(render_iso(i) for i in node.items)
        return body if top else f"( {body} )"
    if isinstance(node, ebnf.Opt):
        return f"[ {render_iso(node.item, top=True)} ]"
    if isinstance(node, ebnf.Rep):
        return "{ " + render_iso(node.item, top=True) + " }"
    raise TypeError(node)


def render_w3c(node, top=False) -> str:
    """W3C notation: '?' for option, '*' for repetition."""
    if isinstance(node, ebnf.Term):
        return quote(node.text)
    if isinstance(node, ebnf.NonTerm):
        return node.name
    if isinstance(node, ebnf.Special):
        return f"/* {node.text} */"
    if isinstance(node, ebnf.Seq):
        return " ".join(render_w3c(i) for i in node.items)
    if isinstance(node, ebnf.Alt):
        body = " | ".join(render_w3c(i) for i in node.items)
        return body if top else f"( {body} )"
    if isinstance(node, ebnf.Opt):
        return f"({render_w3c(node.item, top=True)})?"
    if isinstance(node, ebnf.Rep):
        return f"({render_w3c(node.item, top=True)})*"
    raise TypeError(node)


def main():
    g = ebnf.load(GRAMMAR)
    names = set(g.rules)
    lines = [HEADER]
    iso = ["(* Граматика мови Krok у нотації ISO/IEC 14977 для EBNF 2 RailRoad. *)\n"]
    w3c = ["/* Граматика мови Krok у нотації W3C для Railroad Diagram Generator. */\n"]
    for section, rules in g.sections:
        lines.append(f"(* {section} *)")
        iso.append(f"(* {section} *)")
        w3c.append(f"/* {section} */")
        for name in rules:
            rule = g.rules[name]
            lines.append(f"{name} = {render(rule.body, top=True)} .")
            iso.append(f"{name} = {render_iso(rule.body, top=True)} ;")
            w3c.append(f"{name} ::= {render_w3c(rule.body, top=True)}")
        for acc in (lines, iso, w3c):
            acc.append("")
    text = "\n".join(lines)
    OUT.write_text(text, encoding="utf-8", newline="\r\n")
    OUT_ISO.write_text("\n".join(iso), encoding="utf-8", newline="\r\n")
    OUT_W3C.write_text("\n".join(w3c), encoding="utf-8", newline="\r\n")
    print(f"written {OUT_ISO.name}, {OUT_W3C.name}")

    # sanity check: a bare word is a terminal only if no rule carries that name
    bare = {t for r in g.rules.values() for t in ebnf.terminals(r.body)}
    clashes = sorted(bare & names)
    print(f"written {OUT.name}: {len(g.rules)} rules")
    print("terminal/nonterminal name clashes:", clashes or "none")


if __name__ == "__main__":
    main()
