// Copyright 2025 Shiitake/shimane_u_pim/motopim-keebs
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "quantum.h"

// 追加するキーコード
enum keycodes_MDS{
    AUTO_MOUSE_TOGGLE = QK_KB_0,
    JOYSTICK_CARIBRATION,
    JOYSTICK_LEFT_MODE_CHANGE,
    JOYSTICK_RIGHT_MODE_CHANGE
};

bool process_record_addedkeycodes(uint16_t keycode, keyrecord_t *record);