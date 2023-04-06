
//  * This file is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * Copyright (c) 2020-2023 Ioannis Chatzikonstantinou.
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

#include <src/common.h>
#define CLKREF_FREQ_HZ              (4000000)
#define FRCLK_FREQ_HZ               CLKREF_FREQ_HZ

typedef struct {
    float Vbus;
    uint8_t errors;
} SystemState;

typedef struct {
    float Vbus_tau;
    float Vbus_D;
} SystemConfig;

void system_init(void);
void system_update(void);
void system_reset(void);

inline uint32_t system_get_uid(void)
{
    return PAC55XX_INFO1->UNIQUEID[0] ^ PAC55XX_INFO1->UNIQUEID[1] ^ PAC55XX_INFO1->UNIQUEID[2];
}

float system_get_Vbus(void);
bool system_get_calibrated(void);
uint8_t system_get_errors(void);
bool errors_exist(void);

#endif /* SYSTEM_SYSTEM_H_ */
