
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

#include <src/encoder/Encoder.hpp>
#include <src/system.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include <src/encoder/ssp_func.h>

#ifdef __cplusplus
}
#endif

Encoder::Encoder(System sys_): Component(sys_)
{
    ssp_init(SSPD, SSP_MS_MASTER, 0, 0); // Mode 0
    systm.DelayUS(16000); // ensure 16ms sensor startup time
}

PAC5XXX_RAMFUNC int Encoder::GetAngle(void)
{
    // TODO: Make SSP reference configurable
	ssp_write_one(SSPD, MA_CMD_ANGLE);
    while (!PAC55XX_SSPD->STAT.RNE) {}
    return (PAC55XX_SSPD->DAT.DATA) >> 3;
}
