# Hardware notes

## Keyboard

Keyboardio Atreus, production model. ATmega32U4 @ 16MHz, Caterina bootloader,
4×12 diode matrix (`COL2ROW`).

Matrix pins (from upstream `keyboards/keyboardio/atreus/info.json`):

    Rows: F6, F7, B1, B2
    Cols: B6, B5, D7, C6, D6, F5, F4, B7, D4, E6, D2, D3

Free GPIO pins (everything else on the chip):

    B3, B4, C7, D0, D1, D5, E2, F0, F1

## Trackpoint daughterboard

KiCad project in `hardware/atreus_trackpoint.sch`. Lives between the Atreus
mainboard and a PS/2 IBM-style TrackPoint module. Bridges two roles:

1. **PS/2 passthrough** — the trackpoint's CLK / DATA / RST / VCC / GND wires
   land on TP2-TP6 on one side. CLK and DATA continue through to TP8/TP9 on
   the "To Keyboard" side. The board adds pull-up resistors (R2 on CLK, R3 on
   DATA, R1 on RST) and a power-on reset RC network (R1 + C1 on RST). The
   trackpoint's RST is generated locally — the MCU does **not** drive it.

2. **Indicator LEDs** — two WS2811 driver chips (U1, U2) drive a chain of 5
   through-hole LEDs (D1-D5). DIN comes in from the keyboard side on TP7
   (signal labeled "LED"). The LED chain is independent of the trackpoint.

### Pads exposed on the "To Keyboard" side

| Pad   | Signal | Connects to (MCU pin)           |
|-------|--------|----------------------------------|
| TP7   | LED    | **TODO: trace** (guessed `D5`)   |
| TP8   | CLK    | **TODO: trace** (guessed `D1` = INT1) |
| TP9   | DATA   | **TODO: trace** (guessed `D0`)   |
| TP10  | VCC    | +5V                              |
| TP11  | GND    | GND                              |

### Pads exposed on the "To Trackpoint" side

| Pad   | Signal | Notes                                              |
|-------|--------|----------------------------------------------------|
| TP2   | CLK    | with pull-up R2                                    |
| TP3   | DATA   | with pull-up R3                                    |
| TP4   | RST    | driven by R1+C1, also pulled up to VCC             |
| TP5   | VCC    | +5V                                                |
| TP6   | GND    |                                                    |

### Constraints when picking MCU pins

- `PS2_CLOCK_PIN` **must** be an external interrupt pin on the ATmega32U4:
  `D0` (INT0) or `D1` (INT1). The other available INT pins (INT2/3/6) are in
  use by USB or fixed peripherals.
- `PS2_DATA_PIN` can be any free digital pin.
- `WS2812_DI_PIN` can be any free digital pin; PWM-capable pins help if
  using the `pwm` driver, but `bitbang` works anywhere.

### How to verify the pins

Easiest, no disassembly: short the trackpoint's CLK or DATA wire to GND with
a multimeter probe while watching `qmk console` output, and see which pin
the PS/2 init complains about. Or: open the case, identify the three wires
coming off the daughterboard's "To Keyboard" pads, and follow them with a
multimeter in continuity mode to the matching pin on the Atreus MCU.

If neither is practical, the dumped firmware *does* contain the PS/2 init
code that touches the actual PORT/DDR/PIN registers — a short avr-objdump
+ search session would recover the pins definitively.
