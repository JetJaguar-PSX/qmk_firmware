// Copyright 2025 Shiitake/shimane_u_pim/motopim-keebs
// SPDX-License-Identifier: GPL-2.0-or-later

#include "mugendakensammyaku.h"
#include "add_keycodes.h"
#include "analog.h"
#include "math.h"

// mode周りをconfigで設定
int mode_num = 3;

status_mds_main_t state_mds_main;
status_mds_ajs_t state_mds_joystick[2];

keypos_t dpad_keypos[2][4];
bool dpad_flag[2][4] ={
    {false,false,false,false},
    {false,false,false,false}
};

int joystick_origin[2][2] = {
    {0,0},
    {0,0}
};

void savedata_update(void){
    state_mds_main.savedata0 = state_mds_joystick[0].mode;
    state_mds_main.savedata1 = state_mds_joystick[1].mode;
    eeconfig_update_kb(state_mds_main.savedata_total);
}

void savedata_load(void){
    state_mds_main.savedata_total = eeconfig_read_kb();
    state_mds_joystick[0].mode = state_mds_main.savedata0;
    state_mds_joystick[1].mode = state_mds_main.savedata1;
}

void joystick_check(void){
    joystick_caribration();
    if(joystick_origin[0][0] < 50 || joystick_origin[0][1] < 50){
        state_mds_joystick[0].connected = false;
    }else{
        state_mds_joystick[0].connected = true;
    }
    if(joystick_origin[1][0] < 50 || joystick_origin[1][1] < 50){
        state_mds_joystick[1].connected = false;
    }else{
        state_mds_joystick[1].connected = true;
    }
}


void joystick_caribration(void){
    joystick_origin[0][0] = analogReadPin(LEFT_JOYSTICK_XPIN);
    joystick_origin[0][1] = analogReadPin(LEFT_JOYSTICK_YPIN);
    joystick_origin[1][0] = analogReadPin(RIGHT_JOYSTICK_XPIN);
    joystick_origin[1][1] = analogReadPin(RIGHT_JOYSTICK_YPIN);
}


void joystick_mode_change(int lr_flag){
    state_mds_joystick[lr_flag].mode = state_mds_joystick[lr_flag].mode +1;
    if(state_mds_joystick[lr_flag].mode > mode_num){
        state_mds_joystick[lr_flag].mode = 1;
    }
    if(state_mds_joystick[lr_flag].mode < 1){
        state_mds_joystick[lr_flag].mode = 1;
    }
    savedata_update();
    joystick_caribration();
    for(int i=0; i<2; i++){
        for(int j=0; j<4; j++){
            unregister_code(keymap_key_to_keycode(0, dpad_keypos[i][j]));
        }
    }
}

bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    process_record_addedkeycodes(keycode, record);
    return process_record_user(keycode, record);
}

void matrix_init_kb(void) {
    dpad_keypos[0][0].row = LEFT_DPAD_ROW;
    dpad_keypos[0][0].col = 1;
    dpad_keypos[0][1].row = LEFT_DPAD_ROW;
    dpad_keypos[0][1].col = 2;
    dpad_keypos[0][2].row = LEFT_DPAD_ROW;
    dpad_keypos[0][2].col = 3;
    dpad_keypos[0][3].row = LEFT_DPAD_ROW;
    dpad_keypos[0][3].col = 4;
    
    dpad_keypos[1][0].row = RIGHT_DPAD_ROW;
    dpad_keypos[1][0].col = 1;
    dpad_keypos[1][1].row = RIGHT_DPAD_ROW;
    dpad_keypos[1][1].col = 2;
    dpad_keypos[1][2].row = RIGHT_DPAD_ROW;
    dpad_keypos[1][2].col = 3;
    dpad_keypos[1][3].row = RIGHT_DPAD_ROW;
    dpad_keypos[1][3].col = 4;

    matrix_init_user();
}

void pointing_device_init_kb(void){
    savedata_load();
    joystick_check();

    if(state_mds_joystick[0].connected || state_mds_joystick[1].connected){
        joystick_caribration();
    }

    set_auto_mouse_enable(state_mds_main.auto_mouse_flag);

    savedata_update();
    pointing_device_init_user();
}

float fraction_x = 0;
float fraction_y = 0;

report_mouse_t mouse_mode_task(int input_x, int input_y){
    report_mouse_t mouse_report;
    mouse_report.h = 0;
    mouse_report.v = 0;
    mouse_report.buttons = 0;

    float temp_x = 0;
    float temp_y = 0;

    if(abs(input_x) >= MOUSE_DEADZONE){
        temp_x = (float)input_x;
    }
    if(abs(input_y) >= MOUSE_DEADZONE){
        temp_y = (float)input_y;
    }

    temp_x = 0.005 * temp_x + fraction_x;
    temp_y = 0.005 * temp_y + fraction_y;

    temp_x = temp_x > 127 ? 127 : temp_x;
    temp_x = temp_x < -127 ? -127 : temp_x;
    temp_y = temp_y > 127 ? 127 : temp_y;
    temp_y = temp_y < -127 ? -127 : temp_y;

    mouse_report.x = (int8_t)temp_x;
    mouse_report.y = (int8_t)temp_y;

    fraction_x = temp_x - (int)temp_x;
    fraction_y = temp_y - (int)temp_y;

    return mouse_report;
}

float fraction_h = 0;
float fraction_v = 0;

report_mouse_t scroll_mode_task(int input_h, int input_v){
    report_mouse_t scroll_report;
    scroll_report.x = 0;
    scroll_report.y = 0;
    scroll_report.buttons = 0;

    float temp_h = 0;
    float temp_v = 0;

    if(abs(input_h) >= SCROLL_DEADZONE){
        temp_h = (float)input_h;
    }
    if(abs(input_v) >= SCROLL_DEADZONE){
        temp_v = (float)input_v;
    }

    temp_h = 0.0002 * temp_h + fraction_h;
    temp_v = -0.0002 * temp_v + fraction_v;

    temp_h = temp_h > 127 ? 127 : temp_h;
    temp_h = temp_h < -127 ? -127 : temp_h;
    temp_v = temp_v > 127 ? 127 : temp_v;
    temp_v = temp_v < -127 ? -127 : temp_v;

    scroll_report.h = (int8_t)temp_h;
    scroll_report.v = (int8_t)temp_v;

    fraction_h = temp_h - (int)temp_h;
    fraction_v = temp_v - (int)temp_v;
    
    return scroll_report;
}

void dpad_mode_task(int input_x, int input_y, int lr_flag){
    int temp_x = 0;
    int temp_y = 0;

    temp_x = input_x;
    temp_y = input_y;

    if(temp_y < -DPAD_DEADZONE){
        if(dpad_flag[lr_flag][0]){
            register_code(keymap_key_to_keycode(0, dpad_keypos[lr_flag][0]));
            dpad_flag[lr_flag][0] = false;
        }
    }else if(!dpad_flag[lr_flag][0]){
            unregister_code(keymap_key_to_keycode(0, dpad_keypos[lr_flag][0]));
            dpad_flag[lr_flag][0] = true;
    }
    if(temp_x < -DPAD_DEADZONE){
        if(dpad_flag[lr_flag][1]){
            register_code(keymap_key_to_keycode(0, dpad_keypos[lr_flag][1]));
            dpad_flag[lr_flag][1] = false;
        }
    }else if(!dpad_flag[lr_flag][1]){
            unregister_code(keymap_key_to_keycode(0, dpad_keypos[lr_flag][1]));
            dpad_flag[lr_flag][1] = true;
    }
    if(temp_x > DPAD_DEADZONE){
        if(dpad_flag[lr_flag][2]){
            register_code(keymap_key_to_keycode(0, dpad_keypos[lr_flag][2]));
            dpad_flag[lr_flag][2] = false;
        }
    }else if(!dpad_flag[lr_flag][2]){
            unregister_code(keymap_key_to_keycode(0, dpad_keypos[lr_flag][2]));
            dpad_flag[lr_flag][2] = true;
    }
    if(temp_y > DPAD_DEADZONE){
        if(dpad_flag[lr_flag][3]){
            register_code(keymap_key_to_keycode(0, dpad_keypos[lr_flag][3]));
            dpad_flag[lr_flag][3] = false;
        }
    }else if(!dpad_flag[lr_flag][3]){
            unregister_code(keymap_key_to_keycode(0, dpad_keypos[lr_flag][3]));
            dpad_flag[lr_flag][3] = true;
    }
}

// report_mouse_t fusion_mode_task_core(int input_x, int input_y){
//     report_mouse_t mouse_report;
//     mouse_report.x = 0;
//     mouse_report.y = 0;
//     mouse_report.h = 0;
//     mouse_report.v = 0;
//     mouse_report.buttons = 0;

//     return mouse_report;
// }

// report_mouse_t fusion_mode_task_all(int input_x, int input_y, int input_h, int input_v){
//     report_mouse_t mouse_report;
    
//     mouse_report.x = 0;
//     mouse_report.y = 0;
//     mouse_report.h = 0;
//     mouse_report.v = 0;

//     return mouse_report;
// }


report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    mouse_report.x = 0;
    mouse_report.y = 0;
    mouse_report.h = 0;
    mouse_report.v = 0;
    mouse_report.buttons = 0;

    int joystick_current[2][2] = {
        {0,0},
        {0,0}
    };
    
    if(state_mds_joystick[0].connected){
        joystick_current[0][0] = analogReadPin(LEFT_JOYSTICK_XPIN) - joystick_origin[0][0];
        joystick_current[0][1] = analogReadPin(LEFT_JOYSTICK_YPIN) - joystick_origin[0][1];
    }

    if(state_mds_joystick[1].connected){
        joystick_current[1][0] = analogReadPin(RIGHT_JOYSTICK_XPIN) - joystick_origin[1][0];
        joystick_current[1][1] = analogReadPin(RIGHT_JOYSTICK_YPIN) - joystick_origin[1][1];
    }

    joystick_current[1][0] = -joystick_current[1][0];

    if(state_mds_joystick[0].connected && state_mds_joystick[1].connected){
        report_mouse_t cursor_report;
        report_mouse_t scroll_report;
        
        cursor_report.x = 0;
        cursor_report.y = 0;
        cursor_report.h = 0;
        cursor_report.v = 0;
        cursor_report.buttons = 0;
        
        scroll_report.x = 0;
        scroll_report.y = 0;
        scroll_report.h = 0;
        scroll_report.v = 0;
        scroll_report.buttons = 0;
    
    //     if((state_mds_joystick[0].mode == FUSION_MODE_NUMBER) && (state_mds_joystick[1].mode == MOUSE_MODE_NUMBER)){
            
    //     }else if((state_mds_joystick[0].mode == MOUSE_MODE_NUMBER) && (state_mds_joystick[1].mode == FUSION_MODE_NUMBER)){
            
    //     }else{
            if(state_mds_joystick[0].mode == state_mds_joystick[1].mode){
                int temp_x = (abs(joystick_current[0][0]) > abs(joystick_current[1][0])) ? joystick_current[0][0] : joystick_current[1][0];
                int temp_y = (abs(joystick_current[0][1]) > abs(joystick_current[1][1])) ? joystick_current[0][1] : joystick_current[1][1];
    
                if(state_mds_joystick[0].mode == MOUSE_MODE_NUMBER){
                    mouse_report = mouse_mode_task(temp_x, temp_y);
                }
                if(state_mds_joystick[0].mode == SCROLL_MODE_NUMBER){
                    mouse_report = scroll_mode_task(temp_x, temp_y);
                }

                // if(state_mds_joystick[0].mode == FUSION_MODE_NUMBER){
                //     mouse_report = fusion_mode_task_core(temp_x, temp_y);
                // }
            }else{
                if(state_mds_joystick[0].mode == MOUSE_MODE_NUMBER){
                    cursor_report = mouse_mode_task(joystick_current[0][0], joystick_current[0][1]);
                }
                if(state_mds_joystick[1].mode == MOUSE_MODE_NUMBER){
                    cursor_report = mouse_mode_task(joystick_current[1][0], joystick_current[1][1]);
                }
    
                if(state_mds_joystick[0].mode == SCROLL_MODE_NUMBER){
                    scroll_report = scroll_mode_task(joystick_current[0][0], joystick_current[0][1]);
                }
                if(state_mds_joystick[1].mode == SCROLL_MODE_NUMBER){
                    scroll_report = scroll_mode_task(joystick_current[1][0], joystick_current[1][1]);
                }
            
                // if(state_mds_joystick[0].mode == FUSION_MODE_NUMBER){
                //     // cursor_report = fusion_mode_task_core(joystick_current[0][0], joystick_current[0][1]);
                // }
                // if(state_mds_joystick[1].mode == FUSION_MODE_NUMBER){
                //     // cursor_report = fusion_mode_task_core(joystick_current[1][0], joystick_current[1][1]);
                // }
    
                mouse_report.x = cursor_report.x;
                mouse_report.y = cursor_report.y;
                mouse_report.h = scroll_report.h;
                mouse_report.v = scroll_report.v;
            }
    
            if(state_mds_joystick[0].mode == DPAD_MODE_NUMBER){
                dpad_mode_task(joystick_current[0][0], joystick_current[0][1], 0);
            }
            if(state_mds_joystick[1].mode == DPAD_MODE_NUMBER){
                dpad_mode_task(joystick_current[1][0], joystick_current[1][1], 1);
            }
        // }
    }else{
        joystick_check();
    }

    return pointing_device_task_user(mouse_report);
}