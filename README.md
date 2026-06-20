# atreus

QMK firmware for my Keyboardio Atreus + PS/2 TrackPoint mod.

The original firmware source was lost. This repo was rebuilt from the running
keyboard:

- `dump/atreus-flash.hex` was read off the ATmega32U4 over Caterina/avr109.
- The QMK keymap was decoded from the dump and turned back into source under
  `keyboards/atreus_trackpoint/`.
- The trackpoint daughterboard schematic was copied in from KiCad.

If you're future-me opening this cold: start with `docs/DECODING.md` (how
the keymap was recovered) and `docs/HARDWARE.md` (what's on the PCBs).

## Hardware

- Keyboardio Atreus, ATmega32U4, Caterina bootloader
- Custom trackpoint daughterboard (KiCad files in `hardware/`), with:
  - IBM-style PS/2 TrackPoint module
  - 5× WS2811-driven indicator LEDs
- 3D-printed trackpoint holder (STLs live in iCloud Drive under
  `Documents/3dprints/atreus trackpoint holder v*.stl`)

## Layout

5 layers, all built on the standard 4×12 Atreus matrix:

| # | Name    | Activation        | What's on it |
|---|---------|-------------------|--------------|
| 0 | `BASE`  | default           | QWERTY |
| 1 | `SHIFT` | hold right thumb `MO(1)` | every alpha key sent as `LSFT(letter)` — thumb-driven shift |
| 2 | `NUM`   | hold left thumb `MO(2)`  | digits, brackets, shifted symbols |
| 3 | `FN`    | hold `MO(3)` from `SHIFT` | F1-F15, arrow cluster |
| 4 | `ADJ`   | hold `MO(4)` from `NUM`   | numbers + arrows combined |

Right-thumb keys are mod-tap:

- `RCTL_T(KC_QUOT)` — tap for `'`, hold for Right-Ctrl
- `RALT_T(KC_MINS)` — tap for `-`, hold for Right-Alt

Trackpoint mouse buttons are at matrix positions `[2,5]` and `[2,6]` — the
center of the third row, wired through the matrix as `KC_BTN1` / `KC_BTN2`.

See `keyboards/atreus_trackpoint/keymaps/default/keymap.c` for the full
keymap.

## Building

This repo contains only the custom keyboard folder. Drop it into a QMK
checkout to build:

```sh
# 1. Get QMK
git clone https://github.com/qmk/qmk_firmware.git
cd qmk_firmware
make git-submodule        # pulls chibios, lufa, etc.

# 2. Link in this keyboard
ln -s ~/git/atreus/keyboards/atreus_trackpoint keyboards/atreus_trackpoint

# 3. Build
qmk compile -kb atreus_trackpoint -km default
# (output: atreus_trackpoint_default.hex in the repo root)
```

**Before flashing a fresh build**, confirm the trackpoint pins in
`keyboards/atreus_trackpoint/config.h` are correct. They were guessed, not
recovered. See `docs/HARDWARE.md` § "How to verify the pins".

If a new QMK release renamed something and the build breaks, the original
firmware was built around early-2021 QMK — pin to a commit from that window
and port forward in small steps.

## Flashing

```sh
# Restore the captured backup
./flash.sh

# Or flash a fresh build
./flash.sh path/to/atreus_trackpoint_default.hex
```

To enter bootloader mode: hold `Fn+Esc` if the binding survived in your
build, otherwise press the reset button on the underside of the PCB
(requires removing the bottom plate).

## Layout of this repo

```
.
├── README.md
├── flash.sh                          # one-command flash helper
├── dump/
│   ├── atreus-flash.hex              # working firmware backup (re-flashable)
│   └── atreus-flash.bin              # raw binary, same content
├── docs/
│   ├── DECODING.md                   # how the keymap was recovered from the dump
│   └── HARDWARE.md                   # schematic notes, pin assignments, gotchas
├── hardware/
│   └── atreus_trackpoint.*           # KiCad schematic, PCB, gerbers
├── keyboards/
│   └── atreus_trackpoint/            # QMK keyboard (symlink into qmk_firmware/keyboards)
│       ├── atreus_trackpoint.{c,h}
│       ├── config.h                  # matrix + trackpoint pin defs (TODOs!)
│       ├── rules.mk
│       ├── info.json
│       └── keymaps/default/
│           ├── keymap.c              # 5-layer keymap
│           └── rules.mk
└── tools/
    └── decode_keymap.py              # the decoder used on the dump
```
