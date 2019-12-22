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


typedef struct _esp32_ledcontrol_obj_t {
    mp_obj_base_t base;
} _esp32_ledcontrol_obj_t;

const mp_obj_type_t esp32_ledcontrol_type;


STATIC mp_obj_t esp32_ledcontrol_make_new(const mp_obj_type_t *type,
        size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    gpio_num_t pin_id = machine_pin_get_id(args[0]);

    _esp32_ledcontrol_obj_t *self = m_new_obj(_esp32_ledcontrol_obj_t);
    self->base.type = &esp32_ledcontrol_type;

    return MP_OBJ_FROM_PTR(self);
}


STATIC const mp_rom_map_elem_t esp32_ledcontrol_locals_dict_table[] = {
};

STATIC MP_DEFINE_CONST_DICT(esp32_ledcontrol_locals_dict, esp32_ledcontrol_locals_dict_table);

const mp_obj_type_t esp32_ledcontrol_type = {
    { &mp_type_type },
    .name = MP_QSTR_LEDControl,
    .make_new = esp32_ledcontrol_make_new,
    .locals_dict = (mp_obj_t)&esp32_ledcontrol_locals_dict,
};
