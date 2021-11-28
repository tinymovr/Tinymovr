
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

#ifndef ENCODERS_MA702_H_
#define ENCODERS_MA702_H_

#include "src/common.h"

#define MAX_ALLOWED_DELTA			 (1024)

typedef enum {
    ENCODER_ERR_NO_ERROR        = 0x0000,
    ENCODER_ERR_UNSTABLE        = 0x0001
} EncoderError;

// MA702 commands
typedef enum {
    MA_CMD_NOP              = 0x0000,
    MA_CMD_ANGLE            = 0x0000
} MA702Command;

typedef struct
{
	int16_t angle;
} EncoderState;

void encoder_init(void);
PAC5XXX_RAMFUNC void encoder_send_angle_cmd(void);
PAC5XXX_RAMFUNC int16_t encoder_get_angle(void);
PAC5XXX_RAMFUNC void encoder_update_angle(bool check_error);

#endif /* ENCODERS_MA702_H_ */
