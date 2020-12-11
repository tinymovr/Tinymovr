
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
    SYSTEM_ERR_NO_ERROR        			= 0x0000,
	SYSTEM_ERR_VBUS_UNDERVOLTAGE        = 0x0001
} SystemError;

void system_init(void);
void system_reset(void);
void system_delay_us(const uint32_t us);

PAC5XXX_RAMFUNC bool error_flags_exist(void);
PAC5XXX_RAMFUNC int* get_error_flags(void);
PAC5XXX_RAMFUNC uint8_t get_simple_error(void);
PAC5XXX_RAMFUNC void set_error_flag(uint8_t error, SystemModule module);
PAC5XXX_RAMFUNC bool health_check(void);

#endif /* SYSTEM_SYSTEM_H_ */
