/* Copyright 2020 satromi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

enum layer_number {
  QWERTY = 0,
  FUNC,
};

#define MO_FUNC  MO(FUNC)


    /* Qwerty
        ,-----------------------------------------.              ,-----------------------------------------.
        |   [  |   1  |   2  |   3  |   4  |   5  |              |   6  |   7  |   8  |   9  |   0  |   -  |
 ,------+------+------+------+------+------+------+------.,------+------+------+------+------+------+------+------.
 |  ESC |   ]  |   Q  |   W  |   E  |   R  |   T  |  DEL ||  INS |   Y  |   U  |   I  |   O  |   P  |   @  | YEN| |
 |------+------+------+------+------+------+------+------||------+------+------+------+------+------+------+------|
 |  HNK |  ALT |   A  |   S  |   D  |   F  |   G  |  TAB ||  ENT |   H  |   J  |   K  |   L  |   ;  |   :  |   ~  |
 `------+------+------+------+------+------+------+------'`------+------+------+------+------+------+------+------'
        | CTRL |   Z  |   X  |   C  |   V  |   B  |              |   N  |   M  |   ,  |   .  |   /  |   \  |
        `-----------------------------------------'              `-----------------------------------------'
               ,------.      ,---------------------------.,---------------------------.      ,------.
               | PGUP |      |  SPC |  SFT |   MO |  CPS ||   JP | TEAMS|  SFT |  BSP |      |   UP |
        +------+------+------+---------------------------'`---------------------------+------+------+------+
        | HOME | PGDN |  END |                                                        |  LFT |  DWN |  RGT |
        `--------------------'                                                        `--------------------'
   */
    /* Function
        ,-----------------------------------------.              ,-----------------------------------------.
        |      |   F1 |   F2 |   F3 |   F4 |   F5 |              |   F6 |   F7 |   F8 |   F9 |  F10 |      |
 ,------+------+------+------+------+------+------+------.,------+------+------+------+------+------+------+------.
 |      |      |      |      |      |      |      |      ||      |      |      |      |      |      |      |      |
 |------+------+------+------+------+------+------+------||------+------+------+------+------+------+------+------|
 | L_GUI|      |      |      |      |      |      |      ||      |      |      |      |      |      |      |      |
 `------+------+------+------+------+------+------+------'`------+------+------+------+------+------+------+------'
        |      |      |      |      |      |      |              |      |      |      |      |      |      |
        `-----------------------------------------'              `-----------------------------------------'
               ,------.      ,---------------------------.,---------------------------.      ,------.
               |      |      |      |      |      |      ||      |      |      |      |      |      |
        +------+------+------+---------------------------'`---------------------------+------+------+------+
        |      |      |      |                                                        |      |      |      |
        `--------------------'                                                        `--------------------'
   */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
   [QWERTY] = LAYOUT(
            KC_RBRC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,  KC_END, KC_CAPS,                   KC_KANA, KC_LEFT,   KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_MINS,
   KC_ESC,  KC_NUHS,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T, KC_PGDN, MO_FUNC,  KC_DEL,  KC_INS, LSFT(LCTL(KC_M)), KC_DOWN,   KC_Y,    KC_U,    KC_I,    KC_O,    KC_P, KC_LBRC, KC_JYEN,
   KC_ZKHK, KC_LALT,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G, KC_HOME, KC_LSFT,  KC_TAB,  KC_ENT, KC_RSFT, KC_RGHT,   KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,  KC_EQL,
            KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B, KC_PGUP,  KC_SPC,                   KC_BSPC,   KC_UP,   KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,   KC_RO
   ),

   [FUNC] = LAYOUT(
            _______,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5, _______, _______,                   _______, _______,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10, _______,         
   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
   KC_LGUI, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
            _______, _______, _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, _______, _______
   ),
};


