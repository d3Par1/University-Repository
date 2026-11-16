# Генератор постфіксного коду (ПОЛІЗ) для стекової машини PSM версії 0.3 - частина транслятора мови Krok.
# За матеріалами до роботи КП 4 «Трансляція у ПОЛІЗ та виконання програм» і специфікацією PSM:
# функції postfixCodeGen(), createLabel(), setValLabel(), configToPrint() - з матеріалів до роботи;
# решта - модель модуля PSM (.vars, .globVarList, .funcs, .labels, .code), типові послідовності інструкцій
# і запис .postfix-файлів. Програма мовою Krok транслюється в головний модуль і по модулю на кожну функцію.
import os

VERSION = '0.3'
ZERO = {'int': 0, 'float': 0.0, 'bool': False, 'string': ''}
# допоміжні функції, які транслятор додає до програми: ім'я -> (тип результату, кількість параметрів)
HELPERS = {'$b2s': ('string', 1), '$imod': ('int', 2), '$ipow': ('int', 3), '$halt': ('void', 1)}
OUT = ('$out', 'string', True)      # рядок виведення, який накопичують інструкції fmt.Print
PEND = ('$pend', 'bool', True)      # чи є в $out ще не виведений текст


class Module:
    """Модуль PSM - головна програма або функція; зберігається в окремому .postfix-файлі."""
    def __init__(self, name, kind):
        self.name, self.kind = name, kind     # kind: 'main' або 'func'
        self.vars = {}        # .vars: ім'я змінної -> тип; параметри функції - перші, у порядку оголошення
        self.globals = []     # .globVarList: глобальні змінні, до яких звертається функція
        self.funcs = {}       # .funcs: ім'я функції -> (тип результату, кількість параметрів)
        self.code = []        # .code: постфіксний код [(лексема, токен)]
        self.labels = {}      # .labels: таблиця міток {мітка: номер інструкції}
        self.names = set()    # імена змінних мови Krok, уже зайняті в цьому модулі
        self.temps = {}       # лічильники тимчасових змінних поточної інструкції: тип -> кількість
        self.bases = 0        # лічильник тимчасових масивів і структур
        self.bad = []         # [(номер інструкції, рядок програми)] - рядкові літерали, яких PSM не підтримує


modules = {}          # усі модулі програми: ім'я модуля -> Module
helpers = []          # використані допоміжні функції
prog = ''             # ім'я програми (головного модуля)
buffered = False      # програма містить fmt.Print, тому виведення накопичується в $out
root = None           # головний модуль
cur = None            # модуль, код якого зараз генерується
postfixCode = []      # постфіксний код поточного модуля (cur.code)
tableOfLabel = {}     # таблиця міток поточного модуля (cur.labels)
toView = False        # покроковий перегляд процесу трансляції


def reset(name, isBuffered):
    """Почати трансляцію програми name."""
    global prog, buffered, root
    modules.clear()
    del helpers[:]
    prog, buffered = name, isBuffered
    root = newModule(name, 'main')
    if buffered:
        root.vars[OUT[0]], root.vars[PEND[0]] = OUT[1], PEND[1]


def newModule(name, kind):
    m = Module(name, kind)
    modules[name] = m
    setModule(m)
    return m


def setModule(m):
    global cur, postfixCode, tableOfLabel
    cur, postfixCode, tableOfLabel = m, m.code, m.labels


def here():
    """Номер наступної інструкції постфіксного коду."""
    return len(postfixCode)


# ---------------------------------------------------------------- функції з матеріалів до роботи
def postfixCodeGen(case, toTran):
    """Додати інструкцію до постфіксного коду (розд. 3.2.4 методички)."""
    if case == 'lval':
        lex, tok = toTran
        postfixCode.append((lex, 'l-val'))
    elif case == 'rval':
        lex, tok = toTran
        postfixCode.append((lex, 'r-val'))
    else:
        lex, tok = toTran
        postfixCode.append((lex, tok))
    if toView:
        configToPrint(lex)


def configToPrint(lex):
    """Покроковий перегляд трансляції (розд. 3.3 методички): оброблена лексема і отриманий ПОЛІЗ."""
    stage = '\nКрок трансляції\n'
    stage += 'лексема: \'{0}\'\n'
    stage += 'postfixCode = {1}\n'
    print(stage.format(lex, str(postfixCode)))


def createLabel():
    """Нова мітка m<номер> з невизначеним значенням (розд. 3.3.2 методички). Як і радить методичка,
    вільний номер шукається в циклі, тому конфлікт імен міток неможливий."""
    nmb = len(tableOfLabel) + 1
    lexeme = 'm' + str(nmb)
    while tableOfLabel.get(lexeme) is not None:
        nmb += 1
        lexeme = 'm' + str(nmb)
    tableOfLabel[lexeme] = 'val_undef'
    return (lexeme, 'label')


def setValLabel(lbl):
    """Значення мітки - номер запису постфіксного коду, куди її буде записано (розд. 3.3.3 методички)."""
    lex, _tok = lbl
    tableOfLabel[lex] = len(postfixCode)
    return True


# ---------------------------------------------------------------- інструкції
def op(lex, tok):
    postfixCodeGen(tok, (lex, tok))


def note(v):
    """Змінна v = (ім'я, тип, глобальна): звернення з функції до глобальної змінної - у .globVarList."""
    if foreign(v) and v[0] not in cur.globals:
        cur.globals.append(v[0])


def foreign(v):
    return v[2] and cur.kind == 'func'


def lval(v):
    note(v)
    postfixCodeGen('lval', (v[0], 'id'))


def rval(v):
    note(v)
    postfixCodeGen('rval', (v[0], 'id'))


def identity(ptype):
    """Операція, що не змінює значення на вершині стека, але замінює посилання (r-val) самим значенням."""
    return {'int': [('0', 'int'), ('+', 'math_op')], 'float': [('1.0', 'float'), ('*', 'math_op')],
            'bool': [('true', 'bool'), ('AND', 'bool_op')], 'string': [('""', 'string'), ('CAT', 'cat_op')]}[ptype]


def assign(v):
    """Оператор присвоювання: на стеку - посилання (l-val) на змінну v і значення.
    PSM не може записати посилання r-val у змінну іншого модуля, тому воно замінюється значенням."""
    if foreign(v) and postfixCode[-1][1] == 'r-val':
        postfixCode.extend(identity(v[1]))
    op('=', 'assign_op')


def storeTop(v):
    """Значення з вершини стека - у змінну v: l-val кладеться поверх значення і міняється з ним місцями."""
    if foreign(v):
        postfixCode.extend(identity(v[1]))
    lval(v)
    op('SWAP', 'stack_op')
    op('=', 'assign_op')


def copyVar(dst, src):
    """dst = src для двох змінних."""
    lval(dst)
    rval(src)
    assign(dst)


def string(val, line):
    """Рядковий літерал. Послідовність // у .postfix-файлі починає коментар, тому такий рядок складається
    з частин конкатенацією; символів нового рядка та повернення каретки PSM у рядках не підтримує."""
    if '\n' in val or '\r' in val:
        cur.bad.append((len(postfixCode), line))
    parts, piece = [], ''
    for ch in val:
        if ch == '/' and piece.endswith('/'):
            parts.append(piece)
            piece = ''
        piece += ch
    parts.append(piece)
    for k, part in enumerate(parts):
        op('"' + part + '"', 'string')
        if k:
            op('CAT', 'cat_op')


def const(val, ptype, line=None):
    """Літерал типу ptype зі значенням val."""
    if ptype == 'int':
        op(str(val), 'int')
    elif ptype == 'float':
        op(repr(float(val)), 'float')       # запис на кшталт .5 PSM сприйняв би як заголовок секції
    elif ptype == 'bool':
        op('true' if val else 'false', 'bool')
    else:
        string(val, line)


def zero(ptype):
    const(ZERO[ptype], ptype)


def label(lbl):
    postfixCode.append(lbl)


def colon(lbl):
    """m : - мітка стає точкою входу; значення заноситься до таблиці міток (розд. 2.2 методички)."""
    setValLabel(lbl)
    postfixCode.append(lbl)
    postfixCode.append((':', 'colon'))


def jf(lbl):
    postfixCode.append(lbl)
    postfixCode.append(('JF', 'jf'))


def jmp(lbl):
    postfixCode.append(lbl)
    postfixCode.append(('JMP', 'jump'))


def call(name, rtype, arity):
    cur.funcs[name] = (rtype, arity)
    op(name, 'CALL')


def callHelper(name):
    if name not in helpers:
        helpers.append(name)
    call(name, *HELPERS[name])


def insertCode(pos, instrs):
    """Вставити інструкції перед інструкцією pos; значення міток за місцем вставки зсуваються."""
    n = len(instrs)
    for lex, val in tableOfLabel.items():
        if val != 'val_undef' and val >= pos:
            tableOfLabel[lex] = val + n
    cur.bad = [(i + n if i >= pos else i, line) for i, line in cur.bad]
    postfixCode[pos:pos] = instrs
    for k, (lex, tok) in enumerate(instrs):
        if tok == 'label' and k + 1 < n and instrs[k + 1][1] == 'colon':
            tableOfLabel[lex] = pos + k


def cutCode(pos):
    """Вилучити кінець коду, починаючи з інструкції pos, разом з мітками, визначеними в ньому."""
    del postfixCode[pos:]
    for lex in [lex for lex, val in tableOfLabel.items() if val != 'val_undef' and val >= pos]:
        del tableOfLabel[lex]
    cur.bad = [b for b in cur.bad if b[0] < pos]


def isSafe(code):
    """Код виразу не може спричинити помилку виконання чи побічний ефект (немає викликів, ділення, переходів)."""
    for lex, tok in code:
        if tok in ('int', 'float', 'bool', 'string', 'r-val', 'rel_op', 'bool_op', 'cat_op'):
            continue
        if (tok == 'math_op' and lex in ('+', '-', '*', 'NEG')) or (tok == 'conv' and lex == 'i2f'):
            continue
        return False
    return True


# ---------------------------------------------------------------- змінні
def declVar(name, ptype, glob=False):
    (root if glob else cur).vars.setdefault(name, ptype)


def newTemp(ptype):
    """Тимчасова змінна для проміжного значення; після завершення інструкції використовується повторно."""
    k = cur.temps.get(ptype, 0) + 1
    cur.temps[ptype] = k
    name = '${0}{1}'.format(ptype[0], k)
    cur.vars.setdefault(name, ptype)
    return (name, ptype, False)


def resetTemps():
    cur.temps = {}


def newBase():
    """Базове ім'я тимчасового масиву чи структури."""
    cur.bases += 1
    return '$r{0}'.format(cur.bases)


# ---------------------------------------------------------------- типові послідовності
def halt(parts):
    """Помилка часу виконання: повідомлення складається з рядків і значень цілих змінних."""
    for k, part in enumerate(parts):
        if isinstance(part, str):
            string(part, None)
        else:
            rval(part)
            op('i2s', 'conv')
        if k:
            op('CAT', 'cat_op')
    callHelper('$halt')


def dispatch(dyns, action, chosen=()):
    """Доступ до елемента масиву за індексом, відомим лише під час виконання. PSM не має масивів, тому
    елементи - окремі змінні, а потрібна обирається порівняннями індексу з 0, 1, ... та переходами JF.
    dyns - [(змінна з індексом, довжина масиву, рядок програми)]; action(індекси) генерує дію з елементом."""
    if not dyns:
        action(chosen)
        return
    (ref, n, line), rest = dyns[0], dyns[1:]
    end = createLabel()
    for k in range(n):
        nxt = createLabel()
        rval(ref)
        const(k, 'int')
        op('==', 'rel_op')
        jf(nxt)
        dispatch(rest, action, chosen + (k,))
        jmp(end)
        colon(nxt)
    halt(['runtime error RE-04 at {0}: index '.format(line), ref, ' out of range [0, {0}]'.format(n - 1)])
    colon(end)


class Line:
    """Рядок виведення, що збирається на стеку конкатенацією частин (OUT завжди друкує з нового рядка)."""
    def __init__(self):
        self.open = False                 # на стеку вже є початок рядка

    def piece(self):
        """Щойно згенеровано код чергової частини - рядка на вершині стека."""
        if self.open:
            op('CAT', 'cat_op')
        self.open = True

    def text(self, s, line=None):
        """Частина, відома під час трансляції; символ нового рядка завершує рядок виведення."""
        for k, part in enumerate(s.split('\n')):
            if k:
                self.newline()
            if part:
                string(part, line)
                self.piece()

    def newline(self):
        if not self.open:
            string('', None)
        op('OUT', 'out_op')
        self.open = False


def clearOut():
    """$out = "", $pend = false."""
    lval(OUT)
    const('', 'string')
    assign(OUT)
    lval(PEND)
    const(False, 'bool')
    assign(PEND)


def flush(clear=True):
    """Вивести накопичений у $out текст, якщо він є (перед введенням, помилкою та завершенням програми)."""
    m = createLabel()
    rval(PEND)
    jf(m)
    rval(OUT)
    op('OUT', 'out_op')
    if clear:
        clearOut()
    colon(m)


def buildHelpers():
    """Модулі допоміжних функцій, використаних програмою."""
    done = 0
    while done < len(helpers):
        name = helpers[done]
        done += 1
        newModule(prog + '$' + name, 'func')
        if name == '$b2s':                    # текстове подання логічного значення: true або false
            b = ('b', 'bool', False)
            declVar('b', 'bool')
            m1 = createLabel()
            rval(b)
            jf(m1)
            const('true', 'string')
            op('RET', 'RET')
            colon(m1)
            const('false', 'string')
            op('RET', 'RET')
        elif name == '$imod':                 # остача a - (a / b) * b: знак остачі збігається зі знаком a
            a, b = ('a', 'int', False), ('b', 'int', False)
            declVar('a', 'int')
            declVar('b', 'int')
            rval(a)
            rval(a)
            rval(b)
            op('/', 'math_op')
            op('f2i', 'conv')
            rval(b)
            op('*', 'math_op')
            op('-', 'math_op')
            op('RET', 'RET')
        elif name == '$ipow':                 # цілий степінь a ** n множенням у циклі; n < 0 - помилка RE-05
            a, n, line, r = [(v, 'int', False) for v in ('a', 'n', 'line', 'r')]
            for v in (a, n, line, r):
                declVar(v[0], 'int')
            m1, m2, m3 = createLabel(), createLabel(), createLabel()
            rval(n)
            const(0, 'int')
            op('<', 'rel_op')
            jf(m1)
            halt(['runtime error RE-05 at ', line, ': negative exponent ', n])
            colon(m1)
            lval(r)
            const(1, 'int')
            assign(r)
            colon(m2)
            rval(n)
            const(0, 'int')
            op('>', 'rel_op')
            jf(m3)
            lval(r)
            rval(r)
            rval(a)
            op('*', 'math_op')
            assign(r)
            lval(n)
            rval(n)
            const(1, 'int')
            op('-', 'math_op')
            assign(n)
            jmp(m2)
            colon(m3)
            rval(r)
            op('RET', 'RET')
        elif name == '$halt':                 # повідомлення про помилку часу виконання і зупинка машини
            msg = ('msg', 'string', False)
            declVar('msg', 'string')
            if buffered:
                flush(False)
            rval(msg)
            op('OUT', 'out_op')
            # PSM не має інструкції зупинки: перехід на неоголошену мітку аварійно завершує її роботу
            label(('runtime_error', 'label'))
            op('JMP', 'jump')


def checkModule(m):
    """Самоперевірка: таблиця міток, яку вів транслятор, збігається з обчисленою за готовим кодом,
    а кожна мітка, на яку є перехід, визначена."""
    defined = {}
    for i, (lex, tok) in enumerate(m.code):
        if tok == 'label' and i + 1 < len(m.code) and m.code[i + 1][1] == 'colon':
            assert lex not in defined, 'мітку {0} модуля {1} визначено двічі'.format(lex, m.name)
            defined[lex] = i
    assert defined == m.labels, 'таблиця міток модуля {0}: {1} != {2}'.format(m.name, m.labels, defined)
    for lex, tok in m.code:
        assert tok != 'label' or lex in defined or lex == 'runtime_error', 'невизначена мітка {0}'.format(lex)


# ---------------------------------------------------------------- результат трансляції
def moduleText(m):
    """Текст .postfix-файлу модуля (специфікація PSM, розд. 1.8)."""
    def section(name, rows):
        return ['.{0}('.format(name)] + ['\t' + row for row in rows] + [')', ''] if rows else []

    def pad(lex):
        return lex + ' ' * max(1, 12 - len(lex))

    lines = ['.target: Postfix Machine', '.version: ' + VERSION, '']
    lines += section('vars', [pad(name) + ptype for name, ptype in m.vars.items()])
    lines += section('globVarList', m.globals)
    lines += section('funcs', ['{0}{1:<7} {2}'.format(pad(name), rtype, arity)
                               for name, (rtype, arity) in m.funcs.items()])
    lines += section('labels', [pad(lex) + str(val) for lex, val in m.labels.items()])
    lines += ['.code('] + ['\t' + ('RET' if tok == 'RET' else pad(lex) + tok) for lex, tok in m.code] + [')']
    return '\n'.join(lines) + '\n'


def savePostfixCode(directory):
    """Зберегти модулі програми у файлах <модуль>.postfix теки directory; повертає імена файлів."""
    os.makedirs(directory, exist_ok=True)
    files = []
    for m in modules.values():
        files.append(m.name + '.postfix')
        with open(os.path.join(directory, files[-1]), 'w', encoding='utf-8', newline='\n') as f:
            f.write(moduleText(m))
    return files


def serv():
    """Вивести таблиці та постфіксний код кожного модуля (як функція serv() у методичці)."""
    for m in modules.values():
        print('\nМодуль {0}'.format(m.name))
        if m.labels:
            print('Таблиця міток\n  {0:<8} {1}'.format('Label', 'Value'))
            for lex, val in m.labels.items():
                print('  {0:<8} {1}'.format(lex, val))
        if m.vars:
            print('Таблиця ідентифікаторів\n  {0:<6}{1:<14}{2}'.format('Index', 'Ident', 'Type'))
            for i, (name, ptype) in enumerate(m.vars.items()):
                print('  {0:<6}{1:<14}{2}'.format(i, name, ptype))
        if m.globals:
            print('Глобальні змінні: ' + ', '.join(m.globals))
        if m.funcs:
            print('Функції: ' + ', '.join('{0} {1} {2}'.format(n, t, a) for n, (t, a) in m.funcs.items()))
        print('Код програми у постфіксній формі (ПОЛІЗ):\n  {0:<5}{1}'.format('№', 'postfixCode'))
        for i, instr in enumerate(m.code):
            print('  {0:<5}{1}'.format(i, instr))
