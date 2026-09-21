"""Build the Krok specification (.docx) from spec/krok-spec.md, the grammar, diagrams and examples.

Usage:  python tools/build_spec.py
Markdown subset and directives understood by this script:
    # / ## / ### Title {#sec:id}        numbered headings
    Таблиця{#tab:id}: caption           caption of the pipe table that follows
    @rules{#fig:id} caption | A B C     EBNF text of rules A B C + their diagrams as one figure
    @code path                          program listing with line numbers
    @tokentable{#tab:id} caption        token table generated from tools/tokens.py
    @checks{#tab:id} caption            grammar self-check results
    @fullgrammar                        complete grammar file
    [@sec:id] [@tab:id] [@fig:id]       cross-references
"""
import copy
import re
import struct
import sys
from pathlib import Path

from docx import Document
from docx.enum.section import WD_SECTION
from docx.enum.table import WD_TABLE_ALIGNMENT
from docx.enum.text import WD_ALIGN_PARAGRAPH, WD_BREAK, WD_LINE_SPACING
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.shared import Cm, Pt, RGBColor

import check_grammar
import ebnf
from tokens import token_table

ROOT = Path(__file__).resolve().parent.parent
SPEC = ROOT / "spec" / "krok-spec.md"
TITLE_TEMPLATE = ROOT / "spec" / "title-template.docx"
OUT = ROOT / "Специфікація мови Krok — Степаненко Н.Ю., ТВ-43.docx"

FONT = "Times New Roman"
MONO = "Consolas"
TEXT_WIDTH_CM = 17.0


# ------------------------------------------------------------------ numbering
def number_labels(lines):
    """First pass: assign numbers to headings, tables and figures."""
    labels, headings, h = {}, [], [0, 0, 0]
    tab = fig = 0
    for line in lines:
        m = re.match(r"^(#{1,3}) (.+?)(?:\s*\{#([\w:-]+)\})?$", line)
        if m:
            if m.group(2).endswith("{-}"):     # heading without a number
                headings.append("")
                continue
            level = len(m.group(1))
            h[level - 1] += 1
            for i in range(level, 3):
                h[i] = 0
            if level == 1:
                tab = fig = 0
            number = ".".join(str(x) for x in h[:level])
            headings.append(number)
            if m.group(3):
                labels[m.group(3)] = number
            continue
        m = re.match(r"^(?:Таблиця|@tokentable|@checks|@grammarstats|@transforms)\{#([\w:-]+)\}", line)
        if m:
            tab += 1
            labels[m.group(1)] = f"{h[0]}.{tab}"
            continue
        m = re.match(r"^@rules\{#([\w:-]+)\}", line)
        if m:
            fig += 1
            labels[m.group(1)] = f"{h[0]}.{fig}"
    return labels, headings


# ------------------------------------------------------------------ docx helpers
def set_run_font(run, name=FONT, size=None, bold=None, italic=None):
    run.font.name = name
    rpr = run._element.get_or_add_rPr()
    fonts = rpr.find(qn("w:rFonts"))
    if fonts is None:
        fonts = OxmlElement("w:rFonts")
        rpr.insert(0, fonts)
    for attr in ("w:ascii", "w:hAnsi", "w:cs", "w:eastAsia"):
        fonts.set(qn(attr), name)
    if size:
        run.font.size = Pt(size)
    if bold is not None:
        run.bold = bold
    if italic is not None:
        run.italic = italic
    return run


def style_fonts(style, name, size, bold=False, italic=False, color=RGBColor(0, 0, 0)):
    style.font.name, style.font.size, style.font.bold, style.font.italic = name, Pt(size), bold, italic
    style.font.color.rgb = color
    rpr = style.element.get_or_add_rPr()
    fonts = rpr.find(qn("w:rFonts"))
    if fonts is None:
        fonts = OxmlElement("w:rFonts")
        rpr.insert(0, fonts)
    for attr in ("w:ascii", "w:hAnsi", "w:cs", "w:eastAsia"):
        fonts.set(qn(attr), name)
    for attr in ("w:asciiTheme", "w:hAnsiTheme", "w:eastAsiaTheme", "w:cstheme"):
        fonts.attrib.pop(qn(attr), None)


def shade(paragraph, fill="F2F2F2"):
    ppr = paragraph._p.get_or_add_pPr()
    shd = OxmlElement("w:shd")
    shd.set(qn("w:val"), "clear")
    shd.set(qn("w:color"), "auto")
    shd.set(qn("w:fill"), fill)
    ppr.append(shd)


def add_field(paragraph, instr):
    run = paragraph.add_run()
    for kind, text in (("begin", None), ("instr", instr), ("separate", None), ("text", "Оновіть поле (F9)"), ("end", None)):
        if kind == "instr":
            el = OxmlElement("w:instrText")
            el.set(qn("xml:space"), "preserve")
            el.text = text
        elif kind == "text":
            el = OxmlElement("w:t")
            el.text = text
        else:
            el = OxmlElement("w:fldChar")
            el.set(qn("w:fldCharType"), kind)
        run._r.append(el)
    return run


def png_size(path):
    with open(path, "rb") as f:
        head = f.read(24)
    return struct.unpack(">II", head[16:24])


def repeat_header(row):
    trpr = row._tr.get_or_add_trPr()
    el = OxmlElement("w:tblHeader")
    el.set(qn("w:val"), "true")
    trpr.append(el)


def keep_with_next(paragraph):
    paragraph.paragraph_format.keep_with_next = True


# ------------------------------------------------------------------ builder
class Builder:
    def __init__(self):
        self.doc = Document()
        self.labels = {}
        self.used_rules = []
        self.grammar = ebnf.load(check_grammar.GRAMMAR)
        self._setup_styles()

    # ---- styles and page
    def _setup_styles(self):
        d = self.doc
        normal = d.styles["Normal"]
        style_fonts(normal, FONT, 14)
        pf = normal.paragraph_format
        pf.line_spacing = 1.5
        pf.space_after = Pt(0)
        pf.space_before = Pt(0)
        pf.first_line_indent = Cm(1.25)
        pf.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
        for level, size, italic in ((1, 16, False), (2, 14, False), (3, 14, True)):
            st = d.styles[f"Heading {level}"]
            style_fonts(st, FONT, size, bold=True, italic=italic)
            st.paragraph_format.first_line_indent = Cm(1.25) if level > 1 else Cm(0)
            st.paragraph_format.space_before = Pt(12 if level > 1 else 0)
            st.paragraph_format.space_after = Pt(6 if level > 1 else 12)
            st.paragraph_format.line_spacing = 1.5
            st.paragraph_format.keep_with_next = True
            st.paragraph_format.alignment = WD_ALIGN_PARAGRAPH.LEFT
        for level in (1, 2, 3):
            toc = d.styles[f"TOC {level}"] if f"TOC {level}" in [s.name for s in d.styles] else None
            if toc is not None:
                style_fonts(toc, FONT, 14)
        sec = d.sections[0]
        sec.page_width, sec.page_height = Cm(21), Cm(29.7)
        sec.left_margin, sec.right_margin = Cm(2.5), Cm(1.5)
        sec.top_margin, sec.bottom_margin = Cm(2), Cm(2)
        sec.different_first_page_header_footer = True
        fp = sec.footer.paragraphs[0]
        fp.alignment = WD_ALIGN_PARAGRAPH.RIGHT
        fp.paragraph_format.first_line_indent = Cm(0)
        set_run_font(add_field(fp, "PAGE"), size=12)
        settings = d.settings.element
        upd = OxmlElement("w:updateFields")
        upd.set(qn("w:val"), "true")
        settings.append(upd)

    # ---- title page
    def title_page(self):
        tpl = Document(str(TITLE_TEMPLATE))
        p = tpl.paragraphs

        def put(par, text):
            par.runs[0].text = text
            for r in par.runs[1:]:
                r.text = ""

        put(p[15], "Мова програмування Krok")
        put(p[16], "Рівень 2 Основний")
        put(p[21], "\tСтепаненко Назар Юрійович")
        put(p[22], "\tАніщенко Артем Олександрович")
        put(p[23], "")
        put(p[27], "Виконав: студент ІІІ курсу, групи ТВ-43")
        p[29].runs[1].text = "Степаненко Назар Юрійович\t\t\t\t_ _ _  _ _ _  _ _ _ "
        put(p[48], "Київ – 2026")
        for par in p:
            for r in par.runs:
                r.font.highlight_color = None
                if r.font.size is None:
                    r.font.size = Pt(12)
        # the body font is larger than in the template: drop a few spacer paragraphs
        for par in p[40:46]:
            par._p.getparent().remove(par._p)
        body = self.doc.element.body
        anchor = body[0]
        for el in tpl.element.body:
            if el.tag == qn("w:sectPr"):
                continue
            el = copy.deepcopy(el)
            for tag in ("w:shd", "w:highlight"):
                for node in list(el.iter(qn(tag))):
                    node.getparent().remove(node)
            for mark in el.iter(qn("w:pPr")):
                rpr = mark.find(qn("w:rPr"))
                if rpr is None:
                    rpr = OxmlElement("w:rPr")
                    mark.append(rpr)
                if rpr.find(qn("w:sz")) is None:
                    sz = OxmlElement("w:sz")
                    sz.set(qn("w:val"), "24")
                    rpr.append(sz)
            for ppr in el.iter(qn("w:pPr")):
                ind = ppr.find(qn("w:ind"))
                if ind is None:
                    ind = OxmlElement("w:ind")
                    ppr.append(ind)
                if ind.get(qn("w:firstLine")) is None:
                    ind.set(qn("w:firstLine"), "0")
                sp = ppr.find(qn("w:spacing"))
                if sp is None:
                    sp = OxmlElement("w:spacing")
                    ppr.append(sp)
                sp.set(qn("w:line"), "240")
                sp.set(qn("w:lineRule"), "auto")
            anchor.addprevious(el)
        # drop trailing empty paragraphs of the template so the title fits one page
        self.page_break()

    def page_break(self):
        par = self.doc.add_paragraph()
        par.paragraph_format.first_line_indent = Cm(0)
        par.add_run().add_break(WD_BREAK.PAGE)

    def toc(self):
        par = self.doc.add_paragraph()
        par.alignment = WD_ALIGN_PARAGRAPH.CENTER
        par.paragraph_format.first_line_indent = Cm(0)
        set_run_font(par.add_run("ЗМІСТ"), bold=True, size=16)
        par = self.doc.add_paragraph()
        par.paragraph_format.first_line_indent = Cm(0)
        add_field(par, 'TOC \\o "1-3" \\h \\z \\u')
        self.page_break()

    # ---- inline text
    def refs(self, text):
        def sub(m):
            key = m.group(1)
            if key not in self.labels:
                raise KeyError(f"unknown reference {key}")
            return self.labels[key]
        return re.sub(r"\[@([\w:-]+)\]", sub, text)

    def inline(self, par, text, size=None, bold=None, italic=None):
        text = self.refs(text)
        pos = 0
        for m in re.finditer(r"`([^`]+)`|\*\*(.+?)\*\*|\*([^*\s`][^*`]*)\*", text):
            if m.start() > pos:
                set_run_font(par.add_run(text[pos:m.start()]), size=size, bold=bold, italic=italic)
            if m.group(1) is not None:
                set_run_font(par.add_run(m.group(1)), MONO, size=(size or 14) - 1.5, bold=bold, italic=italic)
            elif m.group(2) is not None:
                self.inline(par, m.group(2), size=size, bold=True, italic=italic)
            else:
                self.inline(par, m.group(3), size=size, bold=bold, italic=True)
            pos = m.end()
        if pos < len(text):
            set_run_font(par.add_run(text[pos:]), size=size, bold=bold, italic=italic)

    def paragraph(self, text):
        self.inline(self.doc.add_paragraph(), text)

    def list_item(self, marker, text):
        par = self.doc.add_paragraph()
        pf = par.paragraph_format
        pf.left_indent = Cm(1.25 + 0.75)
        pf.first_line_indent = Cm(-0.75)
        set_run_font(par.add_run(f"{marker}\t"))
        pf.tab_stops.add_tab_stop(Cm(2.0))
        self.inline(par, text)

    def heading(self, level, title):
        num = self.heading_numbers.pop(0)
        title = title.replace("{-}", "").strip()
        if level == 1 and self._started:
            self.page_break()
        self._started = True
        par = self.doc.add_paragraph(style=f"Heading {level}")
        run = par.add_run(f"{num} {title}" if num else title)
        set_run_font(run, bold=True, size=16 if level == 1 else 14, italic=level == 3)

    # ---- blocks
    def code(self, lines, numbered=False, size=10.5):
        width = len(str(len(lines)))
        for i, line in enumerate(lines, 1):
            par = self.doc.add_paragraph()
            pf = par.paragraph_format
            pf.first_line_indent = Cm(0)
            pf.left_indent = Cm(0.3)
            pf.line_spacing = 1.0
            pf.alignment = WD_ALIGN_PARAGRAPH.LEFT
            pf.keep_together = True
            if i < len(lines) and len(lines) <= 25:
                pf.keep_with_next = True
            shade(par)
            text = line.replace("\t", "    ")
            if numbered:
                set_run_font(par.add_run(f"{i:>{width}}  "), MONO, 9.5).font.color.rgb = RGBColor(0x80, 0x80, 0x80)
            set_run_font(par.add_run(text if text else " "), MONO, size)
        spacer = self.doc.add_paragraph()
        spacer.paragraph_format.line_spacing = 0.6

    def caption(self, kind, label, text):
        par = self.doc.add_paragraph()
        par.paragraph_format.first_line_indent = Cm(0)
        par.paragraph_format.space_before = Pt(4)
        par.paragraph_format.space_after = Pt(8 if kind == "Рисунок" else 2)
        par.alignment = WD_ALIGN_PARAGRAPH.CENTER if kind == "Рисунок" else WD_ALIGN_PARAGRAPH.LEFT
        if kind == "Таблиця":
            keep_with_next(par)
        self.inline(par, f"{kind} {self.labels[label]} – {text}", size=13)

    def table(self, rows, label, caption, widths=None, font=12, header=True):
        self.caption("Таблиця", label, caption)
        t = self.doc.add_table(rows=len(rows), cols=len(rows[0]))
        t.style = "Table Grid"
        t.alignment = WD_TABLE_ALIGNMENT.CENTER
        for r, row in enumerate(rows):
            # schema order inside w:trPr: cantSplit must precede tblHeader
            t.rows[r]._tr.get_or_add_trPr().append(OxmlElement("w:cantSplit"))
            if r == 0 and header:
                repeat_header(t.rows[0])
            for c, cell_text in enumerate(row):
                cell = t.cell(r, c)
                par = cell.paragraphs[0]
                pf = par.paragraph_format
                pf.first_line_indent = Cm(0)
                pf.line_spacing = 1.0
                pf.alignment = WD_ALIGN_PARAGRAPH.LEFT
                if r == 0 and header:
                    pf.alignment = WD_ALIGN_PARAGRAPH.CENTER
                    self.inline(par, str(cell_text), size=font, bold=True)
                else:
                    self.inline(par, str(cell_text), size=font)
                if widths:
                    cell.width = Cm(widths[c])
        spacer = self.doc.add_paragraph()
        spacer.paragraph_format.line_spacing = 1.0

    def rules(self, label, caption, names):
        missing = [n for n in names if n not in self.grammar.rules]
        if missing:
            raise KeyError(f"@rules {label}: unknown rules {missing}")
        self.used_rules.extend(names)
        self.code("\n".join(self.grammar.rules[n].source for n in names).split("\n"), size=9.5)
        for n in names:
            img = ROOT / "diagrams" / f"{n}.png"
            w_px, _ = png_size(img)
            width_cm = min(w_px / 2 / 96 * 2.54 * 0.95, TEXT_WIDTH_CM - 0.5)
            par = self.doc.add_paragraph()
            par.alignment = WD_ALIGN_PARAGRAPH.CENTER
            par.paragraph_format.first_line_indent = Cm(0)
            par.paragraph_format.line_spacing = 1.0
            par.paragraph_format.keep_with_next = True
            par.add_run().add_picture(str(img), width=Cm(width_cm))
        self.caption("Рисунок", label, caption)

    # ---- main loop
    def build(self):
        lines = SPEC.read_text(encoding="utf-8").splitlines()
        self.labels, self.heading_numbers = number_labels(lines)
        self._started = False
        self.title_page()
        self.toc()
        i = 0
        while i < len(lines):
            line = lines[i]
            if not line.strip():
                i += 1
                continue
            m = re.match(r"^(#{1,3}) (.+?)(?:\s*\{#([\w:-]+)\})?$", line)
            if m:
                self.heading(len(m.group(1)), m.group(2))
                i += 1
                continue
            if line.startswith("```"):
                j = i + 1
                while not lines[j].startswith("```"):
                    j += 1
                self.code(lines[i + 1:j])
                i = j + 1
                continue
            m = re.match(r"^@rules\{#([\w:-]+)\}\s*(.+?)\s*\|\s*(.+)$", line)
            if m:
                self.rules(m.group(1), m.group(2), m.group(3).split())
                i += 1
                continue
            m = re.match(r"^@code\s+(\S+)$", line)
            if m:
                src = (ROOT / m.group(1)).read_text(encoding="utf-8").rstrip("\n").split("\n")
                self.code(src, numbered=True)
                i += 1
                continue
            m = re.match(r"^@tokentable\{#([\w:-]+)\}\s*(.+)$", line)
            if m:
                rows = [("Код", "Приклади лексем", "Токен", "Клас", "Неформальний опис")]
                rows += [(str(c), f"`{ex}`" if tok not in ("id", "intnum", "floatnum", "strlit") else ex,
                          tok, cls, desc) for c, ex, tok, cls, desc in token_table()]
                self.table(rows, m.group(1), m.group(2), widths=[1.2, 4.6, 2.6, 1.6, 6.0])
                i += 1
                continue
            m = re.match(r"^@transforms\{#([\w:-]+)\}\s*(.+)$", line)
            if m:
                self.table(transform_rows(self.grammar), m.group(1), m.group(2), widths=[4.5, 12.0], font=11)
                i += 1
                continue
            m = re.match(r"^@grammarstats\{#([\w:-]+)\}\s*(.+)$", line)
            if m:
                self.table(grammar_stats(self.grammar), m.group(1), m.group(2), widths=[7.0, 5.0, 4.5])
                i += 1
                continue
            m = re.match(r"^@checks\{#([\w:-]+)\}\s*(.+)$", line)
            if m:
                _, _, report = check_grammar.run()
                rows = [("№", "Властивість граматики", "Результат")]
                rows += [(str(k), title, "виконується" if ok else "НЕ виконується")
                         for k, (title, ok, _) in enumerate(report, 1)]
                if not all(ok for _, ok, _ in report):
                    raise SystemExit("grammar self-check failed; fix grammar before building the spec")
                self.table(rows, m.group(1), m.group(2), widths=[1.0, 12.5, 3.0])
                i += 1
                continue
            if line.strip() == "@fullgrammar":
                self.code(check_grammar.GRAMMAR.read_text(encoding="utf-8").rstrip("\n").split("\n"), size=9.5)
                i += 1
                continue
            m = re.match(r"^Таблиця\{#([\w:-]+)\}:\s*(.+)$", line)
            if m:
                j = i + 1
                while j < len(lines) and not lines[j].startswith("|"):
                    j += 1
                rows = []
                while j < len(lines) and lines[j].startswith("|"):
                    if not re.match(r"^\|[\s\-|:]+\|$", lines[j]):
                        rows.append(split_row(lines[j]))
                    j += 1
                self.table(rows, m.group(1), m.group(2))
                i = j
                continue
            m = re.match(r"^- (.+)$", line)
            if m:
                self.list_item("–", m.group(1))
                i += 1
                continue
            m = re.match(r"^(\d+)\. (.+)$", line)
            if m:
                self.list_item(f"{m.group(1)}.", m.group(2))
                i += 1
                continue
            self.paragraph(line)
            i += 1
        self.verify_rules()
        self.doc.save(str(OUT))
        print(f"written {OUT.name}")

    def verify_rules(self):
        counts = {}
        for n in self.used_rules:
            counts[n] = counts.get(n, 0) + 1
        dup = sorted(n for n, c in counts.items() if c > 1)
        missing = sorted(set(self.grammar.rules) - set(counts))
        if dup or missing:
            raise SystemExit(f"rule/diagram coverage broken: duplicated={dup} missing={missing}")
        print(f"all {len(counts)} rules presented exactly once with diagrams")


def transform_rows(g):
    """Rows for the table of standard grammar transformations (lecture 3)."""
    ll = ebnf.LL1(g, "Program", check_grammar.TOKEN_CLASSES)
    nullable = sorted(n for n in ll.names if ll.nullable[n])
    chains = ebnf.chain_productions(g)
    cycles = ebnf.chain_cycles(g)
    useless = sorted(set(g.rules) - ebnf.reachable(g, check_grammar.ROOTS)) + \
              sorted(set(g.rules) - ebnf.productive(g))
    return [
        ("Трансформація", "Стан граматики мови Krok"),
        ("Видалення ε-продукцій",
         f"правил виду `A = ε` немає: порожній ланцюжок породжують лише метасимволи `[ ]` та `{{ }}`. "
         f"Анульовними є {len(nullable)} нетермінали: " + ", ".join(f"`{n}`" for n in nullable) +
         ". Перевірка вимагає, щоб тіло `[ ]` чи `{{ }}` само не було анульовним"),
        ("Видалення марних символів",
         "не потрібне: марних символів немає" if not useless else ", ".join(useless)),
        ("Видалення ланцюгових продукцій",
         f"{len(chains)} ланцюгових продукцій збережено свідомо: вони або класифікують конструкції "
         "(`Statement`, `Operand`, `Type`, `Token`), або іменують рівні пріоритету "
         "(`Expression = OrExpr`). Для LL(1)-аналізу вони нешкідливі — кожній відповідає один виклик "
         "процедури рекурсивного спуску"),
        ("Видалення циклів",
         "не потрібне: циклів `A ⇒⁺ A` немає" if not cycles else "; ".join(" → ".join(c) for c in cycles)),
        ("Факторизація",
         "застосовано до `SimpleStmt` (спільний префікс — вираз) та до груп операторів `AddOp`, `MulOp`"),
        ("Заміна рекурсії ітерацією",
         "застосовано до всіх рівнів виразів і до списків `X { ',' X }` — див. розд. 8.2"),
        ("Нормальні форми Хомського та Грейбах",
         "не застосовуються: вони потрібні алгоритмам загального КВ-розбору (наприклад, CYK), "
         "тоді як для рекурсивного спуску достатньо відсутності лівої рекурсії та факторизації; "
         "крім того, бінарна нормальна форма несумісна з метасимволами РБНФ"),
    ]


def grammar_stats(g):
    """Rows for the table with the quantitative characteristics of Γ = (N, T, P, S)."""
    tokens = check_grammar.TOKEN_CLASSES

    def part(roots, stop):
        names = ebnf.reachable(g, roots, stop=stop) - stop
        terms, specials = set(), set()
        for n in names:
            terms |= ebnf.terminals(g.rules[n].body)
            specials |= {s for s in _specials(g.rules[n].body)}
        return names, terms, specials

    syn_names, syn_terms, _ = part(["Program"], tokens)
    lex_names, lex_terms, lex_special = part(["SourceText", "Alphabet"], frozenset())
    return [
        ("Характеристика", "Синтаксична граматика", "Лексична граматика"),
        ("Аксіома S", "`Program`", "`SourceText`, `Alphabet`"),
        ("Нетермінали |N|", str(len(syn_names)), str(len(lex_names))),
        ("Термінали |T|", f"{len(syn_terms)} + {len(tokens)} класи токенів", f"{len(lex_terms)} символів + {len(lex_special)} спец. послідовності"),
        ("Правила РБНФ |P|", str(len(syn_names)), str(len(lex_names))),
        ("Клас мови", "контекстно вільна (тип 2), LL(1)", "регулярна (тип 3); ДСА"),
    ]


def _specials(node, acc=None):
    acc = set() if acc is None else acc
    if isinstance(node, ebnf.Special):
        acc.add(node.text)
    elif isinstance(node, (ebnf.Seq, ebnf.Alt)):
        for i in node.items:
            _specials(i, acc)
    elif isinstance(node, (ebnf.Opt, ebnf.Rep)):
        _specials(node.item, acc)
    return acc


def split_row(line):
    """Split a pipe-table row, ignoring pipes inside `code` spans."""
    cells, cur, in_code = [], "", False
    for ch in line.strip()[1:-1]:
        if ch == "`":
            in_code = not in_code
        if ch == "|" and not in_code:
            cells.append(cur.strip())
            cur = ""
        else:
            cur += ch
    cells.append(cur.strip())
    return cells


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    Builder().build()
