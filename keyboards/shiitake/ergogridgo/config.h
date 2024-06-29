/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

/* USB Device descriptor parameter */
#define VENDOR_ID       0x504B
#define PRODUCT_ID      0x4A58
#define MANUFACTURER    "Shiitake"
#define PRODUCT "ergogridgo"

// #define BMP_BOOTPIN_AS_RESET
/* key matrix size */
#define MATRIX_ROWS_DEFAULT 7
#define MATRIX_COLS_DEFAULT 9
#define THIS_DEVICE_ROWS 7
#define THIS_DEVICE_COLS 9

// // wiring of each half
#define MATRIX_ROW_PINS { F4, F5, F6, F7, B1, B3, B2 }
#define MATRIX_COL_PINS { D3, D2, D1, D0, D4, C6, D7, E6, B4 }
// #define MATRIX_ROW_PINS { 21, 20, 19, 18, 15, 14, 16 }
// #define MATRIX_COL_PINS { 1, 0, 2, 3, 4, 5, 6, 7, 8 }

// // #define IS_LEFT_HAND  true
// #define BMP_DEFAULT_MODE SINGLE
// #define ACTION_DEBUG
// /* key matrix size */

#define DIODE_DIRECTION COL2ROW

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE