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
bool dpad_available[2][4] ={
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

    if(state_mds_main.savedata0 < 1 || state_mds_main.savedata0 > mode_num){
        state_mds_joystick[0].mode = state_mds_main.savedata0;
    }else{
        state_mds_joystick[0].mode = MOUSE_MODE_NUMBER;
    }

    if(state_mds_main.savedata1 < 1 || state_mds_main.savedata1 > mode_num){
        state_mds_joystick[1].mode = state_mds_main.savedata1;
    }else{
        state_mds_joystick[1].mode = MOUSE_MODE_NUMBER;
    }
}

void joystick_check(void){
    int joystick_check[2][2] = {
        {0,0},
        {0,0}
    };
    joystick_check[0][0] = analogReadPin(LEFT_JOYSTICK_XPIN);
    joystick_check[0][1] = analogReadPin(LEFT_JOYSTICK_YPIN);
    joystick_check[1][0] = analogReadPin(RIGHT_JOYSTICK_XPIN);
    joystick_check[1][1] = analogReadPin(RIGHT_JOYSTICK_YPIN);

    if(joystick_check[0][0] < 50 || joystick_check[0][1] < 50){
        state_mds_joystick[0].connected = false;
    }else{
        state_mds_joystick[0].connected = true;
    }
    if(joystick_check[1][0] < 50 || joystick_check[1][1] < 50){
        state_mds_joystick[1].connected = false;
    }else{
        state_mds_joystick[1].connected = true;
    }
}


void joystick_calibration(void){
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
    joystick_calibration();
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
        joystick_calibration();
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
    float deno = vector2d_magnitude(input_a) * vector2d_magnitude(input_b);
    if(deno){
        return vector2d_dot(input_a, input_b) / deno;
    }else{
        return 0.0f;
    }
}

bool deadzone_checker(int input_x, int input_y, int deadzone_val){
    int32_t magnitude = (int32_t)input_x * input_x + (int32_t)input_y * input_y;
    int32_t dz_sq = (int32_t)deadzone_val * deadzone_val;
    return magnitude > dz_sq;
}

float fraction_x = 0;
float fraction_y = 0;

report_mouse_t mouse_mode_task(int input_x, int input_y, int btn_num){
    report_mouse_t mouse_report;
    mouse_report.x = 0;
    mouse_report.y = 0;
    mouse_report.h = 0;
    mouse_report.v = 0;
    mouse_report.buttons = 0;

    float temp_x = 0;
    float temp_y = 0;
    bool active_flag = deadzone_checker(input_x, input_y, MOUSE_DEADZONE);

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

report_mouse_t scroll_mode_task(int input_h, int input_v){
    report_mouse_t scroll_report;
    scroll_report.x = 0;
    scroll_report.y = 0;
    scroll_report.h = 0;
    scroll_report.v = 0;
    scroll_report.buttons = 0;

    float temp_h = 0;
    float temp_v = 0;
    bool active_flag = deadzone_checker(input_h, input_v, SCROLL_DEADZONE);

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
            if(dpad_available[lr_flag][0]){
                layer_num[0] = layer_switch_get_layer(dpad_keypos[lr_flag][0]);
                register_code(keymap_key_to_keycode(layer_num[0], dpad_keypos[lr_flag][0]));
                dpad_available[lr_flag][0] = false;
            }
        }
    }else if(!dpad_available[lr_flag][0]){
            layer_num[0] = layer_switch_get_layer(dpad_keypos[lr_flag][0]);
            unregister_code(keymap_key_to_keycode(layer_num[0], dpad_keypos[lr_flag][0]));
            dpad_available[lr_flag][0] = true;
    }
    if(input_x < -each_axis_deadzone){
        if(active_flag){
            if(dpad_available[lr_flag][1]){
                layer_num[1] = layer_switch_get_layer(dpad_keypos[lr_flag][1]);
                register_code(keymap_key_to_keycode(layer_num[1], dpad_keypos[lr_flag][1]));
                dpad_available[lr_flag][1] = false;
            }
        }
    }else if(!dpad_available[lr_flag][1]){
            layer_num[1] = layer_switch_get_layer(dpad_keypos[lr_flag][1]);
            unregister_code(keymap_key_to_keycode(layer_num[1], dpad_keypos[lr_flag][1]));
            dpad_available[lr_flag][1] = true;
    }
    if(input_x > each_axis_deadzone){
        if(active_flag){
            if(dpad_available[lr_flag][2]){
                layer_num[2] = layer_switch_get_layer(dpad_keypos[lr_flag][2]);
                register_code(keymap_key_to_keycode(layer_num[2], dpad_keypos[lr_flag][2]));
                dpad_available[lr_flag][2] = false;
            }
        }
    }else if(!dpad_available[lr_flag][2]){
            layer_num[2] = layer_switch_get_layer(dpad_keypos[lr_flag][2]);
            unregister_code(keymap_key_to_keycode(layer_num[2], dpad_keypos[lr_flag][2]));
            dpad_available[lr_flag][2] = true;
    }
    if(input_y > each_axis_deadzone){
        if(active_flag){
            if(dpad_available[lr_flag][3]){
                layer_num[3] = layer_switch_get_layer(dpad_keypos[lr_flag][3]);
                register_code(keymap_key_to_keycode(layer_num[3], dpad_keypos[lr_flag][3]));
                dpad_available[lr_flag][3] = false;
            }
        }
    }else if(!dpad_available[lr_flag][3]){
            layer_num[3] = layer_switch_get_layer(dpad_keypos[lr_flag][3]);
            unregister_code(keymap_key_to_keycode(layer_num[3], dpad_keypos[lr_flag][3]));
            dpad_available[lr_flag][3] = true;
    }
}

bool twinstick_mode = false;
uint16_t twinstick_start_time = 0;
uint16_t twinstick_exit_time = 0;
bool exit_flag = false;
int current_state = 0;

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
            exit_flag = false;

            if (vector2d_normalized_dot(input_0, input_1) > (0.5)){
                if (current_state == SAME_WAY_START){
                    if(timer_elapsed(twinstick_start_time) < TWINSTICK_START_MS){
                        state = SAME_WAY_START;
                    }else{
                        state = SAME_WAY_ACTIVE;
                    }
                }
    
                if (current_state == SAME_WAY_ACTIVE){
                    state = SAME_WAY_ACTIVE;
                }
            }else if (vector2d_normalized_dot(input_0, input_1) < (-0.5)){
                if (current_state == OPPOSITE_WAY_START){
                    if(timer_elapsed(twinstick_start_time) < TWINSTICK_START_MS){
                        state = OPPOSITE_WAY_START;
                    }else{
                        state = OPPOSITE_WAY_ACTIVE;
                    }
                }
    
                if (current_state == OPPOSITE_WAY_ACTIVE){
                    state = OPPOSITE_WAY_ACTIVE;
                }
            }
        }else{
            if (!exit_flag){
                state = current_state;
                twinstick_exit_time = timer_read();
                exit_flag = true;
            }else{
                if (timer_elapsed(twinstick_exit_time) >= TWINSTICK_EXIT_MS){
                    twinstick_mode = false;
                    exit_flag = false;
                    state = 0;
                }else{
                    state = current_state;
                }
            }
        }
    }else{
        if((active_flag[0][1]) && (active_flag[1][1])){
            if (vector2d_normalized_dot(input_0, input_1) > (0.86)){
                state = SAME_WAY_START;
                twinstick_start_time = timer_read();
                twinstick_mode = true;
            }else if (vector2d_normalized_dot(input_0, input_1) < (-0.86)){
                state = OPPOSITE_WAY_START;
                twinstick_start_time = timer_read();
                twinstick_mode = true;
            }
        }else if(active_flag[1-lr_flag][0]){
            state = MOUSE_ONLY_ACTIVE;
        }else if(active_flag[lr_flag][0]){
            state = FUSION_ONLY_ACTIVE;
        }
    }

    int mean_input[3] = { // 分母が2ではないが、これはテスト運用の際にカーソル移動量が過剰だったため。
        (input[0][0] + input[1][0]) /3,
        (input[0][1] + input[1][1]) /3,
        (abs(input[0][0]) + abs(input[1][0])) /4
    };

    switch(state){
        case SAME_WAY_ACTIVE:
            mouse_report = mouse_mode_task(mean_input[0], mean_input[1], MOUSE_BTN3);
        case SAME_WAY_START:
            shift_trigger = true;
            break;
        case OPPOSITE_WAY_ACTIVE:
            if((input[0][0] < 0)&&(input[1][0] > 0)){
                mouse_report = scroll_mode_task(0, -mean_input[2]);
            }else if((input[0][0] > 0)&&(input[1][0] < 0)){
                mouse_report = scroll_mode_task(0, mean_input[2]);
            }
        case OPPOSITE_WAY_START:
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
        case MOUSE_ONLY_ACTIVE:
            mouse_report = mouse_mode_task(input[1-lr_flag][0],input[1-lr_flag][1], 0);
            break;
        case FUSION_ONLY_ACTIVE:
            mouse_report = mouse_mode_task(input[lr_flag][0],input[lr_flag][1], MOUSE_BTN3);
            break;
        default:
            break;
    }

    current_state = state;
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
                    mouse_report = mouse_mode_task(temp_x, temp_y, 0);
                    break;
                case SCROLL_MODE_NUMBER:
                    mouse_report = scroll_mode_task(temp_x, temp_y);
                    break;
                case FUSION_MODE_NUMBER:
                    mouse_report = mouse_mode_task(temp_x, temp_y, MOUSE_BTN3);
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
                        cursor_report = mouse_mode_task(joystick_current[0][0], joystick_current[0][1], 0);
                        break;
                    case SCROLL_MODE_NUMBER:
                        scroll_report = scroll_mode_task(joystick_current[0][0], joystick_current[0][1]);
                        break;
                    case FUSION_MODE_NUMBER:
                        cursor_report = mouse_mode_task(joystick_current[0][0], joystick_current[0][1], MOUSE_BTN3);
                    break;
                }
                switch(state_mds_joystick[1].mode){
                    case MOUSE_MODE_NUMBER:
                        cursor_report = mouse_mode_task(joystick_current[1][0], joystick_current[1][1], 0);
                        break;
                    case SCROLL_MODE_NUMBER:
                        scroll_report = scroll_mode_task(joystick_current[1][0], joystick_current[1][1]);
                        break;
                    case FUSION_MODE_NUMBER:
                        cursor_report = mouse_mode_task(joystick_current[1][0], joystick_current[1][1], MOUSE_BTN3);
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