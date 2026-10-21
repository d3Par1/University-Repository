"""Тестування синтаксичного аналізатора krok_parse.py.

Кожен випадок має очікуваний результат (прийняти / відхилити) і додатково звіряється
з незалежним розпізнавачем, що працює безпосередньо за граматикою специфікації
(RGR/RGR1/tools/krok_parse.py): обидва мають ухвалити однакове рішення.
"""
import contextlib, glob, importlib.util, io, os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
KP = os.path.dirname(os.path.dirname(HERE))
RGR_TOOLS = os.path.join(KP, '..', 'RGR', 'RGR1', 'tools')
sys.path.insert(0, os.path.dirname(HERE))
import krok_parse

sys.path.insert(0, RGR_TOOLS)
spec = importlib.util.spec_from_file_location('spec_parser', os.path.join(RGR_TOOLS, 'krok_parse.py'))
spec_parser = importlib.util.module_from_spec(spec); spec.loader.exec_module(spec_parser)
reference = spec_parser.Parser()


def ours(text):
    with contextlib.redirect_stdout(io.StringIO()):
        return krok_parse.parse(text)[1]


def theirs(text):
    try:
        reference.parse(text)
        return True
    except Exception:
        return False


def body(lines):
    return 'package main\n\nimport "fmt"\n\nfunc main() {\n' + lines + '\n}\n'


CASES = [   # (група плану тестування, опис, текст програми, очікується успіх)
    ('1 пропуск термінала', 'немає {', body('if x > 0\n\ty = 1\n}'), False),
    ('1 пропуск термінала', 'немає )', body('fmt.Println(x'), False),
    ('1 пропуск термінала', 'немає ; у заголовку for', body('for i := 0; i < 3 i++ {\n}'), False),
    ('2 зайвий термінал', 'зайва )', body('x = (1 + 2))'), False),
    ('2 зайвий термінал', 'зайва кома', body('fmt.Println(x,)'), False),
    ('2 зайвий термінал', 'зайве int', 'package main\nvar x int int\n', False),
    ('3 помилка у наборі ключового слова', 'fucn', 'package main\nfucn main() {\n}\n', False),
    ('3 помилка у наборі ключового слова', 'retrun', body('retrun 1'), False),
    ('4 помилковий оператор у виразах', '+ *', body('x = 3 + * 4'), False),
    ('4 помилковий оператор у виразах', '<>', body('a = b <> c'), False),
    ('4 помилковий оператор у виразах', 'a < b < c', body('x = 1 < 2 < 3'), False),
    ('5 перевірка пріоритетності операторів', 'a + b * c', body('x = a + b * c'), True),
    ('5 перевірка пріоритетності операторів', '-2 ** 2 && !f || g', body('ok = -2 ** 2 > 0 && !f || g'), True),
    ('6 перевірка асоціативності операторів', 'a - b - c', body('x = a - b - c'), True),
    ('6 перевірка асоціативності операторів', '2 ** 3 ** 2', body('y = 2 ** 3 ** 2'), True),
]
programs = sorted(glob.glob(os.path.join(KP, 'KP1', 'tests', '*.krok')))
invalid = sorted(glob.glob(os.path.join(KP, '..', 'RGR', 'RGR1', 'examples', 'invalid', '*.krok')))
CASES += [('7 вкладені конструкції' if p.endswith('nested.krok') else '8 коректні програми', os.path.basename(p), open(p, encoding='utf-8').read(), True) for p in programs]
CASES += [('9 некоректні програми', os.path.basename(p), open(p, encoding='utf-8').read(), False) for p in invalid]

if __name__ == '__main__':
    failed = 0
    for group, name, text, expected in CASES:
        got, ref = ours(text), theirs(text)
        ok = got == expected and ref == expected
        failed += not ok
        print('{0:4} {1:40} {2}{3}'.format('ok' if ok else 'FAIL', group, name,
                                           '' if ok else '  (парсер {0}, еталон {1})'.format(got, ref)))
    print('-' * 40)
    print('усі {0} тестів пройдено'.format(len(CASES)) if not failed else 'не пройдено: {0}'.format(failed))
    sys.exit(1 if failed else 0)
