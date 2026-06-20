# How the keymap and firmware identity were recovered

The original source for this firmware was lost. This document captures what
was determined by inspecting the on-chip firmware, so future-you (or anyone
else opening this repo) can audit the reconstruction.

## Recovering the binary

The keyboard's ATmega32U4 was put into Caterina bootloader mode (poked the
reset button through the hole in the bottom plate with a paperclip). Caterina enumerates as a USB CDC
serial device for ~8 seconds. `avrdude` over the `avr109` protocol read the
full 32KB flash:

```
avrdude -p atmega32u4 -c avr109 -P /dev/cu.usbmodematreus1 -b 57600 \
        -D -U flash:r:atreus-flash.hex:i
```

Output: `dump/atreus-flash.hex` (Intel HEX, 32768 bytes of flash). The raw
`dump/atreus-flash.bin` was produced from the HEX for easier analysis.

## Identifying the firmware as QMK

The dump contained almost no embedded strings:

- `jonlorusso`, `jonlorusso@gmail.com` — used as USB iManufacturer/iSerial
- `CATERINA` — bootloader signature
- The rest of the binary is code/data, no plugin or focus-protocol strings

Kaleidoscope builds embed many plugin / focus strings. The absence of those,
combined with the keymap layout pattern below, points to **QMK**.

USB descriptors visible at runtime via `ioreg`:

    Vendor: Keyboardio  (VID 0x1209)
    Product: Atreus     (PID 0x2303)

These match both Kaleidoscope's stock Atreus build and QMK's
`keyboards/keyboardio/atreus` — so VID/PID alone is ambiguous.

## Decoding the keymap

QMK stores `keymaps` as a `uint16_t[LAYERS][ROWS][COLS]` array in PROGMEM.
For the Atreus that's 4×12×2 = 96 bytes per layer.

The QWERTY top row `Q W E R T _ _ Y U I O P` translates to HID usage codes
`14 00 1a 00 08 00 15 00 17 00 00 00 00 00 1c 00 18 00 0c 00 12 00 13 00`.
Searching the binary for that signature found a unique match at offset
**0x00D5**, immediately following the user's email string in flash.

`tools/decode_keymap.py` walks forward from there and decodes layers using
QMK's old (pre-2022) keycode encoding:

| range            | meaning                                               |
| ---------------- | ----------------------------------------------------- |
| `0x0000-0x00FF`  | basic HID keycodes (`KC_A` = 0x04, etc.)              |
| `0x0100-0x1FFF`  | modifier-prefixed (`LSFT(KC_X)` = `0x0200 \| 0x????`) |
| `0x4000-0x4FFF`  | Layer-Tap                                             |
| `0x5100-0x51FF`  | Momentary layer (`MO(N)`)                             |
| `0x6000-0x7FFF`  | Mod-Tap (5-bit mod in upper byte)                     |

5 clean layers were decoded (`L_BASE`, `L_SHIFT`, `L_NUM`, `L_FN`, `L_ADJ`).
Two more "layers" past those decode to plausible-looking but likely-garbage
keycodes (e.g. one of them spells `F U C K 4` on the top row, which is more
likely PROGMEM data adjacent to the keymap than an intentional easter egg).
The reconstruction stops at 5 layers.

### Notable findings

- The center thumb-gap positions in row 2 (matrix `[2,5]` and `[2,6]`) hold
  custom keycodes `0xF4` and `0xF5` in every layer. On a stock Atreus those
  matrix slots are `KC_NO`. These are very likely the **trackpoint mouse
  buttons** wired through the matrix — in the reconstructed source they're
  expressed as `KC_BTN1` / `KC_BTN2` (the `process_record_user` translation
  the original firmware presumably did becomes unnecessary).
- Right-thumb mod-taps: `RCTL_T(KC_QUOT)` and `RALT_T(KC_MINS)`. Tap for the
  symbol, hold for the modifier.
- Layer 1 (the SHIFT layer) is entirely `LSFT(letter)` — a thumb-driven
  shift in place of a pinky-driven shift.

## Pin assignments for the trackpoint

The schematic (`hardware/atreus_trackpoint.sch`) shows the daughterboard's
"To Keyboard" pads expose LED + CLK + DATA + VCC + GND. Which MCU pins those
five pads were soldered to was **not** recoverable from the dump without
disassembly of the AVR PORT/DDR register accesses in the compiled PS/2 code.

`keyboards/atreus_trackpoint/config.h` contains best-guess pins (`PS2_CLOCK_PIN
D1`, `PS2_DATA_PIN D0`, `WS2812_DI_PIN D5`) with a `TODO` note. Verify by
tracing the actual daughterboard wires before flashing a fresh build.
