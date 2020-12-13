
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

#include "src/common.h"
#include "src/system/system.h"
#include "src/ssp/ssp_func.h"
#include <src/encoder/encoder.h>

static struct MA702State state = { 0 };

void MA_Init(void)
{
    ssp_init(SSPD, SSP_MS_MASTER, 0, 0); // Mode 0
    system_delay_us(16000); // ensure 16ms sensor startup time as per the datasheet
}

PAC5XXX_RAMFUNC int16_t MA_GetAngle(void)
{
    return state.angle_buffer;
}

PAC5XXX_RAMFUNC void MA_ReadAngle(void)
{
    // TODO: Make SSP reference configurable
	ssp_write_one(SSPD, MA_CMD_ANGLE);
    while (!PAC55XX_SSPD->STAT.RNE) {}
    const int16_t new_angle = (PAC55XX_SSPD->DAT.DATA) >> 3;
    const int16_t delta = state.angle_buffer - new_angle;
    if (delta > MAX_ALLOWED_DELTA || delta > -MAX_ALLOWED_DELTA)
    {
    	set_error_flag(ENCODER_ERR_UNSTABLE, MODULE_ENCODER);
    }
    state.angle_buffer = new_angle;
}
