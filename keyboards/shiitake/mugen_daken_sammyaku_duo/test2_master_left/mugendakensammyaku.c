// Copyright 2025 Shiitake/shimane_u_pim/motopim-keebs
// SPDX-License-Identifier: GPL-2.0-or-later

#include "mugendakensammyaku.h"
#include "add_keycodes.h"
#include "analog.h"
#include "math.h"

// mode周りをconfigで設定
int mode_num = 4;

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

bool shift_trigger = false;
bool shift_state = false;
bool ctrl_trigger = false;
bool ctrl_state = false;

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
    // state_mds_joystick[0].mode = MOUSE_MODE_NUMBER;
    // state_mds_joystick[1].mode = FUSION_MODE_NUMBER;
    joystick_check();

    if(state_mds_joystick[0].connected || state_mds_joystick[1].connected){
        joystick_caribration();
    }

    set_auto_mouse_enable(state_mds_main.auto_mouse_flag);

    savedata_update();
    pointing_device_init_user();
}

float vector2d_magnitude(int input[2]){
    float magnitude_square = (float)input[0] * (float)input[0] + (float)input[1] * (float)input[1];
    float magnitude = sqrt(magnitude_square);
    return magnitude;
}

float vector2d_dot(int input_a[2], int input_b[2]){
    return (float)input_a[0] * (float)input_b[0] + (float)input_a[1] * (float)input_b[1];
}

float vector2d_normalized_dot(int input_a[2], int input_b[2]){
    return  (vector2d_dot(input_a, input_b) / vector2d_magnitude(input_a) )/ vector2d_magnitude(input_b);
}

bool deadzone_checker(int input_x, int input_y, int deadzone_val){
    int32_t magnitude = (int32_t)input_x * input_x + (int32_t)input_y * input_y;
    int32_t dz_sq = (int32_t)deadzone_val * deadzone_val;
    return magnitude > dz_sq;
}

float fraction_x = 0;
float fraction_y = 0;

report_mouse_t mouse_mode_task(int input_x, int input_y, int btn_num, bool shift_flag){
    report_mouse_t mouse_report;
    mouse_report.x = 0;
    mouse_report.y = 0;
    mouse_report.h = 0;
    mouse_report.v = 0;
    mouse_report.buttons = 0;

    float temp_x = 0;
    float temp_y = 0;
    bool active_flag = deadzone_checker(input_x, input_y, MOUSE_DEADZONE);

    shift_trigger = false;
    if(shift_flag){
        if(deadzone_checker(3 * input_x, 3 * input_y, (2 * MOUSE_DEADZONE))){
            shift_trigger = true;
        }
    }

    if(active_flag){
        temp_x = (float)input_x;
        temp_y = (float)input_y;

        temp_x = 0.00004 * temp_x * abs(temp_x) + fraction_x;
        temp_y = 0.00004 * temp_y * abs(temp_y) + fraction_y;

        temp_x = temp_x > 127 ? 127 : temp_x;
        temp_x = temp_x < -127 ? -127 : temp_x;
        temp_y = temp_y > 127 ? 127 : temp_y;
        temp_y = temp_y < -127 ? -127 : temp_y;

        mouse_report.x = (int8_t)temp_x;
        mouse_report.y = (int8_t)temp_y;

        fraction_x = temp_x - (int)temp_x;
        fraction_y = temp_y - (int)temp_y;

        mouse_report.buttons |= btn_num;
    }

    return mouse_report;
}

float fraction_h = 0;
float fraction_v = 0;

report_mouse_t scroll_mode_task(int input_h, int input_v, bool ctrl_flag){
    report_mouse_t scroll_report;
    scroll_report.x = 0;
    scroll_report.y = 0;
    scroll_report.h = 0;
    scroll_report.v = 0;
    scroll_report.buttons = 0;

    float temp_h = 0;
    float temp_v = 0;
    bool active_flag = deadzone_checker(input_h, input_v, SCROLL_DEADZONE);

    ctrl_trigger = false;
    if(ctrl_flag){
        if(deadzone_checker(3 * input_h, 3 * input_v, (2 * SCROLL_DEADZONE))){
            ctrl_trigger =true;
        }
    }

    if(active_flag){
        temp_h = (float)input_h;
        temp_v = -(float)input_v;

       temp_h = 0.0002 * temp_h + fraction_h;
       temp_v = 0.0002 * temp_v + fraction_v;

       temp_h = temp_h > 127 ? 127 : temp_h;
       temp_h = temp_h < -127 ? -127 : temp_h;
       temp_v = temp_v > 127 ? 127 : temp_v;
       temp_v = temp_v < -127 ? -127 : temp_v;

       scroll_report.h = (int8_t)temp_h;
       scroll_report.v = (int8_t)temp_v;

       fraction_h = temp_h - (int)temp_h;
       fraction_v = temp_v - (int)temp_v;
    }
    
    return scroll_report;
}

void dpad_mode_task(int input_x, int input_y, int lr_flag){
    int layer_num[4] = {0,0,0,0};

    bool active_flag = false;
    active_flag = deadzone_checker(input_x, input_y, DPAD_DEADZONE);
    float each_axis_deadzone = (float)DPAD_DEADZONE / sqrt(2);
    
    if(input_y < -each_axis_deadzone){
        if(active_flag){
            if(dpad_flag[lr_flag][0]){
                layer_num[0] = layer_switch_get_layer(dpad_keypos[lr_flag][0]);
                register_code(keymap_key_to_keycode(layer_num[0], dpad_keypos[lr_flag][0]));
                dpad_flag[lr_flag][0] = false;
            }
        }
    }else if(!dpad_flag[lr_flag][0]){
            layer_num[0] = layer_switch_get_layer(dpad_keypos[lr_flag][0]);
            unregister_code(keymap_key_to_keycode(layer_num[0], dpad_keypos[lr_flag][0]));
            dpad_flag[lr_flag][0] = true;
    }
    if(input_x < -each_axis_deadzone){
        if(active_flag){
            if(dpad_flag[lr_flag][1]){
                layer_num[1] = layer_switch_get_layer(dpad_keypos[lr_flag][1]);
                register_code(keymap_key_to_keycode(layer_num[1], dpad_keypos[lr_flag][1]));
                dpad_flag[lr_flag][1] = false;
            }
        }
    }else if(!dpad_flag[lr_flag][1]){
            layer_num[1] = layer_switch_get_layer(dpad_keypos[lr_flag][1]);
            unregister_code(keymap_key_to_keycode(layer_num[1], dpad_keypos[lr_flag][1]));
            dpad_flag[lr_flag][1] = true;
    }
    if(input_x > each_axis_deadzone){
        if(active_flag){
            if(dpad_flag[lr_flag][2]){
                layer_num[2] = layer_switch_get_layer(dpad_keypos[lr_flag][2]);
                register_code(keymap_key_to_keycode(layer_num[2], dpad_keypos[lr_flag][2]));
                dpad_flag[lr_flag][2] = false;
            }
        }
    }else if(!dpad_flag[lr_flag][2]){
            layer_num[2] = layer_switch_get_layer(dpad_keypos[lr_flag][2]);
            unregister_code(keymap_key_to_keycode(layer_num[2], dpad_keypos[lr_flag][2]));
            dpad_flag[lr_flag][2] = true;
    }
    if(input_y > each_axis_deadzone){
        if(active_flag){
            if(dpad_flag[lr_flag][3]){
                layer_num[3] = layer_switch_get_layer(dpad_keypos[lr_flag][3]);
                register_code(keymap_key_to_keycode(layer_num[3], dpad_keypos[lr_flag][3]));
                dpad_flag[lr_flag][3] = false;
            }
        }
    }else if(!dpad_flag[lr_flag][3]){
            layer_num[3] = layer_switch_get_layer(dpad_keypos[lr_flag][3]);
            unregister_code(keymap_key_to_keycode(layer_num[3], dpad_keypos[lr_flag][3]));
            dpad_flag[lr_flag][3] = true;
    }
}

// int fusion_mode_way_checker(int input_x, int input_y){
//     int ans = 0;
//     if(abs(input_x) > abs(input_y)){
//         if(input_x > MOUSE_DEADZONE){
//             ans = LEFT_NUMBER;
//         }else if(input_x < -MOUSE_DEADZONE){
//             ans = -LEFT_NUMBER;
//         }
//     }else if(abs(input_x) < abs(input_y)){
//         if(input_y > MOUSE_DEADZONE){
//             ans = -UP_NUMBER;
//         }else if(input_y < -MOUSE_DEADZONE){
//             ans = UP_NUMBER;
//         }
//     }
//     return ans;
// }

// int barrelroll_count = 0; // 要らなくなったら削除

// report_mouse_t fusion_mode_barrelroll(int8_t rotate_way){
//     report_mouse_t mouse_report;
//     mouse_report.x = 0;
//     mouse_report.y = 0;
//     mouse_report.h = 0;
//     mouse_report.v = 0;
//     mouse_report.buttons = 0;
//     mouse_report.buttons |= MOUSE_BTN3;
//     shift_trigger = true;
    
//     switch(barrelroll_count){
//         case 0:
//             mouse_report.x = 2 * rotate_way;
//             break;
//         case 1:
//             mouse_report.x = rotate_way;
//             mouse_report.y = -rotate_way;
//             break;
//         case 2:
//             mouse_report.y = -2 * rotate_way;
//             break;
//         case 3:
//             mouse_report.x = -rotate_way;
//             mouse_report.y = -rotate_way;
//             break;
//         case 4:
//             mouse_report.x = -2 * rotate_way;
//             break;
//         case 5:
//             mouse_report.x = -rotate_way;
//             mouse_report.y = rotate_way;
//             break;
//         case 6:
//             mouse_report.y = 2 * rotate_way;
//             break;
//         case 7:
//             mouse_report.x = rotate_way;
//             mouse_report.y = rotate_way;
//             barrelroll_count = -1;
//             break;
//         default:
//             break;
//     }

//     barrelroll_count++;
//     return mouse_report;
// }

bool twinstick_mode = false;

report_mouse_t fusion_mode_task_all(int input[2][2], int lr_flag){
    report_mouse_t mouse_report;
    mouse_report.x = 0;
    mouse_report.y = 0;
    mouse_report.h = 0;
    mouse_report.v = 0;
    mouse_report.buttons = 0;
    bool active_flag[2][3] = {
        {false, false, false},
        {false, false, false}
    };
    int state = 0;

    for(int i=0;i<2;i++){
        if(deadzone_checker(input[i][0], input[i][1], MOUSE_DEADZONE)){
            active_flag[i][0] = true;
        }
        if(deadzone_checker(5 * input[i][0], 5 * input[i][1], (4 * MOUSE_DEADZONE))){
            active_flag[i][1] = true;
        }
        if(deadzone_checker(10 * input[i][0], 10 * input[i][1], (7 * MOUSE_DEADZONE))){
            active_flag[i][2] = true;
        }
    }

    int input_0[2] = {input[0][0], input[0][1]};
    int input_1[2] = {input[1][0], input[1][1]};

    if(twinstick_mode){
        if((active_flag[0][2]) && (active_flag[1][2])){
            if (vector2d_normalized_dot(input_0, input_1) > (0.5)){
                state = SAME_WAY_NUMBER;
            }else if (vector2d_normalized_dot(input_0, input_1) < (-0.5)){
                state = OPPOSITE_WAY_NUMBER;
            }
        }else if((!active_flag[0][2]) && (!active_flag[1][2])){
            twinstick_mode = false;
            state = 0;
        }
    }else{
        if((active_flag[0][1]) && (active_flag[1][1])){
            // if (vector2d_normalized_dot(input_0, input_1) > (0.86)){
            //     state = SAME_WAY_NUMBER;
            // }else if (vector2d_normalized_dot(input_0, input_1) < (-0.86)){
            //     state = OPPOSITE_WAY_NUMBER;
            // }
            twinstick_mode = true;
        }else if(active_flag[1-lr_flag][0]){
            state = MOUSE_ONLY_NUMBER;
        }else if(active_flag[lr_flag][0]){
            state = FUSION_ONLY_NUMBER;
        }
    }

    // if((active_flag[1-lr_flag][1]) && (!active_flag[lr_flag][0])){
    //     state = MOUSE_ONLY_NUMBER;
    // }else if((!active_flag[1-lr_flag][0]) && (active_flag[lr_flag][1])){
    //     state = FUSION_ONLY_NUMBER;
    // }else if((active_flag[1-lr_flag][0]) && (active_flag[lr_flag][0])){
    //     int joystick_way[2] = {
    //         fusion_mode_way_checker(input[0][0], input[0][1]),
    //         fusion_mode_way_checker(input[1][0], input[1][1])
    //     };
    //     if(joystick_way[0] == joystick_way[1]){
    //         state = SAME_WAY_NUMBER;
    //         twinstick_lock_flag = true;
    //     }else{
    //         switch(joystick_way[0] * abs(joystick_way[1])){
    //             case (UP_NUMBER * UP_NUMBER):
    //                 // 右回り
    //                 state = -LEFT_NUMBER;
    //                 break;
    //             case (-UP_NUMBER * UP_NUMBER):
    //                 // 左回り
    //                 state = LEFT_NUMBER;
    //                 break;
    //             case (LEFT_NUMBER * LEFT_NUMBER):
    //                 // 拡大
    //                 state = UP_NUMBER;
    //                 twinstick_lock_flag = true;
    //                 break;
    //             case (-LEFT_NUMBER * LEFT_NUMBER):
    //                 // 縮小
    //                 state = -UP_NUMBER;
    //                 twinstick_lock_flag = true;
    //                 break;
    //             default:
    //                 break;
    //         }
    //     }
    // }else if((!active_flag[1-lr_flag][0]) && (!active_flag[lr_flag][0])){
    //     twinstick_lock_flag = false;
    // }

    // if(twinstick_lock_flag){
    //     if(current_state == SAME_WAY_NUMBER){
    //         state = current_state;
    //         shift_trigger = true;
    //     }
    //     if(current_state == UP_NUMBER){
    //         state = current_state;
    //         ctrl_trigger = true;
    //     }
    //     if(current_state == -UP_NUMBER){
    //         state = current_state;
    //         ctrl_trigger = true;
    //     }
    // }

    int mean_input[3] = {
        (input[0][0] + input[1][0]) /3,
        (input[0][1] + input[1][1]) /3,
        // (input[0][0] > input[1][0]) ? input[0][0] : input[1][0],
        // (input[0][1] > input[1][1]) ? input[0][1] : input[1][1],
        (abs(input[0][0]) + abs(input[1][0])) /4
    };

    switch(state){
        case SAME_WAY_NUMBER:
            mouse_report = mouse_mode_task(mean_input[0], mean_input[1], MOUSE_BTN3, true);
            shift_trigger = true;
            break;
        case OPPOSITE_WAY_NUMBER:
            if((input[0][0] < 0)&&(input[1][0] > 0)){
                mouse_report = scroll_mode_task(0, -mean_input[2], true);
            }else if((input[0][0] > 0)&&(input[1][0] < 0)){
                mouse_report = scroll_mode_task(0, mean_input[2], true);
            }
            ctrl_trigger =true;
            break;
        // case -LEFT_NUMBER:
        //     // 右回り
        //     mouse_report = fusion_mode_barrelroll(5);
        //     break;
        // case LEFT_NUMBER:
        //     // 左回り
        //     mouse_report = fusion_mode_barrelroll(-5);
        //     break;
        // case UP_NUMBER:
        //     // 拡大
        //     mouse_report = scroll_mode_task(0, mean_input[2], true);
        //     break;
        // case -UP_NUMBER:
        //     // 縮小
        //     mouse_report = scroll_mode_task(0, -mean_input[2], true);
        //     break;
        case MOUSE_ONLY_NUMBER:
            mouse_report = mouse_mode_task(input[1-lr_flag][0],input[1-lr_flag][1], 0, false);
            break;
        case FUSION_ONLY_NUMBER:
            mouse_report = mouse_mode_task(input[lr_flag][0],input[lr_flag][1], MOUSE_BTN3, false);
            break;
        default:
            break;
    }

    return mouse_report;
}


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
    
        shift_trigger = false;
        ctrl_trigger = false;

        if(state_mds_joystick[0].mode == state_mds_joystick[1].mode){
            int temp_x = (abs(joystick_current[0][0]) > abs(joystick_current[1][0])) ? joystick_current[0][0] : joystick_current[1][0];
            int temp_y = (abs(joystick_current[0][1]) > abs(joystick_current[1][1])) ? joystick_current[0][1] : joystick_current[1][1];
    
            switch(state_mds_joystick[0].mode){
                case MOUSE_MODE_NUMBER:
                    mouse_report = mouse_mode_task(temp_x, temp_y, 0, false);
                    break;
                case SCROLL_MODE_NUMBER:
                    mouse_report = scroll_mode_task(temp_x, temp_y, false);
                    break;
                case FUSION_MODE_NUMBER:
                    mouse_report = mouse_mode_task(temp_x, temp_y, MOUSE_BTN3, true);
                    break;
                default:
                    break;
            }
        }else{
            if((state_mds_joystick[0].mode == MOUSE_MODE_NUMBER) && (state_mds_joystick[1].mode == FUSION_MODE_NUMBER)){
                mouse_report = fusion_mode_task_all(joystick_current, 1);
            }else if((state_mds_joystick[0].mode == FUSION_MODE_NUMBER) && (state_mds_joystick[1].mode == MOUSE_MODE_NUMBER)){
                mouse_report = fusion_mode_task_all(joystick_current, 0);
            }else{
                switch(state_mds_joystick[0].mode){
                    case MOUSE_MODE_NUMBER:
                        cursor_report = mouse_mode_task(joystick_current[0][0], joystick_current[0][1], 0, false);
                        break;
                    case SCROLL_MODE_NUMBER:
                        scroll_report = scroll_mode_task(joystick_current[0][0], joystick_current[0][1], false);
                        break;
                    case FUSION_MODE_NUMBER:
                        cursor_report = mouse_mode_task(joystick_current[0][0], joystick_current[0][1], MOUSE_BTN3, false);
                    break;
                }
                switch(state_mds_joystick[1].mode){
                    case MOUSE_MODE_NUMBER:
                        cursor_report = mouse_mode_task(joystick_current[1][0], joystick_current[1][1], 0, false);
                        break;
                    case SCROLL_MODE_NUMBER:
                        scroll_report = scroll_mode_task(joystick_current[1][0], joystick_current[1][1], false);
                        break;
                    case FUSION_MODE_NUMBER:
                        cursor_report = mouse_mode_task(joystick_current[1][0], joystick_current[1][1], MOUSE_BTN3, false);
                    break;
                }
                mouse_report.x = cursor_report.x;
                mouse_report.y = cursor_report.y;
                mouse_report.h = scroll_report.h;
                mouse_report.v = scroll_report.v;
                mouse_report.buttons = cursor_report.buttons;
            }
        }
        if(state_mds_joystick[0].mode == DPAD_MODE_NUMBER){
            dpad_mode_task(joystick_current[0][0], joystick_current[0][1], 0);
        }
        if(state_mds_joystick[1].mode == DPAD_MODE_NUMBER){
            dpad_mode_task(joystick_current[1][0], joystick_current[1][1], 1);
        }

        if(shift_trigger){
            if(!shift_state){
                register_code(KC_LSFT);
                shift_state = true;
            }
        }else{
            if(shift_state){
                unregister_code(KC_LSFT);
                shift_state = false;
            }
        }

        if(ctrl_trigger){
            if(!ctrl_state){
                register_code(KC_LCTL);
                ctrl_state = true;
            }
        }else{
            if(ctrl_state){
                unregister_code(KC_LCTL);
                ctrl_state = false;
            }
        }

    }else{
        joystick_check();
    }

    return pointing_device_task_user(mouse_report);
}