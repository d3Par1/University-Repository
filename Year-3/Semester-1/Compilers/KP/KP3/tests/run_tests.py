"""Тестування семантичного аналізатора krok_sem.py.

Кожен випадок має очікуваний результат: 'ok' (програма семантично правильна; додатково - очікувана кількість
попереджень) або код помилки трансляції за специфікацією мови (CE-01 ... CE-17). Групи тестів відповідають
розділам методички до роботи КП 3 (таблиця імен, неоголошені змінні, ініціація змінних, типи виразів,
типи інструкцій) та переліку помилок трансляції специфікації мови Krok.
"""
import contextlib, glob, importlib.util, io, os, sys, textwrap

HERE = os.path.dirname(os.path.abspath(__file__))
KP = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.dirname(HERE))
import krok_sem

# синтаксичний аналізатор з КП 2: семантичний аналізатор не повинен змінювати множину синтаксично правильних програм
spec = importlib.util.spec_from_file_location('kp2_parse', os.path.join(KP, 'KP2', 'krok_parse.py'))
kp2_parse = importlib.util.module_from_spec(spec); spec.loader.exec_module(kp2_parse)


def syntaxOk(text):
    with contextlib.redirect_stdout(io.StringIO()):
        return kp2_parse.parse(text)[1]


def run(text):
    """(результат, код помилки, кількість попереджень, вивід аналізатора)."""
    out = io.StringIO()
    with contextlib.redirect_stdout(out):
        ok = krok_sem.analyze(text)[1]
    return ok, krok_sem.lastError, len(krok_sem.warnings), out.getvalue()


def prog(decls='', main=''):
    return 'package main\n\nimport "fmt"\n\n' + decls + '\nfunc main() {\n' + main + '\n}\n'


SWAP = 'func swap(a, b *int) {\n\tt := *a\n\t*a = *b\n\t*b = t\n}\n'
FACT = 'func fact(n int) int {\n\tif n <= 1 {\n\t\treturn 1\n\t}\n\treturn n * fact(n-1)\n}\n'
NOP = 'func nop() {\n}\n'
TEMP = 'type Celsius float\ntype Fahrenheit float\n'
POINT = 'type Point struct {\n\tx, y float\n}\n'
EVEN_ODD = ('func isEven(n int) bool {\n\tif n == 0 {\n\t\treturn true\n\t}\n\treturn isOdd(n - 1)\n}\n'
            'func isOdd(n int) bool {\n\tif n == 0 {\n\t\treturn false\n\t}\n\treturn isEven(n - 1)\n}\n')
RECT_LATE = 'func area(r Rect) float {\n\treturn r.w * r.h\n}\n\ntype Rect struct {\n\tw, h float\n}\n'

G1 = '1 таблиця імен, унікальність оголошення'
G2 = '2 неоголошене ім\'я, області видимості'
G3 = '3 фіксація та перевірка ініціації'
G4 = '4 перевірка типів виразів'
G5 = '5 перевірка типів інструкцій'
G6 = '6 приведення типів'
G7 = '7 l-значення'
G8 = '8 виклики функцій'
G9 = '9 інструкція return'
G10 = '10 константні вирази'
G11 = '11 масиви'
G12 = '12 структури та іменовані типи'
G13 = '13 структура програми, імпорт'
G14 = '14 значення виклику, прості інструкції'
G15 = '15 некоректні фрагменти зі специфікації'

CASES = [   # (група, опис, текст програми, очікуваний результат: ('ok', попереджень) або 'CE-NN')
    (G1, 'x := 1 і x := 2 в одному блоці', prog(main='\tx := 1\n\tx := 2\n\tfmt.Println(x)'), 'CE-04'),
    (G1, 'var a, a int', prog(main='\tvar a, a int\n\ta = 1'), 'CE-04'),
    (G1, 'глобальна змінна і функція з тим самим ім\'ям', prog('var f int\n\nfunc f() {\n}\n'), 'CE-04'),
    (G1, 'два оголошення типу T', prog('type T int\ntype T float\n'), 'CE-04'),
    (G1, 'повторне ім\'я параметра', prog('func g(a int, a float) {\n}\n'), 'CE-04'),
    (G1, 'локальна змінна з ім\'ям параметра', prog('func g(a int) {\n\tvar a int\n\ta = 1\n}\n'), 'CE-04'),
    (G1, 'затінення у вкладеному блоці', prog(main='\tx := 1\n\t{\n\t\tx := 2.5\n\t\tfmt.Println(x)\n\t}\n\tfmt.Println(x)'), ('ok', 0)),
    (G1, 'затінення глобальної змінної', prog('var x int = 1\n', '\tx := "s"\n\tfmt.Println(x)'), ('ok', 0)),
    (G2, 'неоголошена змінна y = 5', prog(main='\ty = 5'), 'CE-03'),
    (G2, 'неоголошена змінна у виразі', prog(main='\tfmt.Println(z + 1)'), 'CE-03'),
    (G2, 'змінна блоку поза блоком', prog(main='\t{\n\t\tz := 1\n\t\tfmt.Println(z)\n\t}\n\tfmt.Println(z)'), 'CE-03'),
    (G2, 'змінна циклу після циклу', prog(main='\tfor i := 0; i < 2; i++ {\n\t}\n\tfmt.Println(i)'), 'CE-03'),
    (G2, 'неоголошений тип', prog(main='\tvar p Pt\n\tfmt.Println(p)'), 'CE-03'),
    (G2, 'константа до свого оголошення', prog('var a = B\nconst B = 1\n'), 'CE-03'),
    (G2, 'x := x + 1: праворуч зовнішня x', prog(main='\tx := 1\n\t{\n\t\tx := x + 1\n\t\tfmt.Println(x)\n\t}'), ('ok', 0)),
    (G2, 'взаємна рекурсія функцій', prog(EVEN_ODD, '\tfmt.Println(isEven(10))'), ('ok', 0)),
    (G2, 'тип оголошено після використання', prog(RECT_LATE, '\tvar r Rect\n\tr.w = 2\n\tr.h = 3\n\tfmt.Println(area(r))'), ('ok', 0)),
    (G2, 'ініціалізатор з типом, оголошеним нижче', prog('const C Celsius = 36.6\n\ntype Celsius float\n', '\tfmt.Println(C)'), ('ok', 0)),
    (G2, 'тип нижче, його довжина: константа між ними', prog('var r Row = [3]int{1, 2, 3}\nconst N = 3\ntype Row [N]int\n', '\tfmt.Println(r[2])'), ('ok', 0)),
    (G2, 'глобальна змінна нижче функції видима в тілі', prog('func show() {\n\tfmt.Println(late, K)\n}\n\nvar late int = 5\nconst K = 7\n', '\tshow()'), ('ok', 0)),
    (G2, 'довжина масиву: константа, оголошена нижче', prog('var a [M]int\nconst M = 2\n'), 'CE-03'),
    (G3, 'читання до присвоєння: попередження', prog(main='\tvar n int\n\tfmt.Println(n)'), ('ok', 1)),
    (G3, 'fmt.Scan(&n) ініціює змінну', prog(main='\tvar n int\n\tfmt.Scan(&n)\n\tfmt.Println(n)'), ('ok', 0)),
    (G3, 'присвоювання ініціює змінну', prog(main='\tvar n int\n\tn = 3\n\tfmt.Println(n)'), ('ok', 0)),
    (G3, 'передача &n ініціює змінну', prog(SWAP, '\tvar a, b int\n\tswap(&a, &b)\n\tfmt.Println(a, b)'), ('ok', 0)),
    (G3, 'присвоєння елементу ініціює масив', prog(main='\tvar a [3]int\n\ta[0] = 1\n\tfmt.Println(a[0])'), ('ok', 0)),
    (G3, 's = s + i у циклі до присвоєння', prog(main='\tvar s int\n\tfor i := 0; i < 3; i++ {\n\t\ts = s + i\n\t}\n\tfmt.Println(s)'), ('ok', 1)),
    (G3, 'len(a) не читає елементів', prog(main='\tvar a [3]int\n\tfor i := 0; i < len(a); i++ {\n\t\ta[i] = i\n\t}\n\tfmt.Println(a[1])'), ('ok', 0)),
    (G4, 'int + string', prog(main='\tx := 1 + "a"\n\tfmt.Println(x)'), 'CE-05'),
    (G4, 'string - string', prog(main='\tx := "a" - "b"\n\tfmt.Println(x)'), 'CE-05'),
    (G4, 'float % int', prog(main='\tx := 5.5 % 2\n\tfmt.Println(x)'), 'CE-05'),
    (G4, 'bool < bool', prog(main='\tok := true < false\n\tfmt.Println(ok)'), 'CE-05'),
    (G4, '!5', prog(main='\tb := !5\n\tfmt.Println(b)'), 'CE-05'),
    (G4, '-"s"', prog(main='\tx := -"s"\n\tfmt.Println(x)'), 'CE-05'),
    (G4, '1 && true', prog(main='\tok := 1 && true\n\tfmt.Println(ok)'), 'CE-05'),
    (G4, 'порівняння масивів', prog(main='\tvar a, b [2]int\n\tok := a == b\n\tfmt.Println(ok)'), 'CE-05'),
    (G4, 'Celsius + Fahrenheit', prog(TEMP, '\tvar c Celsius = 1\n\tvar f Fahrenheit = 2\n\tx := c + f\n\tfmt.Println(x)'), 'CE-05'),
    (G4, 'c*9/5 + 32: літерали набувають типу Celsius', prog(TEMP, '\tvar c Celsius = 36.6\n\tc = c*9/5 + 32\n\tfmt.Println(c)'), ('ok', 0)),
    (G4, '1 + 2.5 має тип float', prog(main='\tvar n int = 1 + 2.5\n\tfmt.Println(n)'), 'CE-05'),
    (G4, '7 / 2 має тип int, розширюється до float', prog(main='\tvar f float = 7 / 2\n\tfmt.Println(f)'), ('ok', 0)),
    (G4, 'індекс типу float', prog(main='\tvar a [3]int\n\ta[1.5] = 1'), 'CE-05'),
    (G4, 'індексування не масиву', prog(main='\tn := 5\n\tfmt.Println(n[0])'), 'CE-05'),
    (G4, 'індексування рядка', prog(main='\ts := "abc"\n\tfmt.Println(s[0])'), 'CE-05'),
    (G4, 'розіменування не вказівника', prog(main='\tn := 5\n\tfmt.Println(*n)'), 'CE-05'),
    (G4, 'len(5)', prog(main='\tfmt.Println(len(5))'), 'CE-05'),
    (G4, 'ім\'я функції як значення', prog(FACT, '\tx := fact\n\tfmt.Println(x)'), 'CE-05'),
    (G4, 'ім\'я типу як значення', prog(TEMP, '\tx := Celsius\n\tfmt.Println(x)'), 'CE-05'),
    (G4, 'x / 0: попередження про RE-01', prog(main='\tvar x int\n\tfmt.Scan(&x)\n\tfmt.Println(x / 0)'), ('ok', 1)),
    (G5, 'умова for типу int', prog(main='\tn := 3\n\tfor n {\n\t\tn--\n\t}'), 'CE-05'),
    (G5, 'умова трикомпонентного for типу int', prog(main='\tfor i := 0; i; i++ {\n\t}'), 'CE-05'),
    (G5, 'умова for є присвоєнням', prog(main='\tn := 3\n\tfor n = 1 {\n\t}'), 'CE-05'),
    (G5, 'присвоєння string змінній int', prog(main='\tn := 1\n\tn = "s"'), 'CE-05'),
    (G5, 'fmt.Println масиву', prog(main='\tvar a [2]int\n\tfmt.Println(a)'), 'CE-05'),
    (G5, 'fmt.Scan структури', prog(POINT, '\tvar p Point\n\tfmt.Scan(&p)'), 'CE-05'),
    (G5, 's++ для string', prog(main='\ts := "a"\n\ts++'), 'CE-05'),
    (G5, 'var a, b int = 1', prog(main='\tvar a, b int = 1\n\tfmt.Println(a, b)'), 'CE-05'),
    (G5, 'return "s" з функції int', prog('func f() int {\n\treturn "s"\n}\n'), 'CE-05'),
    (G5, 'константа типу масиву', prog('const A [2]int = [2]int{1, 2}\n'), 'CE-05'),
    (G5, 'вказівник у змінній', prog('func f(p *int) {\n\tq := p\n\t*q = 1\n}\n'), 'CE-05'),
    (G5, 'елемент літерала масиву типу string', prog(main='\ta := [2]int{1, "a"}\n\tfmt.Println(a[0])'), 'CE-05'),
    (G5, 'присвоєння Celsius <- Fahrenheit', prog(TEMP, '\tvar c Celsius\n\tvar f Fahrenheit\n\tc = f'), 'CE-05'),
    (G5, 'присвоєння Celsius <- float', prog(TEMP, '\tvar c Celsius\n\tx := 1.5\n\tc = x'), 'CE-05'),
    (G6, 'int(2.9), float(3), Celsius(n)', prog(TEMP, '\tn := 2\n\tvar c Celsius = Celsius(n)\n\tfmt.Println(int(2.9), float(3), c)'), ('ok', 0)),
    (G6, 'float(true)', prog(main='\tfmt.Println(float(true))'), 'CE-06'),
    (G6, 'string(5)', prog(main='\tfmt.Println(string(5))'), 'CE-06'),
    (G6, 'приведення структури Point(p)', prog(POINT, '\tvar p Point\n\tq := Point(p)\n\tfmt.Println(q.x)'), 'CE-06'),
    (G7, 'присвоєння результату виклику', prog(FACT, '\tfact(1) = 2'), 'CE-07'),
    (G7, '5++', prog(main='\t5++'), 'CE-07'),
    (G7, 'fmt.Scan(&N) для константи', prog('const N = 3\n', '\tfmt.Scan(&N)'), 'CE-07'),
    (G7, 'присвоєння параметру-вказівнику', prog('func f(p, q *int) {\n\tp = q\n}\n'), 'CE-07'),
    (G7, 'адреса літерала &3', prog(SWAP, '\tx := 1\n\tswap(&3, &x)'), 'CE-07'),
    (G8, 'неправильна кількість аргументів', prog(FACT, '\tfmt.Println(fact())'), 'CE-08'),
    (G8, 'аргумент неправильного типу', prog(FACT, '\tfmt.Println(fact("a"))'), 'CE-08'),
    (G8, 'значення замість адреси для *int', prog(SWAP, '\ta := 1\n\tb := 2\n\tswap(a, b)'), 'CE-08'),
    (G8, 'адреса для параметра-значення', prog(FACT, '\tn := 3\n\tfmt.Println(fact(&n))'), 'CE-08'),
    (G8, '&float для параметра *int', prog(SWAP, '\tvar x float\n\ty := 1\n\tswap(&x, &y)'), 'CE-08'),
    (G8, 'виклик змінної n(1)', prog(main='\tn := 1\n\tn(1)'), 'CE-08'),
    (G8, 'параметр-вказівник передано далі', prog(SWAP + 'func swap2(a, b *int) {\n\tswap(a, b)\n}\n', '\tx := 1\n\ty := 2\n\tswap2(&x, &y)\n\tfmt.Println(x, y)'), ('ok', 0)),
    (G9, 'немає return у гілці', prog('func f(a bool) int {\n\tif a {\n\t\treturn 1\n\t}\n}\n'), 'CE-09'),
    (G9, 'return без виразу у функції int', prog('func f() int {\n\treturn\n}\n'), 'CE-09'),
    (G9, 'return з виразом у функції без результату', prog('func f() {\n\treturn 1\n}\n'), 'CE-09'),
    (G9, 'цикл не є завершальною інструкцією', prog('func f() int {\n\tfor true {\n\t\treturn 1\n\t}\n}\n'), 'CE-09'),
    (G9, 'if/else з return в обох гілках', prog('func sign(x int) int {\n\tif x < 0 {\n\t\treturn -1\n\t} else if x == 0 {\n\t\treturn 0\n\t} else {\n\t\treturn 1\n\t}\n}\n', '\tfmt.Println(sign(5))'), ('ok', 0)),
    (G10, 'const Z = 1 / 0', prog('const Z = 1 / 0\n'), 'CE-10'),
    (G10, 'цілий літерал > max int', prog(main='\tx := 9223372036854775808\n\tfmt.Println(x)'), 'CE-10'),
    (G10, 'переповнення в константному виразі', prog('const B = 9223372036854775807 + 1\n'), 'CE-10'),
    (G10, 'від\'ємний цілий показник степеня', prog('const P = 2 ** -1\n'), 'CE-10'),
    (G10, 'ініціалізатор константи не константний', prog(main='\tn := 1\n\tconst C = n\n\tfmt.Println(C)'), 'CE-10'),
    (G10, 'мінімальне int: -9223372036854775807 - 1', prog('const Min = -9223372036854775807 - 1\n', '\tfmt.Println(Min)'), ('ok', 0)),
    (G10, 'дійсний літерал 1e400', prog(main='\tfmt.Println(1e400)'), 'CE-10'),
    (G11, 'var a [0]int', prog(main='\tvar a [0]int\n\tfmt.Println(len(a))'), 'CE-11'),
    (G11, 'довжина масиву є змінною', prog(main='\tn := 3\n\tvar a [n]int\n\tfmt.Println(len(a))'), 'CE-11'),
    (G11, 'довжина масиву типу float', prog(main='\tvar a [2.5]int\n\tfmt.Println(len(a))'), 'CE-11'),
    (G11, 'зайві елементи в літералі', prog(main='\ta := [2]int{1, 2, 3}\n\tfmt.Println(a[0])'), 'CE-11'),
    (G11, 'константний індекс поза межами', prog(main='\tvar a [3]int\n\ta[3] = 1'), 'CE-11'),
    (G11, 'довжина масиву: [N]int, [len(a)]float', prog('const N = 4\n', '\tvar a [N]int\n\tvar b [len(a)]float\n\tb[3] = 1\n\tfmt.Println(len(b), b[3])'), ('ok', 0)),
    (G12, 'повторне поле структури', prog('type P struct {\n\tx int\n\tx float\n}\n'), 'CE-12'),
    (G12, 'неіснуюче поле', prog(POINT, '\tvar p Point\n\tp.z = 1'), 'CE-12'),
    (G12, 'поле значення, що не є структурою', prog(main='\tn := 1\n\tfmt.Println(n.x)'), 'CE-12'),
    (G12, 'рекурсивне включення структури', prog('type Node struct {\n\tnext Node\n}\n'), 'CE-12'),
    (G12, 'рекурсія через масив', prog('type Node struct {\n\tkids [2]Node\n}\n'), 'CE-12'),
    (G12, 'циклічне визначення типів', prog('type A B\ntype B A\n'), 'CE-12'),
    (G12, 'іменований масив <- неіменований', prog('type Row [3]int\n', '\tvar r Row = [3]int{1, 2, 3}\n\tfmt.Println(r[2])'), ('ok', 0)),
    (G13, 'ім\'я пакета не main', 'package util\n\nfunc main() {\n}\n', 'CE-13'),
    (G13, 'немає функції main', 'package main\n\nfunc f() {\n}\n', 'CE-13'),
    (G13, 'main з параметром', 'package main\n\nfunc main(x int) {\n}\n', 'CE-13'),
    (G13, 'fmt без import', 'package main\n\nfunc main() {\n\tfmt.Println(1)\n}\n', 'CE-14'),
    (G13, 'import "os"', 'package main\n\nimport "os"\n\nfunc main() {\n}\n', 'CE-14'),
    (G13, 'повторний import "fmt"', 'package main\n\nimport "fmt"\nimport "fmt"\n\nfunc main() {\n\tfmt.Println(1)\n}\n', 'CE-14'),
    (G14, 'x := nop(): виклик без результату', prog(NOP, '\tx := nop()\n\tfmt.Println(x)'), 'CE-15'),
    (G14, 'fmt.Println(nop())', prog(NOP, '\tfmt.Println(nop())'), 'CE-15'),
    (G14, '1 + nop()', prog(NOP, '\ty := 1 + nop()\n\tfmt.Println(y)'), 'CE-15'),
    (G14, 'глобальна змінна = виклик', prog(FACT + 'var g = fact(3)\n'), 'CE-16'),
    (G14, 'глобальна змінна = інша змінна', prog('var a = 1\nvar b = a\n'), 'CE-16'),
    (G14, 'інструкція-вираз x + 1', prog(main='\tx := 1\n\tx + 1'), 'CE-17'),
    (G14, 'інструкція-вираз int(x)', prog(main='\tx := 1.5\n\tint(x)'), 'CE-17'),
    (G14, 'a[0] := 1', prog(main='\tvar a [2]int\n\ta[0] := 1'), 'CE-17'),
    (G14, ':= у наступному кроці for', prog(main='\tfor i := 0; i < 3; j := 1 {\n\t}'), 'CE-17'),
    (G14, 'виклик функції як інструкція', prog(FACT + NOP, '\tfact(3)\n\tnop()'), ('ok', 0)),
    (G15, 'var n int = 2.5', prog(main='\tvar n int = 2.5\n\tfmt.Println(n)'), 'CE-05'),
    (G15, 'if n { } для n int', prog(main='\tn := 1\n\tif n {\n\t}'), 'CE-05'),
    (G15, 'const N = 3 і N = 4', prog('const N = 3\n', '\tN = 4'), 'CE-07'),
    (G15, 'x := 1 і x := 2 в одному блоці', prog(main='\tx := 1\n\tx := 2'), 'CE-04'),
    (G15, 'func f() int { if a { return 1 } }', prog('var a bool\n\nfunc f() int { if a { return 1 } }\n'), 'CE-09'),
    (G15, 'var a [0]int', prog(main='\tvar a [0]int'), 'CE-11'),
    (G15, 'fmt.Println(int("5"))', prog(main='\tfmt.Println(int("5"))'), 'CE-06'),
]
WARN = {'08_runtime_errors.krok': 1}
CODE = {'leading_zero.krok': 'CE-01'}
programs = sorted(glob.glob(os.path.join(KP, 'KP1', 'tests', '*.krok')))
invalid = sorted(glob.glob(os.path.join(KP, '..', 'RGR', 'RGR1', 'examples', 'invalid', '*.krok')))
CASES += [('16 коректні програми', os.path.basename(p), open(p, encoding='utf-8').read(),
           ('ok', WARN.get(os.path.basename(p), 0))) for p in programs]
CASES += [('17 лексичні та синтаксичні помилки', os.path.basename(p), open(p, encoding='utf-8').read(),
           CODE.get(os.path.basename(p), 'CE-02')) for p in invalid]


def check(expected, ok, code, nwarn, text):
    # синтаксичний вердикт збігається з КП 2: синтаксично неправильну програму відхилено,
    # синтаксично правильну - не відхилено як лексично чи синтаксично неправильну
    if syntaxOk(text) == (code in ('CE-01', 'CE-02')):
        return False
    if isinstance(expected, tuple):
        return ok and nwarn == expected[1]
    return not ok and code == expected


if __name__ == '__main__':
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8')
    verbose = '-v' in sys.argv
    failed = 0
    for group, name, text, expected in CASES:
        ok, code, nwarn, out = run(text)
        good = check(expected, ok, code, nwarn, text)
        failed += not good
        got = 'ok, попереджень {0}'.format(nwarn) if ok else code
        exp = 'ok, попереджень {0}'.format(expected[1]) if isinstance(expected, tuple) else expected
        print('{0:4} {1:>2}  {2:46} {3}'.format('ok' if good else 'FAIL', group.split()[0], name, exp if good else
                                               'очікувалось {0}, отримано {1}'.format(exp, got)))
        if verbose or not good:
            print('\n'.join('       | ' + line for line in out.splitlines()
                            if line.startswith(('Semantic', '\t', 'Parser', 'Lexer'))))
    covered = sorted({e for _, _, _, e in CASES if isinstance(e, str)})
    print('-' * 40)
    print(textwrap.fill('коди помилок, що перевіряються: ' + ', '.join(covered), 90, subsequent_indent='    '))
    missing = ['CE-{0:02d}'.format(n) for n in range(1, 18) if 'CE-{0:02d}'.format(n) not in covered]
    if missing:
        print('НЕ перевіряються: ' + ', '.join(missing))
    print('усі {0} тестів пройдено'.format(len(CASES)) if not failed else 'не пройдено: {0}'.format(failed))
    sys.exit(1 if failed or missing else 0)
