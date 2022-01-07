
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
    SYSTEM_FLT_NONE         			= 0x0000,
	SYSTEM_FLT_VBUS_UNDERVOLTAGE        = 0x0001
} SystemFault;

void system_init(void);
void system_reset(void);

PAC5XXX_RAMFUNC inline uint32_t get_unique_id(void) {
    return PAC55XX_INFO1->UNIQUEID[0] ^ PAC55XX_INFO1->UNIQUEID[1] ^ PAC55XX_INFO1->UNIQUEID[2];
}

PAC5XXX_RAMFUNC uint8_t system_get_faults(void);
PAC5XXX_RAMFUNC bool system_has_faults(void);

#endif /* SYSTEM_SYSTEM_H_ */
