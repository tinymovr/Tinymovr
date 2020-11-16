
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

#define ENCODER_TICKS                (8192)
#define ENCODER_HALF_TICKS           (ENCODER_TICKS / 2)

// MA702 commands
typedef enum {
    MA_CMD_NOP              = 0x0000,
    MA_CMD_ANGLE            = 0x0000
} MA702Command;

struct MA702State
{
    uint16_t angle_buffer;    // buffer for angle results from last transfer
    MA702Command last_command;    // command sent during last Transfer
};

struct MA702Config
{
    uint16_t kCountsPerRev;
    float kRadPerRev;
};

void MA_Init(void);
PAC5XXX_RAMFUNC int MA_GetAngle(void);
PAC5XXX_RAMFUNC void MA_ReadAngle(void);

#endif /* ENCODERS_MA702_H_ */
