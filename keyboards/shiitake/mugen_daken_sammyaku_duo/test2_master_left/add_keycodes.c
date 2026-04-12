// Copyright 2025 Shiitake/shimane_u_pim/motopim-keebs
// SPDX-License-Identifier: GPL-2.0-or-later

#include "add_keycodes.h"
#include "mugendakensammyaku.h"

uint16_t startup_timer;

bool process_record_addedkeycodes(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case AUTO_MOUSE_TOGGLE:
            if (record->event.pressed) {
                state_mds_main.auto_mouse_flag = !state_mds_main.auto_mouse_flag;
                set_auto_mouse_enable(state_mds_main.auto_mouse_flag);
                savedata_update();
            }
            return false;
            break;
        
        case JOYSTICK_CARIBRATION:
            if (record->event.pressed) {
                joystick_caribration();
            }
            return false;
            break;
        
        case JOYSTICK_LEFT_MODE_CHANGE:
            if (record->event.pressed) {
                joystick_mode_change(0);
            }
            return false;
            break;

        case JOYSTICK_RIGHT_MODE_CHANGE:
            if (record->event.pressed) {
                joystick_mode_change(1);
            }
            return false;
            break;
        case LCTL_CMD_AUTO:
            if (record->event.pressed) {
                if (detected_host_os() == OS_MACOS || detected_host_os() == OS_IOS){
                    register_code(KC_LGUI);
                } else {
                    register_code(KC_LCTL);
                }
            } else {
                if (detected_host_os() == OS_MACOS || detected_host_os() == OS_IOS){
                    unregister_code(KC_LGUI);
                } else {
                    unregister_code(KC_LCTL);
                }
            }
            return false;
            break;
        case RCTL_CMD_AUTO:
            if (record->event.pressed) {
                if (detected_host_os() == OS_MACOS || detected_host_os() == OS_IOS){
                    register_code(KC_RGUI);
                } else {
                    register_code(KC_RCTL);
                }
            } else {
                if (detected_host_os() == OS_MACOS || detected_host_os() == OS_IOS){
                    unregister_code(KC_RGUI);
                } else {
                    unregister_code(KC_RCTL);
                }
            }
            return false;
            break;
        case MUHENKAN_EISUU_AUTO:
            if (record->event.pressed) {
                if (detected_host_os() == OS_MACOS || detected_host_os() == OS_IOS){
                    register_code(KC_LNG2);
                } else {
                    register_code(KC_INT5);
                }
            } else {
                if (detected_host_os() == OS_MACOS || detected_host_os() == OS_IOS){
                    unregister_code(KC_LNG2);
                } else {
                    unregister_code(KC_INT5);
                }
            }
            return false;
            break;
        case HENKAN_KANA_AUTO:
            if (record->event.pressed) {
                if (detected_host_os() == OS_MACOS || detected_host_os() == OS_IOS){
                    register_code(KC_LNG1);
                } else {
                    register_code(KC_INT4);
                }
            } else {
                if (detected_host_os() == OS_MACOS || detected_host_os() == OS_IOS){
                    unregister_code(KC_LNG1);
                } else {
                    unregister_code(KC_INT4);
                }
            }
            return false;
            break;
    }
    return true;
}
