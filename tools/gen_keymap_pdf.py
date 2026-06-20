#!/usr/bin/env python3
"""
Generate a printable PDF of the keymap (one page per layer).

Renders an HTML page representing the Atreus matrix with all three layers,
then drives headless Chrome to print it to PDF.

Usage:
    python3 tools/gen_keymap_pdf.py
        -> writes docs/keymap.html and docs/keymap.pdf

The keymap is defined inline below — if you change keymap.c, update the
LAYERS constant here to match (or rewrite this to parse the .c file).
"""

from __future__ import annotations
import subprocess
from pathlib import Path

# --- Keymap definition (mirrors keyboards/atreus_trackpoint/keymaps/default/keymap.c) ---
# Each row is a list of 12 strings. None for the matrix gaps in rows 0-2.

BASE = [
    ["Q",   "W",   "E",   "R",   "T",   None,  None,  "Y",   "U",   "I",   "O",   "P"],
    ["A",   "S",   "D",   "F",   "G",   None,  None,  "H",   "J",   "K",   "L",   ";"],
    ["Z",   "X",   "C",   "V",   "B",   "BT1", "BT2", "N",   "M",   ",",   ".",   "/"],
    ["Hyper", "Esc\n/ Ctrl", "Tab", "⌘", "⇧", "Space\n/ SYM", "Bsp\n/ NAV", "Enter", "⇧", "⌘", "⌥", "'"],
]

NAV = [
    ["F1",   "F2",   "F3",   "F4",   "F5",   None,  None,  "F6",    "F7",    "F8",    "F9",    "F10"],
    ["▶❚",  "Vol−", "Vol+", "Mute", "—",    None,  None,  "←",     "↓",     "↑",     "→",     "Del"],
    ["⏮",   "⏭",    "—",    "—",    "—",    "BT1", "BT2", "Home",  "PgDn",  "PgUp",  "End",   "F11"],
    ["—",   "—",   "—",   "—",   "—",   "—",  "—",  "—",   "—",   "—",   "—",   "F12"],
]

SYM = [
    ["!",   "@",   "#",   "$",   "%",   None,  None,  "^",   "&",   "*",   "(",   ")"],
    ["1",   "2",   "3",   "4",   "5",   None,  None,  "6",   "7",   "8",   "9",   "0"],
    ["`",   "~",   "[",   "]",   "\\",  "BT1", "BT2", "−",   "=",   "{",   "}",   "|"],
    ["—",   "—",   "—",   "—",   "—",   "—",  "—",  "—",   "_",   "+",   "—",   "—"],
]

LAYERS = [
    ("Layer 0 — BASE",
     "QWERTY with all modifiers on the thumb row. "
     "Tap the Esc key for Esc, hold for Ctrl. Tap Space for space, hold to "
     "activate SYM. Tap Bsp for backspace, hold to activate NAV.",
     BASE),
    ("Layer 1 — NAV (hold the Backspace key)",
     "Vim-style arrows on HJKL, page navigation around them, F-keys on the "
     "number row, media keys on the left. Everything else falls through to BASE.",
     NAV),
    ("Layer 2 — SYM (hold the Space key)",
     "Numbers on the home row (one row down from a normal keyboard), shifted "
     "symbols on the top row, brackets and operators on the third row. "
     "Modifier thumbs are transparent — Shift/Cmd/Alt still work while held.",
     SYM),
]

# --- HTML generation ---

CSS = """
@page { size: letter landscape; margin: 0.5in; }
body { font-family: -apple-system, "SF Pro Text", Helvetica, Arial, sans-serif;
       color: #222; margin: 0; padding: 0; }
.layer { page-break-after: always; padding: 0; }
.layer:last-child { page-break-after: auto; }
h1 { font-size: 18pt; margin: 0 0 4pt 0; font-weight: 600; }
.desc { font-size: 9.5pt; color: #555; margin-bottom: 14pt; max-width: 9in; line-height: 1.4; }

.matrix { display: grid; grid-template-columns: repeat(12, 1fr); gap: 6pt;
          max-width: 9in; }
.key { border: 1pt solid #888; border-radius: 5pt; padding: 8pt 4pt;
       min-height: 38pt; display: flex; align-items: center; justify-content: center;
       text-align: center; font-size: 11pt; line-height: 1.2;
       white-space: pre-line; background: #fafafa; }
.key.gap { border: 1pt dashed #ccc; background: transparent; color: #bbb;
           font-size: 7pt; }
.key.mod { background: #e8e8f5; font-weight: 600; }
.key.layer { background: #e8f5e8; font-weight: 600; }
.key.special { background: #fff5e6; }
.key.mouse { background: #fde8e8; font-size: 9pt; }
.key.transparent { color: #aaa; font-style: italic; background: #f5f5f5; }
.key.hyper { background: #f0e8f5; font-weight: 600; }

/* Spacer rows 0-2 are 10 keys with a 2-wide gap in the middle.
   Row 3 uses all 12 columns. */
.row { display: contents; }

.legend { font-size: 9pt; color: #666; margin-top: 16pt; max-width: 9in; }
.legend .swatch { display: inline-block; width: 10pt; height: 10pt;
                  border: 1pt solid #888; border-radius: 2pt;
                  vertical-align: middle; margin-right: 4pt; }
.legend span.item { margin-right: 16pt; white-space: nowrap; }
"""

MOD_KEYS = {"Hyper", "Ctrl", "⌘", "⇧", "⌥", "Cmd", "Shift", "Alt"}
LAYER_TAP_LABELS = {"Esc\n/ Ctrl", "Space\n/ SYM", "Bsp\n/ NAV"}
SPECIAL_KEYS = {"Esc", "Tab", "Enter", "Bsp", "Del", "'", "Space"}
TRANSPARENT_LABELS = {"—"}


def classify(label: str | None) -> str:
    if label is None:
        return "gap"
    if label == "Hyper":
        return "hyper"
    if label in LAYER_TAP_LABELS:
        return "layer"
    if label in MOD_KEYS:
        return "mod"
    if label.startswith("BT"):
        return "mouse"
    if label in TRANSPARENT_LABELS:
        return "transparent"
    if label in SPECIAL_KEYS or label in {"Esc", "Tab", "Enter", "Bsp", "Del"}:
        return "special"
    return ""


def render_layer(title: str, desc: str, rows: list[list[str | None]]) -> str:
    cells = []
    for r, row in enumerate(rows):
        for label in row:
            cls = classify(label)
            text = label if label is not None else " "
            # In NAV/SYM the trackpoint button cells could read as gaps in
            # base rendering — keep them showing the mouse icon.
            cells.append(f'<div class="key {cls}">{text}</div>')
    matrix_html = "\n".join(cells)
    return f"""
<section class="layer">
  <h1>{title}</h1>
  <div class="desc">{desc}</div>
  <div class="matrix">
{matrix_html}
  </div>
  <div class="legend">
    <span class="item"><span class="swatch" style="background:#f0e8f5"></span>Hyper</span>
    <span class="item"><span class="swatch" style="background:#e8e8f5"></span>Modifier</span>
    <span class="item"><span class="swatch" style="background:#e8f5e8"></span>Layer (tap/hold)</span>
    <span class="item"><span class="swatch" style="background:#fff5e6"></span>Special</span>
    <span class="item"><span class="swatch" style="background:#fde8e8"></span>Trackpoint button</span>
    <span class="item"><span class="swatch" style="background:#f5f5f5"></span>Transparent (falls to BASE)</span>
  </div>
</section>
"""


def render_html() -> str:
    sections = "\n".join(render_layer(t, d, rows) for t, d, rows in LAYERS)
    return f"""<!DOCTYPE html>
<html><head><meta charset="utf-8"><title>Atreus + Trackpoint keymap</title>
<style>{CSS}</style></head>
<body>
{sections}
</body></html>
"""


def main() -> int:
    repo = Path(__file__).resolve().parent.parent
    out_html = repo / "docs" / "keymap.html"
    out_pdf  = repo / "docs" / "keymap.pdf"
    out_html.write_text(render_html())
    print(f"wrote {out_html}")

    chrome = "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
    if not Path(chrome).exists():
        print("Chrome not found; HTML written but PDF not generated.")
        print("Open the HTML in any browser and Print > Save as PDF.")
        return 0

    subprocess.run([
        chrome, "--headless=new", "--disable-gpu", "--no-pdf-header-footer",
        f"--print-to-pdf={out_pdf}", out_html.as_uri()
    ], check=True)
    print(f"wrote {out_pdf}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
