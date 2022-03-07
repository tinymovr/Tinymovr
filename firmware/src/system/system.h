
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

#ifndef SYSTEM_SYSTEM_H_
#define SYSTEM_SYSTEM_H_

#include "src/common.h"

typedef enum {
	ENCODER_MA7XX = 0,
	ENCODER_HALL = 1
} EncoderType;

typedef struct
{
    EncoderType encoder_type;
} SystemConfig;

void system_init(void);
void system_reset(void);

PAC5XXX_RAMFUNC bool error_flags_exist(void);
PAC5XXX_RAMFUNC uint8_t* get_error_flags(void);
PAC5XXX_RAMFUNC void add_error_flag(uint8_t error);
PAC5XXX_RAMFUNC bool health_check(void);

PAC5XXX_RAMFUNC EncoderType system_get_encoder_type(void);
void system_set_encoder_type(EncoderType enc_type);

SystemConfig* system_get_config(void);
void system_restore_config(SystemConfig* config_);

#endif /* SYSTEM_SYSTEM_H_ */
