"""Parser and analyser for the Wirth-style EBNF used in the Krok specification.

Notation (see spec, section 1.5):
    Name = ...        rule; a new rule starts at a line beginning with `Name =`
    |  [ ]  { }  ( )  alternative, option, repetition, grouping
    Abc               nonterminal (starts with an upper-case letter)
    abc               keyword terminal (starts with a lower-case letter)
    'x'  "x"          terminal written literally
    ? text ?          special sequence (informal set of characters)
    (* ... *)         comment; `(* § Title *)` groups the rules below it
"""
from __future__ import annotations

import re
from dataclasses import dataclass, field
from pathlib import Path


# ----------------------------------------------------------------- AST nodes
@dataclass(frozen=True)
class Term:
    text: str
    quoted: bool


@dataclass(frozen=True)
class NonTerm:
    name: str


@dataclass(frozen=True)
class Special:
    text: str


@dataclass(frozen=True)
class Seq:
    items: tuple


@dataclass(frozen=True)
class Alt:
    items: tuple


@dataclass(frozen=True)
class Opt:
    item: object


@dataclass(frozen=True)
class Rep:
    item: object


@dataclass
class Rule:
    name: str
    body: object
    source: str          # rule text exactly as written in the grammar file
    section: str


@dataclass
class Grammar:
    rules: dict[str, Rule] = field(default_factory=dict)
    sections: list[tuple[str, list[str]]] = field(default_factory=list)


# ------------------------------------------------------------------- parsing
RULE_START = re.compile(r"^([A-Z][A-Za-z0-9]*)\s*=(.*)$")
TOKEN = re.compile(r"""\s*(?:('[^']*')|("[^"]*")|(\?[^?]*\?)|([A-Za-z_][A-Za-z0-9_]*)|([|\[\]{}()]))""")


def _tokenize(text: str):
    pos, out = 0, []
    text = text.rstrip()
    while pos < len(text):
        m = TOKEN.match(text, pos)
        if not m or m.end() == pos:
            raise SyntaxError(f"cannot tokenize EBNF near: {text[pos:pos + 30]!r}")
        pos = m.end()
        q1, q2, sp, word, sym = m.groups()
        if q1 or q2:
            out.append(("term", (q1 or q2)[1:-1]))
        elif sp:
            out.append(("special", sp[1:-1].strip()))
        elif word:
            out.append(("nt" if word[0].isupper() else "kw", word))
        elif sym:
            out.append(("sym", sym))
    return out


class _Parser:
    def __init__(self, toks):
        self.toks, self.i = toks, 0

    def peek(self):
        return self.toks[self.i] if self.i < len(self.toks) else (None, None)

    def expect(self, sym):
        kind, val = self.peek()
        if kind != "sym" or val != sym:
            raise SyntaxError(f"expected {sym!r}, got {val!r}")
        self.i += 1

    def expr(self):
        items = [self.seq()]
        while self.peek() == ("sym", "|"):
            self.i += 1
            items.append(self.seq())
        return items[0] if len(items) == 1 else Alt(tuple(items))

    def seq(self):
        items = []
        while True:
            kind, val = self.peek()
            if kind is None or (kind == "sym" and val in "|)]}"):
                break
            items.append(self.factor())
        if not items:
            raise SyntaxError("empty sequence")
        return items[0] if len(items) == 1 else Seq(tuple(items))

    def factor(self):
        kind, val = self.peek()
        self.i += 1
        if kind == "term":
            return Term(val, True)
        if kind == "kw":
            return Term(val, False)
        if kind == "nt":
            return NonTerm(val)
        if kind == "special":
            return Special(val)
        closers = {"[": "]", "{": "}", "(": ")"}
        if kind == "sym" and val in closers:
            inner = self.expr()
            self.expect(closers[val])
            return {"[": Opt, "{": Rep, "(": lambda x: x}[val](inner)
        raise SyntaxError(f"unexpected {val!r}")


def load(path: str | Path) -> Grammar:
    text = Path(path).read_text(encoding="utf-8")
    g = Grammar()
    section = ""
    current: list[str] | None = None
    name: str | None = None

    def flush():
        if name is None:
            return
        src = "\n".join(current).rstrip()
        p = _Parser(_tokenize(src.split("=", 1)[1]))
        body = p.expr()
        if p.i != len(p.toks):
            raise SyntaxError(f"{name}: trailing tokens")
        if name in g.rules:
            raise SyntaxError(f"rule {name} defined twice")
        g.rules[name] = Rule(name, body, src, section)
        g.sections[-1][1].append(name)

    # strip block comments but remember section markers
    for chunk in re.split(r"(\(\*.*?\*\))", text, flags=re.S):
        if chunk.startswith("(*"):
            m = re.match(r"\(\*\s*§\s*(.+?)\s*\*\)", chunk)
            if m:
                flush()
                name, current = None, None
                section = m.group(1)
                g.sections.append((section, []))
            continue
        for line in chunk.splitlines():
            m = RULE_START.match(line)
            if m:
                flush()
                name, current = m.group(1), [line.rstrip()]
            elif line.strip() and current is not None:
                current.append(line.rstrip())
    flush()
    return g


# ------------------------------------------------------------------ printing
def to_text(node) -> str:
    if isinstance(node, Term):
        return f"'{node.text}'" if node.quoted else node.text
    if isinstance(node, NonTerm):
        return node.name
    if isinstance(node, Special):
        return f"? {node.text} ?"
    if isinstance(node, Seq):
        return " ".join(f"( {to_text(i)} )" if isinstance(i, Alt) else to_text(i) for i in node.items)
    if isinstance(node, Alt):
        return " | ".join(to_text(i) for i in node.items)
    if isinstance(node, Opt):
        return f"[ {to_text(node.item)} ]"
    if isinstance(node, Rep):
        return f"{{ {to_text(node.item)} }}"
    raise TypeError(node)


# ------------------------------------------------------------------ analysis
EPS = "ε"
END = "⊣"


def referenced(node, acc=None):
    acc = set() if acc is None else acc
    if isinstance(node, NonTerm):
        acc.add(node.name)
    elif isinstance(node, (Seq, Alt)):
        for i in node.items:
            referenced(i, acc)
    elif isinstance(node, (Opt, Rep)):
        referenced(node.item, acc)
    return acc


def reachable(g: Grammar, roots, stop=frozenset()):
    seen, stack = set(), list(roots)
    while stack:
        n = stack.pop()
        if n in seen or n not in g.rules:
            continue
        seen.add(n)
        if n not in stop:
            stack.extend(referenced(g.rules[n].body))
    return seen


def productive(g: Grammar):
    prod: set[str] = set()

    def ok(node):
        if isinstance(node, (Term, Special)):
            return True
        if isinstance(node, NonTerm):
            return node.name in prod
        if isinstance(node, Seq):
            return all(ok(i) for i in node.items)
        if isinstance(node, Alt):
            return any(ok(i) for i in node.items)
        return True  # Opt / Rep derive ε

    changed = True
    while changed:
        changed = False
        for r in g.rules.values():
            if r.name not in prod and ok(r.body):
                prod.add(r.name)
                changed = True
    return prod


class LL1:
    """FIRST/FOLLOW computation and LL(1) conflict detection for a sub-grammar.

    `tokens` are nonterminals treated as atomic tokens (defined by the lexical grammar).
    """

    def __init__(self, g: Grammar, root: str, tokens: set[str]):
        self.g, self.root, self.tokens = g, root, tokens
        self.names = sorted(reachable(g, [root], stop=tokens) - tokens)
        self.first = {n: set() for n in self.names}
        self.nullable = {n: False for n in self.names}
        self.follow = {n: set() for n in self.names}
        self._fix_first()
        self._fix_follow()

    def f(self, node) -> set[str]:
        """FIRST of an arbitrary node; contains EPS if the node is nullable."""
        if isinstance(node, Term):
            return {node.text}
        if isinstance(node, Special):
            return {f"?{node.text}?"}
        if isinstance(node, NonTerm):
            if node.name in self.tokens:
                return {node.name}
            s = set(self.first[node.name])
            if self.nullable[node.name]:
                s.add(EPS)
            return s
        if isinstance(node, Seq):
            out = set()
            for item in node.items:
                fi = self.f(item)
                out |= fi - {EPS}
                if EPS not in fi:
                    return out
            return out | {EPS}
        if isinstance(node, Alt):
            return set().union(*(self.f(i) for i in node.items))
        if isinstance(node, (Opt, Rep)):
            return self.f(node.item) | {EPS}
        raise TypeError(node)

    def _fix_first(self):
        changed = True
        while changed:
            changed = False
            for n in self.names:
                fi = self.f(self.g.rules[n].body)
                new_first, new_null = fi - {EPS}, EPS in fi
                if new_first != self.first[n] or new_null != self.nullable[n]:
                    self.first[n], self.nullable[n] = new_first, new_null
                    changed = True

    def _contexts(self, node, after):
        """Yield (child, follow-set-of-child) pairs for a node in context `after`."""
        if isinstance(node, Seq):
            tail, pairs = set(after), []
            for item in reversed(node.items):
                pairs.append((item, tail))
                fi = self.f(item)
                tail = (fi - {EPS}) | (tail if EPS in fi else set())
            return list(reversed(pairs))
        if isinstance(node, Alt):
            return [(i, after) for i in node.items]
        if isinstance(node, Opt):
            return [(node.item, after)]
        if isinstance(node, Rep):
            return [(node.item, after | (self.f(node.item) - {EPS}))]
        return []

    def _walk_follow(self, node, after):
        changed = False
        if isinstance(node, NonTerm) and node.name in self.follow:
            if not after <= self.follow[node.name]:
                self.follow[node.name] |= after
                changed = True
        for child, ctx in self._contexts(node, after):
            changed |= self._walk_follow(child, ctx)
        return changed

    def _fix_follow(self):
        self.follow[self.root].add(END)
        changed = True
        while changed:
            changed = False
            for n in self.names:
                changed |= self._walk_follow(self.g.rules[n].body, set(self.follow[n]))

    def conflicts(self):
        problems = []

        def check(rule, node, after):
            if isinstance(node, Alt):
                firsts = [self.f(i) for i in node.items]
                for a in range(len(firsts)):
                    for b in range(a + 1, len(firsts)):
                        common = (firsts[a] & firsts[b]) - {EPS}
                        if common:
                            problems.append((rule, f"alternatives '{to_text(node.items[a])}' and "
                                                   f"'{to_text(node.items[b])}' share {sorted(common)}"))
                nullables = [i for i, fi in enumerate(firsts) if EPS in fi]
                if len(nullables) > 1:
                    problems.append((rule, f"several nullable alternatives in '{to_text(node)}'"))
                if nullables:
                    common = (set().union(*firsts) - {EPS}) & after
                    if common:
                        problems.append((rule, f"nullable alternative in '{to_text(node)}' "
                                               f"clashes with FOLLOW {sorted(common)}"))
            elif isinstance(node, (Opt, Rep)):
                fi = self.f(node.item)
                if EPS in fi:
                    problems.append((rule, f"body of '{to_text(node)}' derives ε"))
                common = (fi - {EPS}) & after
                if common:
                    problems.append((rule, f"'{to_text(node)}' FIRST clashes with FOLLOW {sorted(common)}"))
            for child, ctx in self._contexts(node, after):
                check(rule, child, ctx)

        for n in self.names:
            check(n, self.g.rules[n].body, self.follow[n])
        return problems

    def left_recursion(self):
        def leftmost(node):
            if isinstance(node, NonTerm):
                return {node.name} if node.name in self.first else set()
            if isinstance(node, Seq):
                out = set()
                for item in node.items:
                    out |= leftmost(item)
                    if EPS not in self.f(item):
                        break
                return out
            if isinstance(node, Alt):
                return set().union(*(leftmost(i) for i in node.items))
            if isinstance(node, (Opt, Rep)):
                return leftmost(node.item)
            return set()

        edges = {n: leftmost(self.g.rules[n].body) for n in self.names}
        cycles = []
        for start in self.names:
            stack, seen = [(start, [start])], set()
            while stack:
                cur, path = stack.pop()
                for nxt in edges[cur]:
                    if nxt == start:
                        cycles.append(path + [start])
                    elif nxt not in seen:
                        seen.add(nxt)
                        stack.append((nxt, path + [nxt]))
        return cycles


def alternatives(node):
    """Top-level alternatives of a rule body (a body without '|' is a single alternative)."""
    return list(node.items) if isinstance(node, Alt) else [node]


def chain_productions(g: Grammar, names=None):
    """Chain productions A -> B (a whole alternative that is just one nonterminal)."""
    names = set(g.rules) if names is None else set(names)
    out = []
    for name in sorted(names):
        for alt in alternatives(g.rules[name].body):
            if isinstance(alt, NonTerm) and alt.name in g.rules:
                out.append((name, alt.name))
    return out


def chain_cycles(g: Grammar, names=None):
    """Cycles A =>+ A built only from chain productions (lecture 3: 'видалення циклів')."""
    edges = {}
    for a, b in chain_productions(g, names):
        edges.setdefault(a, set()).add(b)
    cycles = []
    for start in edges:
        stack, seen = [(start, [start])], set()
        while stack:
            cur, path = stack.pop()
            for nxt in edges.get(cur, ()):
                if nxt == start:
                    cycles.append(path + [start])
                elif nxt not in seen:
                    seen.add(nxt)
                    stack.append((nxt, path + [nxt]))
    return cycles


def terminals(node, acc=None):
    acc = set() if acc is None else acc
    if isinstance(node, Term):
        acc.add(node.text)
    elif isinstance(node, (Seq, Alt)):
        for i in node.items:
            terminals(i, acc)
    elif isinstance(node, (Opt, Rep)):
        terminals(node.item, acc)
    return acc
