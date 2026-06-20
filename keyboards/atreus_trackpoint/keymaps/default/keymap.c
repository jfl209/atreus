#include QMK_KEYBOARD_H

/*
 * Keymap reconstructed from the dumped firmware (dump/atreus-flash.hex).
 * See docs/DECODING.md for how this was derived.
 *
 * Right thumb keys are mod-tap: hold them for a modifier, tap them for the
 * base key (quote / minus). MO(1) makes the entire base layer LSFT(letter),
 * effectively acting as a thumb-driven shift.
 */

enum layers {
    L_BASE,   /* QWERTY */
    L_SHIFT,  /* hold MO(1) — every alpha key sent with LSFT */
    L_NUM,    /* hold MO(2) — digits + shifted symbols */
    L_FN,     /* hold MO(3) — function keys + arrow cluster */
    L_ADJ,    /* hold MO(4) — secondary num/symbol variant */
};

/* Right-thumb mod-tap aliases */
#define RCTL_QT RCTL_T(KC_QUOT)
#define RALT_MN RALT_T(KC_MINS)

/* Trackpoint mouse buttons — wired through matrix positions [2,5] and [2,6].
 * In the original firmware these were custom user keycodes 0xF4/0xF5. Modern
 * QMK lets us use the built-in mouse-button keycodes directly. */
#define TP_BTN1 KC_BTN1
#define TP_BTN2 KC_BTN2

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [L_BASE] = LAYOUT(
        KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                       KC_Y,   KC_U,    KC_I,    KC_O,    KC_P,
        KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                       KC_H,   KC_J,    KC_K,    KC_L,    KC_SCLN,
        KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,   TP_BTN1, TP_BTN2,   KC_N,   KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
        KC_ESC, KC_TAB,  KC_LGUI, KC_LSFT, KC_SPC, MO(L_NUM), MO(L_SHIFT),
                                                                       KC_BSPC, RCTL_QT, RALT_MN, KC_QUOT, KC_ENT
    ),

    [L_SHIFT] = LAYOUT(
        S(KC_Q), S(KC_W), S(KC_E), S(KC_R), S(KC_T),                   S(KC_Y), S(KC_U), S(KC_I), S(KC_O), S(KC_P),
        S(KC_A), S(KC_S), S(KC_D), S(KC_F), S(KC_G),                   S(KC_H), S(KC_J), S(KC_K), S(KC_L), S(KC_SCLN),
        S(KC_Z), S(KC_X), S(KC_C), S(KC_V), S(KC_B), TP_BTN1, TP_BTN2, S(KC_N), S(KC_M), S(KC_COMM), S(KC_DOT), S(KC_SLSH),
        KC_ESC,  KC_TAB,  KC_LGUI, KC_LSFT, KC_SPC,  MO(L_FN), KC_TRNS,
                                                                       KC_BSPC, RCTL_QT, RALT_MN, S(KC_QUOT), KC_ENT
    ),

    [L_NUM] = LAYOUT(
        KC_0,    KC_1,    KC_2,    KC_3,    KC_4,                      KC_LBRC, S(KC_9),    S(KC_0),    KC_RBRC, S(KC_BSLS),
        KC_5,    KC_6,    KC_7,    KC_8,    KC_9,                      KC_GRV,  S(KC_LBRC), S(KC_RBRC), S(KC_GRV), KC_BSLS,
        S(KC_1), S(KC_2), S(KC_3), S(KC_4), S(KC_5), TP_BTN1, TP_BTN2, S(KC_6), S(KC_7),    S(KC_8),    S(KC_EQL), KC_EQL,
        KC_ESC,  KC_TAB,  KC_LGUI, KC_LSFT, KC_SPC,  KC_TRNS, MO(L_ADJ),
                                                                       KC_BSPC, RCTL_QT, RALT_MN, KC_QUOT, KC_ENT
    ),

    [L_FN] = LAYOUT(
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_5,                      KC_LBRC, S(KC_9), S(KC_0), KC_RBRC, KC_PGUP,
        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,                    KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_PGDN,
        KC_F11,  KC_F12,  KC_F13,  KC_F14,  KC_F15,  TP_BTN1, TP_BTN2, S(KC_6), S(KC_7), S(KC_8), S(KC_EQL), KC_EQL,
        KC_ESC,  KC_TAB,  KC_LGUI, KC_LSFT, KC_SPC,  MO(L_NUM), MO(L_ADJ),
                                                                       KC_BSPC, RCTL_QT, RALT_MN, KC_QUOT, KC_ENT
    ),

    [L_ADJ] = LAYOUT(
        KC_0,    KC_1,    KC_2,    KC_3,    KC_4,                      KC_LBRC, S(KC_9), S(KC_0), KC_RBRC, KC_PGUP,
        KC_5,    KC_6,    KC_7,    KC_8,    KC_9,                      KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_PGDN,
        S(KC_1), S(KC_2), S(KC_3), S(KC_4), S(KC_5), TP_BTN1, TP_BTN2, KC_NO,   S(KC_7), S(KC_8), S(KC_EQL), KC_EQL,
        KC_ESC,  KC_TAB,  KC_LGUI, KC_LSFT, KC_SPC,  MO(L_FN), MO(L_ADJ),
                                                                       KC_BSPC, RCTL_QT, RALT_MN, KC_QUOT, KC_ENT
    ),
};
