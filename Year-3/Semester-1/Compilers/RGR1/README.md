# РГР 1 — Специфікація мови програмування Krok

**Дисципліна:** Основи розробки трансляторів (2026-2027)
**Виконавець:** Степаненко Назар Юрійович, ТВ-43 (спільний проєкт з Аніщенком Артемом)
**Рівень:** 2 (Основний) · **Мова-прототип:** Go · **Варіант інструкцій:** `for` (з передумовою, трикомпонентний) + `if / else if / else`

## Результат

| Файл | Опис |
|---|---|
| `Специфікація мови Krok — Степаненко Н.Ю., ТВ-43.docx` | специфікація для здачі |
| `Специфікація мови Krok — Степаненко Н.Ю., ТВ-43.pdf` | та сама специфікація у PDF |

## Структура

```
grammar/krok.ebnf        повна граматика (РБНФ) — єдине джерело для діаграм і тексту
spec/krok-spec.md        текст специфікації з директивами збирання
spec/title-template.docx зразок титульного аркуша
examples/*.krok          контрольні приклади (base_example.krok — базовий приклад)
examples/invalid/*.krok  некоректні програми, які мають відхилятися
diagrams/                згенеровані діаграми Вірта (SVG + PNG)
tools/                   перевірка граматики, лексер/парсер, генератор діаграм і документа
```

## Збирання

Потрібні Python 3.12+ (`pip install python-docx railroad-diagrams`), Microsoft Edge (рендер PNG) та Microsoft Word (оновлення змісту й PDF).

```sh
python tools/check_grammar.py                              # КС, марні символи, ліва рекурсія, LL(1), алфавіт
python tools/krok_parse.py --coverage examples/*.krok      # приклади розбираються; базовий покриває всі токени
python tools/krok_parse.py --invalid examples/invalid/*.krok
python tools/gen_diagrams.py                               # grammar/krok.ebnf -> diagrams/
python tools/build_spec.py                                 # spec/krok-spec.md -> .docx
pwsh tools/finalize.ps1                                    # оновити зміст у Word, експорт PDF
```

`build_spec.py` зупиняє збирання, якщо граматика не проходить самоперевірку або якщо якесь правило не наведене в розділах 2–3 рівно один раз разом зі своєю діаграмою.
