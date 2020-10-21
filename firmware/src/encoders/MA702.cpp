
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

#include <src/encoders/MA702.hpp>
#include <src/system/system.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include "src/ssp/ssp_func.h"

#ifdef __cplusplus
}
#endif

PAC5XXX_RAMFUNC static inline void MA_Transfer(MA702Command cmd);

static struct MA702State state = {
        .angle_buffer = 0,
        .last_command = MA_CMD_NOP
};

void MA_Init(void)
{
    ssp_init(SSPD, SSP_MS_MASTER, 0, 0); // Mode 0
    System_DelayUS(16000); // ensure 16ms sensor startup time
}

PAC5XXX_RAMFUNC int MA_GetAngle(void)
{
    return (int) (state.angle_buffer);
}

PAC5XXX_RAMFUNC void MA_ReadAngle(void)
{
    // TODO: Make SSP reference configurable
    MA_Transfer(MA_CMD_ANGLE);
    while (!PAC55XX_SSPD->STAT.RNE) {}
    state.angle_buffer = (PAC55XX_SSPD->DAT.DATA) >> 3;
}

PAC5XXX_RAMFUNC static inline void MA_Transfer(MA702Command cmd)
{
    ssp_write_one(SSPD, cmd);
    state.last_command = cmd;
}
