/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Laszlo Zsolt Nagy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

#include "py/nlr.h"
#include "py/runtime.h"
#include "modmachine.h"
#include "mphalport.h"

// Default PWM frequency
#define LEDCONTROL_DEFAULT_FREQ (5000)
// Default PWM resolution
#define LEDCONTROL_DEFAULT_RES (LEDC_TIMER_10_BIT)

typedef struct _esp32_ledcontrol_obj_t {
    mp_obj_base_t base;
    u_int timer_num;
    u_int freq_hz;
    u_int duty_res;
} _esp32_ledcontrol_obj_t;

const mp_obj_type_t esp32_ledcontrol_type;

// Defined in machine_time.c; simply added the error message
// Fixme: Should use this updated error hadline more widely in the ESP32 port.
//        At least update the method in machine_time.c.
STATIC esp_err_t check_esp_err(esp_err_t code) {
    if (code) {
        mp_raise_msg(&mp_type_OSError, esp_err_to_name(code));
    }

    return code;
}

// Number of LEDControl objects created
STATIC u_int instances = 0;

STATIC mp_obj_t esp32_ledcontrol_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_timer_num,       MP_ARG_REQUIRED |                  MP_ARG_INT, {.u_int = LEDC_TIMER_0} },
        { MP_QSTR_freq_hz,         MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = LEDCONTROL_DEFAULT_FREQ} },
        { MP_QSTR_duty_res,        MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = LEDCONTROL_DEFAULT_RES} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ledc_timer_config_t ledc_timer = {
        .duty_resolution = args[2].u_int,     // resolution of PWM duty
        .freq_hz = args[1].u_int,             // frequency of PWM signal
        .speed_mode = LEDC_HIGH_SPEED_MODE,   // timer mode
        .timer_num = args[0].u_int,           // timer index
    };
    check_esp_err(ledc_timer_config(&ledc_timer));
    
    if (instances==0) {
        check_esp_err(ledc_fade_func_install(0  /* (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED */));
    }
    instances += 1;

    _esp32_ledcontrol_obj_t *self = m_new_obj(_esp32_ledcontrol_obj_t);
    self->base.type = &esp32_ledcontrol_type;
    self->timer_num = ledc_timer.timer_num;
    self->freq_hz = ledc_timer.freq_hz;
    self->duty_res = ledc_timer.duty_resolution;

    return MP_OBJ_FROM_PTR(self);
}


STATIC void esp32_ledconrol_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    _esp32_ledcontrol_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "RMT(timer=%u, freq_hz=%u, duty_res=%u)",
            self->timer_num, self->freq_hz, self->duty_res);
}


STATIC mp_obj_t esp32_ledcontrol_deinit(mp_obj_t self_in) {
    _esp32_ledcontrol_obj_t *self = MP_OBJ_TO_PTR(self_in);
    instances -= 1;
    if (instances==0) {
        ledc_fade_func_uninstall();
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_ledcontrol_deinit_obj, esp32_ledcontrol_deinit);



// Return the timer number
STATIC mp_obj_t esp32_ledcontrol_timer_num(mp_obj_t self_in) {
    _esp32_ledcontrol_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->timer_num);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_ledcontrol_timer_num_obj, esp32_ledcontrol_timer_num);

// Return the frequency
STATIC mp_obj_t esp32_ledcontrol_freq_hz(mp_obj_t self_in) {
    _esp32_ledcontrol_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->freq_hz);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_ledcontrol_freq_hz_obj, esp32_ledcontrol_freq_hz);

// Return the duty resolution
STATIC mp_obj_t esp32_ledcontrol_duty_res(mp_obj_t self_in) {
    _esp32_ledcontrol_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->duty_res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_ledcontrol_duty_res_obj, esp32_ledcontrol_duty_res);

STATIC const mp_rom_map_elem_t esp32_ledcontrol_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&esp32_ledcontrol_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&esp32_ledcontrol_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_timer_num), MP_ROM_PTR(&esp32_ledcontrol_timer_num_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq_hz), MP_ROM_PTR(&esp32_ledcontrol_freq_hz_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty_res), MP_ROM_PTR(&esp32_ledcontrol_duty_res_obj) },

};

STATIC MP_DEFINE_CONST_DICT(esp32_ledcontrol_locals_dict, esp32_ledcontrol_locals_dict_table);

const mp_obj_type_t esp32_ledcontrol_type = {
    { &mp_type_type },
    .name = MP_QSTR_LEDControl,
    .make_new = esp32_ledcontrol_make_new,
    .print = esp32_ledconrol_print,
    .locals_dict = (mp_obj_t)&esp32_ledcontrol_locals_dict,
};
