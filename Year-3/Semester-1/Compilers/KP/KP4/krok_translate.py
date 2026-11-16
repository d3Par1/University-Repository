# Транслятор мови Krok у постфіксний код (ПОЛІЗ) для стекової машини PSM версії 0.3
# За матеріалами до роботи КП 4 «Трансляція у ПОЛІЗ та виконання програм»: синтаксичний і семантичний аналізатор
# з КП 3 (рекурсивний спуск) доповнено компонентами перекладу. Функції розбору виразів додають до ПОЛІЗу коди
# операндів, а після них - оператор; функції розбору інструкцій - інструкції з мітками та переходами JF і JMP.
# Місця, де генерується код, позначено коментарем «Трансляція». Сам код формують функції модуля krok_codegen
# (postfixCodeGen, createLabel, setValLabel); результат - файли <програма>.postfix і <програма>$<функція>.postfix.
# Запуск:  python krok_translate.py <файл.krok> [-o <тека>] [--quiet] [--view] [--trace]
#          python PSM.py -p <тека> -m <програма>
import contextlib
import io
import math
import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'KP1'))
import krok_lex
from krok_lex import tokenTable
import krok_codegen as gen

tableOfSymb = krok_lex.tableOfSymb    # { n_rec : (num_line, lexeme, token, idxIdConst) }
tableOfConst = krok_lex.tableOfConst  # { literal : (token, idxConst) }
numRow = 1                            # номер поточного запису таблиці розбору
len_tableOfSymb = 0                   # довжина таблиці розбору

TYPES = ('int', 'float', 'bool', 'string')
NUMERIC = ('int', 'float')
LITERALS = ('intnum', 'floatnum', 'strlit', 'boolval')
REL_OPS = ('==', '!=', '<', '<=', '>', '>=')
SPECIAL = ('void', 'type_error', 'undeclared_variable', 'func', 'type', 'struct', 'pending', 'resolving')
INT_MIN, INT_MAX = -2 ** 63, 2 ** 63 - 1

# ---------------------------------------------------------------- таблиця імен і стан аналізу
# tabName = { контекст : {'declIn': охоплюючий контекст, 'level': рівень блока, 'blocks': лічильник вкладених
#                         блоків, 'names': { ім'я : (номер у блоці, клас, тип, значення, параметри) }} }
# Класи: var, param, const, type, func. Значення змінної - 'undefined' або 'assigned' (розд. 2.3 методички),
# константи - її значення. Параметри: функції - кортеж типів параметрів, структури - словник полів.
tabName = {}
topRow = {}           # { ім'я блока пакета : номер запису таблиці розбору, з якого починається оголошення }
cxt = 'univ'          # поточний контекст
passNo = 1            # прохід 1 - оголошення блока пакета, прохід 2 - тіла функцій
curDeclRow = 0        # запис, з якого починається оголошення блока пакета, що зараз опрацьовується
currentFunc = None    # (ім'я, тип результату) функції, тіло якої аналізується
fmtImported = False
warnings = []
lastError = None      # код останньої помилки: CE-01 (лексична), CE-02 (синтаксична), CE-03...CE-17
trace = False         # друкувати дерево викликів функцій розбору (як у КП 2)
varName = {}          # (контекст, ім'я змінної мови Krok) -> базове ім'я змінних PSM
funcParams = {}       # ім'я функції -> [(ім'я параметра, тип)]
ptrParams = []        # параметри-вказівники поточної функції: [(ім'я, базове ім'я змінних PSM, тип значення)]
addrTaken = set()     # імена змінних, адреси яких беруться в поточній функції
globalNames = set()   # імена глобальних змінних програми
assigned = set()      # імена змінних, яким у поточній функції щось присвоюється
strConst = {}         # (контекст, ім'я) -> значення рядкової змінної, відоме транслятору
FSuccess = ('codeGeneration', False)   # ознака успішності трансляції

semCodes = {          # вид семантичної помилки : код помилки трансляції за специфікацією мови
    'undeclared_variable': 'CE-03', 'redeclared': 'CE-04', 'type_error': 'CE-05', 'conversion_error': 'CE-06',
    'not_lvalue': 'CE-07', 'call_error': 'CE-08', 'return_error': 'CE-09', 'const_error': 'CE-10',
    'array_error': 'CE-11', 'field_error': 'CE-12', 'program_error': 'CE-13', 'import_error': 'CE-14',
    'void_value': 'CE-15', 'global_init': 'CE-16', 'simple_stmt': 'CE-17'}


class Ty(str):
    """Тип виразу або інструкції - рядок ('int', '[3]float', 'Point', '*int', 'void', 'type_error') з атрибутами:
    val - значення константного виразу (None - не константа); lit - літерал (можливо, з унарним + чи -);
    lvalue - (контекст, ім'я) змінної, якій належить адресований вираз (None - не l-значення);
    call - результат виклику функції; addr - вираз &x; kind, name - клас та ім'я ідентифікатора-операнда;
    term - інструкція завершальна (закінчується return); empty - порожній список інструкцій.
    Для генерування коду: place - місце зберігання значення (змінна, елемент масиву, поле); ptr - місце, на яке
    посилається параметр-вказівник; start, end - межі коду виразу в ПОЛІЗі; loadPos, loadEnd - межі коду, що
    кладе значення місця на стек (його вилучають, якщо вираз виявився лівою частиною присвоювання)."""
    def __new__(cls, typeName, **attrs):
        obj = str.__new__(cls, typeName)
        obj.val = attrs.get('val')
        obj.lit = attrs.get('lit', False)
        obj.lvalue = attrs.get('lvalue')
        obj.call = attrs.get('call', False)
        obj.addr = attrs.get('addr', False)
        obj.kind = attrs.get('kind')
        obj.name = attrs.get('name')
        obj.term = attrs.get('term', False)
        obj.empty = attrs.get('empty', False)
        obj.place = attrs.get('place')
        obj.ptr = attrs.get('ptr')
        obj.start = attrs.get('start')
        obj.end = obj.loadPos = obj.loadEnd = None
        return obj


class Place:
    """Місце зберігання значення. PSM має лише скалярні змінні, тому масив і структура розгортаються в набір
    змінних з іменами на кшталт grid[1][2] чи seg.from.x, а місце - це базове ім'я і шлях до складової:
    ('f', поле), ('i', сталий індекс) або ('d', змінна з індексом, довжина масиву, рядок програми)."""
    def __init__(self, base, glob, typ, path=()):
        self.base, self.glob, self.typ, self.path = base, glob, str(typ), tuple(path)

    def child(self, comp, typ):
        return Place(self.base, self.glob, typ, self.path + (comp,))

    def dyn(self):
        """Індекси, значення яких відомі лише під час виконання."""
        return [comp[1:] for comp in self.path if comp[0] == 'd']

    def name(self, chosen=()):
        """Ім'я змінної PSM (або спільний початок імен складових) для обраних значень змінних індексів."""
        s, k = self.base, 0
        for comp in self.path:
            if comp[0] == 'f':
                s += '.' + comp[1]
            elif comp[0] == 'i':
                s += '[{0}]'.format(comp[1])
            else:
                s += '[{0}]'.format(chosen[k])
                k += 1
        return s


stepIndt = 2
indt = 0


def nextIndt():
    global indt
    indt += stepIndt
    return ' ' * indt


def predIndt():
    global indt
    indt -= stepIndt
    return ' ' * indt


def rule(f):
    """Функція правила граматики; у режимі --trace друкується її ім'я з відступом (як у КП 2)."""
    def wrapper(*args):
        show = trace and passNo == 2
        if show:
            print(nextIndt() + f.__name__ + '():')
        try:
            return f(*args)
        finally:
            if show:
                predIndt()
    wrapper.__name__ = f.__name__
    return wrapper


# ---------------------------------------------------------------- доступ до таблиці розбору (з КП 2)
def getSymb():
    if numRow > len_tableOfSymb:
        failParse('getSymb(): неочікуваний кінець програми', numRow)
    numLine, lexeme, token, _ = tableOfSymb[numRow]
    return numLine, lexeme, token


def peek():
    if numRow > len_tableOfSymb:
        return None, None
    return tableOfSymb[numRow][1], tableOfSymb[numRow][2]


def parseToken(lexeme, token):
    global numRow
    if numRow > len_tableOfSymb:
        failParse('неочікуваний кінець програми', (lexeme or token, token, numRow))
    numLine, lex, tok = getSymb()
    numRow += 1
    if tok == token and lexeme in (None, lex):
        if trace and passNo == 2:
            print(' ' * (indt + stepIndt) + 'parseToken: В рядку {0} токен {1}'.format(numLine, (lex, tok)))
        return True
    failParse('невідповідність токенів', (numLine, lex, tok, lexeme or token, token))


def expect(lexeme):
    return parseToken(lexeme, tokenTable[lexeme])


def failParse(str, tuple):
    global lastError
    lastError = 'CE-02'
    if str == 'неочікуваний кінець програми':
        (lexeme, token, numRow) = tuple
        print('Parser ERROR: \n\t Неочікуваний кінець програми - в таблиці символів (розбору) немає запису '
              'з номером {1}. \n\t Очікувалось - {0}'.format((lexeme, token), numRow))
        exit(1001)
    if str == 'getSymb(): неочікуваний кінець програми':
        numRow = tuple
        print('Parser ERROR: \n\t Неочікуваний кінець програми - в таблиці символів (розбору) немає запису '
              'з номером {0}. \n\t Останній запис - {1}'.format(numRow, tableOfSymb[numRow - 1]))
        exit(1002)
    (numLine, lex, tok, expected) = tuple[:4]
    if str == 'невідповідність токенів':
        print('Parser ERROR: \n\t В рядку {0} неочікуваний елемент ({1},{2}). \n\t Очікувався - ({3},{4}).'
              .format(numLine, lex, tok, expected, tuple[4]))
        exit(1)
    codes = {'невідповідність інструкцій': 2, 'невідповідність у виразі': 3,
             'невідповідність у типі': 4, 'невідповідність оголошень': 5}
    print('Parser ERROR: \n\t В рядку {0} неочікуваний елемент ({1},{2}). \n\t Очікувався - {3}.'
          .format(numLine, lex, tok, expected))
    exit(codes[str])


def failSem(kind, numLine, message):
    """Повідомити про семантичну помилку та аварійно завершити аналіз (код завершення 100 + номер CE)."""
    global lastError
    lastError = semCodes[kind]
    print('Semantic ERROR: \n\t В рядку {0} {1} ({2}).'.format(numLine, message, lastError))
    exit(100 + int(lastError[3:]))


def failTran(numLine, message):
    """Конструкція мови, яку неможливо виразити інструкціями цільової машини."""
    global lastError
    lastError = 'PSM'
    print('Translator ERROR: \n\t В рядку {0} {1} - стекова машина PSM 0.3 цього не підтримує.'
          .format(numLine, message))
    exit(200)


def warnSem(numLine, message):
    """Попередження: програма коректна, але, ймовірно, поводиться не так, як очікує програміст."""
    if (numLine, message) not in warnings:
        warnings.append((numLine, message))
        print('Semantic WARNING: в рядку {0} {1}'.format(numLine, message))


def srcText(r1, r2):
    """Текст програми, що відповідає записам r1..r2-1 таблиці розбору (для повідомлень про помилки)."""
    s = ' '.join(tableOfSymb[r][1] for r in range(r1, r2))
    s = re.sub(r' ([)\],])', r'\1', s)
    s = re.sub(r'([(\[&]) ', r'\1', s)
    s = re.sub(r'(\w) ([(\[])', r'\1\2', s)
    return s.replace(' . ', '.')


def startsExpression(lex, tok):
    return tok in LITERALS or tok == 'id' or lex in ('-', '+', '!', '*', '(', '[', 'len') + TYPES


def startsType(lex, tok):
    return lex in TYPES or tok == 'id' or lex == '['


def startsStatement(lex, tok):
    return lex in ('var', 'const', 'if', 'for', 'return', 'fmt', '{') or startsExpression(lex, tok)


# ---------------------------------------------------------------- робота з таблицею імен
def openContext(name=None):
    """Вхід у новий блок: контекст функції (ім'я функції) або вкладений блок (<охоплюючий>.<номер>)."""
    global cxt
    if name is None:
        tabName[cxt]['blocks'] += 1
        name = '{0}.{1}'.format(cxt, tabName[cxt]['blocks'])
    tabName[name] = {'declIn': cxt, 'level': tabName[cxt]['level'] + 1, 'blocks': 0, 'names': {}}
    cxt = name


def closeContext():
    global cxt
    cxt = tabName[cxt]['declIn']


def insertName(c, name, numLine, kind, typ, val, extra='-'):
    """Додати ім'я до контексту c з контролем унікальності оголошення (insert з forTabNames.py)."""
    names = tabName[c]['names']
    if name in names:
        failSem('redeclared', numLine, 'повторне оголошення {0} в {1}'.format(name, c))
    names[name] = (len(names), kind, str(typ), val, extra)


def findName(name, c, contexts=None):
    """Пошук імені від контексту c до блока пакета (findName з forTabNames.py).
    Повертає (контекст, атрибути, переглянуті контексти); якщо ім'я не знайдено - контекст None."""
    contexts = [] if contexts is None else contexts
    contexts.append(c)
    attr = tabName[c]['names'].get(name)
    if attr is not None:
        return c, attr, contexts
    if c == 'univ':
        return None, None, contexts
    return findName(name, tabName[c]['declIn'], contexts)


def getTypeVar(name):
    """Тип імені або 'undeclared_variable', якщо його не оголошено (розд. 2.2 методички)."""
    c, attr, _ = findName(name, cxt)
    return 'undeclared_variable' if c is None else attr[2]


def isInitVar(name, c):
    """Чи набула змінна значення (розд. 2.3 методички)."""
    try:
        return tabName[c]['names'][name][3] == 'assigned'
    except KeyError:
        return 'undeclared_variable'


def setAssigned(lvalue):
    """Зафіксувати факт ініціації змінної: 'assigned' замість 'undefined' (присвоювання, введення, &x)."""
    if lvalue is not None:
        c, name = lvalue
        idx, kind, typ, val, extra = tabName[c]['names'][name]
        if val == 'undefined':
            tabName[c]['names'][name] = (idx, kind, typ, 'assigned', extra)


def registerTopNames():
    """Попередній перегляд таблиці розбору: зареєструвати імена блока пакета в порядку оголошення.
    Типи та функції видимі в усьому файлі незалежно від порядку оголошень (специфікація, «Області видимості»)."""
    depth = 0
    names = tabName['univ']['names']
    for row in range(1, len_tableOfSymb + 1):
        lex = tableOfSymb[row][1]
        if lex == '{':
            depth += 1
        elif lex == '}':
            depth -= 1
        elif depth == 0 and lex in ('var', 'const', 'type', 'func'):
            r = row + 1
            while r <= len_tableOfSymb and tableOfSymb[r][2] == 'id':
                name = tableOfSymb[r][1]
                if name not in topRow:
                    topRow[name] = row
                    names[name] = (len(names), lex, 'pending', '-', '-')
                if lex == 'var' and r + 1 <= len_tableOfSymb and tableOfSymb[r + 1][1] == ',':
                    r += 2
                else:
                    break


def declare(name, numLine, kind, typ, val, extra='-'):
    """Оголосити ім'я в поточному контексті; для блока пакета - заповнити попередньо зареєстрований запис."""
    if cxt != 'univ':
        insertName(cxt, name, numLine, kind, typ, val, extra)
        return
    names = tabName['univ']['names']
    if topRow.get(name) != curDeclRow:
        failSem('redeclared', numLine, 'повторне оголошення {0} в univ (вперше - в рядку {1})'
                .format(name, tableOfSymb[topRow[name]][0]))
    if names[name][2] in ('pending', 'resolving'):
        names[name] = (names[name][0], kind, str(typ), val, extra)


def uniqueName(name):
    """Ім'я змінної PSM для змінної name мови Krok. Імена змінних модуля PSM не повторюються, тому змінна
    вкладеного блока, що затінює іншу (локальну чи глобальну), отримує суфікс: x#2, x#3, ..."""
    cand, k = name, 1
    while cand in gen.cur.names or cand in globalNames:
        k += 1
        cand = '{0}#{1}'.format(name, k)
    gen.cur.names.add(cand)
    return cand


def declareVar(name, numLine, typ, val, init=None):
    """Оголосити змінну: запис у таблиці імен і змінні PSM для кожної скалярної складової її значення.
    init - значення константного ініціалізатора: якщо рядковій локальній змінній далі нічого не присвоюється,
    транслятор знає її значення (PSM не може обчислити довжину рядка, тож len(s) обчислюється тут)."""
    declare(name, numLine, 'var', typ, val)
    glob = cxt == 'univ'
    if init is not None and not glob and underlying(typ) == 'string' and name not in assigned | addrTaken:
        strConst[(cxt, name)] = init
    base = name if glob else uniqueName(name)
    varName[(cxt, name)] = base
    declLeaves(base, typ, glob)
    return Place(base, glob, typ)


def startTopDecl(name):
    """Позначити оголошення блока пакета як таке, що опрацьовується (виявлення циклічних визначень)."""
    names = tabName['univ']['names']
    if cxt == 'univ' and passNo == 1 and topRow.get(name) == curDeclRow and names[name][2] == 'pending':
        a = names[name]
        names[name] = (a[0], a[1], 'resolving', a[3], a[4])


def resolveTop(name):
    """Опрацювати оголошення блока пакета, що стоїть нижче, на вимогу (тип чи функцію використано раніше)."""
    global numRow, cxt, curDeclRow
    saved = numRow, cxt, curDeclRow
    numRow, cxt = topRow[name], 'univ'
    parseTopLevelDecl()
    numRow, cxt, curDeclRow = saved


def topAttr(name):
    """Атрибути імені блока пакета (з опрацюванням оголошення на вимогу)."""
    attr = tabName['univ']['names'][name]
    if attr[2] == 'pending':
        resolveTop(name)
        attr = tabName['univ']['names'][name]
    return attr


# ---------------------------------------------------------------- типи
def isNamed(t):
    """Чи є t ім'ям користувацького типу."""
    if not t or t in TYPES or t in SPECIAL or t[0] in '[*':
        return False
    attr = tabName['univ']['names'].get(t)
    return attr is not None and attr[1] == 'type'


def underlying(t):
    """Базовий тип: для іменованого типу - тип, яким його визначено ('float', '[3]int', 'struct')."""
    if not isNamed(t):
        return str(t)
    attr = topAttr(t)
    if attr[2] == 'resolving':
        failSem('field_error', tableOfSymb[topRow[t]][0], 'рекурсивне визначення типу {0}'.format(t))
    return attr[2]


def isScalar(t):
    return underlying(t) in TYPES


def elemType(arrType):
    u = underlying(arrType)
    return u[u.index(']') + 1:]


def arrayLen(arrType):
    u = underlying(arrType)
    return int(u[1:u.index(']')])


def fieldsOf(t):
    underlying(t)
    return topAttr(t)[4]


def literalFits(v, t):
    """Літерал v (можливо, зі знаком) неявно набуває іменованого типу t (специфікація, «Користувацькі скалярні типи»)."""
    if not v.lit:
        return False
    u = underlying(t)
    return v == u or (v == 'int' and u == 'float')


def assignable(v, t):
    """Сумісність за присвоєнням значення типу v з типом t (специфікація, «Еквівалентність та сумісність типів»)."""
    if v == t:                                    # 1) типи ідентичні
        return True
    if v == 'int' and t == 'float':               # 2) неявне розширення int -> float
        return True
    if isNamed(t) and literalFits(v, t):          # 3) літерал набуває іменованого типу
        return True
    u = underlying(t)                             # 4) неіменований масив -> іменований тип масиву
    return isNamed(t) and u.startswith('[') and str(v).startswith('[') and v == u


def baseOp(lU, op, rU):
    """Тип результату оператора для базових типів операндів (специфікація, табл. «Арифметичні оператори» тощо)."""
    arith = lU in NUMERIC and rU in NUMERIC
    if op in ('+', '-', '*', '/', '**'):
        if arith:
            return 'int' if lU == rU == 'int' else 'float'
        if op == '+' and lU == rU == 'string':
            return 'string'
    elif op == '%':
        if lU == rU == 'int':
            return 'int'
    elif op in ('<', '<=', '>', '>='):
        if arith or lU == rU == 'string':
            return 'bool'
    elif op in ('==', '!='):
        if arith or (lU == rU and lU in ('string', 'bool')):
            return 'bool'
    elif op in ('&&', '||'):
        if lU == rU == 'bool':
            return 'bool'
    return 'type_error'


def getTypeOp(lType, op, rType):
    """Тип результату застосування бінарного оператора op до операндів типів lType і rType
    (розд. 3.1.2 методички); для присвоювання '=' - 'void' або 'type_error' (розд. 3.2.1 методички)."""
    if op == '=':
        return 'void' if assignable(rType, lType) else 'type_error'
    named = None
    if isNamed(lType) or isNamed(rType):          # номінальна еквівалентність іменованих типів
        if lType == rType or (isNamed(lType) and literalFits(rType, lType)):
            named = lType
        elif isNamed(rType) and literalFits(lType, rType):
            named = rType
        else:
            return 'type_error'
        lU = rU = underlying(named)
    else:
        lU, rU = str(lType), str(rType)
    typeRes = baseOp(lU, op, rU)
    if typeRes != 'type_error' and named and op not in REL_OPS:
        typeRes = named                           # операція над іменованим типом дає той самий тип
    return typeRes


def operandBase(lType, rType):
    """Спільний базовий тип операндів для обчислення константного виразу."""
    lU, rU = underlying(lType), underlying(rType)
    if isNamed(lType) and not isNamed(rType):
        rU = lU
    elif isNamed(rType) and not isNamed(lType):
        lU = rU
    return lU if lU == rU else 'float'


def getTypeConst(literal):
    """Тип константи за її токеном у таблиці констант лексичного аналізатора (розд. 3.1.1 методички)."""
    return {'intnum': 'int', 'floatnum': 'float', 'strlit': 'string', 'boolval': 'bool'}[tableOfConst[literal][0]]


def strValue(lexeme):
    """Значення рядкового літерала (escape-послідовності \\n \\t \\r \\" \\\\)."""
    esc = {'n': '\n', 't': '\t', 'r': '\r', '"': '"', '\\': '\\'}
    body, res, i = lexeme[1:-1], [], 0
    while i < len(body):
        if body[i] == '\\':
            res.append(esc[body[i + 1]])
            i += 2
        else:
            res.append(body[i])
            i += 1
    return ''.join(res)


def convertVal(val, typ):
    if underlying(typ) == 'float' and type(val) is int:
        return float(val)
    return val


def idiv(a, b):
    """Цілочислове ділення з округленням частки в бік нуля."""
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b >= 0) else -q


def fold(op, l, r, base, numLine):
    """Значення константного виразу l op r; помилка обчислення - CE-10 (специфікація, «Константні вирази»)."""
    if op == '&&':
        return l and r
    if op == '||':
        return l or r
    if base == 'float':
        l, r = float(l), float(r)
    if op in REL_OPS:
        return {'==': l == r, '!=': l != r, '<': l < r, '<=': l <= r, '>': l > r, '>=': l >= r}[op]
    if base == 'string':
        return l + r
    if op in ('/', '%') and r == 0:
        failSem('const_error', numLine, 'ділення на нуль у константному виразі')
    if base == 'int':
        if op == '+':
            v = l + r
        elif op == '-':
            v = l - r
        elif op == '*':
            v = l * r
        elif op == '/':
            v = idiv(l, r)
        elif op == '%':
            v = l - idiv(l, r) * r
        else:
            if r < 0:
                failSem('const_error', numLine, "від'ємний показник степеня {0} у цілочисловому константному виразі"
                        .format(r))
            v = INT_MAX + 1 if abs(l) > 1 and r > 64 else l ** r
        if not INT_MIN <= v <= INT_MAX:
            failSem('const_error', numLine, 'переповнення int у константному виразі {0} {1} {2}'.format(l, op, r))
        return v
    try:
        v = {'+': lambda: l + r, '-': lambda: l - r, '*': lambda: l * r, '/': lambda: l / r,
             '**': lambda: math.pow(l, r)}[op]()
    except (OverflowError, ValueError):
        v = math.inf
    if math.isinf(v) or math.isnan(v):
        failSem('const_error', numLine, 'результат дійсної операції у константному виразі - нескінченність або NaN')
    return v


def useValue(t, numLine, read=True):
    """Вираз використовується як значення (r-value): це не виклик функції без результату (CE-15) і не ім'я
    функції чи типу; для змінної - перевірка факту ініціації (розд. 2.3 методички). За специфікацією Krok
    змінна без ініціалізатора має нульове значення свого типу, тому це попередження, а не помилка."""
    if t == 'void':
        failSem('void_value', numLine, 'виклик функції без результату використано як значення')
    if t.kind in ('func', 'type'):
        failSem('type_error', numLine, "ім'я {0} {1} не є значенням"
                .format('функції' if t.kind == 'func' else 'типу', t.name))
    if read and t.lvalue is not None:
        c, name = t.lvalue
        if c != 'univ' and not isInitVar(name, c):
            warnSem(numLine, 'змінна {0} використовується до присвоєння їй значення - буде використано '
                             'нульове значення типу {1}'.format(name, tabName[c]['names'][name][2]))


def ptype(t):
    """Тип PSM (int, float, bool, string) значення скалярного типу мови Krok."""
    return underlying(t)


def leaves(typ):
    """Скалярні складові значення типу typ: [(суфікс імені змінної, тип PSM)]."""
    u = underlying(typ)
    if u.startswith('['):
        parts = leaves(elemType(u))
        return [('[{0}]{1}'.format(k, s), pt) for k in range(arrayLen(u)) for s, pt in parts]
    if u == 'struct':
        return [('.{0}{1}'.format(f, s), pt) for f, ft in fieldsOf(typ).items() for s, pt in leaves(ft)]
    return [('', u)]


def declLeaves(base, typ, glob):
    for s, pt in leaves(typ):
        gen.declVar(base + s, pt, glob)


def selectLeaf(place, action):
    """Виконати action(ім'я) для місця place; якщо серед індексів є змінні - з вибором елемента переходами."""
    gen.dispatch(place.dyn(), lambda chosen: action(place.name(chosen)))


def loadPlace(place):
    """Покласти на стек значення місця: кожну його скалярну складову як r-val."""
    parts = leaves(place.typ)
    selectLeaf(place, lambda name: [gen.rval((name + s, pt, place.glob)) for s, pt in parts])


def storePlace(place):
    """Записати в місце place значення зі стека (його складові лежать у порядку оголошення)."""
    parts = leaves(place.typ)
    selectLeaf(place, lambda name: [gen.storeTop((name + s, pt, place.glob)) for s, pt in reversed(parts)])


def copyToPlace(place, srcs):
    """Скопіювати в місце place значення змінних srcs (по одній на кожну скалярну складову)."""
    parts = leaves(place.typ)
    selectLeaf(place, lambda name: [gen.copyVar((name + s, pt, place.glob), src)
                                    for (s, pt), src in zip(parts, srcs)])


def leafVar(place):
    """Змінна PSM, що відповідає скалярному місцю без змінних індексів."""
    return (place.name(), ptype(place.typ), place.glob)


def placeVars(place):
    """Змінні PSM усіх скалярних складових місця без змінних індексів."""
    return [(place.name() + s, pt, place.glob) for s, pt in leaves(place.typ)]


def loadValue(t):
    """ПОЛІЗ змінної - її ідентифікатор (r-val); для масиву чи структури - усі складові."""
    t.loadPos = gen.here()
    loadPlace(t.place if t.place is not None else t.ptr)
    t.loadEnd = gen.here()


def dropLoad(t):
    """Вираз потрібен як місце, а не як значення: вилучити щойно згенерований код завантаження."""
    if t.loadPos is not None:
        assert t.loadEnd == gen.here(), 'код завантаження не в кінці ПОЛІЗу'
        gen.cutCode(t.loadPos)
        t.loadPos = None


def placeOf(t):
    """Місце, яке позначає вираз t; значення-масив зі стека (літерал масиву) зберігається в тимчасових змінних."""
    if t.place is not None:
        dropLoad(t)
    elif underlying(t) == 'struct' or underlying(t).startswith('['):
        t.place = Place(gen.newBase(), False, t)
        declLeaves(t.place.base, t, False)
        storePlace(t.place)
    return t.place


def widen(e, end):
    """Неявне розширення int -> float значення виразу e; end - номер інструкції за його кодом.
    Для константи змінюється сам літерал, інакше додається інструкція i2f."""
    if e.val is not None:
        gen.postfixCode[end - 1] = (repr(float(e.val)), 'float')
    else:
        gen.insertCode(end, [('i2f', 'conv')])


def convert(e, dst):
    """Значення виразу e (його код - у кінці ПОЛІЗу) призначене місцю типу dst."""
    if isScalar(dst) and ptype(dst) != ptype(e):
        widen(e, gen.here())


def slotVars(prefix, typ):
    """Глобальні змінні, через які функція передає викликачеві значення типу typ."""
    slots = [(prefix + s, pt, True) for s, pt in leaves(typ)]
    for name, pt, _ in slots:
        gen.declVar(name, pt, True)
    return slots


def copyOut():
    """Перед поверненням з функції значення, на які посилаються параметри-вказівники, записуються в глобальні
    змінні $функція.параметр; викликач після CALL копіює їх у свої змінні (PSM передає лише значення)."""
    for pname, base, typ in ptrParams:
        for (s, pt), slot in zip(leaves(typ), slotVars('${0}.{1}'.format(currentFunc[0], pname), typ)):
            gen.copyVar(slot, (base + s, pt, False))


def discard(t):
    """Результат виклику, використаного як інструкція, відкидається."""
    if t.loadPos is not None:
        dropLoad(t)
    elif t != 'void' and t.place is None:
        gen.op('POP', 'stack_op')


def hasCall(code):
    """Чи містить код виклик функції програми (він може змінити значення змінних)."""
    return any(tok == 'CALL' and not lex.startswith('$') for lex, tok in code)


def constText(val):
    """Текстове подання значення, відомого під час трансляції (специфікація, «Інструкції введення та виведення»)."""
    if isinstance(val, bool):
        return 'true' if val else 'false'
    return repr(val) if isinstance(val, float) else str(val)


def toString(t):
    """Перетворити значення на вершині стека на рядок для виведення."""
    pt = ptype(t)
    if pt == 'int':
        gen.op('i2s', 'conv')
    elif pt == 'float':
        gen.op('f2s', 'conv')
    elif pt == 'bool':
        gen.callHelper('$b2s')            # PSM друкує логічні значення як True/False


def zeroInit(place):
    """Змінна без ініціалізатора отримує нульове значення свого типу (PSM не дозволяє читати змінну без значення)."""
    for v in placeVars(place):
        gen.lval(v)
        gen.zero(v[1])
        gen.assign(v)


def initVar(place, e):
    """Ініціалізація нової змінної значенням виразу e, код якого - інструкції e.start .. e.end-1.
    Скаляр: <змінна> l-val ПОЛІЗ(e) = ; масив чи структура: складові значення вже на стеку."""
    if isScalar(place.typ):
        tail = [('=', 'assign_op')]
        if ptype(place.typ) != ptype(e):
            if e.val is not None:
                gen.postfixCode[e.end - 1] = (repr(float(e.val)), 'float')
            else:
                tail.insert(0, ('i2f', 'conv'))
        gen.insertCode(e.end, tail)
        gen.insertCode(e.start, [(place.name(), 'l-val')])
    elif e.loadPos is not None and e.loadEnd == gen.here() and not e.place.dyn():
        dropLoad(e)
        copyToPlace(place, placeVars(e.place))
    else:
        storePlace(place)


def assignTo(place, e, direct):
    """Присвоєння значення виразу e (його код щойно згенеровано) місцю place."""
    if isScalar(place.typ):
        convert(e, place.typ)
        if direct:
            gen.assign(leafVar(place))        # <змінна> l-val уже в ПОЛІЗі перед кодом виразу
        else:
            storePlace(place)                 # елемент зі змінним індексом
    elif e.loadPos is not None and not place.dyn() and not e.place.dyn():
        dropLoad(e)
        copyToPlace(place, placeVars(e.place))
    else:
        storePlace(place)


def addressTaken():
    """Імена змінних, адреси яких беруться (&x) у тілі функції, що починається з поточної лексеми '{'.
    Змінну, адреса якої не береться, виклик функції змінити не може. Заодно збираються імена змінних,
    яким щось присвоюється (x = ..., x++, x--): решта змінних зберігає значення свого ініціалізатора."""
    global assigned
    names, depth, row = set(), 0, numRow
    assigned = set()
    while row <= len_tableOfSymb:
        lex = tableOfSymb[row][1]
        depth += (lex == '{') - (lex == '}')
        if tableOfSymb[row][2] == 'id' and row < len_tableOfSymb and tableOfSymb[row + 1][1] in ('=', '++', '--'):
            assigned.add(lex)
        if lex == '&':
            r = row + 1
            while r <= len_tableOfSymb and tableOfSymb[r][1] in ('(', '*'):
                r += 1
            if r <= len_tableOfSymb and tableOfSymb[r][2] == 'id':
                names.add(tableOfSymb[r][1])
        row += 1
        if depth == 0:
            break
    return names


def binary(lType, op, rType, numLine, mid):
    """Перевірка типів бінарної операції (розд. 3.1.2 методички КП 3), обчислення константного виразу
    і трансляція: коди операндів уже в ПОЛІЗі (mid - номер першої інструкції правого операнда),
    оператор додається після них."""
    useValue(lType, numLine)
    useValue(rType, numLine)
    resType = getTypeOp(lType, op, rType)
    if resType == 'type_error':
        failSem('type_error', numLine, 'оператор {0} не застосовний до операндів типів {1} і {2}'
                .format(op, lType, rType))
    val = None
    if lType.val is not None and rType.val is not None:
        val = fold(op, lType.val, rType.val, operandBase(lType, rType), numLine)
    elif op in ('/', '%') and rType.val is not None and rType.val == 0:
        # п. 11 семантики прикладу з методички: правий операнд '/' не повинен дорівнювати нулю
        warnSem(numLine, 'ділення на нуль - під час виконання виникне помилка RE-01')
    res = Ty(resType, val=val, start=lType.start)
    if val is not None:                       # константний вираз обчислює транслятор
        gen.cutCode(lType.start)
        gen.const(val, ptype(resType), numLine)
        return res
    pl, pr, widened = ptype(lType), ptype(rType), False
    if pl != pr:                              # неявне розширення int -> float одного з операндів
        if pr == 'int':
            widen(rType, gen.here())
        else:
            widen(lType, mid)
            widened = True
        pl = 'float'
    if op in ('&&', '||'):
        logical(op, mid)
        return res
    if not widened and lType.loadPos is not None and hasCall(gen.postfixCode[mid:]):
        gen.insertCode(mid, gen.identity(pl))     # лівий операнд обчислюється до виклику функції в правому
    if op == '+' and pl == 'string':
        gen.op('CAT', 'cat_op')
    elif op in ('+', '-', '*'):
        gen.op(op, 'math_op')
    elif op == '/':
        gen.op('/', 'math_op')
        if pl == 'int':
            gen.op('f2i', 'conv')             # ділення PSM дає float; дробова частина відкидається
    elif op == '%':
        gen.callHelper('$imod')
    elif op == '**' and pl == 'int':
        gen.const(numLine, 'int')
        gen.callHelper('$ipow')
    elif op == '**':
        gen.op('^', 'pow_op')
    else:
        if pl == 'string':
            failTran(numLine, 'порівняння рядків оператором {0}'.format(op))
        gen.op(op, 'rel_op')
    return res


def logical(op, mid):
    """&& та ||. Якщо правий операнд не може спричинити помилку чи побічний ефект - інструкції AND і OR;
    інакше обчислення за скороченою схемою: правий операнд обходиться умовним переходом."""
    if gen.isSafe(gen.postfixCode[mid:]):
        gen.op('AND' if op == '&&' else 'OR', 'bool_op')
        return
    m1, m2 = gen.createLabel(), gen.createLabel()
    if op == '&&':                            # ПОЛІЗ(A) m1 JF ПОЛІЗ(B) m2 JMP m1 : false m2 :
        gen.insertCode(mid, [m1, ('JF', 'jf')])
        gen.jmp(m2)
        gen.colon(m1)
        gen.const(False, 'bool')
    else:                                     # ПОЛІЗ(A) m1 JF true m2 JMP m1 : ПОЛІЗ(B) m2 :
        gen.insertCode(mid, [m1, ('JF', 'jf'), ('true', 'bool'), m2, ('JMP', 'jump'), m1, (':', 'colon')])
    gen.colon(m2)


# ---------------------------------------------------------------- структура програми
# Program = PackageClause ';' { ImportDecl ';' } { TopLevelDecl ';' }
@rule
def parseProgram():
    parsePackageClause(); expect(';')
    while peek()[0] == 'import':
        parseImportDecl(); expect(';')
    while numRow <= len_tableOfSymb:
        parseTopLevelDecl(); expect(';')
    return Ty('void')


@rule
def parsePackageClause():                 # PackageClause = package Ident
    expect('package')
    numLine, name, _ = getSymb()
    parseToken(None, 'id')
    if passNo == 1 and name != 'main':
        failSem('program_error', numLine, "ім'я пакета {0} відмінне від main".format(name))


@rule
def parseImportDecl():                    # ImportDecl = import StringLit
    global fmtImported
    expect('import')
    numLine, path, _ = getSymb()
    parseToken(None, 'strlit')
    if passNo == 1:
        if path != '"fmt"':
            failSem('import_error', numLine, 'імпорт шляху {0} - доступний лише пакет "fmt"'.format(path))
        if fmtImported:
            failSem('import_error', numLine, 'повторний імпорт "fmt"')
        fmtImported = True


@rule
def parseTopLevelDecl():                  # TopLevelDecl = VarDecl | ConstDecl | TypeDecl | FuncDecl
    global curDeclRow
    numLine, lex, tok = getSymb()
    curDeclRow = numRow
    if passNo == 2 and lex in ('var', 'const', 'type'):
        skipDecl()                        # оголошення блока пакета опрацьовано в проході 1
    elif lex == 'var':
        parseVarDecl()
    elif lex == 'const':
        parseConstDecl()
    elif lex == 'type':
        parseTypeDecl()
    elif lex == 'func':
        parseFuncDecl()
    else:
        failParse('невідповідність оголошень', (numLine, lex, tok, 'var, const, type або func'))
    return Ty('void')


def skipDecl():
    """Пропустити оголошення до роздільника ';' на верхньому рівні вкладеності дужок."""
    global numRow
    depth = 0
    while not (depth == 0 and tableOfSymb[numRow][1] == ';'):
        depth += (tableOfSymb[numRow][1] == '{') - (tableOfSymb[numRow][1] == '}')
        numRow += 1


def skipBlock():
    """Пропустити тіло функції (прохід 1): від '{' до парної їй '}'."""
    global numRow
    expect('{')
    depth = 1
    while depth:
        lex = getSymb()[1]
        numRow += 1
        depth += (lex == '{') - (lex == '}')


def checkProgram():
    """Перевірки після проходу 1: функція main (CE-13) і рекурсивне включення структур (CE-12)."""
    names = tabName['univ']['names']
    main = names.get('main')
    if main is None or main[1] != 'func':
        failSem('program_error', tableOfSymb[len_tableOfSymb][0], 'відсутня функція main')
    if main[4] or main[2] != 'void':
        failSem('program_error', tableOfSymb[topRow['main']][0], 'функція main повинна бути без параметрів і без результату')

    def includes(t, target, seen):
        if t == target:
            return True
        u = underlying(t)
        if u.startswith('['):
            return includes(elemType(u), target, seen)
        if u == 'struct' and t not in seen:
            seen.add(t)
            return any(includes(ft, target, seen) for ft in fieldsOf(t).values())
        return False

    for name, attr in names.items():
        if attr[1] == 'type' and attr[2] == 'struct' and any(includes(ft, name, set()) for ft in attr[4].values()):
            failSem('field_error', tableOfSymb[topRow[name]][0], 'структура {0} рекурсивно включає сама себе'.format(name))


# ---------------------------------------------------------------- оголошення
@rule
def parseVarDecl():     # VarDecl = var IdentList ( Type [ '=' ExpressionList ] | '=' ExpressionList )
    numLine = getSymb()[0]
    expect('var')
    names = parseIdentList()
    declType, exprs = None, []
    if peek()[0] == '=':
        expect('='); exprs = parseExpressionList()
    else:
        declType = parseType()
        if peek()[0] == '=':
            expect('='); exprs = parseExpressionList()
    checkDistinct(names)
    if exprs and len(exprs) != len(names):
        failSem('type_error', numLine, 'кількість виразів ({0}) не дорівнює кількості імен ({1})'
                .format(len(exprs), len(names)))
    places = []
    for i, (name, line) in enumerate(names):
        if not exprs:
            zeroInit(declareVar(name, line, declType, 'undefined'))
            continue
        e = exprs[i]
        useValue(e, line)
        if cxt == 'univ' and e.val is None:
            failSem('global_init', line, 'ініціалізатор глобальної змінної {0} не є константним виразом'.format(name))
        if declType is not None and not assignable(e, declType):
            failSem('type_error', line, 'змінній {0} типу {1} не можна присвоїти значення типу {2}'
                    .format(name, declType, e))
        typ = declType if declType is not None else str(e)
        if typ.startswith('*'):
            failSem('type_error', line, 'вказівник не можна зберегти в змінній {0}'.format(name))
        places.append(declareVar(name, line, typ, 'assigned', e.val))
    # Трансляція: <змінна> l-val ПОЛІЗ(вираз) = ; з кінця списку, щоб вставки не зсували код попередніх виразів
    for place, e in reversed(list(zip(places, exprs))):
        initVar(place, e)
    return Ty('void')


def checkDistinct(names):
    seen = set()
    for name, line in names:
        if name in seen:
            failSem('redeclared', line, 'повторне оголошення {0} в {1}'.format(name, cxt))
        seen.add(name)


@rule
def parseConstDecl():                     # ConstDecl = const Ident [ Type ] '=' Expression
    expect('const')
    numLine, name, _ = getSymb()
    parseToken(None, 'id')
    startTopDecl(name)
    declType = None
    if peek()[0] != '=':
        declType = parseType()
        if not isScalar(declType):
            failSem('type_error', numLine, 'константа {0} може мати лише скалярний тип, а не {1}'.format(name, declType))
    expect('=')
    pos = gen.here()
    e = parseExpression()
    gen.cutCode(pos)                      # Трансляція: значення константи підставляє транслятор, коду немає
    useValue(e, numLine)
    if e.val is None:
        failSem('const_error', numLine, 'ініціалізатор константи {0} не є константним виразом'.format(name))
    if declType is not None:
        if not assignable(e, declType):
            failSem('type_error', numLine, 'константі {0} типу {1} не можна надати значення типу {2}'
                    .format(name, declType, e))
        typ = declType
    else:
        typ = str(e)
        if not isScalar(typ):
            failSem('type_error', numLine, 'константа {0} може мати лише скалярний тип, а не {1}'.format(name, typ))
    declare(name, numLine, 'const', typ, convertVal(e.val, typ))
    return Ty('void')


@rule
def parseTypeDecl():                      # TypeDecl = type Ident ( Type | StructType )
    expect('type')
    numLine, name, _ = getSymb()
    parseToken(None, 'id')
    startTopDecl(name)
    if peek()[0] == 'struct':
        under, extra = 'struct', parseStructType()
    else:
        typ = parseType()
        under = underlying(typ)
        extra = dict(fieldsOf(typ)) if under == 'struct' else '-'
    declare(name, numLine, 'type', under, '-', extra)
    return Ty('void')


@rule
def parseStructType():                    # StructType = struct '{' FieldList '}'
    expect('struct'); expect('{')
    fields = parseFieldList()
    expect('}')
    result = {}
    for name, line, typ in fields:
        if name in result:
            failSem('field_error', line, 'повторне поле {0} структури'.format(name))
        result[name] = typ
    return result


@rule
def parseFieldList():                     # FieldList = [ FieldDecl FieldTail ]
    if peek()[1] == 'id':
        return parseFieldDecl() + parseFieldTail()
    return []


@rule
def parseFieldTail():                     # FieldTail = [ ';' FieldList ]
    if peek()[0] == ';':
        expect(';')
        return parseFieldList()
    return []


@rule
def parseFieldDecl():                     # FieldDecl = IdentList Type
    names = parseIdentList()
    typ = parseType()
    return [(name, line, typ) for name, line in names]


@rule
def parseIdentList():                     # IdentList = Ident { ',' Ident }
    numLine, name, _ = getSymb()
    parseToken(None, 'id')
    names = [(name, numLine)]
    while peek()[0] == ',':
        expect(',')
        numLine, name, _ = getSymb()
        parseToken(None, 'id')
        names.append((name, numLine))
    return names


# ---------------------------------------------------------------- типи
@rule
def parseType():                          # Type = BasicType | TypeName | ArrayType
    numLine, lex, tok = getSymb()
    if lex in TYPES:
        return parseBasicType()
    if tok == 'id':                       # TypeName = Ident
        parseToken(None, 'id')
        c, attr, contexts = findName(lex, cxt)
        if c is None:
            failSem('undeclared_variable', numLine, 'тип {0} не оголошено'.format(lex))
        if attr[1] != 'type':
            failSem('type_error', numLine, "ім'я {0} не є іменем типу".format(lex))
        return lex
    if lex == '[':
        return parseArrayType()
    failParse('невідповідність у типі', (numLine, lex, tok, 'int, float, bool, string, ім\'я типу або ['))


@rule
def parseBasicType():                     # BasicType = int | float | bool | string
    numLine, lex, tok = getSymb()
    if lex not in TYPES:
        failParse('невідповідність у типі', (numLine, lex, tok, 'int, float, bool або string'))
    expect(lex)
    return lex


@rule
def parseArrayType():                     # ArrayType = '[' Expression ']' Type
    numLine = getSymb()[0]
    expect('[')
    pos = gen.here()
    n = parseExpression()
    gen.cutCode(pos)                      # Трансляція: довжина масиву - константа, коду не потребує
    useValue(n, numLine)
    expect(']')
    if n.val is None or underlying(n) != 'int':
        failSem('array_error', numLine, 'довжина масиву не є константою типу int')
    if n.val <= 0:
        failSem('array_error', numLine, 'довжина масиву {0} не є додатною'.format(n.val))
    return '[{0}]{1}'.format(n.val, parseType())


# ---------------------------------------------------------------- функції
@rule
def parseFuncDecl():                      # FuncDecl = func Ident '(' [ ParamList ] ')' [ Type ] Block
    global currentFunc, ptrParams, addrTaken
    expect('func')
    numLine, name, _ = getSymb()
    parseToken(None, 'id')
    startTopDecl(name)
    expect('(')
    params = parseParamList() if peek()[1] == 'id' else []
    expect(')')
    result = parseType() if startsType(*peek()) else 'void'
    if passNo == 1:                       # прохід 1: лише сигнатура функції
        funcParams[name] = [(pname, ptyp) for pname, _, ptyp in params]
        declare(name, numLine, 'func', result, '-', tuple(typ for _, _, typ in params))
        skipBlock()
        return Ty('void')
    gen.newModule(gen.prog + '$' + name, 'func')     # Трансляція: функція - окремий модуль PSM
    openContext(name)                     # блок функції: параметри та інструкції верхнього рівня тіла
    ptrParams = []
    for pname, pline, ptyp in params:
        insertName(cxt, pname, pline, 'param', ptyp, 'assigned')
        base = ('*' if ptyp.startswith('*') else '') + uniqueName(pname)
        varName[(cxt, pname)] = base
        declLeaves(base, ptyp.lstrip('*'), False)    # параметри - перші змінні секції .vars
        if ptyp.startswith('*'):
            ptrParams.append((pname, base, ptyp[1:]))
    addrTaken = addressTaken()
    currentFunc = (name, result)
    body = parseBlock(False)
    if result != 'void' and not body.term:
        failSem('return_error', numLine, 'функція {0} з результатом {1} не завершується інструкцією return '
                                         'на кожному шляху виконання'.format(name, result))
    if not body.term:                     # Трансляція: повернення в кінці тіла функції без результату
        copyOut()
        gen.op('RET', 'RET')
    closeContext()
    currentFunc = None
    gen.setModule(gen.root)
    return body


@rule
def parseParamList():                     # ParamList = ParamGroup { ',' ParamGroup }
    params = parseParamGroup()
    while peek()[0] == ',':
        expect(',')
        params += parseParamGroup()
    return params


@rule
def parseParamGroup():                    # ParamGroup = IdentList ParamType
    names = parseIdentList()
    typ = parseParamType()
    return [(name, line, typ) for name, line in names]


@rule
def parseParamType():                     # ParamType = [ '*' ] Type
    if peek()[0] == '*':
        expect('*')
        return '*' + parseType()
    return parseType()


# ---------------------------------------------------------------- інструкції
@rule
def parseBlock(newScope=True):            # Block = '{' StatementList '}'
    expect('{')
    if newScope:
        openContext()
    res = parseStatementList()
    if newScope:
        closeContext()
    expect('}')
    return res


@rule
def parseStatementList():                 # StatementList = [ Statement StatementTail ]
    lex, tok = peek()
    if lex is None or not startsStatement(lex, tok):
        return Ty('void', empty=True)
    s1 = parseStatement()
    s2 = parseStatementTail()
    # тип послідовності інструкцій Stmnt1; Stmnt2 (розд. 3.2.3 методички)
    if s1 == 'void' and s2 == 'void':
        return Ty('void', term=s1.term if s2.empty else s2.term)
    return Ty('type_error')


@rule
def parseStatementTail():                 # StatementTail = [ ';' StatementList ]
    if peek()[0] == ';':
        expect(';')
        return parseStatementList()
    return Ty('void', empty=True)


@rule
def parseStatement():
    # Statement = VarDecl | ConstDecl | IfStmt | ForStmt | ReturnStmt | IoStmt | Block | SimpleStmt
    numLine, lex, tok = getSymb()
    gen.resetTemps()
    if lex == 'var':
        return parseVarDecl()
    if lex == 'const':
        return parseConstDecl()
    if lex == 'if':
        return parseIfStmt()
    if lex == 'for':
        return parseForStmt()
    if lex == 'return':
        return parseReturnStmt()
    if lex == 'fmt':
        return parseIoStmt()
    if lex == '{':
        return parseBlock()
    if startsExpression(lex, tok):
        r0 = numRow
        form, t = parseSimpleStmt()
        if form == 'expr':
            if not t.call:
                failSem('simple_stmt', numLine,
                        'інструкція-вираз {0} не є викликом функції'.format(srcText(r0, numRow)))
            discard(t)                    # Трансляція: результат виклику-інструкції відкидається (POP)
        return Ty('void')
    failParse('невідповідність інструкцій', (numLine, lex, tok, 'інструкція'))


@rule
def parseSimpleStmt():                    # SimpleStmt = Expression [ AssignOp Expression | IncDecOp ]
    """Повертає (форма, тип): форма - 'define' (x := e), 'assign', 'incdec' або 'expr' (вираз)."""
    numLine, lex, tok = getSymb()
    # оголошення з ініціалізацією: ліва частина - нове ім'я, тому його не шукаємо в таблиці імен
    if tok == 'id' and numRow < len_tableOfSymb and tableOfSymb[numRow + 1][1] == ':=':
        parseToken(None, 'id'); expect(':=')
        e = parseExpression()
        e.end = gen.here()
        useValue(e, numLine)
        if str(e).startswith('*'):
            failSem('type_error', numLine, 'вказівник не можна зберегти в змінній {0}'.format(lex))
        initVar(declareVar(lex, numLine, str(e), 'assigned', e.val), e)   # Трансляція: x l-val ПОЛІЗ(e) =
        return 'define', e
    r0 = numRow
    left = parseExpression()
    leftText = srcText(r0, numRow)
    op = peek()[0]
    if op == ':=':
        failSem('simple_stmt', numLine, 'ліва частина := ({0}) не є ідентифікатором'.format(leftText))
    if op in ('=', '++', '--') and left.lvalue is None:
        what = {'const': ' (константа)', 'func': " (ім'я функції)", 'type': " (ім'я типу)"}.get(left.kind, '')
        if left.kind == 'param' and str(left).startswith('*'):
            what = ' (параметр-вказівник без розіменування)'
        failSem('not_lvalue', numLine, '{0}{1} не є l-значенням'.format(leftText, what))
    if op == '=':
        expect('=')
        # Трансляція: ліва частина - місце, а не значення; для змінної ПОЛІЗ - її ідентифікатор (l-val)
        dropLoad(left)
        direct = isScalar(left) and not left.place.dyn()
        if direct:
            gen.lval(leafVar(left.place))
        right = parseExpression()         # Трансляція: функція сама додасть ПОЛІЗ виразу
        useValue(right, numLine)
        # перевірка типів інструкції присвоювання (розд. 3.2.1 методички)
        if getTypeOp(left, '=', right) == 'type_error':
            failSem('type_error', numLine, '{0} типу {1} не можна присвоїти значення типу {2}'
                    .format(leftText, left, right))
        assignTo(left.place, right, direct)   # Трансляція: оператор '=' додається після своїх операндів
        setAssigned(left.lvalue)
        return 'assign', left
    if op in ('++', '--'):
        expect(op)
        if underlying(left) not in NUMERIC:
            failSem('type_error', numLine, 'оператор {0} не застосовний до значення типу {1}'.format(op, left))
        useValue(left, numLine)
        dropLoad(left)                    # Трансляція: v++ як v = v + 1
        pt, sign = ptype(left), '+' if op == '++' else '-'
        if not left.place.dyn():
            v = leafVar(left.place)
            gen.lval(v)
            gen.rval(v)
            gen.const(1, pt)
            gen.op(sign, 'math_op')
            gen.assign(v)
        else:
            loadPlace(left.place)
            gen.const(1, pt)
            gen.op(sign, 'math_op')
            storePlace(left.place)
        setAssigned(left.lvalue)
        return 'incdec', left
    return 'expr', left


@rule
def parseIfStmt():                        # IfStmt = if Expression Block [ else ( IfStmt | Block ) ]
    numLine = getSymb()[0]
    expect('if')                          # Трансляція: 'if' нічого не додає до ПОЛІЗу
    cond = parseExpression()
    useValue(cond, numLine)
    if underlying(cond) != 'bool':
        failSem('type_error', numLine, 'умова if має тип {0}, а не bool'.format(cond))
    m1 = gen.createLabel()
    gen.jf(m1)                            # Трансляція: m1 JF
    thenRes, elseRes = parseBlock(), None
    if peek()[0] == 'else':
        expect('else')
        m2 = gen.createLabel()
        gen.jmp(m2)                       # Трансляція: m2 JMP m1 :
        gen.colon(m1)
        elseRes = parseIfStmt() if peek()[0] == 'if' else parseBlock()
        gen.colon(m2)                     # Трансляція: m2 :
    else:
        gen.colon(m1)                     # Трансляція: гілки else немає - m1 :
    ok = thenRes == 'void' and (elseRes is None or elseRes == 'void')
    return Ty('void' if ok else 'type_error', term=elseRes is not None and thenRes.term and elseRes.term)


@rule
def parseForStmt():                       # ForStmt = for ForHeader Block
    numLine = getSymb()[0]
    expect('for')
    openContext()                         # неявний блок інструкції for
    condType, back, out = parseForHeader(numLine)
    body = parseBlock()
    gen.jmp(back)                         # Трансляція: після тіла - перехід до умови (або до кроку)
    gen.colon(out)                        # Трансляція: мітка виходу з циклу
    closeContext()
    # if type(BoolExpr) == 'bool' then type(Stmnt) else type_error (розд. 3.2.3 методички)
    return Ty(body if condType == 'bool' else 'type_error')


@rule
def parseForHeader(numLine):              # ForHeader = SimpleStmt [ ';' Expression ';' SimpleStmt ]
    """Повертає (тип умови, мітка для переходу після тіла, мітка виходу з циклу)."""
    r0, pos = numRow, gen.here()
    form, t = parseSimpleStmt()
    if peek()[0] != ';':                  # цикл з передумовою: for Expression Block
        if form != 'expr':
            failSem('type_error', numLine, 'умовою циклу for має бути вираз типу bool, а не інструкція {0}'
                    .format(srcText(r0, numRow)))
        useValue(t, numLine)
        if underlying(t) != 'bool':
            failSem('type_error', numLine, 'умова for має тип {0}, а не bool'.format(t))
        m1, m2 = gen.createLabel(), gen.createLabel()
        gen.insertCode(pos, [m1, (':', 'colon')])     # Трансляція: m1 : ПОЛІЗ(умова) m2 JF
        gen.jf(m2)
        return 'bool', m1, m2
    if form == 'expr':
        if not t.call:
            failSem('simple_stmt', numLine, 'ініціалізація for {0} не є інструкцією'.format(srcText(r0, numRow)))
        discard(t)
    # Трансляція трикомпонентного циклу: ПОЛІЗ(S1) m1 : ПОЛІЗ(E) m4 JF m3 JMP m2 : ПОЛІЗ(S2) m1 JMP m3 :
    m1, m2, m3, m4 = [gen.createLabel() for _ in range(4)]
    gen.colon(m1)
    expect(';')
    cond = parseExpression()
    useValue(cond, numLine)
    if underlying(cond) != 'bool':
        failSem('type_error', numLine, 'умова for має тип {0}, а не bool'.format(cond))
    gen.jf(m4)
    gen.jmp(m3)
    gen.colon(m2)
    expect(';')
    r0 = numRow
    form, t = parseSimpleStmt()
    if form == 'define':
        failSem('simple_stmt', numLine, 'оголошення := в інструкції наступного кроку for')
    if form == 'expr':
        if not t.call:
            failSem('simple_stmt', numLine,
                    'інструкція наступного кроку for {0} не є інструкцією'.format(srcText(r0, numRow)))
        discard(t)
    gen.jmp(m1)
    gen.colon(m3)
    return 'bool', m2, m4


@rule
def parseReturnStmt():                    # ReturnStmt = return [ Expression ]
    numLine = getSymb()[0]
    expect('return')
    name, result = currentFunc
    lex, tok = peek()
    if lex is not None and startsExpression(lex, tok):
        e = parseExpression()
        useValue(e, numLine)
        if result == 'void':
            failSem('return_error', numLine, 'return з виразом у функції {0} без результату'.format(name))
        if not assignable(e, result):
            failSem('type_error', numLine, 'значення типу {0} несумісне з типом результату {1} функції {2}'
                    .format(e, result, name))
        if isScalar(result):
            convert(e, result)            # Трансляція: значення результату лишається на стеку для RET
        else:                             # масив чи структура повертаються через глобальні змінні
            for slot in reversed(slotVars('${0}.ret'.format(name), result)):
                gen.storeTop(slot)
    elif result != 'void':
        failSem('return_error', numLine, 'return без виразу у функції {0} з результатом {1}'.format(name, result))
    copyOut()
    gen.op('RET', 'RET')                  # Трансляція: RET
    return Ty('void', term=True)


@rule
def parseIoStmt():                        # IoStmt = fmt '.' ( OutputCall | InputCall )
    numLine = getSymb()[0]
    expect('fmt')
    if not fmtImported:
        failSem('import_error', numLine, 'використання fmt без import "fmt"')
    expect('.')
    numLine, lex, tok = getSymb()
    if lex in ('Print', 'Println'):
        return parseOutputCall()
    if lex == 'Scan':
        return parseInputCall()
    failParse('невідповідність інструкцій', (numLine, lex, tok, 'Print, Println або Scan'))


@rule
def parseOutputCall():                    # OutputCall = ( 'Print' | 'Println' ) '(' [ ExpressionList ] ')'
    numLine, lex, _ = getSymb()
    expect(lex); expect('(')
    println, pos, line, i = lex == 'Println', gen.here(), gen.Line(), 0
    if gen.buffered:                      # програма містить fmt.Print: рядок виведення накопичується в $out
        if not println:
            gen.lval(gen.OUT)
        gen.rval(gen.OUT)
        line.open = True
    while i == 0 and peek()[0] != ')' or i > 0 and peek()[0] == ',':
        if i > 0:
            expect(',')
            if println:
                line.text(' ')            # Println розділяє значення одним пробілом
        i += 1
        apos = gen.here()
        a = parseExpression()             # parseWrite - 'void', якщо аргументи - значення скалярних типів
        useValue(a, numLine)
        if not isScalar(a):
            failSem('type_error', numLine, 'аргумент {0} fmt.{1} має тип {2}, а не скалярний'.format(i, lex, a))
        if a.val is not None:             # значення відоме під час трансляції: його текст підставляє транслятор
            gen.cutCode(apos)
            line.text(constText(a.val), numLine)
        else:
            toString(a)
            line.piece()
    expect(')')
    if println:
        line.newline()                    # Трансляція: OUT виводить зібраний рядок
        if gen.buffered:
            gen.clearOut()
    elif gen.here() == pos + 2:           # до $out нічого не додано
        gen.cutCode(pos)
    else:
        if not line.open:
            gen.const('', 'string')
        gen.assign(gen.OUT)
        gen.lval(gen.PEND)
        gen.const(line.open, 'bool')
        gen.assign(gen.PEND)
    return Ty('void')


@rule
def parseInputCall():                     # InputCall = 'Scan' '(' AddrExpr { ',' AddrExpr } ')'
    numLine = getSymb()[0]
    expect('Scan'); expect('(')
    if gen.buffered:
        gen.flush()                       # перед введенням накопичений текст виводиться
    i = 0
    while i == 0 or peek()[0] == ',':
        if i > 0:
            expect(',')
        i += 1
        a = parseAddrExpr()
        typ = str(a)[1:]
        if not isScalar(typ):
            failSem('type_error', numLine, 'аргумент {0} fmt.Scan - змінна типу {1}, а не скалярного'.format(i, typ))
        pt = ptype(typ)
        if pt == 'bool':
            failTran(numLine, 'введення значення типу bool (INP повертає рядок, а порівняння рядків немає)')
        dropLoad(a)                       # Трансляція: <змінна> l-val INP s2i = ; INP читає рядок
        direct = not a.place.dyn()
        if direct:
            gen.lval(leafVar(a.place))
        gen.op('INP', 'inp_op')
        if pt != 'string':
            gen.op('s2i' if pt == 'int' else 's2f', 'conv')
        if direct:
            gen.assign(leafVar(a.place))
        else:
            storePlace(a.place)
        setAssigned(a.lvalue)             # змінна набуває значення в інструкції введення
    expect(')')
    return Ty('void')


# ---------------------------------------------------------------- вирази
@rule
def parseExpressionList():                # ExpressionList = Expression { ',' Expression }
    exprs = [parseExpression()]
    exprs[-1].end = gen.here()
    while peek()[0] == ',':
        expect(',')
        exprs.append(parseExpression())
        exprs[-1].end = gen.here()
    return exprs


@rule
def parseExpression():                    # Expression = OrExpr
    return parseOrExpr()


@rule
def parseOrExpr():                        # OrExpr = AndExpr { '||' AndExpr }
    lType = parseAndExpr()
    while peek()[0] == '||':
        numLine = getSymb()[0]
        expect('||')
        mid = gen.here()
        lType = binary(lType, '||', parseAndExpr(), numLine, mid)
    return lType


@rule
def parseAndExpr():                       # AndExpr = RelExpr { '&&' RelExpr }
    lType = parseRelExpr()
    while peek()[0] == '&&':
        numLine = getSymb()[0]
        expect('&&')
        mid = gen.here()
        lType = binary(lType, '&&', parseRelExpr(), numLine, mid)
    return lType


@rule
def parseRelExpr():                       # RelExpr = AddExpr [ RelOp AddExpr ]
    lType = parseAddExpr()
    if peek()[1] == 'rel_op':
        numLine, op, _ = getSymb()
        expect(op)
        mid = gen.here()
        lType = binary(lType, op, parseAddExpr(), numLine, mid)
    return lType


@rule
def parseAddExpr():                       # AddExpr = MulExpr { AddOp MulExpr }
    lType = parseMulExpr()
    while peek()[1] == 'add_op':
        numLine, op, _ = getSymb()
        expect(op)
        mid = gen.here()
        lType = binary(lType, op, parseMulExpr(), numLine, mid)
    return lType


@rule
def parseMulExpr():                       # MulExpr = UnaryExpr { MulOp UnaryExpr }
    lType = parseUnaryExpr()
    while peek()[1] == 'mult_op':
        numLine, op, _ = getSymb()
        expect(op)
        mid = gen.here()
        lType = binary(lType, op, parseUnaryExpr(), numLine, mid)
    return lType


@rule
def parseUnaryExpr():                     # UnaryExpr = UnaryOp UnaryExpr | PowerExpr
    if peek()[0] in ('-', '+', '!'):
        numLine, op, _ = getSymb()
        expect(op)
        start = gen.here()
        t = parseUnaryExpr()              # Трансляція: спершу ПОЛІЗ операнда
        useValue(t, numLine)
        u = underlying(t)
        if (op == '!' and u != 'bool') or (op != '!' and u not in NUMERIC):
            failSem('type_error', numLine, 'унарний оператор {0} не застосовний до типу {1}'.format(op, t))
        val = t.val
        if val is not None and op == '!':
            val = not val
        elif val is not None and op == '-':
            val = -val
            if u == 'int' and not INT_MIN <= val <= INT_MAX:
                failSem('const_error', numLine, 'переповнення int у константному виразі -({0})'.format(t.val))
        if val is not None:               # константний вираз обчислює транслятор
            gen.cutCode(start)
            gen.const(val, u, numLine)
        elif op == '-':
            gen.op('NEG', 'math_op')      # Трансляція: унарний мінус - після свого операнда
        elif op == '!':
            gen.op('NOT', 'bool_op')
        return Ty(t, val=val, lit=t.lit and op != '!', start=start)
    return parsePowerExpr()


@rule
def parsePowerExpr():                     # PowerExpr = PrimaryExpr [ '**' UnaryExpr ]
    lType = parsePrimaryExpr()
    if peek()[0] == '**':
        numLine = getSymb()[0]
        expect('**')
        mid = gen.here()
        lType = binary(lType, '**', parseUnaryExpr(), numLine, mid)
    return lType


@rule
def parsePrimaryExpr():                   # PrimaryExpr = Operand { Selector | Index | Arguments }
    r0, start = numRow, gen.here()
    t = parseOperand()
    while peek()[0] in ('.', '[', '('):
        lex = peek()[0]
        if lex == '.':
            t = parseSelector(t)
        elif lex == '[':
            t = parseIndex(t)
        else:
            t = parseArguments(t, r0)
    if t.loadPos is None and (t.place is not None or t.ptr is not None):
        loadValue(t)                      # Трансляція: ПОЛІЗ змінної - її ідентифікатор (r-val)
    t.start = start
    return t


@rule
def parseSelector(t):                     # Selector = '.' Ident
    numLine = getSymb()[0]
    useValue(t, numLine, False)
    place = placeOf(t)
    expect('.')
    field = getSymb()[1]
    parseToken(None, 'id')
    if underlying(t) != 'struct':
        failSem('field_error', numLine, 'звернення до поля {0} значення типу {1}, що не є структурою'.format(field, t))
    fields = fieldsOf(t)
    if field not in fields:
        failSem('field_error', numLine, 'структура {0} не має поля {1}'.format(t, field))
    return Ty(fields[field], lvalue=t.lvalue, place=place.child(('f', field), fields[field]))


@rule
def parseIndex(t):                        # Index = '[' Expression ']'
    numLine = getSymb()[0]
    useValue(t, numLine, False)
    place = placeOf(t)
    expect('[')
    pos = gen.here()
    i = parseExpression()
    useValue(i, numLine)
    expect(']')
    if not underlying(t).startswith('['):
        failSem('type_error', numLine, 'індексування значення типу {0}, що не є масивом'.format(t))
    if underlying(i) != 'int':
        failSem('type_error', numLine, 'індекс має тип {0}, а не int'.format(i))
    n = arrayLen(t)
    if i.val is not None and not 0 <= i.val < n:
        failSem('array_error', numLine, 'константний індекс {0} поза межами [0, {1}]'.format(i.val, n - 1))
    if i.val is not None:                 # сталий індекс: елемент - конкретна змінна PSM
        gen.cutCode(pos)
        comp = ('i', i.val)
    elif (i.loadPos == pos and i.loadEnd == gen.here() and i.kind in ('var', 'param') and not i.place.path
          and not i.place.glob and i.name not in addrTaken):
        gen.cutCode(pos)                  # індекс - локальна змінна, яку до звернення ніщо не змінить
        comp = ('d', leafVar(i.place), n, numLine)
    else:                                 # значення індексу зберігається в тимчасовій змінній
        ref = gen.newTemp('int')
        gen.storeTop(ref)
        comp = ('d', ref, n, numLine)
    return Ty(elemType(t), lvalue=t.lvalue, place=place.child(comp, elemType(t)))


@rule
def parseArguments(t, r0):                # Arguments = '(' [ ArgumentList ] ')'
    numLine = getSymb()[0]
    callee = srcText(r0, numRow)
    expect('(')
    args = parseArgumentList() if peek()[0] != ')' else []
    expect(')')
    if t.kind == 'func':
        return callFunc(t.name, args, numLine)
    if t.kind == 'type':
        return conversion(t.name, args, numLine)
    failSem('call_error', numLine, 'виклик {0}, що не є функцією чи типом'.format(callee))


def callFunc(name, args, numLine):
    """Виклик функції: кількість і типи аргументів (CE-08); тип виклику - тип результату функції.
    Трансляція: значення аргументів уже на стеку в порядку параметрів, далі <функція> CALL."""
    idx, kind, result, val, params = topAttr(name)
    if len(args) != len(params):
        failSem('call_error', numLine, 'функція {0} очікує аргументів: {1}, отримано: {2}'
                .format(name, len(params), len(args)))
    for i, (a, p) in enumerate(zip(args, params), 1):
        if p.startswith('*'):             # параметр-вказівник: аргумент &x з типом x, ідентичним T
            if a != p:
                failSem('call_error', numLine, 'аргумент {0} функції {1} має бути адресою змінної типу {2} (&x), '
                                               'а не значенням типу {3}'.format(i, name, p[1:], a))
            setAssigned(a.lvalue)         # функція може змінити змінну, адресу якої передано
        else:
            if a.addr:
                failSem('call_error', numLine, 'аргумент {0} функції {1}: параметр типу {2} передається за '
                                               'значенням, адреса недопустима'.format(i, name, p))
            useValue(a, numLine)
            if not assignable(a, p):
                failSem('call_error', numLine, 'аргумент {0} функції {1} типу {2} несумісний з параметром типу {3}'
                        .format(i, name, a, p))
    later = [hasCall(gen.postfixCode[a.end:]) for a in args]
    outs = []
    for i in reversed(range(len(args))):  # з кінця, щоб вставки не зсували код попередніх аргументів
        a, p = args[i], params[i]
        if p.startswith('*'):
            outs.insert(0, (funcParams[name][i][0], a.place if a.addr else a.ptr, p[1:]))
        elif isScalar(p) and ptype(p) != ptype(a):
            widen(a, a.end)               # неявне розширення int -> float
        elif isScalar(p) and a.loadPos is not None and later[i]:
            gen.insertCode(a.end, gen.identity(ptype(p)))    # аргумент обчислюється до наступних викликів
    scalar = result != 'void' and isScalar(result)
    gen.call(name, ptype(result) if scalar else 'void', sum(len(leaves(p.lstrip('*'))) for p in params))
    for pname, place, typ in outs:        # параметри-вказівники: значення копіюються назад у змінні викликача
        copyToPlace(place, slotVars('${0}.{1}'.format(name, pname), typ))
    if result == 'void' or scalar:
        return Ty(result, call=True)
    base = gen.newBase()                  # результат - масив чи структура: з глобальних змінних у тимчасові
    declLeaves(base, result, False)
    for (s, pt), slot in zip(leaves(result), slotVars('${0}.ret'.format(name), result)):
        gen.copyVar((base + s, pt, False), slot)
    return Ty(result, call=True, place=Place(base, False, result))


def conversion(typ, args, numLine):
    """Явне приведення T(x) (специфікація, табл. «Допустимі явні приведення»); недопустиме - CE-06."""
    if len(args) != 1:
        failSem('call_error', numLine, 'приведення до типу {0} потребує рівно одного аргументу'.format(typ))
    a = args[0]
    if a.addr:
        failSem('conversion_error', numLine, 'недопустиме приведення адреси до типу {0}'.format(typ))
    useValue(a, numLine)
    uT, uA = underlying(typ), underlying(a)
    if uT not in TYPES or uA not in TYPES or not (uT == uA or (uT in NUMERIC and uA in NUMERIC)):
        failSem('conversion_error', numLine, 'недопустиме явне приведення типу {0} до типу {1}'.format(a, typ))
    val = a.val
    if val is not None and uT == 'float':
        val = float(val)
    elif val is not None and uT == 'int':
        val = int(val)                    # відкидання дробової частини (округлення в бік нуля)
        if not INT_MIN <= val <= INT_MAX:
            failSem('const_error', numLine, 'значення {0} поза множиною значень int'.format(a.val))
    if val is not None:                   # константний вираз обчислює транслятор
        gen.cutCode(a.start)
        gen.const(val, uT, numLine)
    elif uT != uA:                        # Трансляція: i2f або f2i після коду аргументу
        gen.op('i2f' if uT == 'float' else 'f2i', 'conv')
    return Ty(typ, val=val, start=a.start)


@rule
def parseArgumentList():                  # ArgumentList = Argument { ',' Argument }
    args = [parseArgument()]
    args[-1].end = gen.here()
    while peek()[0] == ',':
        expect(',')
        args.append(parseArgument())
        args[-1].end = gen.here()
    return args


@rule
def parseArgument():                      # Argument = AddrExpr | Expression
    if peek()[0] == '&':
        return parseAddrExpr()
    return parseExpression()


@rule
def parseAddrExpr():                      # AddrExpr = '&' PrimaryExpr
    numLine = getSymb()[0]
    r0 = numRow
    expect('&')
    t = parsePrimaryExpr()
    if t.lvalue is None:
        failSem('not_lvalue', numLine, 'операнд & ({0}) не є адресованим виразом'.format(srcText(r0 + 1, numRow)))
    res = Ty('*' + t, addr=True, lvalue=t.lvalue, place=t.place, start=t.start)
    res.loadPos, res.loadEnd = t.loadPos, t.loadEnd   # Трансляція: значення змінної вже на стеку (для виклику)
    return res


@rule
def parseOperand():
    # Operand = Literal | Ident | Dereference | Conversion | LenCall | ArrayLit | '(' Expression ')'
    numLine, lex, tok = getSymb()
    if tok in LITERALS:
        return parseLiteral()
    if tok == 'id':
        parseToken(None, 'id')
        return identOperand(lex, numLine)
    if lex == '*':
        return parseDereference()
    if lex in TYPES:
        return parseConversion()
    if lex == 'len':
        return parseLenCall()
    if lex == '[':
        return parseArrayLit()
    if lex == '(':
        expect('(')
        t = parseExpression()             # дужки зберігають тип і атрибути виразу, зокрема l-значення
        expect(')')
        return t
    failParse('невідповідність у виразі', (numLine, lex, tok, 'операнд'))


def identOperand(name, numLine):
    """Тип ідентифікатора-операнда за таблицею імен (розд. 2.2 і 3.1.1 методички)."""
    if getTypeVar(name) == 'undeclared_variable':
        _, _, contexts = findName(name, cxt)
        failSem('undeclared_variable', numLine, "ім'я {0} не оголошено (переглянуто контексти {1})"
                .format(name, contexts))
    c, attr, _ = findName(name, cxt)
    idx, kind, typ, val, extra = attr
    if typ in ('pending', 'resolving') and kind in ('var', 'const'):
        if kind == 'const' and typ == 'resolving':
            failSem('const_error', numLine, 'циклічне визначення константи {0}'.format(name))
        if kind == 'var' or topRow[name] >= curDeclRow:
            failSem('undeclared_variable', numLine, "ім'я {0} використано до свого оголошення в рядку {1}"
                    .format(name, tableOfSymb[topRow[name]][0]))
        idx, kind, typ, val, extra = topAttr(name)
    if kind in ('var', 'param'):
        place = Place(varName[(c, name)], c == 'univ', typ.lstrip('*'))
        if typ.startswith('*'):           # параметр-вказівник: значення, на яке він посилається
            return Ty(typ, kind=kind, name=name, ptr=place)
        return Ty(typ, kind=kind, name=name, lvalue=(c, name), place=place)
    if kind == 'const':
        gen.const(val, ptype(typ), numLine)   # Трансляція: значення константи підставляє транслятор
        return Ty(typ, kind=kind, name=name, val=val)
    return Ty(kind, kind=kind, name=name)        # ім'я функції чи типу


@rule
def parseLiteral():                       # Literal = IntLit | FloatLit | StringLit | BoolLit
    numLine, lex, tok = getSymb()
    parseToken(None, tok)
    typ = getTypeConst(lex)
    if typ == 'int':
        val = int(lex)
        if val > INT_MAX:
            failSem('const_error', numLine, 'значення цілого літерала {0} поза множиною значень int'.format(lex))
    elif typ == 'float':
        val = float(lex)
        if math.isinf(val):
            failSem('const_error', numLine, 'значення дійсного літерала {0} поза множиною значень float'.format(lex))
    elif typ == 'string':
        val = strValue(lex)
    else:
        val = lex == 'true'
    gen.const(val, typ, numLine)          # Трансляція: ПОЛІЗ константи - константа
    return Ty(typ, val=val, lit=True)


@rule
def parseDereference():                   # Dereference = '*' Ident
    numLine = getSymb()[0]
    expect('*')
    name = getSymb()[1]
    parseToken(None, 'id')
    typ = getTypeVar(name)
    if typ == 'undeclared_variable':
        failSem('undeclared_variable', numLine, "ім'я {0} не оголошено".format(name))
    c, attr, _ = findName(name, cxt)
    if attr[1] != 'param' or not typ.startswith('*'):
        failSem('type_error', numLine, 'розіменування * застосовне лише до параметра-вказівника, а {0} має тип {1}'
                .format(name, typ))
    return Ty(typ[1:], lvalue=(c, name), place=Place(varName[(c, name)], False, typ[1:]))


@rule
def parseConversion():                    # Conversion = BasicType '(' Expression ')'
    numLine = getSymb()[0]
    typ = parseBasicType()
    expect('(')
    e = parseExpression()
    expect(')')
    return conversion(typ, [e], numLine)


@rule
def parseLenCall():                       # LenCall = len '(' Expression ')'
    numLine = getSymb()[0]
    expect('len'); expect('(')
    pos = gen.here()
    e = parseExpression()
    expect(')')
    useValue(e, numLine, False)           # len масиву не читає значень його елементів
    u = underlying(e)
    if u.startswith('['):
        n = arrayLen(u)
    elif u == 'string':
        if e.val is not None:
            n = len(e.val)
        elif e.kind == 'var' and e.lvalue in strConst:
            n = len(strConst[e.lvalue])   # змінна зберігає значення свого константного ініціалізатора
        else:
            failTran(numLine, 'len рядка, значення якого невідоме під час трансляції,')
    else:
        failSem('type_error', numLine, 'len застосовний лише до масиву або рядка, а не до значення типу {0}'.format(e))
    gen.cutCode(pos)                      # Трансляція: довжина відома транслятору - у ПОЛІЗ іде константа
    gen.const(n, 'int')
    return Ty('int', val=n)


@rule
def parseArrayLit():                      # ArrayLit = ArrayType LiteralValue
    numLine = getSymb()[0]
    typ = parseArrayType()
    return Ty(typ, val=parseLiteralValue(typ, numLine))


@rule
def parseLiteralValue(arrType, numLine):  # LiteralValue = '{' [ Element ElementTail ] '}'
    expect('{')
    elem, n, elems = elemType(arrType), arrayLen(arrType), []
    if peek()[0] != '}':
        elems = [parseElement(elem)] + parseElementTail(elem)
    expect('}')
    if len(elems) > n:
        failSem('array_error', numLine, 'літерал масиву {0} містить {1} елементів - більше, ніж {2}'
                .format(arrType, len(elems), n))
    for _ in range(len(elems), n):        # Трансляція: пропущені елементи отримують нульові значення
        for s, pt in leaves(elem):
            gen.zero(pt)
    if all(e.val is not None for e in elems):
        return tuple(convertVal(e.val, elem) for e in elems)
    return None


@rule
def parseElementTail(elem):               # ElementTail = [ ',' [ Element ElementTail ] ]
    if peek()[0] == ',':
        expect(',')
        if peek()[0] != '}':
            return [parseElement(elem)] + parseElementTail(elem)
    return []


@rule
def parseElement(elem):                   # Element = Expression | LiteralValue
    numLine = getSymb()[0]
    if peek()[0] == '{':
        if not underlying(elem).startswith('['):
            failSem('type_error', numLine, 'список значень у фігурних дужках можливий лише для елементів-масивів, '
                                           'а не типу {0}'.format(elem))
        return Ty(elem, val=parseLiteralValue(elem, numLine))
    e = parseExpression()
    useValue(e, numLine)
    if not assignable(e, elem):
        failSem('type_error', numLine, 'елемент типу {0} несумісний з типом елементів масиву {1}'.format(e, elem))
    convert(e, elem)                      # Трансляція: значення елемента лишається на стеку
    return e


# ---------------------------------------------------------------- таблиця імен: друк
def showVal(kind, val):
    if kind != 'const':
        return str(val)
    if isinstance(val, bool):
        return 'true' if val else 'false'
    if isinstance(val, str):
        return '"' + val.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n') \
                        .replace('\t', '\\t').replace('\r', '\\r') + '"'
    return repr(val)


def showExtra(extra):
    if isinstance(extra, tuple):
        return '(' + ', '.join(extra) + ')'
    if isinstance(extra, dict):
        return '{' + '; '.join('{0} {1}'.format(n, t) for n, t in extra.items()) + '}'
    return extra


def printTabNames():
    """Друк таблиці імен за контекстами (printTabNames з forTabNames.py); порожні блоки не друкуються."""
    print('Таблиця імен: контекст (охоплюючий контекст, рівень блока); ім\'я, номер у блоці, клас, тип, '
          'значення, параметри')
    for c, rec in tabName.items():
        if not rec['names']:
            continue
        print('{0} (declIn={1}, рівень={2})'.format(c, rec['declIn'], rec['level']))
        for name, (idx, kind, typ, val, extra) in rec['names'].items():
            print('  {0:<10} {1:>2}  {2:<6} {3:<12} {4:<10} {5}'.format(
                name, idx, kind, typ, showVal(kind, val), showExtra(extra)))


def finishProgram():
    """Завершення трансляції: у головному модулі - виклик main і виведення залишку рядка; модулі допоміжних
    функцій; перевірка модулів."""
    gen.setModule(gen.root)
    if gen.buffered:
        gen.clearOut()
    gen.call('main', 'void', 0)
    if gen.buffered:
        gen.flush(False)
    gen.buildHelpers()
    for m in gen.modules.values():
        if m.bad:
            failTran(m.bad[0][1], 'рядок із символом нового рядка чи повернення каретки поза аргументом виведення')
        gen.checkModule(m)


def translate(text, name='prog'):
    """Трансляція тексту програми: лексичний аналіз, потім синтаксичний і семантичний аналіз, у процесі якого
    генерується ПОЛІЗ. Прохід 1 - оголошення блока пакета, прохід 2 - тіла функцій."""
    global numRow, len_tableOfSymb, indt, passNo, cxt, curDeclRow, currentFunc, fmtImported, lastError
    global ptrParams, addrTaken, globalNames, FSuccess
    tabName.clear(); topRow.clear(); warnings.clear(); varName.clear(); funcParams.clear()
    tabName['univ'] = {'declIn': '-', 'level': 0, 'blocks': 0, 'names': {}}
    cxt, curDeclRow, currentFunc, fmtImported, lastError = 'univ', 0, None, False, None
    ptrParams, addrTaken, globalNames = [], set(), set()
    strConst.clear()
    print('compileToPostfix: lexer Start Up\n')
    out = io.StringIO()
    with contextlib.redirect_stdout(out):
        FSuccess = krok_lex.analyze(text)
    print('\n'.join(line for line in out.getvalue().splitlines() if line.startswith('Lexer')))
    print('compileToPostfix: lexer-FSuccess ={0}'.format(FSuccess))
    if FSuccess != ('Lexer', True):       # чи був успішним лексичний розбір
        lastError = 'CE-01'
        return FSuccess
    numRow, indt, len_tableOfSymb = 1, 0, len(tableOfSymb)
    print('-' * 55)
    print('compileToPostfix: Start Up compiler = parser + codeGenerator\n')
    FSuccess = ('codeGeneration', False)
    gen.reset(name, any(rec[1] == 'Print' and rec[2] == 'keyword' for rec in tableOfSymb.values()))
    try:
        registerTopNames()
        passNo = 1                        # оголошення блока пакета; код ініціалізації глобальних змінних
        parseProgram()
        checkProgram()
        globalNames = {n for n, attr in tabName['univ']['names'].items() if attr[1] == 'var'}
        passNo, numRow = 2, 1             # тіла функцій: кожна функція - окремий модуль PSM
        parseProgram()
        finishProgram()
        FSuccess = ('codeGeneration', True)
        print('Translator: Переклад у ПОЛІЗ та синтаксичний аналіз завершились успішно' +
              (' (попереджень: {0})'.format(len(warnings)) if warnings else ''))
    except SystemExit as e:
        who = {'CE-02': 'Parser', 'PSM': 'Translator'}.get(lastError, 'Semantic')
        print('{0}: Аварійне завершення програми з кодом {1}'.format(who, e))
    return FSuccess


def serv():
    """Вивести таблиці та постфіксний код модулів програми."""
    gen.serv()


def savePostfixCode(directory):
    """Зберегти постфіксний код: кожен модуль - у файлі <модуль>.postfix, придатному для виконання в PSM."""
    for fileName in gen.savePostfixCode(directory):
        print('postfix-код збережено у файлі {0}'.format(os.path.join(directory, fileName)))


def compileToPostfix(fileName, outDir=None, quiet=False):
    """Трансляція програми з файлу fileName у постфіксний код для PSM (розд. 3.2.1 методички).
    Модулі зберігаються в теці outDir (типово postfix/<ім'я програми>)."""
    name = re.sub(r'\W', '_', os.path.splitext(os.path.basename(fileName))[0])
    with open(fileName, encoding='utf-8') as f:
        text = f.read()
    if translate(text, name) == ('codeGeneration', True):
        if not quiet:
            serv()
        savePostfixCode(outDir or os.path.join('postfix', name))
    return FSuccess


# ---------------------------------------------------------------- запуск


if __name__ == '__main__':
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8')
    args = sys.argv[1:]
    trace, gen.toView = '--trace' in args, '--view' in args
    outDir = args[args.index('-o') + 1] if '-o' in args else None
    files = [a for k, a in enumerate(args) if not a.startswith('-') and (k == 0 or args[k - 1] != '-o')]
    fileName = files[0] if files else os.path.join('..', 'KP1', 'tests', 'base_example.krok')
    sys.exit(0 if compileToPostfix(fileName, outDir, '--quiet' in args) == ('codeGeneration', True) else 1)
