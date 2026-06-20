#pragma once

/*
 * Atreus + TrackPoint (PS/2)
 *
 * Hardware: Keyboardio Atreus (production, ATmega32U4) with a custom
 * trackpoint daughterboard hanging off a few free GPIO pins.
 * See hardware/atreus_trackpoint.sch.
 */

/* Matrix — identical to stock Keyboardio Atreus.
 * If a newer QMK ships with different pin defs for keyboardio/atreus, prefer those. */
#define MATRIX_ROWS 4
#define MATRIX_COLS 12
#define MATRIX_ROW_PINS { F6, F7, B1, B2 }
#define MATRIX_COL_PINS { B6, B5, D7, C6, D6, F5, F4, B7, D4, E6, D2, D3 }
#define DIODE_DIRECTION COL2ROW

/* PS/2 trackpoint pins.
 *
 * TODO: VERIFY by tracing the daughterboard wiring back to the Atreus MCU.
 * The schematic (hardware/atreus_trackpoint.sch) shows the daughterboard's
 * "To Keyboard" side exposes: LED, CLK, DATA, VCC, GND. We don't know which
 * MCU pins they were soldered to.
 *
 * Constraints:
 *   - CLK must be on an external interrupt pin: D0 (INT0) or D1 (INT1)
 *   - DATA can be any digital GPIO
 *   - LED (WS2811 DIN) can be any digital GPIO; PWM-capable preferred
 *
 * Free pins on the Keyboardio Atreus (not in MATRIX_ROW_PINS / MATRIX_COL_PINS):
 *   B3, B4, C7, D0, D1, D5, E2, F0, F1
 *
 * Current guesses (most likely combo given the constraints):
 */
#define PS2_CLOCK_PIN  D1   /* INT1 */
#define PS2_DATA_PIN   D0
#define WS2812_DI_PIN  D5   /* feeds the daughterboard's WS2811 chain */

/* PS/2 driver — interrupt mode is the standard choice on AVR */
#define PS2_DRIVER interrupt
#define PS2_MOUSE_INIT_DELAY 1000  /* trackpoint needs a beat after power-up */

/* WS2811/WS2812 indicator LEDs on the daughterboard (D1..D5 in schematic = 5 LEDs) */
#define RGBLIGHT_LED_COUNT 5
#define RGBLIGHT_DEFAULT_MODE RGBLIGHT_MODE_STATIC_LIGHT

/* USB identifiers — match the keyboard's current USB descriptors so the host
 * keeps recognizing it as the same device (and karabiner rules etc. stay valid). */
#define VENDOR_ID    0x1209
#define PRODUCT_ID   0x2303
#define DEVICE_VER   0x0001
#define MANUFACTURER "Keyboardio"
#define PRODUCT      "Atreus"

/* Debounce */
#define DEBOUNCE 5
