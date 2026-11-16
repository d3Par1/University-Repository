# Звіт до роботи КП 4 «Трансляція у ПОЛІЗ та виконання програм»: -> ТВ-43.КП_4.СтепаненкоНЮ.docx
# Окремих вимог до звіту матеріали до КП 4 не містять, тому оформлення - як у звітах до КП 1 - КП 3.
# Фрагменти коду, таблиці та протокол формуються з коду і справжніх запусків транслятора та PSM.
import ast, os, subprocess, sys, textwrap
from docx import Document
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.shared import Cm, Pt
from PIL import Image, ImageDraw, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, 'tests'))
from run_tests import CASES, EXPRS, kp3

OUT = os.path.join(HERE, 'ТВ-43.КП_4.СтепаненкоНЮ.docx')
SRC = open(os.path.join(HERE, 'krok_translate.py'), encoding='utf-8').read()
GEN = open(os.path.join(HERE, 'krok_codegen.py'), encoding='utf-8').read()
doc = Document()
J, L, C = WD_ALIGN_PARAGRAPH.JUSTIFY, WD_ALIGN_PARAGRAPH.LEFT, WD_ALIGN_PARAGRAPH.CENTER

def font(run, name='Times New Roman', size=12, bold=False, italic=False):
    run.font.name, run.font.size, run.bold, run.italic = name, Pt(size), bold, italic
    rp = run._element.get_or_add_rPr()
    rf = rp.find(qn('w:rFonts'))
    if rf is None:
        rf = OxmlElement('w:rFonts'); rp.insert(0, rf)
    for a in ('w:ascii', 'w:hAnsi', 'w:cs', 'w:eastAsia'):
        rf.set(qn(a), name)
    return run


def para(text, bold=False, size=12, align=J, indent=True):
    p = doc.add_paragraph()
    pf = p.paragraph_format
    pf.space_after, pf.space_before, pf.line_spacing = Pt(0), Pt(0), 1.15
    pf.first_line_indent = Cm(1.0) if indent else Cm(0)
    p.alignment = align
    for i, part in enumerate(text.split('`')):
        if part:
            font(p.add_run(part), 'Consolas' if i % 2 else 'Times New Roman', size - (1 if i % 2 else 0), bold)
    return p


def heading(text):
    p = para(text, bold=True, size=13, align=L, indent=False)
    p.paragraph_format.space_before, p.paragraph_format.space_after = Pt(10), Pt(4)
    p.paragraph_format.keep_with_next = True


def subheading(text):
    p = para(text, bold=True, size=12, align=L, indent=False)
    p.paragraph_format.space_before, p.paragraph_format.space_after = Pt(6), Pt(2)
    p.paragraph_format.keep_with_next = True


def code(lines, size=8):
    # довгий рядок переноситься з відступом свого рядка + 4; короткий блок не розривається між сторінками
    wrapped = [w for l in lines for w in (textwrap.wrap(l, 100, subsequent_indent=' ' * (len(l) - len(l.lstrip()) + 4))
                                          if len(l) > 100 else [l])]
    for k, line in enumerate(wrapped):
        p = doc.add_paragraph()
        pf = p.paragraph_format
        pf.space_after, pf.space_before, pf.line_spacing, pf.left_indent = Pt(0), Pt(0), 1.0, Cm(0.3)
        pf.keep_with_next = len(wrapped) <= 45 and k < len(wrapped) - 1
        sh = OxmlElement('w:shd')
        sh.set(qn('w:val'), 'clear'); sh.set(qn('w:color'), 'auto'); sh.set(qn('w:fill'), 'F2F2F2')
        p._p.get_or_add_pPr().append(sh)
        font(p.add_run(line.replace('\t', '    ') or ' '), 'Consolas', size)
    doc.add_paragraph().paragraph_format.line_spacing = 0.5


def table(rows, widths, size=9.5):
    t = doc.add_table(rows=len(rows), cols=len(rows[0]))
    t.style = 'Table Grid'
    for r, row in enumerate(rows):
        for c, text in enumerate(row):
            t.cell(r, c).width = Cm(widths[c])
            p = t.cell(r, c).paragraphs[0]
            p.paragraph_format.space_after, p.paragraph_format.line_spacing = Pt(0), 1.0
            font(p.add_run(str(text)), size=size, bold=(r == 0))
    doc.add_paragraph().paragraph_format.line_spacing = 0.5


def caption(text):
    p = para(text, align=L, indent=False, size=11)
    p.paragraph_format.keep_with_next = True


def src(*names, text=SRC):
    """Текст функцій транслятора - фрагменти коду у звіті не розходяться з програмою."""
    tree, lines, res = ast.parse(text), text.split('\n'), []
    for node in tree.body:
        if isinstance(node, ast.FunctionDef) and node.name in names:
            start = min([d.lineno for d in node.decorator_list] + [node.lineno]) - 1
            res += lines[start:node.end_lineno] + ['']
    return res[:-1]


def run(args, stdin=''):
    """Справжній запуск команди python <args> у теці роботи; повертає рядки виведення."""
    r = subprocess.run([sys.executable, '-X', 'utf8'] + args, cwd=HERE, input=stdin, capture_output=True,
                       text=True, encoding='utf-8')
    return (r.stdout + r.stderr).rstrip('\n').split('\n')


def figure(path):
    """Блок-схема трикомпонентного циклу for з мітками."""
    im = Image.new('RGB', (760, 600), 'white')
    d = ImageDraw.Draw(im)
    f = ImageFont.truetype(r'C:\Windows\Fonts\times.ttf', 24)
    fi = ImageFont.truetype(r'C:\Windows\Fonts\timesi.ttf', 22)

    def box(cx, cy, text, w=230, h=54):
        d.rectangle([cx - w // 2, cy - h // 2, cx + w // 2, cy + h // 2], outline='black', width=2)
        d.text((cx, cy), text, font=f, fill='black', anchor='mm')

    def arrow(points):
        d.line(points, fill='black', width=2)
        (x1, y1), (x2, y2) = points[-2], points[-1]
        if x1 == x2:
            s = 1 if y2 > y1 else -1
            d.polygon([(x2, y2), (x2 - 7, y2 - 14 * s), (x2 + 7, y2 - 14 * s)], fill='black')
        else:
            s = 1 if x2 > x1 else -1
            d.polygon([(x2, y2), (x2 - 14 * s, y2 - 7), (x2 - 14 * s, y2 + 7)], fill='black')

    cx = 330
    box(cx, 50, 'S1 (ініціалізація)')
    arrow([(cx, 77), (cx, 143)])
    d.polygon([(cx, 143), (cx + 130, 190), (cx, 237), (cx - 130, 190)], outline='black', width=2)
    d.text((cx, 190), 'E (умова)', font=f, fill='black', anchor='mm')
    arrow([(cx, 237), (cx, 303)])
    box(cx, 330, 'B (тіло циклу)')
    arrow([(cx, 357), (cx, 423)])
    box(cx, 450, 'S2 (наступний крок)')
    arrow([(cx, 477), (cx, 520), (90, 520), (90, 110), (cx, 110)])
    arrow([(cx + 130, 190), (640, 190), (640, 560)])
    for x, y, t in ((cx + 12, 118, 'm1 :'), (cx + 12, 268, 'm3 :'), (cx + 12, 388, 'm2 :'), (652, 540, 'm4 :'),
                    (cx + 12, 244, 'true'), (cx + 140, 160, 'false')):
        d.text((x, y), t, font=fi, fill='black')
    im.save(path)


# ---------------------------------------------------------------- оформлення
sec = doc.sections[0]
sec.left_margin, sec.right_margin, sec.top_margin, sec.bottom_margin = Cm(2.5), Cm(1.5), Cm(2), Cm(2)
hp = sec.header.paragraphs[0]; hp.alignment = WD_ALIGN_PARAGRAPH.RIGHT
font(hp.add_run('Степаненко Назар, група ТВ-43, робота КП № 4'), size=10, italic=True)
fp = sec.footer.paragraphs[0]; fp.alignment = C
r = fp.add_run()
for kind in ('begin', 'instr', 'end'):
    el = OxmlElement('w:instrText' if kind == 'instr' else 'w:fldChar')
    if kind == 'instr':
        el.text = 'PAGE'
    else:
        el.set(qn('w:fldCharType'), kind)
    r._r.append(el)
font(r, size=10)

para('Трансляція у ПОЛІЗ та виконання програм', bold=True, size=16, align=C, indent=False)
para('Мета роботи: програмна реалізація транслятора мови Krok у постфіксний код (ПОЛІЗ) і виконання отриманого '
     'коду стековою машиною PSM версії 0.3.')

heading('1. Коротка характеристика мови')
para('Krok є імперативною процедурною мовою зі статичною типізацією; мова-прототип Go, рівень складності 2. '
     'Базові типи: `int`, `float`, `bool`, `string`; є користувацькі скалярні типи, масиви `[N]T` і структури. '
     'Функції можуть бути рекурсивними, параметр типу `*T` передається за посиланням. Інструкції: присвоєння, '
     'оголошення з ініціалізацією `:=`, `if / else if / else`, `for` з передумовою та трикомпонентний, `return`, '
     'введення й виведення через пакет `fmt`. Лексичний, синтаксичний і семантичний аналіз виконують програми '
     'з робіт КП 1 - КП 3; у цій роботі до них додано генерування коду.')

heading('2. Цільова машина')
para('PSM (Postfix Stack Machine) версії 0.3 є стековою машиною без адрес: інструкції беруть операнди зі стека '
     'і кладуть туди результат. Програма складається з модулів, кожен в окремому файлі `.postfix`: головний '
     'модуль і по модулю на функцію (`<програма>$<функція>.postfix`). Файл містить секції `.vars` (змінні модуля, '
     'параметри першими), `.globVarList` (глобальні змінні, до яких звертається функція), `.funcs` (функції, які '
     'модуль викликає), `.labels` (мітки) і `.code`. Інструкції, що їх генерує транслятор, наведено в табл. 1.')
caption('Таблиця 1 — Інструкції PSM, які використовує транслятор')
table([('Інструкція', 'Призначення'),
       ('x l-val, x r-val, 5 int, 2.5 float, true bool, "a" string', 'посилання на змінну, її значення, літерали'),
       ('= assign_op', 'присвоювання: знімає значення і l-val'),
       ('+ - * / math_op, NEG math_op, ^ pow_op', 'арифметика над операндами одного типу'),
       ('== != < <= > >= rel_op; AND OR NOT bool_op; CAT cat_op', 'порівняння, логічні операції, конкатенація'),
       ('i2f f2i i2s f2s s2i s2f conv', 'перетворення типів'),
       ('m1 label, : colon, JF jf, JMP jump', 'мітка, точка входу, умовний і безумовний перехід'),
       ('f CALL, RET', 'виклик функції та повернення'),
       ('OUT out_op, INP inp_op', 'виведення значення з нового рядка, введення рядка'),
       ('POP SWAP stack_op', 'відкинути вершину стека, поміняти місцями два верхні елементи')], [8.3, 8.3])
para('Машина суворо перевіряє типи: операнди арифметичної операції чи порівняння, значення і змінна в '
     'присвоюванні, аргумент і параметр мають бути одного типу. Тому все, що в Krok відбувається неявно, '
     'транслятор записує явними інструкціями. Масивів, структур і передачі за посиланням у PSM немає.')

heading('3. Схеми трансляції')
subheading('3.1. Вирази')
para('Вираз транслюється за рекурсивним означенням ПОЛІЗу з матеріалів до роботи: ПОЛІЗ константи чи змінної '
     'є вона сама, ПОЛІЗ(E1 op E2) = ПОЛІЗ(E1) ПОЛІЗ(E2) op, дужки ігноруються. Пріоритет і асоціативність '
     'уже враховано граматикою, тож кожна функція розбору просто додає оператор після кодів своїх операндів. '
     'Відмінності Krok від PSM зведено в табл. 2.')
caption('Таблиця 2 — Трансляція операторів мови Krok')
table([('Конструкція Krok', 'ПОЛІЗ', 'Пояснення'),
       ('n + x  (int + float)', 'n i2f x +', 'неявне розширення int до float транслятор записує явно'),
       ('n + 2.5', 'n i2f 2.5 +', 'те саме; константа одразу записується як float (x + 2 дає x 2.0 +)'),
       ('a / b  (int)', 'a b / f2i', 'ділення PSM завжди дає float; f2i відкидає дробову частину'),
       ('a % b', 'a b $imod CALL', 'остача зі знаком діленого: a - (a / b) * b'),
       ('a ** b  (int)', 'a b рядок $ipow CALL', 'множення в циклі; від\'ємний показник дає помилку RE-05'),
       ('x ** y  (float)', 'x y ^', 'оператор PSM працює лише з float'),
       ('-a,  !f', 'a NEG,  f NOT', 'префіксний оператор записується після операнда'),
       ('s + t  (string)', 's t CAT', ''),
       ('p && q', 'p q AND', 'якщо q не може дати помилку чи побічний ефект'),
       ('p && q', 'p m1 JF q m2 JMP m1 : false m2 :', 'інакше скорочена схема: q не обчислюється, коли p хибне'),
       ('7 * 3,  len(a),  Size', '21,  5,  5', 'константні вирази обчислює транслятор (розд. «Константні вирази» '
                                              'специфікації)')], [4.0, 5.3, 7.3])
para('Через неявне розширення тип лівого операнда іноді стає відомим уже після того, як згенеровано код '
     'правого. Тоді інструкція `i2f` вставляється всередину коду, а значення міток, розташованих далі, '
     'зсуваються (функція `insertCode`).')

subheading('3.2. Присвоювання та оголошення')
para('Як у розд. 2.1 матеріалів: до ПОЛІЗу додається ідентифікатор з позначкою l-value, потім ПОЛІЗ виразу і '
     'оператор присвоювання. `v1 = (5.4 + a) / 2` дає `v1 l-val 5.4 float a r-val i2f + 2.0 float / =`. '
     'Оголошення `x := e` транслюється так само. Змінна без ініціалізатора за специфікацією має нульове значення, '
     'а PSM читати змінну без значення не дозволяє, тому для `var n int` генерується `n l-val 0 int =`. '
     '`i++` транслюється як `i l-val i r-val 1 int + =`.')

subheading('3.3. Інструкція розгалуження')
para('Схема з розд. 2.5 матеріалів, пристосована до граматики Krok (гілка `else` необов\'язкова, блоки у '
     'фігурних дужках):')
caption('Таблиця 3 — Схема трансляції інструкції if')
table([('Елемент', 'Переклад'), ('if', ''), ('E', 'ПОЛІЗ(E)'), ('{ (початок блоку B1)', 'm1 JF'),
       ('B1', 'ПОЛІЗ(B1)'), ('else', 'm2 JMP m1 :'), ('B2 або наступний if', 'ПОЛІЗ(B2)'),
       ('кінець інструкції', 'm2 :   (без else лише m1 :)')], [6.0, 10.6])
para('Ланцюжок `else if` окремої схеми не потребує: наступний `if` транслюється рекурсивно як гілка `else`.')

subheading('3.4. Інструкція повторення')
para('Цикл з передумовою `for E B` транслюється як `m1 : ПОЛІЗ(E) m2 JF ПОЛІЗ(B) m1 JMP m2 :`. '
     'У трикомпонентному циклі `for S1; E; S2 B` крок S2 у тексті програми стоїть перед тілом, а виконується '
     'після нього. Транслятор однопрохідний, тому код лишається в порядку запису, а потрібний порядок '
     'виконання задають переходи (рис. 1, табл. 4).')
figure(os.path.join(HERE, 'for_scheme.png'))
doc.add_picture(os.path.join(HERE, 'for_scheme.png'), width=Cm(9.5))
doc.paragraphs[-1].alignment = C
para('Рисунок 1 — Блок-схема трикомпонентного циклу for', align=C, indent=False, size=11)
caption('Таблиця 4 — Схема трансляції трикомпонентного циклу for')
table([('Елемент', 'Переклад'), ('for', ''), ('S1', 'ПОЛІЗ(S1)'), ('перша ;', 'm1 :'), ('E', 'ПОЛІЗ(E)'),
       ('друга ;', 'm4 JF m3 JMP m2 :'), ('S2', 'ПОЛІЗ(S2)'), ('{ (початок тіла)', 'm1 JMP m3 :'),
       ('B', 'ПОЛІЗ(B)'), ('} (кінець тіла)', 'm2 JMP m4 :')], [6.0, 10.6])

subheading('3.5. Введення та виведення')
para('`OUT` друкує одне значення і завжди з нового рядка, а логічні значення друкує як `True`/`False`. Тому '
     'рядок виведення транслятор збирає на стеку: кожне значення перетворюється на текст (`i2s`, `f2s`, для '
     '`bool` допоміжна функція `$b2s`) і приєднується інструкцією `CAT`; `fmt.Println(a, b)` дає '
     '`a i2s " " CAT b i2s CAT OUT`. Якщо програма містить `fmt.Print`, який рядка не завершує, текст '
     'накопичується в службовій змінній `$out` і виводиться, коли рядок завершено, перед введенням та '
     'наприкінці програми. `fmt.Scan(&n)` транслюється як `n l-val INP s2i =`: `INP` повертає рядок, тому '
     'кожне значення вводиться з нового рядка.')

subheading('3.6. Функції')
para('Кожна функція стає модулем PSM; головний модуль ініціалізує глобальні змінні й викликає `main`. '
     'Виклик `f(a, b)` дає `ПОЛІЗ(a) ПОЛІЗ(b) f CALL`, `return e` дає `ПОЛІЗ(e) RET`; якщо результат виклику '
     'не потрібен, додається `POP`. Параметр-вказівник `*T` реалізовано копіюванням: `swap(&x, &y)` передає '
     'значення `x` та `y`, функція перед `RET` записує нові значення в глобальні змінні `$swap.a`, `$swap.b`, '
     'а викликач після `CALL` копіює їх назад. Масив чи структуру функція повертає так само, через глобальні '
     'змінні. Змінна, що затінює іншу, отримує в модулі PSM ім\'я із суфіксом (`x#2`).')

subheading('3.7. Масиви та структури')
para('Значення масиву чи структури розгортається в скалярні змінні PSM з іменами `grid[1][2]`, `p.x`, '
     '`seg.from.x`; присвоювання, передача аргументом і повернення копіюють усі складові. Якщо індекс сталий, '
     '`a[2]` є звичайною змінною. Якщо індекс відомий лише під час виконання, потрібна змінна обирається '
     'порівняннями та переходами; `x = a[i]` для масиву з трьох елементів транслюється так:')
code(['x l-val',
      'i r-val 0 int == m2 JF  a[0] r-val  m1 JMP  m2 :',
      'i r-val 1 int == m3 JF  a[1] r-val  m1 JMP  m3 :',
      'i r-val 2 int == m4 JF  a[2] r-val  m1 JMP  m4 :',
      '"runtime error RE-04 at 8: index " i r-val i2s CAT " out of range [0, 2]" CAT $halt CALL',
      'm1 :',
      '='], size=9)
para('Якщо жодне порівняння не справдилось, індекс поза межами масиву: виводиться повідомлення про помилку '
     'RE-04 і машина зупиняється. Інструкції зупинки PSM не має, тому функція `$halt` виконує перехід на '
     'неоголошену мітку, після якого машина завершує роботу.')

subheading('3.8. Чого PSM не дозволяє виразити')
para('Транслятор відхиляє з повідомленням чотири конструкції: порівняння рядків (оператори порівняння PSM '
     'працюють лише з числами та `bool`), `len` рядка, значення якого транслятор не знає, введення значення типу '
     '`bool` і символ нового рядка в рядку, що не є аргументом виведення. Є й відмінності в поведінці: цілі '
     'числа PSM необмежені, тому переповнення (RE-02) не виявляється; цілочислове ділення точне для чисел до '
     '2^53; параметр-вказівник відрізняється від справжнього посилання, якщо до тієї самої змінної функція '
     'звертається ще й напряму.')

heading('4. Програмна реалізація транслятора')
para('Транслятор складається з двох файлів. `krok_translate.py` є аналізатором з КП 3, функції розбору якого '
     'доповнено генеруванням коду (такі місця позначено коментарем «Трансляція»). `krok_codegen.py` містить '
     'функції з матеріалів до роботи та модель модуля PSM. Відповідність функціям з матеріалів наведено в '
     'табл. 5.')
caption('Таблиця 5 — Функції з матеріалів до роботи та їх реалізація')
table([('Функція з матеріалів', 'Реалізація'),
       ('compileToPostfix(fileName)', 'та сама: лексичний аналіз, розбір з генеруванням коду, serv(), '
                                      'savePostfixCode()'),
       ('postfixCodeGen(case, toTran)', 'та сама; інші функції додають інструкції лише через неї'),
       ('createLabel(), setValLabel(lbl)', 'ті самі; вільний номер мітки шукається в циклі, як радить методичка'),
       ('parseAssign()', 'parseSimpleStmt(): присвоювання, :=, ++/-- і виклик починаються з виразу'),
       ('parseExpression(), parseTerm(), parseFactor()', 'parseOrExpr() ... parseMulExpr(), parseUnaryExpr(), '
                                                         'parsePowerExpr(), parsePrimaryExpr(), parseOperand()'),
       ('parseIf(), parseBoolExpr()', 'parseIfStmt(); окремого логічного виразу немає, умова має тип bool'),
       ('configToPrint(), toView', 'ті самі (ключ --view)'),
       ('serv(), savePostfixCode()', 'ті самі; друкують і зберігають усі модулі програми')], [5.6, 11.0])
para('Трансляція присвоювання та розгалуження:')
code(src('parseIfStmt'))
para('Функція `binary()` викликається після розбору обох операндів бінарного оператора. Вона перевіряє типи '
     '(КП 3), а далі додає перетворення типів і сам оператор:')
code(src('binary'))
para('Функції генератора коду з матеріалів до роботи:')
code(src('postfixCodeGen', 'createLabel', 'setValLabel', text=GEN))
para('Значення мітки, як у методичці, є номером запису ПОЛІЗу, де стоїть сама мітка перед двокрапкою. '
     'Наприкінці трансляції функція `checkModule()` перераховує таблицю міток за готовим кодом і порівнює її з '
     'тією, яку вів транслятор.')

heading('5. План тестування')
groups = {}
for g, name, text, inputs, exp in CASES:
    groups[g] = groups.get(g, 0) + 1
rows = [('№', 'Тип випробування', 'Очікуваний результат', 'Тестів')]
want = {'6': 'повідомлення про помилку, зупинка машини', '7': 'транслятор відхиляє програму з повідомленням'}
for g, n in groups.items():
    rows.append((g.split()[0], g.split(' ', 1)[1], want.get(g.split()[0], 'виведення PSM збігається з очікуваним, '
                                                                          'стек порожній'), n))
rows.append(('9', 'вирази зі змінними та з константами', 'результат PSM дорівнює обчисленому транслятором', len(EXPRS)))
rows.append(('10', 'тести семантичного аналізатора КП 3', 'ті самі коди помилок CE-01 ... CE-17', len(kp3.CASES)))
caption('Таблиця 6 — План тестування')
table(rows, [0.8, 6.0, 8.2, 1.6])
para('Кожна програма транслюється і виконується файлом `PSM.py` з матеріалів курсу без змін. Очікуване '
     'виведення прикладів групи 8 узято зі специфікації мови (розд. «Приклади програм»), де воно було записане '
     'ще до появи транслятора. У групі 9 один і той самий вираз обчислюється двічі: кодом у PSM і транслятором '
     'під час згортання констант; результати мають збігтися.')

heading('6. Протокол тестування')
DEMOS = [('assign', '', 'присвоювання з неявним розширенням `a` до `float` та остача; (5.4 + 5) / 2 = 5.2, 5 % 3 = 2.'),
         ('branch', '3\n3\n', 'для введених 3 і 3 виконано гілку `else if`; `: ` є запрошенням PSM до введення.'),
         ('loop', '', 'сума 1 + 2 + 3 + 4 = 10, далі 10 / 2 = 5, 5 / 2 = 2; у коді видно мітки обох форм циклу.'),
         ('array', '', 'елементи впорядковано: два обміни через параметри-вказівники зі змінним індексом.')]
for k, (name, stdin, note) in enumerate(DEMOS, 1):
    p = para('Тест № {0}: examples/{1}.krok.'.format(k, name), bold=True, align=L, indent=False)
    p.paragraph_format.keep_with_next = True
    text = open(os.path.join(HERE, 'examples', name + '.krok'), encoding='utf-8').read().rstrip('\n').split('\n')
    code(['{0:>2}  {1}'.format(i, l) for i, l in enumerate(text, 1)])
    out = run(['krok_translate.py', 'examples/{0}.krok'.format(name)] + (['--quiet'] if name == 'array' else []))
    if name in ('branch', 'loop'):          # лістинг головного модуля і допоміжних функцій не повторюємо
        a = next(i for i, l in enumerate(out) if l.startswith('Модуль ' + name + '$main'))
        b = next(i for i, l in enumerate(out) if l.startswith('postfix-код'))
        out = out[:6] + ['... (модуль {0} пропущено) ...'.format(name)] + out[a - 1:b] + out[b:]
        c = next((i for i, l in enumerate(out) if l.startswith('Модуль ' + name + '$$')), None)
        if c:
            d = next(i for i, l in enumerate(out) if l.startswith('postfix-код'))
            out = out[:c - 1] + ['', '... (допоміжні модулі пропущено) ...'] + out[d:]
    code(['>python krok_translate.py examples/{0}.krok'.format(name) + (' --quiet' if name == 'array' else '')] + out)
    if name == 'assign':
        code(['>type postfix\\assign\\assign$main.postfix'] +
             open(os.path.join(HERE, 'postfix', 'assign', 'assign$main.postfix'), encoding='utf-8').read()
             .rstrip('\n').expandtabs(4).split('\n'))
    if name == 'array':
        code(['>type postfix\\array\\array$swap.postfix'] +
             open(os.path.join(HERE, 'postfix', 'array', 'array$swap.postfix'), encoding='utf-8').read()
             .rstrip('\n').expandtabs(4).split('\n'))
    code(['>python PSM.py -p postfix/{0} -m {0}'.format(name)] +
         run(['PSM.py', '-p', 'postfix/' + name, '-m', name], stdin))
    para('Оцінка: ' + note)
p = para('Тест № 5: базовий приклад зі специфікації мови (введено 4 і 7).', bold=True, align=L, indent=False)
p.paragraph_format.keep_with_next = True
out = run(['krok_translate.py', '../KP1/tests/base_example.krok', '--quiet'])
code(['>python krok_translate.py ../KP1/tests/base_example.krok --quiet'] + out)
code(['>python PSM.py -p postfix/base_example -m base_example'] +
     run(['PSM.py', '-p', 'postfix/base_example', '-m', 'base_example'], '4\n7\n'))
para('Оцінка: виведення збігається з наведеним у специфікації. Програма використовує масиви, структуру, '
     'користувацький тип, рекурсію та параметри-вказівники; транслятор побудував головний модуль, п\'ять модулів '
     'функцій і три допоміжні.')
p = para('Тест № 6: помилка часу виконання (приклад 08_runtime_errors.krok, введено 5 і 1).', bold=True, align=L,
         indent=False)
p.paragraph_format.keep_with_next = True
run(['krok_translate.py', '../KP1/tests/08_runtime_errors.krok', '--quiet'])
code(['>python PSM.py -p postfix/08_runtime_errors -m 08_runtime_errors'] +
     run(['PSM.py', '-p', 'postfix/08_runtime_errors', '-m', '08_runtime_errors'], '5\n1\n'))
para('Оцінка: індекс 5 поза межами масиву з трьох елементів; виведено повідомлення RE-04 з номером рядка '
     'програми, після чого машина зупинилась.')
para('Повний протокол автоматичного тестування:', bold=True, align=L, indent=False)
code(['>python tests/run_tests.py'] + run([os.path.join('tests', 'run_tests.py')]), size=7)

heading('7. Висновки')
for line in (
    'Аналізатор з попередніх робіт доповнено генеруванням постфіксного коду. Транслятор перекладає в код для PSM '
    '0.3 усі конструкції мови Krok, крім чотирьох, для яких у машини немає інструкцій, і всі приклади програм зі '
    'специфікації виконуються в PSM з тим виведенням, яке в специфікації записане.',
    'Схеми присвоювання, розгалуження та виведення з методички підійшли майже без змін. Більше довелося '
    'думати там, де мова багатша за машину. Масиви та структури розгорнуто в окремі змінні, доступ за змінним '
    'індексом зроблено ланцюжком порівнянь, а передачу за посиланням копіюванням значень туди й назад. Таке '
    'рішення працює, але код для масивів виходить довгим: одне звернення до масиву з N елементів займає '
    'близько 10·N інструкцій.',
    'Несподівано багато дрібниць виявилось у самій машині: ділення цілих дає дійсне число, `OUT` завжди '
    'переводить рядок, рядок коду, що починається з крапки, читається як заголовок секції. Кожну таку '
    'особливість знайдено читанням `PSM.py` і перевірено тестом.',
    'Правильність перевірено {0} програмами, які виконує сама PSM, порівнянням результатів {1} виразів, '
    'обчислених машиною і транслятором, та {2} тестами семантичного аналізатора.'
    .format(len(CASES), len(EXPRS), len(kp3.CASES))):
    para(line)

heading('Перелік використаних джерел')
for k, line in enumerate((
    'Стативка Ю. Трансляція та виконання програм: матеріали до виконання роботи № 4. 2023. 35 с.',
    'Специфікація PSM (версія 0.3). 22 с.; програма PSM.py та приклади prog1 - prog4.',
    'Медведєва В. М. Транслятори: внутрішнє подання програм та інтерпретація [Текст]: навч. посіб. / '
    'В. М. Медведєва, В. А. Третяк. К.: Текст, 2015. 144 с.',
    'Стативка Ю. Семантичний аналіз: матеріали до виконання роботи КП 3. 2024. 11 с.',
    'Степаненко Н. Ю. Специфікація мови програмування Krok: розрахунково-графічна робота. 2026.'), 1):
    para('{0}. {1}'.format(k, line), indent=False)

doc.save(OUT)
print('written', os.path.basename(OUT))
