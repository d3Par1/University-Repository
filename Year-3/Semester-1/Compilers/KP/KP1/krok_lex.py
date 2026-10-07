# Лексичний аналізатор мови Krok
# Метод: діаграма станів (програмний імітатор скінченного автомата),
# побудовано за зразком my_lang_lex.py з матеріалів до роботи КП 1.
# Запуск:  python krok_lex.py <файл.krok>
import sys

# Таблиця лексем мови: токени, що однозначно визначаються лексемою
tokenTable = {
    'package': 'keyword', 'import': 'keyword', 'func': 'keyword', 'var': 'keyword',
    'const': 'keyword', 'type': 'keyword', 'struct': 'keyword', 'if': 'keyword',
    'else': 'keyword', 'for': 'keyword', 'return': 'keyword', 'int': 'keyword',
    'float': 'keyword', 'bool': 'keyword', 'string': 'keyword', 'len': 'keyword',
    'fmt': 'keyword', 'Print': 'keyword', 'Println': 'keyword', 'Scan': 'keyword',
    'true': 'boolval', 'false': 'boolval',
    '+': 'add_op', '-': 'add_op', '*': 'mult_op', '/': 'mult_op', '%': 'mult_op',
    '**': 'pow_op', '==': 'rel_op', '!=': 'rel_op', '<': 'rel_op', '<=': 'rel_op',
    '>': 'rel_op', '>=': 'rel_op', '&&': 'and_op', '||': 'or_op', '!': 'not_op',
    '=': 'assign_op', ':=': 'define_op', '++': 'incdec_op', '--': 'incdec_op',
    '&': 'addr_op',
    '(': 'paren', ')': 'paren', '[': 'bracket', ']': 'bracket',
    '{': 'brace', '}': 'brace', ',': 'punct', ';': 'punct', '.': 'punct',
    ' ': 'ws', '\t': 'ws', '\n': 'eol'}
# Решту токенів визначаємо не за лексемою, а за заключним станом
tokStateTable = {2: 'id', 12: 'intnum', 13: 'floatnum', 16: 'strlit'}

# Діаграма станів  M = (Q, Σ, δ, q0, F)
# state-transition function
stf = {
    # ідентифікатори, ключові слова, логічні літерали
    (0, 'Letter'): 1, (1, 'Letter'): 1, (1, 'Digit'): 1, (1, 'other'): 2,
    # числа: 3 - '0', 4 - ціле, 5 - ціле з ведучим нулем, 6 - після крапки,
    #        7 - дробова частина, 8 - після e/E, 9 - після знака, 10 - показник
    (0, '0'): 3, (0, 'Digit'): 4,
    (3, 'Digit'): 5, (3, 'dot'): 6, (3, 'e'): 8, (3, 'E'): 8, (3, 'Letter'): 104, (3, 'other'): 12,
    (4, 'Digit'): 4, (4, 'dot'): 6, (4, 'e'): 8, (4, 'E'): 8, (4, 'Letter'): 104, (4, 'other'): 12,
    (5, 'Digit'): 5, (5, 'dot'): 6, (5, 'e'): 8, (5, 'E'): 8, (5, 'Letter'): 104, (5, 'other'): 102,
    (6, 'Digit'): 7, (6, 'e'): 8, (6, 'E'): 8, (6, 'Letter'): 104, (6, 'other'): 13,
    (7, 'Digit'): 7, (7, 'e'): 8, (7, 'E'): 8, (7, 'Letter'): 104, (7, 'other'): 13,
    (8, 'Digit'): 10, (8, '+'): 9, (8, '-'): 9, (8, 'other'): 103,
    (9, 'Digit'): 10, (9, 'other'): 103,
    (10, 'Digit'): 10, (10, 'Letter'): 104, (10, 'other'): 13,
    # крапка: початок дійсного літерала (.5) або роздільник
    (0, 'dot'): 11, (11, 'Digit'): 7, (11, 'other'): 31,
    # рядковий літерал: 14 - тіло, 15 - після зворотної косої риски
    (0, 'quote'): 14, (14, 'quote'): 16, (14, 'bslash'): 15, (14, 'nl'): 105, (14, 'other'): 14,
    (15, 'n'): 14, (15, 't'): 14, (15, 'r'): 14, (15, 'quote'): 14, (15, 'bslash'): 14,
    (15, 'other'): 106,
    # '/' - ділення або початок коментаря
    (0, '/'): 17, (17, '/'): 18, (17, 'other'): 31,
    (18, 'nl'): 19, (18, 'other'): 18,
    # одно- та двосимвольні оператори
    (0, '+'): 20, (20, '+'): 30, (20, 'other'): 31,
    (0, '-'): 21, (21, '-'): 30, (21, 'other'): 31,
    (0, '*'): 22, (22, '*'): 30, (22, 'other'): 31,
    (0, '='): 23, (23, '='): 30, (23, 'other'): 31,
    (0, '!'): 24, (24, '='): 30, (24, 'other'): 31,
    (0, '<'): 25, (25, '='): 30, (25, 'other'): 31,
    (0, '>'): 26, (26, '='): 30, (26, 'other'): 31,
    (0, '&'): 27, (27, '&'): 30, (27, 'other'): 31,
    (0, '|'): 28, (28, '|'): 30, (28, 'other'): 108,
    (0, ':'): 29, (29, '='): 30, (29, 'other'): 107,
    # односимвольні лексеми
    (0, '%'): 30, (0, '('): 30, (0, ')'): 30, (0, '['): 30, (0, ']'): 30,
    (0, '{'): 30, (0, '}'): 30, (0, ','): 30, (0, ';'): 30,
    (0, 'ws'): 0,
    (0, 'nl'): 32,
    (0, 'other'): 101
}

initState = 0                                   # q0 - стартовий стан
F = {2, 12, 13, 16, 19, 30, 31, 32, 101, 102, 103, 104, 105, 106, 107, 108}
Fstar = {2, 12, 13, 19, 31}                     # з зірочкою
Ferror = {101, 102, 103, 104, 105, 106, 107, 108}   # виявлена помилка

# Після цих токенів/лексем у кінці рядка вставляється ';' (розд. 2.9 специфікації)
semicolonAfterToken = {'id', 'intnum', 'floatnum', 'strlit', 'boolval'}
semicolonAfterLexeme = {'return', 'int', 'float', 'bool', 'string', ')', ']', '}', '++', '--'}

# Таблиці: розбору (символів програми), ідентифікаторів та констант
tableOfSymb = {}    # { n_rec : (num_line, lexeme, token, idxIdConst) }
tableOfId = {}      # { Id : idxId }
tableOfConst = {}   # { Const : (token, idxConst) }

state = initState
FSuccess = ('Lexer', False)     # ознака успішності/неуспішності розбору
sourceCode = ''
lenCode = -1        # номер останнього символа у тексті програми
numLine = 1         # лексичний аналіз починаємо з першого рядка
numChar = -1        # з першого символа (в Python'і нумерація - з 0)
char = ''           # ще не брали жодного символа
lexeme = ''         # ще не починали розпізнавати лексеми
lastRec = None      # (токен, лексема) останньої лексеми поточного рядка


def lex():
    global state, numLine, char, lexeme, numChar, FSuccess
    try:
        while numChar < lenCode:
            char = nextChar()                       # прочитати наступний символ
            classCh = classOfChar(char)             # до якого класу належить
            state = nextState(state, classCh, char) # обчислити наступний стан
            if is_final(state):                     # якщо стан заключний
                processing()                        #   виконати семантичні процедури
            elif state == initState:                # якщо стан стартовий -
                lexeme = ''                         #   нова лексема
            else:                                   # інакше -
                lexeme += char                      #   додати символ до лексеми
        print('Lexer: Лексичний аналіз завершено успішно')
        FSuccess = ('Lexer', True)
    except SystemExit as e:
        # Повідомити про факт виявлення помилки
        print('Lexer: Аварійне завершення програми з кодом {0}'.format(e))


def processing():
    global state, lexeme, char, numLine, numChar, lastRec
    if state == 32:             # \n
        if lastRec is not None and (lastRec[0] in semicolonAfterToken
                                    or lastRec[1] in semicolonAfterLexeme):
            addRecord(';', 'punct', '', ' <- вставлено автоматично')
        lastRec = None
        numLine += 1
        state = initState
    if state in (2, 12, 13):    # keyword, boolval, id, intnum, floatnum
        token = getToken(state, lexeme)
        if token != 'keyword':
            addRecord(lexeme, token, indexIdConst(state, lexeme, token))
        else:
            addRecord(lexeme, token, '')
        lexeme = ''
        numChar = putCharBack(numChar)      # зірочка
        state = initState
    if state == 16:             # strlit
        lexeme += char
        token = getToken(state, lexeme)
        addRecord(lexeme, token, indexIdConst(state, lexeme, token))
        lexeme = ''
        state = initState
    if state == 19:             # коментар - ігнорується
        lexeme = ''
        numChar = putCharBack(numChar)      # зірочка: \n обробляється у стані 0
        state = initState
    if state in (30, 31):       # оператори та роздільники
        if state == 30:
            lexeme += char                  # останній символ належить лексемі
        else:
            numChar = putCharBack(numChar)  # зірочка
        addRecord(lexeme, getToken(state, lexeme), '')
        lexeme = ''
        state = initState
    if state in Ferror:         # ERROR
        fail()


def addRecord(lexeme, token, index, note=''):
    global lastRec
    print('{0:<4d} {1:<22s} {2:<10s} {3}{4}'.format(numLine, lexeme, token, index, note))
    tableOfSymb[len(tableOfSymb) + 1] = (numLine, lexeme, token, index)
    lastRec = (token, lexeme)


def fail():
    print(numLine)
    if state == 101:
        print('Lexer: у рядку ', numLine, ' неочікуваний символ ' + char)
    if state == 102:
        print('Lexer: у рядку ', numLine, ' цілий літерал із ведучим нулем: ' + lexeme)
    if state == 103:
        print('Lexer: у рядку ', numLine, ' неправильна експонента дійсного літерала: ' + lexeme)
    if state == 104:
        print('Lexer: у рядку ', numLine, ' ідентифікатор не може починатися з цифри: ' + lexeme + char)
    if state == 105:
        print('Lexer: у рядку ', numLine, ' незакритий рядковий літерал: ' + lexeme)
    if state == 106:
        print('Lexer: у рядку ', numLine, ' невідома escape-послідовність \\' + char)
    if state == 107:
        print('Lexer: у рядку ', numLine, ' очікувався символ =, а не ' + char)
    if state == 108:
        print('Lexer: у рядку ', numLine, ' очікувався символ |, а не ' + char)
    exit(state)


def is_final(state):
    return state in F


def nextState(state, classCh, char):
    # спершу перехід за конкретним символом (0, e, E, n, t, r), потім - за класом,
    # якщо жодного немає - за класом other
    try:
        return stf[(state, char)]
    except KeyError:
        try:
            return stf[(state, classCh)]
        except KeyError:
            return stf[(state, 'other')]


def nextChar():
    global numChar
    numChar += 1
    return sourceCode[numChar]


def putCharBack(numChar):
    return numChar - 1


def classOfChar(char):
    if char in '.':
        res = "dot"
    elif char in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_':
        res = "Letter"
    elif char in "0123456789":
        res = "Digit"
    elif char in " \t\r":
        res = "ws"
    elif char in "\n":
        res = "nl"
    elif char in '"':
        res = "quote"
    elif char in '\\':
        res = "bslash"
    elif char in "+-*/%=!<>&|:;,()[]{}":
        res = char
    else:
        res = 'other'   # символ не належить алфавіту (допустимий у рядку та коментарі)
    return res


def getToken(state, lexeme):
    try:
        return tokenTable[lexeme]
    except KeyError:
        return tokStateTable[state]


def indexIdConst(state, lexeme, token):
    indx = 0
    if token == 'id':
        indx = tableOfId.get(lexeme)
        if indx is None:
            indx = len(tableOfId) + 1
            tableOfId[lexeme] = indx
    else:                       # intnum, floatnum, strlit, boolval
        rec = tableOfConst.get(lexeme)
        if rec is None:
            indx = len(tableOfConst) + 1
            tableOfConst[lexeme] = (token, indx)
        else:
            indx = rec[1]
    return indx


def analyze(text):
    """Запустити лексичний аналіз тексту програми (таблиці та лічильники - з початку)."""
    global sourceCode, lenCode, state, numLine, numChar, char, lexeme, lastRec, FSuccess
    tableOfSymb.clear(); tableOfId.clear(); tableOfConst.clear()
    sourceCode = text if text.endswith('\n') else text + '\n'   # кінець файлу = кінець рядка
    lenCode = len(sourceCode) - 1
    state, numLine, numChar, char, lexeme, lastRec = initState, 1, -1, '', '', None
    FSuccess = ('Lexer', False)
    lex()
    return FSuccess


if __name__ == '__main__':
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8')
    fileName = sys.argv[1] if len(sys.argv) > 1 else 'tests/base_example.krok'
    f = open(fileName, 'r', encoding='utf-8')
    analyze(f.read())
    f.close()
    # Таблиці: розбору, ідентифікаторів та констант
    print('-' * 30)
    print('tableOfSymb:{0}'.format(tableOfSymb))
    print('tableOfId:{0}'.format(tableOfId))
    print('tableOfConst:{0}'.format(tableOfConst))
