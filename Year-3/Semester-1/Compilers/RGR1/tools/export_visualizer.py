"""Export grammar/krok.ebnf into the dialect of EBNF Visualizer (JKU Linz, dotnet.jku.at).

That tool is the one recommended by the assignment; it is GUI-only, so the build cannot call
it, but the exported file can be opened with "Load Grammar..." to draw any rule on demand.

Its dialect differs from the notation used in the specification:
  * every rule ends with '.'
  * terminals are bare words or strings in " " / ' '
  * a word that matches a rule name is a nonterminal, everything else is a terminal
  * there is no notation for special sequences, so those become quoted strings

Usage:  python tools/export_visualizer.py  ->  grammar/krok-visualizer.ebnf
"""
from pathlib import Path

import ebnf
from check_grammar import GRAMMAR

OUT = GRAMMAR.parent / "krok-visualizer.ebnf"
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


def main():
    g = ebnf.load(GRAMMAR)
    names = set(g.rules)
    lines = [HEADER]
    for section, rules in g.sections:
        lines.append(f"(* {section} *)")
        for name in rules:
            rule = g.rules[name]
            lines.append(f"{name} = {render(rule.body, top=True)} .")
        lines.append("")
    text = "\n".join(lines)
    OUT.write_text(text, encoding="utf-8", newline="\r\n")

    # sanity check: a bare word is a terminal only if no rule carries that name
    bare = {t for r in g.rules.values() for t in ebnf.terminals(r.body)}
    clashes = sorted(bare & names)
    print(f"written {OUT.name}: {len(g.rules)} rules")
    print("terminal/nonterminal name clashes:", clashes or "none")


if __name__ == "__main__":
    main()
