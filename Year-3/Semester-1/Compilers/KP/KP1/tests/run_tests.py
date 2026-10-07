"""Тестування лексичного аналізатора krok_lex.py.

1. Диференційний тест: потік лексем має збігатися з еталонним лексером специфікації
   (RGR/RGR1/tools/krok_lexer.py) на всіх прикладах програм.
2. Окремі лексичні ситуації (правило найдовшого збігу, форми літералів, коментарі).
3. Помилки: кожна має привести у відповідний стан множини Ferror.
"""
import contextlib, glob, io, os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', 'RGR', 'RGR1', 'tools'))
import krok_lex
from krok_lexer import tokenize


def run(text):
    """Повертає (успіх, [лексеми], код помилки або None)."""
    buf, code = io.StringIO(), None
    with contextlib.redirect_stdout(buf):
        ok = krok_lex.analyze(text)[1]
    out = buf.getvalue()
    if not ok:
        code = int(out.strip().splitlines()[-1].split()[-1])
    return ok, [r[1] for r in krok_lex.tableOfSymb.values()], code


failed = 0

def check(name, cond, info=''):
    global failed
    failed += not cond
    print('{0:4} {1} {2}'.format('ok' if cond else 'FAIL', name, '' if cond else info))


for path in sorted(glob.glob(os.path.join(HERE, '*.krok'))):
    text = open(path, encoding='utf-8').read()
    ok, lexemes, _ = run(text)
    expected = [t.lexeme for t in tokenize(text)[0]]
    check('еталон: ' + os.path.basename(path), ok and lexemes == expected,
          '{0} лексем проти {1}'.format(len(lexemes), len(expected)))

cases = [
    ('найдовший збіг a---b', 'a---b', ['a', '--', '-', 'b', ';']),
    ('степінь і унарний мінус', 'x = 2**-3', ['x', '=', '2', '**', '-', '3', ';']),
    ('форми дійсних літералів', 'a = 3.14 + 2. + .5 + 1e3 + 6.02E+23 + 007.5',
     ['a', '=', '3.14', '+', '2.', '+', '.5', '+', '1e3', '+', '6.02E+23', '+', '007.5', ';']),
    ('крапка як роздільник', 'p.x = 1', ['p', '.', 'x', '=', '1', ';']),
    ('коментар і ; перед ним', 'x := 10 // коментар\ny := 2', ['x', ':=', '10', ';', 'y', ':=', '2', ';']),
    ('// у рядку не коментар', 's := "// ні"', ['s', ':=', '"// ні"', ';']),
    ('escape-послідовності', r's := "a\tb\n\"\\"', ['s', ':=', r'"a\tb\n\"\\"', ';']),
    ('без ; після {', 'if x > 0 {\n}', ['if', 'x', '>', '0', '{', '}', ';']),
    ('усі двосимвольні', 'a == b != c <= d >= e && f || g',
     ['a', '==', 'b', '!=', 'c', '<=', 'd', '>=', 'e', '&&', 'f', '||', 'g', ';']),
    ('CRLF', 'x := 1\r\ny := 2\r\n', ['x', ':=', '1', ';', 'y', ':=', '2', ';']),
]
for name, text, expected in cases:
    ok, lexemes, _ = run(text)
    check(name, ok and lexemes == expected, str(lexemes))

errors = [
    ('недопустимий символ', 'x := 5 @ 3', 101), ('ведучий нуль', 'x := 007', 102),
    ('неправильна експонента', 'x := 1e+', 103), ('цифра перед літерою', 'x := 12abc', 104),
    ('незакритий рядок', 's := "abc', 105), ('невідомий escape', r's := "a\q"', 106),
    ('одиночна двокрапка', 'a : b', 107), ('одиночна вертикальна риска', 'a | b', 108),
]
for name, text, state in errors:
    ok, _, code = run(text)
    check('помилка {0}: {1}'.format(state, name), (not ok) and code == state, 'код {0}'.format(code))

print('-' * 40)
print('усі тести пройдено' if not failed else 'не пройдено: {0}'.format(failed))
sys.exit(1 if failed else 0)
