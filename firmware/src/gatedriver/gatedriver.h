
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

#pragma once

#include <src/common.h>

void gate_driver_enable(void);
void gate_driver_disable(void);

static inline void m1_u_set_duty(const float duty)
{
    uint16_t val = ((uint16_t)(duty * (TIMER_FREQ_HZ/PWM_FREQ_HZ) )) >>1;
    PAC55XX_TIMERA->CCTR4.CTR = val;
}

static inline void m1_v_set_duty(const float duty)
{
    uint16_t val = ((uint16_t)(duty * (TIMER_FREQ_HZ/PWM_FREQ_HZ) )) >>1;
    PAC55XX_TIMERA->CCTR5.CTR = val;
}

static inline void m1_w_set_duty(const float duty)
{
    uint16_t val = ((uint16_t)(duty * (TIMER_FREQ_HZ/PWM_FREQ_HZ) )) >>1;
    PAC55XX_TIMERA->CCTR6.CTR = val;
}

static inline void gate_driver_set_duty_cycle(const FloatTriplet *dutycycles)
{
	m1_u_set_duty(dutycycles->A);
	m1_v_set_duty(dutycycles->B);
	m1_w_set_duty(dutycycles->C);
}

static inline bool gate_driver_is_enabled(void)
{
    return ((pac5xxx_tile_register_read(ADDR_ENDRV) & 0x1) == 1);
}
