"""Reference lexer for Krok (section 2 of the specification)."""
from __future__ import annotations

from dataclasses import dataclass

from tokens import BOOL_LITERALS, DELIMITERS, KEYWORDS, OPERATORS, SEMICOLON_TRIGGERS

LETTERS = set("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_")
DIGITS = set("0123456789")
ESCAPES = {"n": "\n", "t": "\t", "r": "\r", '"': '"', "\\": "\\"}
SYMBOLS = sorted({**OPERATORS, **DELIMITERS}, key=len, reverse=True)


class LexError(Exception):
    pass


@dataclass
class Token:
    kind: str        # grammar symbol: 'Ident', 'IntLit', ..., or the lexeme of keywords/operators
    lexeme: str
    line: int
    col: int
    implicit: bool = False   # ';' inserted by the semicolon rule
    row: str = ""            # key of the token-table row this lexeme belongs to

    def __repr__(self):
        return f"{self.kind}({self.lexeme!r})@{self.line}:{self.col}"


def tokenize(text: str) -> tuple[list[Token], set[str]]:
    """Return tokens (with implicit semicolons) and the set of ignorable lexeme kinds seen."""
    toks: list[Token] = []
    seen_ignorable: set[str] = set()
    i, line, col = 0, 1, 1
    n = len(text)

    def insert_semicolon():
        if toks and toks[-1].kind in SEMICOLON_TRIGGERS:
            toks.append(Token(";", ";", line, col, implicit=True, row=";"))

    def err(msg):
        raise LexError(f"{line}:{col}: {msg}")

    while i < n:
        c = text[i]
        if c in " \t":
            seen_ignorable.add("\\32" if c == " " else "\\t")
            i += 1; col += 1
            continue
        if c == "\r" and text[i + 1:i + 2] == "\n" or c == "\n":
            seen_ignorable.add("\\n, \\r\\n")
            insert_semicolon()
            i += 2 if c == "\r" else 1
            line, col = line + 1, 1
            continue
        if text.startswith("//", i):
            seen_ignorable.add("// коментар")
            while i < n and text[i] not in "\r\n":
                i += 1
            continue
        start_col = col
        if c in LETTERS:
            j = i
            while j < n and (text[j] in LETTERS or text[j] in DIGITS):
                j += 1
            word = text[i:j]
            if word in KEYWORDS or word in BOOL_LITERALS:
                toks.append(Token(word, word, line, start_col, row=word))
            else:
                toks.append(Token("Ident", word, line, start_col, row="id"))
            col += j - i; i = j
            continue
        if c in DIGITS or (c == "." and text[i + 1:i + 2] in DIGITS and text[i + 1:i + 2] != ""):
            j = i
            while j < n and text[j] in DIGITS:
                j += 1
            is_float = False
            if j < n and text[j] == ".":
                is_float = True
                j += 1
                while j < n and text[j] in DIGITS:
                    j += 1
            if j < n and text[j] in "eE":
                k = j + 1
                if k < n and text[k] in "+-":
                    k += 1
                if k < n and text[k] in DIGITS:
                    is_float = True
                    j = k
                    while j < n and text[j] in DIGITS:
                        j += 1
                else:
                    err("malformed exponent")
            lexeme = text[i:j]
            if not is_float and len(lexeme) > 1 and lexeme[0] == "0":
                err(f"integer literal with leading zero: {lexeme}")
            if j < n and text[j] in LETTERS:
                err(f"identifier cannot start with a digit: {text[i:j + 1]}")
            kind = "FloatLit" if is_float else "IntLit"
            toks.append(Token(kind, lexeme, line, start_col, row="floatnum" if is_float else "intnum"))
            col += j - i; i = j
            continue
        if c == '"':
            j, value = i + 1, []
            while True:
                if j >= n or text[j] in "\r\n":
                    err("unterminated string literal")
                if text[j] == '"':
                    break
                if text[j] == "\\":
                    if text[j + 1:j + 2] not in ESCAPES:
                        err(f"unknown escape sequence \\{text[j + 1:j + 2]}")
                    j += 2
                else:
                    j += 1
            toks.append(Token("StringLit", text[i:j + 1], line, start_col, row="strlit"))
            col += j + 1 - i; i = j + 1
            continue
        for sym in SYMBOLS:
            if text.startswith(sym, i):
                toks.append(Token(sym, sym, line, start_col, row=sym))
                i += len(sym); col += len(sym)
                break
        else:
            err(f"character {c!r} is not allowed here")
    insert_semicolon()
    return toks, seen_ignorable
