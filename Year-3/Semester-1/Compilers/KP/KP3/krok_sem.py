# Семантичний аналізатор мови Krok
# За матеріалами до роботи КП 3 «Семантичний аналіз програм»: семантичний аналіз виконується в процесі
# синтаксичного аналізу - синтаксичний аналізатор з КП 2 (рекурсивний спуск) доповнено кодом для аналізу
# семантики. Функції розбору виразів повертають тип розібраного виразу, функції розбору інструкцій -
# 'void' (перевірка типів закінчилась успіхом) або 'type_error'; виявлена помилка - виклик failSem().
# Таблиця імен - за зразком forTabNames.py: контексти (блоки) з посиланням declIn на охоплюючий контекст,
# пошук імені від поточного контексту до блока пакета 'univ'; запис імені - (номер у блоці, клас, тип,
# значення, параметри), блок характеризується рівнем (Медведєва В.М., Транслятори, ч. 2, рис. 2.36).
# Запуск:  python krok_sem.py <файл.krok> [--trace]
import contextlib
import io
import math
import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'KP1'))
import krok_lex
from krok_lex import tokenTable

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
    term - інструкція завершальна (закінчується return); empty - порожній список інструкцій."""
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
        return obj


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


def binary(lType, op, rType, numLine):
    """Перевірка типів бінарної операції (розд. 3.1.2 методички) та обчислення константного виразу."""
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
    return Ty(resType, val=val)


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
    for i, (name, line) in enumerate(names):
        if not exprs:
            declare(name, line, 'var', declType, 'undefined')
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
        declare(name, line, 'var', typ, 'assigned')
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
    e = parseExpression()
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
    n = parseExpression()
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
    global currentFunc
    expect('func')
    numLine, name, _ = getSymb()
    parseToken(None, 'id')
    startTopDecl(name)
    expect('(')
    params = parseParamList() if peek()[1] == 'id' else []
    expect(')')
    result = parseType() if startsType(*peek()) else 'void'
    if passNo == 1:                       # прохід 1: лише сигнатура функції
        declare(name, numLine, 'func', result, '-', tuple(typ for _, _, typ in params))
        skipBlock()
        return Ty('void')
    openContext(name)                     # блок функції: параметри та інструкції верхнього рівня тіла
    for pname, pline, ptype in params:
        insertName(cxt, pname, pline, 'param', ptype, 'assigned')
    currentFunc = (name, result)
    body = parseBlock(False)
    if result != 'void' and not body.term:
        failSem('return_error', numLine, 'функція {0} з результатом {1} не завершується інструкцією return '
                                         'на кожному шляху виконання'.format(name, result))
    closeContext()
    currentFunc = None
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
        if form == 'expr' and not t.call:
            failSem('simple_stmt', numLine, 'інструкція-вираз {0} не є викликом функції'.format(srcText(r0, numRow)))
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
        useValue(e, numLine)
        if str(e).startswith('*'):
            failSem('type_error', numLine, 'вказівник не можна зберегти в змінній {0}'.format(lex))
        declare(lex, numLine, 'var', str(e), 'assigned')
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
        right = parseExpression()
        useValue(right, numLine)
        # перевірка типів інструкції присвоювання (розд. 3.2.1 методички)
        if getTypeOp(left, '=', right) == 'type_error':
            failSem('type_error', numLine, '{0} типу {1} не можна присвоїти значення типу {2}'
                    .format(leftText, left, right))
        setAssigned(left.lvalue)
        return 'assign', left
    if op in ('++', '--'):
        expect(op)
        if underlying(left) not in NUMERIC:
            failSem('type_error', numLine, 'оператор {0} не застосовний до значення типу {1}'.format(op, left))
        useValue(left, numLine)
        setAssigned(left.lvalue)
        return 'incdec', left
    return 'expr', left


@rule
def parseIfStmt():                        # IfStmt = if Expression Block [ else ( IfStmt | Block ) ]
    numLine = getSymb()[0]
    expect('if')
    cond = parseExpression()
    useValue(cond, numLine)
    if underlying(cond) != 'bool':
        failSem('type_error', numLine, 'умова if має тип {0}, а не bool'.format(cond))
    thenRes, elseRes = parseBlock(), None
    if peek()[0] == 'else':
        expect('else')
        elseRes = parseIfStmt() if peek()[0] == 'if' else parseBlock()
    ok = thenRes == 'void' and (elseRes is None or elseRes == 'void')
    return Ty('void' if ok else 'type_error', term=elseRes is not None and thenRes.term and elseRes.term)


@rule
def parseForStmt():                       # ForStmt = for ForHeader Block
    numLine = getSymb()[0]
    expect('for')
    openContext()                         # неявний блок інструкції for
    condType = parseForHeader(numLine)
    body = parseBlock()
    closeContext()
    # if type(BoolExpr) == 'bool' then type(Stmnt) else type_error (розд. 3.2.3 методички)
    return Ty(body if condType == 'bool' else 'type_error')


@rule
def parseForHeader(numLine):              # ForHeader = SimpleStmt [ ';' Expression ';' SimpleStmt ]
    r0 = numRow
    form, t = parseSimpleStmt()
    if peek()[0] != ';':                  # цикл з передумовою: for Expression Block
        if form != 'expr':
            failSem('type_error', numLine, 'умовою циклу for має бути вираз типу bool, а не інструкція {0}'
                    .format(srcText(r0, numRow)))
        useValue(t, numLine)
        if underlying(t) != 'bool':
            failSem('type_error', numLine, 'умова for має тип {0}, а не bool'.format(t))
        return 'bool'
    if form == 'expr' and not t.call:
        failSem('simple_stmt', numLine, 'ініціалізація for {0} не є інструкцією'.format(srcText(r0, numRow)))
    expect(';')
    cond = parseExpression()
    useValue(cond, numLine)
    if underlying(cond) != 'bool':
        failSem('type_error', numLine, 'умова for має тип {0}, а не bool'.format(cond))
    expect(';')
    r0 = numRow
    form, t = parseSimpleStmt()
    if form == 'define':
        failSem('simple_stmt', numLine, 'оголошення := в інструкції наступного кроку for')
    if form == 'expr' and not t.call:
        failSem('simple_stmt', numLine, 'інструкція наступного кроку for {0} не є інструкцією'.format(srcText(r0, numRow)))
    return 'bool'


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
    elif result != 'void':
        failSem('return_error', numLine, 'return без виразу у функції {0} з результатом {1}'.format(name, result))
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
    args = parseExpressionList() if peek()[0] != ')' else []
    expect(')')
    for i, a in enumerate(args, 1):       # parseWrite - 'void', якщо аргументи - значення скалярних типів
        useValue(a, numLine)
        if not isScalar(a):
            failSem('type_error', numLine, 'аргумент {0} fmt.{1} має тип {2}, а не скалярний'.format(i, lex, a))
    return Ty('void')


@rule
def parseInputCall():                     # InputCall = 'Scan' '(' AddrExpr { ',' AddrExpr } ')'
    numLine = getSymb()[0]
    expect('Scan'); expect('(')
    args = [parseAddrExpr()]
    while peek()[0] == ',':
        expect(',')
        args.append(parseAddrExpr())
    expect(')')
    for i, a in enumerate(args, 1):
        if not isScalar(str(a)[1:]):
            failSem('type_error', numLine, 'аргумент {0} fmt.Scan - змінна типу {1}, а не скалярного'.format(i, str(a)[1:]))
        setAssigned(a.lvalue)             # змінна набуває значення в інструкції введення
    return Ty('void')


# ---------------------------------------------------------------- вирази
@rule
def parseExpressionList():                # ExpressionList = Expression { ',' Expression }
    exprs = [parseExpression()]
    while peek()[0] == ',':
        expect(',')
        exprs.append(parseExpression())
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
        lType = binary(lType, '||', parseAndExpr(), numLine)
    return lType


@rule
def parseAndExpr():                       # AndExpr = RelExpr { '&&' RelExpr }
    lType = parseRelExpr()
    while peek()[0] == '&&':
        numLine = getSymb()[0]
        expect('&&')
        lType = binary(lType, '&&', parseRelExpr(), numLine)
    return lType


@rule
def parseRelExpr():                       # RelExpr = AddExpr [ RelOp AddExpr ]
    lType = parseAddExpr()
    if peek()[1] == 'rel_op':
        numLine, op, _ = getSymb()
        expect(op)
        lType = binary(lType, op, parseAddExpr(), numLine)
    return lType


@rule
def parseAddExpr():                       # AddExpr = MulExpr { AddOp MulExpr }
    lType = parseMulExpr()
    while peek()[1] == 'add_op':
        numLine, op, _ = getSymb()
        expect(op)
        lType = binary(lType, op, parseMulExpr(), numLine)
    return lType


@rule
def parseMulExpr():                       # MulExpr = UnaryExpr { MulOp UnaryExpr }
    lType = parseUnaryExpr()
    while peek()[1] == 'mult_op':
        numLine, op, _ = getSymb()
        expect(op)
        lType = binary(lType, op, parseUnaryExpr(), numLine)
    return lType


@rule
def parseUnaryExpr():                     # UnaryExpr = UnaryOp UnaryExpr | PowerExpr
    if peek()[0] in ('-', '+', '!'):
        numLine, op, _ = getSymb()
        expect(op)
        t = parseUnaryExpr()
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
        return Ty(t, val=val, lit=t.lit and op != '!')
    return parsePowerExpr()


@rule
def parsePowerExpr():                     # PowerExpr = PrimaryExpr [ '**' UnaryExpr ]
    lType = parsePrimaryExpr()
    if peek()[0] == '**':
        numLine = getSymb()[0]
        expect('**')
        lType = binary(lType, '**', parseUnaryExpr(), numLine)
    return lType


@rule
def parsePrimaryExpr():                   # PrimaryExpr = Operand { Selector | Index | Arguments }
    r0 = numRow
    t = parseOperand()
    while peek()[0] in ('.', '[', '('):
        lex = peek()[0]
        if lex == '.':
            t = parseSelector(t)
        elif lex == '[':
            t = parseIndex(t)
        else:
            t = parseArguments(t, r0)
    return t


@rule
def parseSelector(t):                     # Selector = '.' Ident
    numLine = getSymb()[0]
    useValue(t, numLine, False)
    expect('.')
    field = getSymb()[1]
    parseToken(None, 'id')
    if underlying(t) != 'struct':
        failSem('field_error', numLine, 'звернення до поля {0} значення типу {1}, що не є структурою'.format(field, t))
    fields = fieldsOf(t)
    if field not in fields:
        failSem('field_error', numLine, 'структура {0} не має поля {1}'.format(t, field))
    return Ty(fields[field], lvalue=t.lvalue)


@rule
def parseIndex(t):                        # Index = '[' Expression ']'
    numLine = getSymb()[0]
    useValue(t, numLine, False)
    expect('[')
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
    return Ty(elemType(t), lvalue=t.lvalue)


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
    """Виклик функції: кількість і типи аргументів (CE-08); тип виклику - тип результату функції."""
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
    return Ty(result, call=True)


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
    return Ty(typ, val=val)


@rule
def parseArgumentList():                  # ArgumentList = Argument { ',' Argument }
    args = [parseArgument()]
    while peek()[0] == ',':
        expect(',')
        args.append(parseArgument())
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
    return Ty('*' + t, addr=True, lvalue=t.lvalue)


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
        return Ty(typ, kind=kind, name=name, lvalue=None if typ.startswith('*') else (c, name))
    if kind == 'const':
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
    return Ty(typ[1:], lvalue=(c, name))


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
    e = parseExpression()
    expect(')')
    useValue(e, numLine, False)           # len масиву не читає значень його елементів
    u = underlying(e)
    if u.startswith('['):
        return Ty('int', val=arrayLen(u))
    if u == 'string':
        return Ty('int', val=None if e.val is None else len(e.val))
    failSem('type_error', numLine, 'len застосовний лише до масиву або рядка, а не до значення типу {0}'.format(e))


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


# ---------------------------------------------------------------- запуск
def analyze(text):
    """Лексичний, а потім синтаксичний і семантичний аналіз тексту програми (два проходи)."""
    global numRow, len_tableOfSymb, indt, passNo, cxt, curDeclRow, currentFunc, fmtImported, lastError
    tabName.clear(); topRow.clear(); warnings.clear()
    tabName['univ'] = {'declIn': '-', 'level': 0, 'blocks': 0, 'names': {}}
    cxt, curDeclRow, currentFunc, fmtImported, lastError = 'univ', 0, None, False, None
    out = io.StringIO()
    with contextlib.redirect_stdout(out):
        lexOk = krok_lex.analyze(text) == ('Lexer', True)
    if not lexOk:
        lastError = 'CE-01'
        print('\n'.join(line for line in out.getvalue().splitlines() if line.startswith('Lexer')))
        return ('Semantic', False)
    print('Lexer: Лексичний аналіз завершено успішно')
    numRow, indt, len_tableOfSymb = 1, 0, len(tableOfSymb)
    try:
        registerTopNames()
        passNo = 1
        print('Semantic: прохід 1 - оголошення блока пакета (типи, константи, змінні, сигнатури функцій)')
        parseProgram()
        checkProgram()
        passNo, numRow = 2, 1
        print('Semantic: прохід 2 - тіла функцій')
        parseProgram()
        print('Parser: Синтаксичний аналіз завершився успішно')
        print('Semantic: Семантичний аналіз завершився успішно' +
              (' (попереджень: {0})'.format(len(warnings)) if warnings else ''))
        printTabNames()
        return ('Semantic', True)
    except SystemExit as e:
        who = 'Parser' if lastError == 'CE-02' else 'Semantic'
        print('{0}: Аварійне завершення програми з кодом {1}'.format(who, e))
        return ('Semantic', False)


if __name__ == '__main__':
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8')
    trace = '--trace' in sys.argv
    files = [a for a in sys.argv[1:] if a != '--trace']
    fileName = files[0] if files else os.path.join('..', 'KP1', 'tests', 'base_example.krok')
    with open(fileName, encoding='utf-8') as f:
        analyze(f.read())
