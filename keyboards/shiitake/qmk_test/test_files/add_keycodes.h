// Copyright 2023 Shiitake (GitHub: Jetjaguar-PSX, Twitter: @Jetjaguar_zl)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "quantum.h"

// 追加するキーコード
enum TH_keycodes{
    LCTL_CMD_AUTO = QK_KB_0,
    RCTL_CMD_AUTO,
    DEV4,
    DEV5
};

bool process_record_addedkeycodes(uint16_t keycode, keyrecord_t *record);