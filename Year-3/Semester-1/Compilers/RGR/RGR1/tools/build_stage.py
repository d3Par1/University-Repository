"""Build a partial version of the specification for one of the five РГР stages.

The РГР is submitted in five stages across the semester, so this script renders only the
chosen top-level chapters into a separate .docx, leaving the full document untouched.
Cross-references to chapters that are not included keep the numbers of the full document.

Usage:
    python tools/build_stage.py 1-2          # chapters 1 and 2  -> "... (етап 1).docx"
    python tools/build_stage.py 1-2,10 --name "етап 2"
"""
import re
import sys
from pathlib import Path

import build_spec

ROOT = Path(__file__).resolve().parent.parent


def parse_spec(arg):
    wanted = set()
    for part in arg.split(","):
        if "-" in part:
            lo, hi = part.split("-")
            wanted |= set(range(int(lo), int(hi) + 1))
        else:
            wanted.add(int(part))
    return wanted


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    wanted = parse_spec(args[0] if args else "1-2")
    name = "етап 1"
    if "--name" in sys.argv:
        name = sys.argv[sys.argv.index("--name") + 1]

    lines = build_spec.SPEC.read_text(encoding="utf-8").splitlines()
    labels, _ = build_spec.number_labels(lines)

    # keep only the wanted chapters (numbered '# ' headings, in order)
    kept, chapter, keep = [], 0, False
    for line in lines:
        if line.startswith("# "):
            if not line.rstrip().endswith("{-}"):
                chapter += 1
            keep = chapter in wanted
        if keep:
            kept.append(line)

    # freeze cross-references to the numbering of the full document
    text = "\n".join(kept)
    text = re.sub(r"\[@([\w:-]+)\]", lambda m: labels.get(m.group(1), m.group(1)), text)

    stage_md = ROOT / "spec" / "krok-spec-stage.md"
    stage_md.write_text(text + "\n", encoding="utf-8", newline="\n")

    out = ROOT / f"Специфікація мови Krok — Степаненко Н.Ю., ТВ-43 ({name}).docx"
    build_spec.SPEC = stage_md
    build_spec.OUT = out
    build_spec.Builder.verify_rules = lambda self: print(
        f"partial build: chapters {sorted(wanted)} (rule coverage not checked)")
    build_spec.Builder().build()
    stage_md.unlink()


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    main()
