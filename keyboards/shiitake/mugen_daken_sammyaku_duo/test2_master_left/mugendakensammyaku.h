// Copyright 2025 Shiitake/shimane_u_pim/motopim-keebs
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "quantum.h"
#include "config.h"

typedef union{
    uint32_t savedata_total;
    struct{
        bool auto_mouse_flag;
        uint8_t savedata0;
        uint8_t savedata1;
    };
} status_mds_main_t;

typedef struct{
        bool connected;
        uint8_t mode;
        // int deadzone;
} status_mds_ajs_t;

extern status_mds_main_t state_mds_main;
extern status_mds_ajs_t state_mds_joystick[2];

void joystick_check(void);
void joystick_caribration(void);
void joystick_mode_change(int lr_flag);
void savedata_update(void);