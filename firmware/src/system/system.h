
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

typedef struct {
    float Vbus;
    uint8_t errors;
} SystemState;

typedef struct {
    float Vbus_tau;
    float Vbus_D;
} SystemConfig;

void system_init(void);
PAC5XXX_RAMFUNC void system_update(void);
void system_reset(void);

PAC5XXX_RAMFUNC inline uint32_t system_get_uid(void)
{
    return PAC55XX_INFO1->UNIQUEID[0] ^ PAC55XX_INFO1->UNIQUEID[1] ^ PAC55XX_INFO1->UNIQUEID[2];
}

PAC5XXX_RAMFUNC bool error_flags_exist(void);
PAC5XXX_RAMFUNC uint8_t *get_error_flags(void);
PAC5XXX_RAMFUNC void add_error_flag(uint8_t error);
PAC5XXX_RAMFUNC bool health_check(void);

#endif /* SYSTEM_SYSTEM_H_ */
