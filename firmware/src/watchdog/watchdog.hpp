
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

#ifndef WATCHDOG_WATCHDOG_H_
#define WATCHDOG_WATCHDOG_H_

#include <src/common.hpp>

class Watchdog
{
public:
    Watchdog();
    void Watchdog_SetEnabled(bool enabled);
    bool Watchdog_GetEnabled(void);
    void Watchdog_SetAutoEnable(bool auto_enable);
    bool Watchdog_GetAutoEnable(void);
    int32_t Watchdog_GetTimeout(void);
    void Watchdog_SetTimeout(int32_t new_timeout);
    PAC5XXX_RAMFUNC void Watchdog_Feed(void);

private:
    struct WatchdogConfig
    {
        bool auto_enable = false;
        uint16_t timeout = 1000; // ms
    };
    WatchdogConfig config;
}

#endif /* WATCHDOG_WATCHDOG_H_ */
