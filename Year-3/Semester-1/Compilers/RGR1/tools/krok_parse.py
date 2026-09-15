"""Grammar-driven LL(1) recogniser: parses Krok programs directly with grammar/krok.ebnf.

Usage:
    python tools/krok_parse.py examples/*.krok            # must parse
    python tools/krok_parse.py --invalid examples/invalid/* # must be rejected
    python tools/krok_parse.py --coverage examples/base_example.krok
"""
import sys
from pathlib import Path

import ebnf
from check_grammar import GRAMMAR, TOKEN_CLASSES
from krok_lexer import LexError, tokenize
from tokens import token_table


class ParseError(Exception):
    pass


class Parser:
    def __init__(self):
        self.g = ebnf.load(GRAMMAR)
        self.ll = ebnf.LL1(self.g, "Program", TOKEN_CLASSES)

    def parse(self, text: str):
        self.toks, _ = tokenize(text)
        self.pos = 0
        self.node(ebnf.NonTerm("Program"))
        if self.pos != len(self.toks):
            self.fail("end of file")

    def la(self):
        return self.toks[self.pos].kind if self.pos < len(self.toks) else ebnf.END

    def fail(self, expected):
        t = self.toks[self.pos] if self.pos < len(self.toks) else None
        where = f"{t.line}:{t.col}" if t else "EOF"
        got = repr(t.lexeme) if t else "end of file"
        raise ParseError(f"{where}: expected {expected}, got {got}")

    def starts(self, node):
        return self.la() in self.ll.f(node)

    def node(self, node):
        if isinstance(node, ebnf.Term) or (isinstance(node, ebnf.NonTerm) and node.name in TOKEN_CLASSES):
            want = node.text if isinstance(node, ebnf.Term) else node.name
            if self.la() != want:
                self.fail(repr(want))
            self.pos += 1
        elif isinstance(node, ebnf.NonTerm):
            self.node(self.g.rules[node.name].body)
        elif isinstance(node, ebnf.Seq):
            for item in node.items:
                self.node(item)
        elif isinstance(node, ebnf.Alt):
            for item in node.items:
                if self.starts(item):
                    return self.node(item)
            for item in node.items:
                if ebnf.EPS in self.ll.f(item):
                    return self.node(item)
            self.fail("one of " + ", ".join(sorted(self.ll.f(node) - {ebnf.EPS})))
        elif isinstance(node, ebnf.Opt):
            if self.starts(node.item):
                self.node(node.item)
        elif isinstance(node, ebnf.Rep):
            while self.starts(node.item):
                self.node(node.item)


def coverage(path):
    text = Path(path).read_text(encoding="utf-8")
    toks, ignorable = tokenize(text)
    seen = {t.row for t in toks if not t.implicit} | ignorable
    explicit_semicolon = any(t.kind == ";" and not t.implicit for t in toks)
    missing = []
    for code, example, token, _ in token_table():
        key = {"id": "id", "intnum": "intnum", "floatnum": "floatnum", "strlit": "strlit"}.get(token, example)
        if token == ";" or example == ";":
            if not explicit_semicolon:
                missing.append(example)
        elif key not in seen:
            missing.append(example)
    return missing


def main():
    args = sys.argv[1:]
    invalid = "--invalid" in args
    cover = "--coverage" in args
    files = [a for a in args if not a.startswith("--")]
    parser, failed = Parser(), False
    for f in files:
        try:
            parser.parse(Path(f).read_text(encoding="utf-8"))
            result = None
        except (LexError, ParseError) as e:
            result = e
        if invalid:
            status = "rejected" if result else "ACCEPTED (expected rejection)"
            failed |= result is None
            print(f"{status:30} {Path(f).name}: {result or ''}")
        else:
            failed |= result is not None
            print(f"{'ok' if result is None else 'ERROR':6} {Path(f).name} {result or ''}")
        if cover and result is None:
            missing = coverage(f)
            failed |= bool(missing)
            print("       token coverage:", "complete" if not missing else f"missing {missing}")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
