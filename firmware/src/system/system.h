
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
    SYST_ERROR_NONE = 0,
    SYST_ERROR_VBUS_UNDERVOLTAGE = (1 << 0),
    SYST_ERROR_DRIVER_FAULT = (1 << 1)
} SystemErrors;

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

PAC5XXX_RAMFUNC float system_get_Vbus(void);
PAC5XXX_RAMFUNC uint8_t system_get_errors(void);
PAC5XXX_RAMFUNC bool errors_exist(void);

#endif /* SYSTEM_SYSTEM_H_ */
