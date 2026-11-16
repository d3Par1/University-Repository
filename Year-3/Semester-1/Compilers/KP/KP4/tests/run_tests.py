"""Тестування транслятора krok_translate.py: програма транслюється в ПОЛІЗ і виконується стековою машиною
PSM.py (версія 0.3, файл з матеріалів курсу, без змін). Для кожного тесту перевіряється: трансляція успішна,
PSM завершила роботу без помилки, виведення збігається з очікуваним, стек після виконання порожній.
Очікуване виведення прикладів програм узято зі специфікації мови Krok (розд. «Приклади програм»).
"""
import builtins, contextlib, importlib.util, io, os, re, shutil, sys, tempfile, textwrap

HERE = os.path.dirname(os.path.abspath(__file__))
KP4 = os.path.dirname(HERE)
KP = os.path.dirname(KP4)
sys.path.insert(0, KP4)
import krok_translate as tr
import krok_codegen as gen


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


psm = load('PSM', os.path.join(KP4, 'PSM.py'))          # стекова машина з матеріалів курсу
sys.path.insert(0, os.path.join(KP, 'KP3', 'tests'))
kp3 = load('kp3_tests', os.path.join(KP, 'KP3', 'tests', 'run_tests.py'))
TMP = tempfile.mkdtemp(prefix='krok_psm_')


def translate(text, name='t'):
    out = io.StringIO()
    with contextlib.redirect_stdout(out):
        ok = tr.translate(text, name) == ('codeGeneration', True)
    return ok, tr.lastError, out.getvalue()


def execute(name, inputs):
    """Виконати модуль name з теки TMP: (стан, рядки виведення, залишок стека)."""
    shutil.rmtree(TMP, ignore_errors=True)
    gen.savePostfixCode(TMP)
    it, out, cwd, real_input = iter(inputs), io.StringIO(), os.getcwd(), builtins.input

    def fake_input(prompt=''):
        try:
            return next(it)
        except StopIteration:
            raise EOFError

    os.chdir(TMP)
    builtins.input = fake_input
    psm.console.indent = 0
    vm, state = psm.VirtualPostfixMachine(name), 'ok'
    try:
        with contextlib.redirect_stdout(out):
            vm.load_module()
            vm.run()
    except SystemExit:
        state = 'stop'
    except Exception as e:
        state = type(e).__name__
    finally:
        builtins.input = real_input
        os.chdir(cwd)
    return state, out.getvalue().split('\n')[:-1], vm.stack


def prog(decls='', main=''):
    return ('package main\n\nimport "fmt"\n\n' + textwrap.dedent(decls).strip('\n') + '\n\nfunc main() {\n' +
            textwrap.indent(textwrap.dedent(main).strip('\n'), '\t') + '\n}\n')


def example(name):
    return open(os.path.join(KP, 'KP1', 'tests', name + '.krok'), encoding='utf-8').read()


POINT = 'type Point struct {\n\tx, y float\n}\n'
SWAP = 'func swap(a, b *int) {\n\tt := *a\n\t*a = *b\n\t*b = t\n}\n'
RE04 = 'runtime error RE-04 at {0}: index {1} out of range [0, {2}]'
HALT = 'ПОМИЛКА: Невідома мітка для JMP переходу: runtime_error'
G = ['1 вирази та присвоювання', '2 розгалуження і цикли', '3 функції', '4 масиви та структури',
     '5 введення і виведення', '6 помилки часу виконання', '7 конструкції, яких немає в PSM',
     '8 приклади програм зі специфікації']

CASES = [   # (група, опис, текст програми, введення, очікуване виведення | 'PSM' | ('stop', виведення))
    (G[0], 'цілі: + - * / % (ділення з відкиданням дробової частини)',
     prog(main='a := 7\nb := -3\nfmt.Println(a + b, a - b, a * b, a / b, a % b, -a / 2, -a % 2)'), [],
     ['4 10 -21 -2 1 -3 -1']),
    (G[0], 'дійсні та змішані вирази: неявне int -> float',
     prog(main='x := 2.5\nn := 3\nvar f float = n\nfmt.Println(x + n, n / x, x * 2, n / 2, float(n) / 2, f, int(x * 3))'),
     [], ['5.5 1.2 5.0 1 1.5 3.0 7']),
    (G[0], 'степінь: правоасоціативний, цілий і дійсний',
     prog(main='n := 3\nx := 2.0\nfmt.Println(2 ** n ** 2, x ** n, n ** 0, x ** -1, -n ** 2)'), [],
     ['512 8.0 1 0.5 -9']),
    (G[0], 'порівняння та логічні оператори',
     prog(main='a := 5\nb := 7.5\nok := a < b && !(a == 5) || a != 6\nfmt.Println(a < b, a >= 6, ok, ok == false)'),
     [], ['true false true false']),
    (G[0], 'рядки: конкатенація, len константи, // у рядку',
     prog(main='s := "Kr" + "ok"\nt := s + "!"\nfmt.Println(t, len("абв"), "a//b", t + "//")'), [],
     ['Krok! 3 a//b Krok!//']),
    (G[0], 'іменовані типи та константи',
     prog('type Celsius float\nconst Zero Celsius = -273.15\nconst N = 4', 'var c Celsius = 36.6\nc = c*9/5 + 32\n'
          'var k [N]int\nfmt.Println(c, Zero, N * 2, len(k))'), [],
     ['{0} -273.15 8 4'.format(repr(36.6 * 9.0 / 5.0 + 32.0))]),
    (G[0], '&& і || за скороченою схемою (індекс поза межами не обчислюється)',
     prog(main='var a [3]int\ni := 5\nif i < 3 && a[i] > 0 {\n\tfmt.Println("in")\n} else {\n\tfmt.Println("out")\n}\n'
               'if i >= 3 || a[i] == 0 {\n\tfmt.Println("ok")\n}'), [], ['out', 'ok']),
    (G[0], 'операнди обчислюються зліва направо (виклик змінює глобальну змінну)',
     prog('var g int\n\nfunc bump() int {\n\tg = g + 10\n\treturn 1\n}', 'g = 1\nx := g + bump()\nfmt.Println(x, g)'),
     [], ['2 11']),
    (G[0], 'інкремент, декремент, нульові значення',
     prog(main='var n int\nvar x float\nvar b bool\nvar s string\nn++\nn++\nx--\nfmt.Println(n, x, b, s + "|")'), [],
     ['2 -1.0 false |']),
    (G[1], 'if / else if / else', prog(main='var t int\nfmt.Scan(&t)\nif t < 0 {\n\tfmt.Println("мінус")\n} else if t == 0 '
                                           '{\n\tfmt.Println("нуль")\n} else {\n\tfmt.Println("плюс")\n}'), ['0'], ['нуль']),
    (G[1], 'for з передумовою і вкладений трикомпонентний',
     prog(main='s := 0\nn := 3\nfor n > 0 {\n\tfor i := 0; i < n; i++ {\n\t\ts = s + i*n\n\t}\n\tn--\n}\nfmt.Println(s, n)'),
     [], ['{0} 0'.format(sum(i * n for n in (3, 2, 1) for i in range(n)))]),
    (G[1], 'крок циклу - виклик функції з параметром-вказівником',
     prog('func inc(p *int) {\n\t*p = *p + 1\n}', 'for i := 0; i < 3; inc(&i) {\n\tfmt.Println(i)\n}'), [],
     ['0', '1', '2']),
    (G[2], 'рекурсія та взаємна рекурсія',
     prog('func fact(n int) int {\n\tif n <= 1 {\n\t\treturn 1\n\t}\n\treturn n * fact(n-1)\n}\n' + kp3.EVEN_ODD,
          'fmt.Println(fact(10), isEven(7), isOdd(7))'), [], ['3628800 false true']),
    (G[2], 'масив і структура передаються за значенням',
     prog(POINT + 'func zero(v [3]int, p Point) int {\n\tv[0] = 9\n\tp.x = 9\n\treturn v[0]\n}',
          'var p Point\na := [3]int{1, 2, 3}\nfmt.Println(zero(a, p), a[0], p.x)'), [], ['9 1 0.0']),
    (G[2], 'параметри-вказівники: змінна, елемент масиву, структура',
     prog(POINT + SWAP + 'func move(p *Point, d float) {\n\t*p.x = *p.x + d\n}',
          'a := [3]int{1, 2, 3}\ni := 0\nx := 7\nswap(&a[i], &a[i+2])\nswap(&x, &a[1])\nvar p Point\nmove(&p, 2)\n'
          'fmt.Println(a[0], a[1], a[2], x, p.x)'), [], ['3 7 1 2 2.0']),
    (G[2], 'параметр-вказівник передається далі; та сама змінна двічі',
     prog(SWAP + 'func twice(a, b *int) {\n\tswap(a, b)\n\tswap(a, b)\n\t*a = *a + 1\n}',
          'x := 1\ny := 2\ntwice(&x, &y)\nswap(&x, &x)\nfmt.Println(x, y)'), [], ['2 2']),
    (G[2], 'функція повертає структуру та масив',
     prog(POINT + 'func mk(a, b float) Point {\n\tvar p Point\n\tp.x = a\n\tp.y = b\n\treturn p\n}\n'
          'func row(k int) [3]int {\n\treturn [3]int{k, k + 1, k + 2}\n}',
          'q := mk(1, 2)\nr := row(5)\nfmt.Println(q.x + mk(3, 4).y, r[2], row(1)[1])'), [], ['5.0 7 2']),
    (G[2], 'глобальні змінні з функцій; виклик як інструкція',
     prog(POINT + 'var g int\nvar gp Point\nvar total int = 5\n\nfunc set(v int, p Point) int {\n\tg = v\n\tgp = p\n'
          '\ttotal = total + g\n\treturn g\n}', 'var p Point\np.y = 1.5\nset(4, p)\nfmt.Println(g, gp.y, total)'), [],
     ['4 1.5 9']),
    (G[3], 'змінний індекс: читання, запис, ++, двовимірний масив',
     prog(main='var a [4]int\nvar m [2][3]int\nfor i := 0; i < 4; i++ {\n\ta[i] = i * i\n}\nk := 1\na[k+1]++\n'
               'for i := 0; i < 2; i++ {\n\tfor j := 0; j < 3; j++ {\n\t\tm[i][j] = i*3 + j\n\t}\n}\nm[k][k]++\n'
               'fmt.Println(a[k], a[k+1], a[a[1]+2], m[k][2], m[k][k], m[0][k])'), [], ['1 5 9 5 5 1']),
    (G[3], 'присвоєння масиву копіює елементи; рядок масиву як значення',
     prog(main='a := [3]int{1, 2, 3}\nb := a\nb[0] = 9\nvar m [2][3]int\ni := 1\nm[i] = [3]int{7, 8, 9}\nr := m[i]\n'
               'm[0] = r\nfmt.Println(a[0], b[0], r[1], m[0][2], m[1][0])'), [], ['1 9 8 9 7']),
    (G[3], 'масив структур і структура з масивом',
     prog('type P struct {\n\tv [2]int\n\tname string\n}', 'var ps [2]P\nps[1].v[0] = 5\ni := 1\nps[i].name = "x"\n'
          'c := ps[i]\nps[0] = c\nfmt.Println(ps[i].v[0], ps[0].name, ps[0].v[i-1])'), [], ['5 x 5']),
    (G[3], 'літерал масиву: пропущені елементи нульові; індексування літерала',
     prog(main='a := [4]int{1, 2}\ng := [2][2]float{{1}, {3, 4}}\ni := 1\n'
               'fmt.Println(a[2], a[3], g[0][0], g[0][1], g[1][1], [3]int{4, 5, 6}[i])'), [], ['0 0 1.0 0.0 4.0 5']),
    (G[3], 'затінення: змінні з однаковим ім\'ям у вкладених блоках',
     prog('var x int = 1', 'fmt.Println(x)\nx := 2.5\n{\n\tx := "s"\n\tfmt.Println(x)\n}\nfor x := 0; x < 1; x++ {\n'
          '\tfmt.Println(x)\n}\nfmt.Println(x)'), [], ['1', 's', '0', '2.5']),
    (G[4], 'fmt.Print накопичує рядок; порожній Println; \\n у константі',
     prog(main='fmt.Print("a")\nfmt.Print(1, 2)\nfmt.Println("b", true)\nfmt.Println()\nfmt.Print("x\\ny\\n")\n'
               'fmt.Print()\nfmt.Print("кінець")'), [], ['a12b true', '', 'x', 'y', 'кінець']),
    (G[4], 'fmt.Scan: int, float, string, елемент масиву, поле структури',
     prog(POINT, 'var n int\nvar s string\nvar a [2]float\nvar p Point\ni := 1\nfmt.Print("n? ")\n'
          'fmt.Scan(&n, &s, &a[i], &p.y)\nfmt.Println(n + 1, s, a[1], p.y)'), ['41', 'Krok', '2.5', '3'],
     ['n? ', '42 Krok 2.5 3.0']),
    (G[4], 'подання значень: float, bool, іменований тип',
     prog('type Flag bool', 'x := 0.1\nvar f Flag = true\nfmt.Println(x + 0.2, 1e16, 2.5e-5, x * 50, f, !f)'), [],
     ['{0} 1e+16 2.5e-05 5.0 true false'.format(repr(0.1 + 0.2))]),
    (G[5], 'RE-04: індекс поза межами (у функції, після fmt.Print)',
     prog('func get(v [3]int, i int) int {\n\treturn v[i]\n}', 'var a [3]int\nfmt.Print("до")\nfmt.Println(get(a, 3))'),
     [], ('stop', ['до', RE04.format(6, 3, 2), HALT])),
    (G[5], 'RE-01: ділення на нуль', prog(main='z := 0\nfmt.Println(10 / z)'), [], ('stop', ['ПОМИЛКА: Ділення на нуль'])),
    (G[5], 'RE-05: від\'ємний цілий показник степеня', prog(main='n := -2\nfmt.Println(3 ** n)'), [],
     ('stop', ['runtime error RE-05 at 9: negative exponent -2', HALT])),
    (G[6], 'порівняння рядків', prog(main='s := "a"\nfmt.Println(s == "b")'), [], 'PSM'),
    (G[6], 'len рядка, прочитаного fmt.Scan', prog(main='var s string\nfmt.Scan(&s)\nfmt.Println(len(s))'), [], 'PSM'),
    (G[6], 'введення значення типу bool', prog(main='var b bool\nfmt.Scan(&b)'), [], 'PSM'),
    (G[6], 'символ нового рядка в рядку-змінній', prog(main='s := "a\\nb"\nfmt.Println(s)'), [], 'PSM'),
    (G[7], 'base_example.krok', example('base_example'), ['4', '7'],
     ['n = 7 m = 4 n! = 5040', '0 1 last', 'Krok:\t"крок" \\ 2026', '43.6 5.0 true true 0.0015 true 512 10']),
    (G[7], '01_types_conversions.krok', example('01_types_conversions'), [], ['3 3.5 3.5 -3 3 Krok 4.0 4']),
    (G[7], '02_if_else.krok (22)', example('02_if_else'), ['22'], ['комфортно']),
    (G[7], '02_if_else.krok (20)', example('02_if_else'), ['20'], ['спекотно або рівно 20']),
    (G[7], '02_if_else.krok (-3.5)', example('02_if_else'), ['-3.5'], ['мороз']),
    (G[7], '03_for_loops.krok', example('03_for_loops'), ['2026'],
     ['сума цифр: 10', '1\t2\t3\t', '2\t4\t6\t', '3\t6\t9\t', '8 5 2 -1 ']),
    (G[7], '04_recursion.krok', example('04_recursion'), [], ['55 12 0.125 0.125']),
    (G[7], '05_reference_params.krok', example('05_reference_params'), ['5', '2', '9', '1', '7', '3'], ['1 2 3 5 7 9']),
    (G[7], '06_arrays_structs.krok', example('06_arrays_structs'), [], ['3 16 5.0 AB copy']),
    (G[7], '07_scope_shadowing.krok', example('07_scope_shadowing'), [], ['1', '3.5', 'local', '0', '1', 'global x = 1']),
    (G[7], '08_runtime_errors.krok (5 1)', example('08_runtime_errors'), ['5', '1'],
     ('stop', [RE04.format(14, 5, 2), HALT])),
    (G[7], '08_runtime_errors.krok (1 0)', example('08_runtime_errors'), ['1', '0'],
     ('stop', ['0', 'ПОМИЛКА: Ділення на нуль'])),
    (G[7], 'nested.krok', example('nested'), [], ['2 4 6 велике парне']),
]
# вирази зі змінними (код виконує PSM) та ті самі вирази з константами (значення обчислює транслятор)
EXPRS = ['a / b', 'a % b', 'b / a', 'b % a', '-a / c', '-a % c', 'a % -c', 'a ** c', 'b ** c', 'c ** c ** c', 'a + b * c',
         '(a + b) * c - a / c', 'x / y', 'x * y - a', 'a / x', 'x ** c', 'y ** c', 'a < b', 'x >= y', 'a == c + 5',
         'a != b && x > y', 'a > b || !(x < y)', 'float(a) / float(c)', 'int(x * y)', 'int(-x)', 'a - -b', 'x + a % c']
VALUES = {'a': '7', 'b': '-3', 'c': '2', 'x': '2.5', 'y': '-0.5'}


def differential():
    line = ', '.join(EXPRS)
    const = re.sub(r'\b([abcxy])\b', lambda m: '(' + VALUES[m.group(1)] + ')', line)
    decl = '\n'.join('{0} := {1}'.format(n, v) for n, v in VALUES.items())
    res = []
    for text in (prog(main=decl + '\nfmt.Println(' + line + ')'), prog(main='fmt.Println(' + const + ')')):
        ok, _, _ = translate(text)
        res.append(execute('t', [])[1] if ok else ['помилка трансляції'])
    return res


if __name__ == '__main__':
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8')
    failed = 0
    for group, name, text, inputs, expected in CASES:
        ok, code, log = translate(text)
        if expected == 'PSM':
            good, got = not ok and code == 'PSM', 'трансляцію відхилено' if not ok else 'трансльовано'
        elif not ok:
            good, got = False, 'помилка трансляції ' + str(code)
        else:
            state, out, stack = execute('t', inputs)
            if state == 'stop':               # повідомлення PSM про помилку - з відступом і порожнім рядком
                out = [l.strip() for l in out if l.strip()]
            want = expected if isinstance(expected, tuple) else ('ok', expected)
            good = (state, out) == want and (state != 'ok' or stack == [])
            got = '{0}: {1}{2}'.format(state, out, '' if stack == [] or state != 'ok' else ' стек: ' + str(stack))
        failed += not good
        print('{0:4} {1}  {2}'.format('ok' if good else 'FAIL', group.split()[0], name))
        if not good:
            print('       очікувалось {0}\n       отримано    {1}'.format(expected, got))
            print('\n'.join('       | ' + l for l in log.splitlines() if l.startswith(('\t', 'Semantic ERROR', 'Trans'))))
    run, const = differential()
    good = run == const and len(run) == 1 and len(run[0].split()) == len(EXPRS)
    failed += not good
    print('{0:4} 9  {1} виразів: виконання в PSM = обчислення транслятором'.format('ok' if good else 'FAIL', len(EXPRS)))
    if not good:
        print('       PSM:        {0}\n       транслятор: {1}'.format(run, const))
    bad = []
    for group, name, text, expected in kp3.CASES:     # семантичні перевірки КП 3 не змінилися
        ok, code, _ = translate(text)
        if (ok, None) != (True, None) and isinstance(expected, tuple) and code != 'PSM' or \
                isinstance(expected, str) and (ok or code != expected):
            bad.append(name)
    failed += bool(bad)
    print('{0:4} 10 {1} тестів семантичного аналізатора КП 3: ті самі коди помилок'
          .format('ok' if not bad else 'FAIL', len(kp3.CASES)) + ('' if not bad else ': ' + '; '.join(bad)))
    shutil.rmtree(TMP, ignore_errors=True)
    print('-' * 40)
    total = len(CASES) + 2
    print('усі {0} тестів пройдено'.format(total) if not failed else 'не пройдено: {0} із {1}'.format(failed, total))
    sys.exit(1 if failed else 0)
