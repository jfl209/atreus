#include QMK_KEYBOARD_H

/*
 * "Mac-Natural" layout, 3 layers.
 *
 * BASE (QWERTY):    standard letter layout with all modifiers on the thumb row.
 * NAV  (hold Bsp):  arrows on HJKL, page nav, F-keys on the number row, media.
 * SYM  (hold Spc):  numbers on home row, shifted symbols on top row, brackets.
 *
 * Two mod-taps make the bottom row pull double duty:
 *   - Esc:   tap = Esc, hold = Ctrl (vim-friendly)
 *   - Space: tap = Space, hold = SYM
 *   - Bsp:   tap = Backspace, hold = NAV
 *
 * Hyper sits on the bottom-left pinky (KC_HYPR sends Ctrl+Shift+Alt+Cmd —
 * pair it with whatever Karabiner-Elements rules you already have on macOS).
 *
 * Trackpoint mouse buttons are matrix-wired at [2,5] and [2,6] — they map
 * directly to KC_BTN1/KC_BTN2 on every layer (transparent on NAV/SYM so
 * the mouse stays clickable even with a layer held).
 */

enum layers {
    L_BASE,
    L_NAV,
    L_SYM,
};

/* Mod-taps & layer-taps */
#define CTL_ESC LCTL_T(KC_ESC)
#define SYM_SPC LT(L_SYM, KC_SPC)
#define NAV_BSP LT(L_NAV, KC_BSPC)

/* Trackpoint buttons live in matrix positions [2,5] and [2,6]. */
#define TP_BTN1 KC_BTN1
#define TP_BTN2 KC_BTN2

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [L_BASE] = LAYOUT(
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                       KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                       KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,   TP_BTN1, TP_BTN2,   KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
        KC_HYPR, CTL_ESC, KC_TAB,  KC_LCMD, KC_LSFT, SYM_SPC, NAV_BSP,  KC_ENT,  KC_RSFT, KC_RCMD, KC_RALT, KC_QUOT
    ),

    [L_NAV] = LAYOUT(
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                      KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,
        KC_MPLY, KC_VOLD, KC_VOLU, KC_MUTE, KC_NO,                      KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_DEL,
        KC_MPRV, KC_MNXT, KC_NO,   KC_NO,   KC_NO,  KC_TRNS, KC_TRNS,   KC_HOME, KC_PGDN, KC_PGUP, KC_END,  KC_F11,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_F12
    ),

    [L_SYM] = LAYOUT(
        S(KC_1), S(KC_2),   S(KC_3), S(KC_4), S(KC_5),                  S(KC_6), S(KC_7), S(KC_8),    S(KC_9),    S(KC_0),
        KC_1,    KC_2,      KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,       KC_9,       KC_0,
        KC_GRV,  S(KC_GRV), KC_LBRC, KC_RBRC, KC_BSLS, KC_TRNS, KC_TRNS, KC_MINS, KC_EQL, S(KC_LBRC), S(KC_RBRC), S(KC_BSLS),
        KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, S(KC_MINS), S(KC_EQL), KC_TRNS, KC_TRNS
    ),
};
