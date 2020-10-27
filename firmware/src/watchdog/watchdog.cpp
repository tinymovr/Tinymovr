
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

#include <src/watchdog/watchdog.hpp>

int32_t ClkValueForMs(int32_t ms)
{
    return (int32_t)((float)ms * 1.201);
}

Watchdog::Watchdog()
{
    uint16_t val = ClkValueForMs(config.timeout);

    PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_WRITE_AVALABLE;

    PAC55XX_WWDT->WWDTCTL.CLKSEL = WWDTCTL_CLKSEL_ROSCCLK;
    PAC55XX_WWDT->WWDTCTL.CLKDIV = WWDT_CLKDIV_DIV16384;
    // Watchdog counter must be less than the WINDOW
    // value when it's cleared otherwise reset will occur
    PAC55XX_WWDT->WWDTCDCTL.WINDOW = val;
    // Starting value for WWDT count down
    PAC55XX_WWDT->WWDTCDCTL.CDV = val;
    
    PAC55XX_WWDT->WWDTCTL.RSTEN = 1;
    PAC55XX_WWDT->WWDTCTL.INTEN = 0;

    if (config.auto_enable)
    {
        SetEnabled(true);
    }

    PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_READ_ONLY;
}

void Watchdog::SetEnabled(bool enabled)
{
    PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_WRITE_AVALABLE;
    if (enabled)
    {
        PAC55XX_WWDT->WWDTCTL.EN = 1;
    }
    else
    {
        PAC55XX_WWDT->WWDTCTL.EN = 0;
    }
    PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_READ_ONLY;
}

bool Watchdog::GetEnabled(void)
{
    return PAC55XX_WWDT->WWDTCTL.EN;
}

void Watchdog::SetAutoEnable(bool auto_enable)
{
    if (auto_enable)
    {
        config.auto_enable = 1;
    }
    else
    {
        config.auto_enable = 0;
    }
}

bool Watchdog::GetAutoEnable(void)
{
    return config.auto_enable;
}

void Watchdog::Feed(void)
{
    PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_WRITE_AVALABLE;
    // Write any value to WWDTCLEAR to start WWDT over
    PAC55XX_WWDT->WWDTCLEAR = 1;
    PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_READ_ONLY;
}

int32_t Watchdog::GetTimeout(void)
{
    return config.timeout;
}

void Watchdog::SetTimeout(int32_t new_timeout)
{
    if (new_timeout > 0)
    {
        config.timeout = new_timeout;

        uint16_t val = (uint16_t)ClkValueForMs(config.timeout);
        PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_WRITE_AVALABLE;
        // Watchdog counter must be less than the WINDOW
        // value when it's cleared otherwise reset will occur
        PAC55XX_WWDT->WWDTCDCTL.WINDOW = val;
        // Starting value for WWDT count down
        PAC55XX_WWDT->WWDTCDCTL.CDV = val;
        PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_READ_ONLY;
    }
}

