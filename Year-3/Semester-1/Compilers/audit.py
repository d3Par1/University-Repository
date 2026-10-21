"""Повний аудит робіт з «Основ розробки трансляторів» проти методичних матеріалів.

Вимоги до звітів КП1 і КП2 витягуються безпосередньо з PDF методичок (нумеровані списки
«Структура змістовної частини звіту» та таблиці планів тестування), тому аудит перевіряє саме те,
що написано в джерелі. Результат: AUDIT.md поруч зі скриптом.
Запуск:  python audit.py
"""
import ast, glob, hashlib, os, re, subprocess, sys, zipfile
import pymupdf

HERE = os.path.dirname(os.path.abspath(__file__))
DL = r'C:\Users\Nazar\Downloads'
WORD = r'D:\OneDrive - kpi.ua\Word_KPI\Основи_розробки_трансляторів'
rows, fails = [], 0


def check(area, name, ok, info=''):
    global fails
    fails += not ok
    rows.append((area, name, 'так' if ok else 'НІ', info))


def pdf_text(path):
    t = '\n'.join(p.get_text() for p in pymupdf.open(path)).replace('i', 'і').replace('I', 'І')
    return re.sub(r'[ \t]*\n[ \t]*', ' ', re.sub(r'-\n', '', t))


def required_items(guide):
    """Нумерований перелік із розділу «Структура змістовної частини звіту» методички."""
    t = pdf_text(guide)
    part = t[t.rindex('Структура змістовної частини звіту'):]
    part = part[:part.find('Література')] if 'Література' in part else part
    out, pos = [], 0                       # пункти шукаємо послідовно: « 1. », « 2. », ...
    while True:
        a = part.find(' {0}. '.format(len(out) + 1), pos)
        if a < 0:
            return out
        b = part.find(' {0}. '.format(len(out) + 2), a + 3)
        out.append(part[a + 4:b if b > 0 else a + 200].strip()[:160])
        pos = a + 3


def report_audit(area, guide, report, title, stems, plan_types, work_no):
    items = required_items(guide)
    check(area, 'з методички витягнуто пунктів структури звіту', len(items) == len(stems), str(len(items)))
    doc = pymupdf.open(report)
    pages = [p.get_text() for p in doc]
    full = '\n'.join(pages)
    heads = re.findall(r'(?m)^(\d{1,2})\. (.+)$', full)
    heads = [(int(n), h) for n, h in heads if int(n) <= len(stems)]
    for i, (item, stem) in enumerate(zip(items, stems), 1):
        in_guide = stem.lower() in item.lower()
        in_report = any(n == i and stem.lower() in h.lower() for n, h in heads) or \
            (stem.lower() in full.lower() and any(n == i for n, _ in heads))
        check(area, 'п. {0} «{1}»'.format(i, item[:70]), in_guide and in_report,
              '' if in_guide else 'ключ «{0}» не з методички'.format(stem))
    name = os.path.basename(report)
    check(area, 'назва файлу НомерГрупи.КП_Номер.ПрізвищеІніціали.pdf',
          re.fullmatch(r'ТВ-43\.КП_{0}\.Степаненко[А-ЯІЇЄ]{{2}}\.pdf'.format(work_no), name) is not None, name)
    check(area, 'формат PDF', name.endswith('.pdf'))
    check(area, 'прізвище, ім\'я, група, номер роботи у верхньому колонтитулі кожної сторінки',
          all('Степаненко Назар' in p and 'ТВ-43' in p and 'КП № {0}'.format(work_no) in p for p in pages))
    numbered = sum(1 for p in doc if any(b[4].strip().isdigit() for b in p.get_text('blocks') if b[1] > p.rect.height * .9))
    check(area, 'нумерація сторінок у нижньому колонтитулі', numbered == len(doc), '{0} із {1}'.format(numbered, len(doc)))
    first = [l for l in pages[0].split('\n') if l.strip() and 'Степаненко' not in l and not l.strip().isdigit()]
    check(area, 'без титульного аркуша; назва роботи вгорі першого аркуша', first[0].strip() == title, first[0][:60])
    check(area, 'змістовна частина починається на першому аркуші', '1. ' in pages[0])
    gt = pdf_text(guide)
    for typ in plan_types:
        check(area, 'тип випробування з методички: «{0}»'.format(typ), typ in gt and typ in full.replace('\n', ' '))
    check(area, 'протокол: текстові копії термінала', full.count('>python') >= 5, str(full.count('>python')))
    check(area, 'немає службових залишків', not any(x in full for x in ('Оновіть поле', '[@', 'Error!', '@rules', '@code')))


def method_audit(area, sample_zip, member, ours, mapping):
    src = zipfile.ZipFile(sample_zip).read(member).decode('utf-8', 'replace')
    sample = {n.name for n in ast.walk(ast.parse(src)) if isinstance(n, ast.FunctionDef)}
    mine = open(ours, encoding='utf-8').read()
    names = {n.name for n in ast.walk(ast.parse(mine)) if isinstance(n, ast.FunctionDef)}
    for f in sorted(sample):
        target = mapping.get(f, f)
        check(area, 'функція зразка {0}() має відповідник'.format(f), target in names, target)
    for var in re.findall(r'^([A-Za-z_]\w*)\s*=', src, re.M):
        if var in ('f', 'sourceCode', 'FSucces', 'res', 'lenCode', 'char', 'state', 'lexeme'):
            continue
        check(area, 'змінна/таблиця зразка {0}'.format(var), re.search(r'^{0}\s*='.format(var), mine, re.M) is not None)


def run(cmd, cwd):
    r = subprocess.run([sys.executable] + cmd, cwd=cwd, capture_output=True, text=True,
                       encoding='utf-8', env={**os.environ, 'PYTHONIOENCODING': 'utf-8'})
    return r.returncode, r.stdout.strip().splitlines()[-1] if r.stdout.strip() else r.stderr[-200:]


def sha(path):
    return hashlib.sha1(open(path, 'rb').read()).hexdigest()


# ---------------------------------------------------------------- РГР
RGR = os.path.join(HERE, 'RGR', 'RGR1')
for cmd, name in ((['tools/check_grammar.py'], 'граматика: 9 формальних властивостей'),
                  (['tools/krok_parse.py', '--coverage', 'examples/base_example.krok'], 'базовий приклад покриває всі лексеми'),
                  (['tools/verify_doc.py'], 'оформлення документів специфікації')):
    code, last = run(cmd, RGR)
    check('РГР', name, code == 0, last)
code, _ = run(['tools/krok_parse.py'] + sorted(glob.glob(os.path.join(RGR, 'examples', '*.krok'))), RGR)
check('РГР', 'усі приклади специфікації розбираються', code == 0)
code, _ = run(['tools/krok_parse.py', '--invalid'] + sorted(glob.glob(os.path.join(RGR, 'examples', 'invalid', '*.krok'))), RGR)
check('РГР', 'усі некоректні приклади відхиляються', code == 0)
spec = pdf_text(glob.glob(os.path.join(RGR, 'Специфікація*ТВ-43.pdf'))[0])
for part in ('Вступ', 'Лексична структура', 'Типи даних і змінні', 'Вирази', 'Інструкції', 'Декларації',
             'Структура програми', 'Повна граматика мови'):
    check('РГР', 'розділ за розд. 2.4 посібника Стативки: «{0}»'.format(part), part in spec)
rgr_task = pdf_text(os.path.join(DL, 'Розробка і специфікація мови програмування. РГР. 2026.pdf'))
for req in ('Таблиця токенів', 'Базового прикладу', 'Повна граматика мови'):
    check('РГР', 'вимога завдання «{0}» є в завданні та виконана'.format(req),
          req.lower() in rgr_task.lower() and req.split()[0].lower()[:6] in spec.lower())
check('РГР', 'повна граматика — останній розділ', spec.rindex('Повна граматика мови') > spec.rindex('Приклади програм'))

# ---------------------------------------------------------------- КП1
KP1 = os.path.join(HERE, 'KP', 'KP1')
code, last = run(['tests/run_tests.py'], KP1)
check('КП1', 'тести лексера (зокрема звірка з еталонним лексером)', code == 0, last)
report_audit('КП1', os.path.join(DL, 'КП1. Лексичний аналiз методом дiаграми станiв.pdf'),
             os.path.join(KP1, 'ТВ-43.КП_1.СтепаненкоНЮ.pdf'), 'Лексичний аналіз методом діаграми станів',
             ['характеристика мови', 'лексична структура', 'граматик', 'графічн', 'символьн', 'семантичн',
              'програмн', 'приклад', 'план тестування', 'протокол тестування', 'висновки'],
             ['базовий приклад', 'вкладені конструкції', 'наявність недопустимих лексем',
              'помилково написані ключові слова'], 1)
method_audit('КП1', os.path.join(DL, 'my_lang_lexer.zip'), 'my_lang_lexer/my_lang_lex.py',
             os.path.join(KP1, 'krok_lex.py'), {})
check('КП1', 'діаграма для JFLAP і зображення є', all(os.path.exists(os.path.join(KP1, f)) for f in ('krok_lex.jff', 'krok_lex.png')))

# ---------------------------------------------------------------- КП2
KP2 = os.path.join(HERE, 'KP', 'KP2')
code, last = run(['tests/run_tests.py'], KP2)
check('КП2', 'тести парсера (зокрема звірка з граматичним розпізнавачем)', code == 0, last)
report_audit('КП2', os.path.join(DL, 'КП2. Синтаксичний аналіз методом рекурсивного спуску.pdf'),
             os.path.join(KP2, 'ТВ-43.КП_2.СтепаненкоНЮ.pdf'), 'Синтаксичний аналіз методом рекурсивного спуску',
             ['характеристика мови', 'граматика', 'лексичн', 'програмн', 'план тестування', 'протокол тестування',
              'висновки'],
             ['пропуск термінала', 'зайвий термінал', 'помилка у наборі ключового слова',
              'помилковий оператор у виразах', 'перевірка пріоритетності операторів',
              'перевірка асоціативності операторів', 'вкладені конструкції'], 2)
method_audit('КП2', os.path.join(DL, 'my_lang_parser.zip'), 'my_lang_parser/parser_02/parse_02.py',
             os.path.join(KP2, 'krok_parse.py'),
             {'parseAssign': 'parseSimpleStmt', 'parseTerm': 'parseMulExpr', 'parseFactor': 'parseOperand',
              'parseIf': 'parseIfStmt', 'parseBoolExpr': 'parseRelExpr'})

# ---------------------------------------------------------------- Word_KPI
for f in ('Транслятори_01.2026.pdf', 'Транслятори_02. 2026.pdf', 'Транслятори_03. 2026. Трансформація граматик.pdf',
          'Стативка.Формальні мови.pdf', 'Медведєва В.М. Третяк В.А. Транслятори_1.pdf',
          'Медведєва В.М. Третяк В.А. Транслятори_2.pdf', 'Іменовані константи Приклади.pdf',
          'Графік освітніх активностей. Транслятори. 2026-2027.pdf', 'JFLAP.zip', 'Ebnf-Visualizer.zip'):
    p = os.path.join(WORD, f)
    check('Word_KPI', 'матеріал курсу: ' + f, os.path.exists(p) and sha(p) == sha(os.path.join(DL, f)))
for work, task, repo in (('РГР', 'Розробка і специфікація мови програмування. РГР. 2026.pdf', RGR),
                         ('КП1', 'КП1. Лексичний аналiз методом дiаграми станiв.pdf', KP1),
                         ('КП2', 'КП2. Синтаксичний аналіз методом рекурсивного спуску.pdf', KP2)):
    check('Word_KPI', '{0}: завдання'.format(work), os.path.exists(os.path.join(WORD, work, task)))
    stale = []
    for src in glob.glob(os.path.join(repo, '**', '*'), recursive=True):
        rel = os.path.relpath(src, repo)
        if os.path.isdir(src) or '__pycache__' in rel or rel.startswith('.git') or rel.endswith(('.docx', '.pdf')) and work == 'РГР':
            continue
        dst = os.path.join(WORD, work, 'Відповідь', rel)
        if not os.path.exists(dst) or sha(dst) != sha(src):
            stale.append(rel)
    check('Word_KPI', '{0}: відповідь збігається з репозиторієм'.format(work), not stale, ', '.join(stale[:4]))

# ---------------------------------------------------------------- звіт
with open(os.path.join(HERE, 'AUDIT.md'), 'w', encoding='utf-8') as f:
    f.write('# Аудит робіт з «Основ розробки трансляторів»\n\n'
            'Згенеровано `audit.py`. Вимоги до звітів КП взято безпосередньо з PDF методичок.\n\n'
            '**Перевірок: {0}, не пройдено: {1}**\n\n| Робота | Перевірка | Пройдено | Примітка |\n|---|---|---|---|\n'
            .format(len(rows), fails))
    for r in rows:
        f.write('| {0} | {1} | {2} | {3} |\n'.format(*(str(x).replace('|', '\\|') for x in r)))
for r in rows:
    if r[2] == 'НІ':
        print('НІ  ', r[0], '|', r[1], '|', r[3])
print('перевірок: {0}, не пройдено: {1}'.format(len(rows), fails))
sys.exit(1 if fails else 0)
