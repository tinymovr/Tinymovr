
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

#include <stdbool.h>
#include "src/common.h"

#define ENCODER_TICKS                (8192)
#define ENCODER_HALF_TICKS           (ENCODER_TICKS / 2)
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

struct MA702State
{
	int16_t angle;
};

void MA_Init(void);
PAC5XXX_RAMFUNC int16_t MA_GetAngle(void);
PAC5XXX_RAMFUNC void MA_RequestAngle(void);
PAC5XXX_RAMFUNC void MA_UpdateAngle(bool check_error);

#endif /* ENCODERS_MA702_H_ */
