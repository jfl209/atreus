#!/usr/bin/env python3
"""
Decode the QMK keymap embedded in a firmware binary dump for the Atreus.

Usage:
    python3 tools/decode_keymap.py [path/to/firmware.bin]

Defaults to dump/atreus-flash.bin if no argument is given. This is the same
decoder that was used to reconstruct keyboards/atreus_trackpoint/keymaps/default/keymap.c
from the firmware dump — kept around so future-you can re-verify or extend.
"""

from __future__ import annotations
import sys
from pathlib import Path

ROWS, COLS = 4, 12
LAYER_BYTES = ROWS * COLS * 2

# Known QMK basic keycodes (subset; enough for everything seen in this firmware).
KC = {
    0x00: "XXXX", 0x01: "TRNS",
    0x04: "A", 0x05: "B", 0x06: "C", 0x07: "D", 0x08: "E", 0x09: "F",
    0x0a: "G", 0x0b: "H", 0x0c: "I", 0x0d: "J", 0x0e: "K", 0x0f: "L",
    0x10: "M", 0x11: "N", 0x12: "O", 0x13: "P", 0x14: "Q", 0x15: "R",
    0x16: "S", 0x17: "T", 0x18: "U", 0x19: "V", 0x1a: "W", 0x1b: "X",
    0x1c: "Y", 0x1d: "Z",
    0x1e: "1", 0x1f: "2", 0x20: "3", 0x21: "4", 0x22: "5",
    0x23: "6", 0x24: "7", 0x25: "8", 0x26: "9", 0x27: "0",
    0x28: "ENT", 0x29: "ESC", 0x2a: "BSPC", 0x2b: "TAB", 0x2c: "SPC",
    0x2d: "MINS", 0x2e: "EQL", 0x2f: "LBRC", 0x30: "RBRC", 0x31: "BSLS",
    0x33: "SCLN", 0x34: "QUOT", 0x35: "GRV", 0x36: "COMM", 0x37: "DOT",
    0x38: "SLSH", 0x39: "CAPS",
    0x3a: "F1",  0x3b: "F2",  0x3c: "F3",  0x3d: "F4",  0x3e: "F5",
    0x3f: "F6",  0x40: "F7",  0x41: "F8",  0x42: "F9",  0x43: "F10",
    0x44: "F11", 0x45: "F12",
    0x4a: "HOME", 0x4b: "PGUP", 0x4c: "DEL", 0x4d: "END", 0x4e: "PGDN",
    0x4f: "RGHT", 0x50: "LEFT", 0x51: "DOWN", 0x52: "UP",
    0xe0: "LCTL", 0xe1: "LSFT", 0xe2: "LALT", 0xe3: "LGUI",
    0xe4: "RCTL", 0xe5: "RSFT", 0xe6: "RALT", 0xe7: "RGUI",
}

def kcname(lo: int) -> str:
    return KC.get(lo, f"_{lo:02x}_")

def decode_kc(v: int) -> str:
    lo, hi = v & 0xFF, (v >> 8) & 0xFF
    if v == 0:      return "XXXX"
    if v == 0x0001: return "TRNS"
    if hi == 0:     return kcname(lo)
    # Modifier-prefixed keycodes: 0x0100=LCTL, 0x0200=LSFT, 0x0400=LALT, 0x0800=LGUI
    if hi == 0x01:  return f"C({kcname(lo)})"
    if hi == 0x02:  return f"S({kcname(lo)})"
    if hi == 0x04:  return f"A({kcname(lo)})"
    if hi == 0x08:  return f"G({kcname(lo)})"
    # Layer ops
    if (v & 0xFF00) == 0x5000: return f"TO({lo})"
    if (v & 0xFF00) == 0x5100: return f"MO({lo & 0xF})"
    if (v & 0xFF00) == 0x5200: return f"DF({lo & 0xF})"
    if (v & 0xFF00) == 0x5300: return f"TG({lo & 0xF})"
    # Layer-tap: 0x4XYY (X=layer, YY=kc)
    if 0x4000 <= v <= 0x4FFF:
        return f"LT({(v >> 8) & 0xF},{kcname(lo)})"
    # Old-QMK Mod-Tap: 0x6XYY/0x7XYY (5-bit mod in upper, kc in lower)
    # Modern QMK source-level macro is MT(MOD_xxx, KC_xxx); the binary value differs.
    if 0x6000 <= v <= 0x7FFF:
        mod = (v >> 8) & 0x1F
        return f"MT({mod:#04x},{kcname(lo)})"
    return f"?{v:04x}"

def find_qwerty(data: bytes) -> int:
    """Locate the keymap by scanning for the QWERTY signature on row 0."""
    sig = bytes([0x14, 0, 0x1A, 0, 0x08, 0, 0x15, 0, 0x17, 0])  # Q W E R T
    i = data.find(sig)
    if i < 0:
        raise SystemExit("no QWERTY signature in firmware — not a stock Atreus layout?")
    return i

def main(argv: list[str]) -> int:
    path = Path(argv[1] if len(argv) > 1 else "dump/atreus-flash.bin")
    if not path.exists():
        raise SystemExit(f"not found: {path}")

    data = path.read_bytes()
    start = find_qwerty(data)
    print(f"Keymap start: 0x{start:04x}\n")

    for layer in range(8):
        base = start + layer * LAYER_BYTES
        if base + LAYER_BYTES > len(data):
            break
        # Heuristic stop: thumb row should be mostly real keycodes
        thumb = [data[base + (3 * COLS + c) * 2] | (data[base + (3 * COLS + c) * 2 + 1] << 8)
                 for c in range(COLS)]
        if sum(1 for v in thumb if v == 0 or v == 1 or v < 0x8000) < 10:
            break

        print(f"=== Layer {layer} @ 0x{base:04x} ===")
        for r in range(ROWS):
            row = [decode_kc(data[base + (r * COLS + c) * 2] |
                             (data[base + (r * COLS + c) * 2 + 1] << 8))
                   for c in range(COLS)]
            left, mid, right = row[:5], row[5:7], row[7:]
            print("  " + " ".join(f"{k:>9}" for k in left)
                  + " |" + " ".join(f"{k:>6}" for k in mid)
                  + "| " + " ".join(f"{k:>9}" for k in right))
        print()
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv))
