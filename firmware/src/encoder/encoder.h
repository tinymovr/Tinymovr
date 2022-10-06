
//  * This file is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * Copyright (c) 2020 Ioannis Chatzikonstantinou.
//  * 
//  * This program is free software: you can redistribute it and/or modify  
//  * it under the terms of the GNU General Public License as published by  
//  * the Free Software Foundation, version 3.
//  *
//  * This program is distributed in the hope that it will be useful, but 
//  * WITHOUT ANY WARRANTY; without even the implied warranty of 
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
//  * General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License 
//  * along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <src/common.h>

// TODO: Make external in a separate unit
typedef bool (*bool_getter)(void);
typedef void (*bool_setter)(bool);
typedef uint8_t (*uint8_getter)(void);
typedef int16_t (*int16_getter)(void);
typedef void (*void_func)(void);

typedef enum {
	ENCODER_MA7XX = 0,
	ENCODER_HALL = 1
} EncoderType;

typedef struct {
    EncoderType current_encoder_type;
    uint8_getter get_error_ptr;
    bool_getter get_calibrated_ptr;
    bool_setter update_angle_ptr;
    int16_getter get_angle_ptr;
    void_func reset_encoder_ptr;
    uint16_t ticks;
} EncoderState;

typedef struct
{
    EncoderType encoder_type;
} EncoderConfig;

void encoder_init(void);
void encoder_reset(void);

PAC5XXX_RAMFUNC int16_t encoder_get_angle(void);
PAC5XXX_RAMFUNC void encoder_update(bool check_error);

PAC5XXX_RAMFUNC uint16_t encoder_get_ticks(void);
PAC5XXX_RAMFUNC float encoder_ticks_to_eangle(void);

PAC5XXX_RAMFUNC EncoderType encoder_get_type(void);
void encoder_set_type(EncoderType enc_type);

PAC5XXX_RAMFUNC bool encoder_get_calibrated(void);

PAC5XXX_RAMFUNC uint8_t encoder_get_errors(void);

EncoderConfig* encoder_get_config(void);
void encoder_restore_config(EncoderConfig* config_);

