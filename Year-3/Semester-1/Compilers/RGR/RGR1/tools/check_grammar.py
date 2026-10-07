"""Self-check of the Krok grammar against checklist 4.1 of the assignment.

Usage:  python tools/check_grammar.py [--sets]
"""
import sys
from pathlib import Path

import ebnf

ROOT = Path(__file__).resolve().parent.parent
GRAMMAR = ROOT / "grammar" / "krok.ebnf"
TOKEN_CLASSES = {"Ident", "IntLit", "FloatLit", "StringLit"}
ROOTS = ["Program", "SourceText", "Alphabet"]
LEXICAL_SECTIONS = ("Лексеми", "Ідентифікатори та літерали", "Алфавіт")
# Groupings of tokens shared by both grammars (they name token classes, not characters).
SHARED = {"BoolLit", "RelOp", "AssignOp", "IncDecOp"}
ESCAPES = {r"\n": "\n", r"\t": "\t", r"\r": "\r"}


def alphabet(g):
    chars = set()
    for name in ("LowerLetter", "UpperLetter", "Letter", "Digit", "NonZeroDigit",
                 "SpecialChar", "WhiteSpace", "EndOfLine"):
        for t in ebnf.terminals(g.rules[name].body):
            if len(t) == 1 or t in ESCAPES:
                chars.add(ESCAPES.get(t, t))
    return chars


def run():
    """Return (grammar, LL1 analysis, [(title, passed, details)])."""
    g = ebnf.load(GRAMMAR)
    report = []

    defined = set(g.rules)
    used = set().union(*(ebnf.referenced(r.body) for r in g.rules.values()))
    undefined = sorted(used - defined)
    report.append(("Усі нетермінали, що використовуються, визначені", not undefined, ", ".join(undefined)))
    report.append(("Граматика контекстно-вільна: у лівій частині кожного правила один нетермінал",
                   True, f"{len(g.rules)} правил"))

    unreachable = sorted(defined - ebnf.reachable(g, ROOTS))
    report.append(("Недосяжні символи відсутні", not unreachable, ", ".join(unreachable)))

    unproductive = sorted(defined - ebnf.productive(g))
    report.append(("Непродуктивні символи відсутні", not unproductive, ", ".join(unproductive)))

    ll = ebnf.LL1(g, "Program", TOKEN_CLASSES)
    cycles = ll.left_recursion()
    report.append(("Ліва рекурсія (безпосередня та опосередкована) відсутня", not cycles,
                   "; ".join(" → ".join(c) for c in cycles)))

    cycles = ebnf.chain_cycles(g)
    report.append(("Цикли A ⇒⁺ A (ланцюгові) відсутні", not cycles,
                   "; ".join(" → ".join(c) for c in cycles)))

    conflicts = ll.conflicts()
    report.append(("Синтаксична граматика належить до класу LL(1), отже однозначна", not conflicts,
                   "\n".join(f"{r}: {m}" for r, m in conflicts)))

    abc = alphabet(g)
    bad = sorted({t for r in g.rules.values() for t in ebnf.terminals(r.body)
                  if not set(ESCAPES.get(t, t)) <= abc})
    report.append(("Усі термінали складені із символів алфавіту", not bad, ", ".join(bad)))

    lexical = {n for s, names in g.sections if s in LEXICAL_SECTIONS for n in names}
    leaked = sorted((ebnf.reachable(g, ["Program"], stop=TOKEN_CLASSES) & lexical) - TOKEN_CLASSES - SHARED)
    report.append(("Синтаксична граматика оперує токенами, а не символами", not leaked, ", ".join(leaked)))
    return g, ll, report


def main():
    g, ll, report = run()
    for title, passed, details in report:
        print(f"{'OK  ' if passed else 'FAIL'} {title}")
        if details:
            print("     " + details.replace("\n", "\n     "))
    if "--transform" in sys.argv:
        nullable = sorted(n for n in ll.names if ll.nullable[n])
        chains = ebnf.chain_productions(g)
        print(f"\nnullable nonterminals ({len(nullable)}): {', '.join(nullable)}")
        print(f"chain productions ({len(chains)}): " + ", ".join(f"{a}->{b}" for a, b in chains))
    if "--sets" in sys.argv:
        for n in ll.names:
            print(f"{n:15} nullable={ll.nullable[n]!s:5} FIRST={sorted(ll.first[n])} FOLLOW={sorted(ll.follow[n])}")
    sys.exit(0 if all(p for _, p, _ in report) else 1)


if __name__ == "__main__":
    main()
