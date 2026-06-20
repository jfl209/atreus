#pragma once

#include "quantum.h"

/*
 * Physical layout — matches stock Keyboardio Atreus (4 rows × 12 cols),
 * with the row-2 center positions (k25, k26) reused for the trackpoint's
 * left/right mouse buttons. On stock firmware those slots are KC_NO.
 *
 *     k00 k01 k02 k03 k04         k07 k08 k09 k0A k0B
 *     k10 k11 k12 k13 k14         k17 k18 k19 k1A k1B
 *     k20 k21 k22 k23 k24 k25 k26 k27 k28 k29 k2A k2B
 *     k30 k31 k32 k33 k34 k35 k36 k37 k38 k39 k3A k3B
 */
#define LAYOUT( \
    k00, k01, k02, k03, k04,                k07, k08, k09, k0A, k0B, \
    k10, k11, k12, k13, k14,                k17, k18, k19, k1A, k1B, \
    k20, k21, k22, k23, k24, k25, k26,      k27, k28, k29, k2A, k2B, \
    k30, k31, k32, k33, k34, k35, k36,      k37, k38, k39, k3A, k3B  \
) { \
    { k00,   k01,   k02,   k03,   k04,   KC_NO, KC_NO, k07,   k08,   k09,   k0A,   k0B   }, \
    { k10,   k11,   k12,   k13,   k14,   KC_NO, KC_NO, k17,   k18,   k19,   k1A,   k1B   }, \
    { k20,   k21,   k22,   k23,   k24,   k25,   k26,   k27,   k28,   k29,   k2A,   k2B   }, \
    { k30,   k31,   k32,   k33,   k34,   k35,   k36,   k37,   k38,   k39,   k3A,   k3B   }  \
}
