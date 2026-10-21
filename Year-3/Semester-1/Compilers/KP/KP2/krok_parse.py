# Синтаксичний аналізатор мови Krok методом рекурсивного спуску
# За зразком parse_02.py з матеріалів до роботи КП 2: одна функція на кожне правило граматики,
# вхід - таблиця розбору tableOfSymb лексичного аналізатора з КП 1.
# Запуск:  python krok_parse.py <файл.krok>
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'KP1'))
import krok_lex
from krok_lex import tokenTable

tableOfSymb = krok_lex.tableOfSymb  # { n_rec : (num_line, lexeme, token, idxIdConst) }
numRow = 1                          # номер поточного запису таблиці розбору
len_tableOfSymb = 0                 # довжина таблиці розбору (номер останнього запису)

TYPES = ('int', 'float', 'bool', 'string')
LITERALS = ('intnum', 'floatnum', 'strlit', 'boolval')

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
    """Вхід у функцію правила: збільшити відступ, вивести її ім'я; на виході - зменшити відступ."""
    def wrapper(*args):
        print(nextIndt() + f.__name__ + '():')
        try:
            return f(*args)
        finally:
            predIndt()
    return wrapper


# ---------------------------------------------------------------- доступ до таблиці розбору
def getSymb():
    """Поточний запис таблиці розбору: номер рядка програми, лексема, токен."""
    if numRow > len_tableOfSymb:
        failParse('getSymb(): неочікуваний кінець програми', numRow)
    numLine, lexeme, token, _ = tableOfSymb[numRow]
    return numLine, lexeme, token


def peek():
    """Лексема й токен поточного запису без аварійного завершення (для вибору альтернативи)."""
    if numRow > len_tableOfSymb:
        return None, None
    return tableOfSymb[numRow][1], tableOfSymb[numRow][2]


def parseToken(lexeme, token):
    """Перевірити, що поточний запис - очікувана лексема (lexeme=None: будь-яка лексема токена)."""
    global numRow
    indent = nextIndt()
    if numRow > len_tableOfSymb:
        failParse('неочікуваний кінець програми', (lexeme or token, token, numRow))
    numLine, lex, tok = getSymb()
    numRow += 1
    if tok == token and lexeme in (None, lex):
        print(indent + 'parseToken: В рядку {0} токен {1}'.format(numLine, (lex, tok)))
        res = True
    else:
        failParse('невідповідність токенів', (numLine, lex, tok, lexeme or token, token))
        res = False
    predIndt()
    return res


def expect(lexeme):
    """parseToken для лексеми, токен якої однозначно визначається таблицею лексем мови."""
    return parseToken(lexeme, tokenTable[lexeme])


def failParse(str, tuple):
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


def startsExpression(lex, tok):
    return tok in LITERALS or tok == 'id' or lex in ('-', '+', '!', '*', '(', '[', 'len') + TYPES


def startsType(lex, tok):
    return lex in TYPES or tok == 'id' or lex == '['


def startsStatement(lex, tok):
    return lex in ('var', 'const', 'if', 'for', 'return', 'fmt', '{') or startsExpression(lex, tok)


# ---------------------------------------------------------------- структура програми
# Program = PackageClause ';' { ImportDecl ';' } { TopLevelDecl ';' }
@rule
def parseProgram():
    parsePackageClause(); expect(';')
    while peek()[0] == 'import':
        parseImportDecl(); expect(';')
    while numRow <= len_tableOfSymb:
        parseTopLevelDecl(); expect(';')
    return True


@rule
def parsePackageClause():                 # PackageClause = package Ident
    expect('package'); parseToken(None, 'id')


@rule
def parseImportDecl():                    # ImportDecl = import StringLit
    expect('import'); parseToken(None, 'strlit')


@rule
def parseTopLevelDecl():                  # TopLevelDecl = VarDecl | ConstDecl | TypeDecl | FuncDecl
    numLine, lex, tok = getSymb()
    if lex == 'var':
        parseVarDecl()
    elif lex == 'const':
        parseConstDecl()
    elif lex == 'type':
        parseTypeDecl()
    elif lex == 'func':
        parseFuncDecl()
    else:
        failParse('невідповідність оголошень', (numLine, lex, tok, 'var, const, type або func'))


# ---------------------------------------------------------------- оголошення
@rule
def parseVarDecl():     # VarDecl = var IdentList ( Type [ '=' ExpressionList ] | '=' ExpressionList )
    expect('var'); parseIdentList()
    if peek()[0] == '=':
        expect('='); parseExpressionList()
    else:
        parseType()
        if peek()[0] == '=':
            expect('='); parseExpressionList()


@rule
def parseConstDecl():                     # ConstDecl = const Ident [ Type ] '=' Expression
    expect('const'); parseToken(None, 'id')
    if peek()[0] != '=':
        parseType()
    expect('='); parseExpression()


@rule
def parseTypeDecl():                      # TypeDecl = type Ident ( Type | StructType )
    expect('type'); parseToken(None, 'id')
    if peek()[0] == 'struct':
        parseStructType()
    else:
        parseType()


@rule
def parseStructType():                    # StructType = struct '{' FieldList '}'
    expect('struct'); expect('{'); parseFieldList(); expect('}')


@rule
def parseFieldList():                     # FieldList = [ FieldDecl FieldTail ]
    if peek()[1] == 'id':
        parseFieldDecl(); parseFieldTail()


@rule
def parseFieldTail():                     # FieldTail = [ ';' FieldList ]
    if peek()[0] == ';':
        expect(';'); parseFieldList()


@rule
def parseFieldDecl():                     # FieldDecl = IdentList Type
    parseIdentList(); parseType()


@rule
def parseIdentList():                     # IdentList = Ident { ',' Ident }
    parseToken(None, 'id')
    while peek()[0] == ',':
        expect(','); parseToken(None, 'id')


# ---------------------------------------------------------------- типи
@rule
def parseType():                          # Type = BasicType | TypeName | ArrayType
    numLine, lex, tok = getSymb()
    if lex in TYPES:
        parseBasicType()
    elif tok == 'id':
        parseToken(None, 'id')            # TypeName = Ident
    elif lex == '[':
        parseArrayType()
    else:
        failParse('невідповідність у типі', (numLine, lex, tok, 'int, float, bool, string, ім\'я типу або ['))


@rule
def parseBasicType():                     # BasicType = int | float | bool | string
    numLine, lex, tok = getSymb()
    if lex not in TYPES:
        failParse('невідповідність у типі', (numLine, lex, tok, 'int, float, bool або string'))
    expect(lex)


@rule
def parseArrayType():                     # ArrayType = '[' Expression ']' Type
    expect('['); parseExpression(); expect(']'); parseType()


# ---------------------------------------------------------------- функції
@rule
def parseFuncDecl():                      # FuncDecl = func Ident '(' [ ParamList ] ')' [ Type ] Block
    expect('func'); parseToken(None, 'id'); expect('(')
    if peek()[1] == 'id':
        parseParamList()
    expect(')')
    if startsType(*peek()):
        parseType()
    parseBlock()


@rule
def parseParamList():                     # ParamList = ParamGroup { ',' ParamGroup }
    parseParamGroup()
    while peek()[0] == ',':
        expect(','); parseParamGroup()


@rule
def parseParamGroup():                    # ParamGroup = IdentList ParamType
    parseIdentList(); parseParamType()


@rule
def parseParamType():                     # ParamType = [ '*' ] Type
    if peek()[0] == '*':
        expect('*')
    parseType()


# ---------------------------------------------------------------- інструкції
@rule
def parseBlock():                         # Block = '{' StatementList '}'
    expect('{'); parseStatementList(); expect('}')


@rule
def parseStatementList():                 # StatementList = [ Statement StatementTail ]
    lex, tok = peek()
    if lex is not None and startsStatement(lex, tok):
        parseStatement(); parseStatementTail()


@rule
def parseStatementTail():                 # StatementTail = [ ';' StatementList ]
    if peek()[0] == ';':
        expect(';'); parseStatementList()


@rule
def parseStatement():
    # Statement = VarDecl | ConstDecl | IfStmt | ForStmt | ReturnStmt | IoStmt | Block | SimpleStmt
    numLine, lex, tok = getSymb()
    if lex == 'var':
        parseVarDecl()
    elif lex == 'const':
        parseConstDecl()
    elif lex == 'if':
        parseIfStmt()
    elif lex == 'for':
        parseForStmt()
    elif lex == 'return':
        parseReturnStmt()
    elif lex == 'fmt':
        parseIoStmt()
    elif lex == '{':
        parseBlock()
    elif startsExpression(lex, tok):
        parseSimpleStmt()
    else:
        failParse('невідповідність інструкцій', (numLine, lex, tok, 'інструкція'))
    return True


@rule
def parseSimpleStmt():                    # SimpleStmt = Expression [ AssignOp Expression | IncDecOp ]
    parseExpression()
    lex = peek()[0]
    if lex in ('=', ':='):
        expect(lex); parseExpression()
    elif lex in ('++', '--'):
        expect(lex)


@rule
def parseIfStmt():                        # IfStmt = if Expression Block [ else ( IfStmt | Block ) ]
    expect('if'); parseExpression(); parseBlock()
    if peek()[0] == 'else':
        expect('else')
        if peek()[0] == 'if':
            parseIfStmt()
        else:
            parseBlock()


@rule
def parseForStmt():                       # ForStmt = for ForHeader Block
    expect('for'); parseForHeader(); parseBlock()


@rule
def parseForHeader():                     # ForHeader = SimpleStmt [ ';' Expression ';' SimpleStmt ]
    parseSimpleStmt()
    if peek()[0] == ';':
        expect(';'); parseExpression(); expect(';'); parseSimpleStmt()


@rule
def parseReturnStmt():                    # ReturnStmt = return [ Expression ]
    expect('return')
    lex, tok = peek()
    if lex is not None and startsExpression(lex, tok):
        parseExpression()


@rule
def parseIoStmt():                        # IoStmt = fmt '.' ( OutputCall | InputCall )
    expect('fmt'); expect('.')
    numLine, lex, tok = getSymb()
    if lex in ('Print', 'Println'):
        parseOutputCall()
    elif lex == 'Scan':
        parseInputCall()
    else:
        failParse('невідповідність інструкцій', (numLine, lex, tok, 'Print, Println або Scan'))


@rule
def parseOutputCall():                    # OutputCall = ( 'Print' | 'Println' ) '(' [ ExpressionList ] ')'
    expect(peek()[0]); expect('(')
    if peek()[0] != ')':
        parseExpressionList()
    expect(')')


@rule
def parseInputCall():                     # InputCall = 'Scan' '(' AddrExpr { ',' AddrExpr } ')'
    expect('Scan'); expect('('); parseAddrExpr()
    while peek()[0] == ',':
        expect(','); parseAddrExpr()
    expect(')')


# ---------------------------------------------------------------- вирази
@rule
def parseExpressionList():                # ExpressionList = Expression { ',' Expression }
    parseExpression()
    while peek()[0] == ',':
        expect(','); parseExpression()


@rule
def parseExpression():                    # Expression = OrExpr
    parseOrExpr()


@rule
def parseOrExpr():                        # OrExpr = AndExpr { '||' AndExpr }
    parseAndExpr()
    while peek()[0] == '||':
        expect('||'); parseAndExpr()


@rule
def parseAndExpr():                       # AndExpr = RelExpr { '&&' RelExpr }
    parseRelExpr()
    while peek()[0] == '&&':
        expect('&&'); parseRelExpr()


@rule
def parseRelExpr():                       # RelExpr = AddExpr [ RelOp AddExpr ]
    parseAddExpr()
    if peek()[1] == 'rel_op':
        expect(peek()[0]); parseAddExpr()


@rule
def parseAddExpr():                       # AddExpr = MulExpr { AddOp MulExpr }
    parseMulExpr()
    while peek()[1] == 'add_op':
        expect(peek()[0]); parseMulExpr()


@rule
def parseMulExpr():                       # MulExpr = UnaryExpr { MulOp UnaryExpr }
    parseUnaryExpr()
    while peek()[1] == 'mult_op':
        expect(peek()[0]); parseUnaryExpr()


@rule
def parseUnaryExpr():                     # UnaryExpr = UnaryOp UnaryExpr | PowerExpr
    if peek()[0] in ('-', '+', '!'):
        expect(peek()[0]); parseUnaryExpr()
    else:
        parsePowerExpr()


@rule
def parsePowerExpr():                     # PowerExpr = PrimaryExpr [ '**' UnaryExpr ]
    parsePrimaryExpr()
    if peek()[0] == '**':
        expect('**'); parseUnaryExpr()


@rule
def parsePrimaryExpr():                   # PrimaryExpr = Operand { Selector | Index | Arguments }
    parseOperand()
    while peek()[0] in ('.', '[', '('):
        lex = peek()[0]
        if lex == '.':
            parseSelector()
        elif lex == '[':
            parseIndex()
        else:
            parseArguments()


@rule
def parseSelector():                      # Selector = '.' Ident
    expect('.'); parseToken(None, 'id')


@rule
def parseIndex():                         # Index = '[' Expression ']'
    expect('['); parseExpression(); expect(']')


@rule
def parseArguments():                     # Arguments = '(' [ ArgumentList ] ')'
    expect('(')
    if peek()[0] != ')':
        parseArgumentList()
    expect(')')


@rule
def parseArgumentList():                  # ArgumentList = Argument { ',' Argument }
    parseArgument()
    while peek()[0] == ',':
        expect(','); parseArgument()


@rule
def parseArgument():                      # Argument = AddrExpr | Expression
    if peek()[0] == '&':
        parseAddrExpr()
    else:
        parseExpression()


@rule
def parseAddrExpr():                      # AddrExpr = '&' PrimaryExpr
    expect('&'); parsePrimaryExpr()


@rule
def parseOperand():
    # Operand = Literal | Ident | Dereference | Conversion | LenCall | ArrayLit | '(' Expression ')'
    numLine, lex, tok = getSymb()
    if tok in LITERALS:
        parseLiteral()
    elif tok == 'id':
        parseToken(None, 'id')
    elif lex == '*':
        parseDereference()
    elif lex in TYPES:
        parseConversion()
    elif lex == 'len':
        parseLenCall()
    elif lex == '[':
        parseArrayLit()
    elif lex == '(':
        expect('('); parseExpression(); expect(')')
    else:
        failParse('невідповідність у виразі', (numLine, lex, tok, 'операнд'))


@rule
def parseLiteral():                       # Literal = IntLit | FloatLit | StringLit | BoolLit
    parseToken(None, peek()[1])


@rule
def parseDereference():                   # Dereference = '*' Ident
    expect('*'); parseToken(None, 'id')


@rule
def parseConversion():                    # Conversion = BasicType '(' Expression ')'
    parseBasicType(); expect('('); parseExpression(); expect(')')


@rule
def parseLenCall():                       # LenCall = len '(' Expression ')'
    expect('len'); expect('('); parseExpression(); expect(')')


@rule
def parseArrayLit():                      # ArrayLit = ArrayType LiteralValue
    parseArrayType(); parseLiteralValue()


@rule
def parseLiteralValue():                  # LiteralValue = '{' [ Element ElementTail ] '}'
    expect('{')
    if peek()[0] != '}':
        parseElement(); parseElementTail()
    expect('}')


@rule
def parseElementTail():                   # ElementTail = [ ',' [ Element ElementTail ] ]
    if peek()[0] == ',':
        expect(',')
        if peek()[0] != '}':
            parseElement(); parseElementTail()


@rule
def parseElement():                       # Element = Expression | LiteralValue
    if peek()[0] == '{':
        parseLiteralValue()
    else:
        parseExpression()


# ---------------------------------------------------------------- запуск
def parse(text):
    """Лексичний, а потім синтаксичний аналіз тексту програми."""
    global numRow, len_tableOfSymb, indt
    if krok_lex.analyze(text) != ('Lexer', True):
        return ('Parser', False)
    numRow, indt, len_tableOfSymb = 1, 0, len(tableOfSymb)
    print('-' * 30)
    print(('len_tableOfSymb', len_tableOfSymb))
    try:
        parseProgram()
        print('Parser: Синтаксичний аналіз завершився успішно')
        return ('Parser', True)
    except SystemExit as e:
        print('Parser: Аварійне завершення програми з кодом {0}'.format(e))
        return ('Parser', False)


if __name__ == '__main__':
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8')
    fileName = sys.argv[1] if len(sys.argv) > 1 else os.path.join('..', 'KP1', 'tests', 'base_example.krok')
    with open(fileName, encoding='utf-8') as f:
        parse(f.read())
