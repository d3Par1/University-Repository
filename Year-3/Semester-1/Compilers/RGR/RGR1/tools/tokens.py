"""Single source of truth for Krok lexemes: used by the reference lexer and the token table in the spec."""

KEYWORDS = [
    "package", "import", "func", "var", "const", "type", "struct",
    "if", "else", "for", "return",
    "int", "float", "bool", "string", "len",
    "fmt", "Print", "Println", "Scan",
]
BOOL_LITERALS = ["true", "false"]

# lexeme -> (token, description); ordered longest first inside each group for maximal munch
OPERATORS = {
    "**": ("pow_op", "піднесення до степеня"),
    "==": ("rel_op", "дорівнює"),
    "!=": ("rel_op", "не дорівнює"),
    "<=": ("rel_op", "менше або дорівнює"),
    ">=": ("rel_op", "більше або дорівнює"),
    "&&": ("and_op", "логічне «і»"),
    "||": ("or_op", "логічне «або»"),
    ":=": ("define_op", "оголошення з ініціалізацією"),
    "++": ("incdec_op", "інкремент"),
    "--": ("incdec_op", "декремент"),
    "+": ("add_op", "додавання, унарний плюс, конкатенація"),
    "-": ("add_op", "віднімання, унарний мінус"),
    "*": ("mult_op", "множення; розіменування"),
    "/": ("mult_op", "ділення"),
    "%": ("mult_op", "остача від ділення"),
    "<": ("rel_op", "менше"),
    ">": ("rel_op", "більше"),
    "!": ("not_op", "логічне заперечення"),
    "=": ("assign_op", "присвоєння"),
    "&": ("addr_op", "взяття адреси"),
}
DELIMITERS = {
    "(": ("paren", "ліва кругла дужка"),
    ")": ("paren", "права кругла дужка"),
    "[": ("bracket", "ліва квадратна дужка"),
    "]": ("bracket", "права квадратна дужка"),
    "{": ("brace", "ліва фігурна дужка"),
    "}": ("brace", "права фігурна дужка"),
    ",": ("punct", "кома"),
    ";": ("punct", "крапка з комою (явна або вставлена)"),
    ".": ("punct", "крапка (доступ до поля, fmt.)"),
}

# Tokens after which a line break (or end of file) produces an implicit ';'
SEMICOLON_TRIGGERS = {"Ident", "IntLit", "FloatLit", "StringLit", "true", "false",
                      "return", "int", "float", "bool", "string", ")", "]", "}", "++", "--"}


def token_table():
    """Rows (code, examples, token, class, description) for the spec, in presentation order.

    The class column follows the classification used in Медведєва/Третяк [2]:
    TRM — keywords, operators and delimiters; IDN — identifiers; CON — literals.
    Rows marked "—" are lexemes that are not tokens (they never reach the parser).
    """
    rows = [
        ("x, total, Point, _tmp1", "id", "IDN", "ідентифікатор"),
        ("0, 42, 1000", "intnum", "CON", "ціле без знака"),
        ("3.14, 2., .5, 1e3, 6.02E+23", "floatnum", "CON", "дійсне без знака"),
        ('"", "Krok", "a\\tb\\n"', "strlit", "CON", "рядковий літерал"),
        ("true", "boolval", "CON", "логічне значення"),
        ("false", "boolval", "CON", "логічне значення"),
    ]
    rows += [(k, "keyword", "TRM", f"символ {k}") for k in KEYWORDS]
    order = ["+", "-", "*", "/", "%", "**", "==", "!=", "<", "<=", ">", ">=",
             "&&", "||", "!", "=", ":=", "++", "--", "&"]
    rows += [(op, OPERATORS[op][0], "TRM", OPERATORS[op][1]) for op in order]
    rows += [(d, tok, "TRM", desc) for d, (tok, desc) in DELIMITERS.items()]
    rows += [
        ("// коментар", "comment", "—", "однорядковий коментар (ігнорується)"),
        ("\\32", "ws", "—", "пробіл (ігнорується)"),
        ("\\t", "ws", "—", "горизонтальна табуляція (ігнорується)"),
        ("\\n, \\r\\n", "eol", "—", "кінець рядка (ігнорується; може породити ;)"),
    ]
    return [(i + 1, *r) for i, r in enumerate(rows)]
