// Copyright 2023 Shiitake (GitHub: Jetjaguar-PSX, Twitter: @Jetjaguar_zl)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ergogridgo2_default.h"
#include "os_detection.h"
#include "add_keycodes.h"


uint16_t startup_timer;
bool process_record_addedkeycodes(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
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
        case DEV4:
            if (record->event.pressed) {
                if (detected_host_os() == OS_LINUX){
                    register_code(KC_F14);
                } else {
                    register_code(KC_INT4);
                }
            } else {
                if (detected_host_os() == OS_LINUX){
                    unregister_code(KC_F14);
                } else {
                    unregister_code(KC_INT4);
                }
            }
            return false;
            break;
        case DEV5:
            if (record->event.pressed) {
                if (detected_host_os() == OS_LINUX){
                    register_code(KC_F15);
                } else {
                    register_code(KC_INT5);
                }
            } else {
                if (detected_host_os() == OS_LINUX){
                    unregister_code(KC_F15);
                } else {
                    unregister_code(KC_INT5);
                }
            }
            return false;
            break;
    }
    return true;
}
