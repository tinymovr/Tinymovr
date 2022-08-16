
//  * This file is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * 
//  * Copyright (c) 2022 @eufrizz
//  * Copyright (c) 2022 Ioannis Chatzikonstantinou.
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

/*
The watchdog is used as a timer for inactivity on the CAN bus.
When it times out, it sets the control state back to idle as a failsafe if the ESC becomes disconnected.
It can be enabled/disabled via the set_watchdog can endpoint.
Timing works via the PAC55 Watchdog timer (WWDT)
*/

#pragma once

#include "src/system/system.h"

// 4MHz CLKREF >>> FRCLK (set in system.h) >>> /32768 WWDT = 122.07 Hz
// 2^16 / 122.07 = 536.87s maximum
#define WWDT_CLKDIV 32768

static const float wwdt_freq = FRCLK_FREQ_HZ / WWDT_CLKDIV;

typedef struct
{
    bool enabled;
    bool triggered;
} TimeoutWatchdog;

void Watchdog_init(void);
void Watchdog_reset(void);
bool Watchdog_triggered(void);
void Watchdog_set_timeout_cycles(uint16_t cycles);
void Watchdog_set_timeout_seconds(float s);
void Watchdog_enable(void);
void Watchdog_disable(void);
void WWDT_process_interrupt(void);