
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

#include "src/common.h"
#include "timer.h"

void Timer_Init(void)
{
    // Configure Timer A Controls
    pac5xxx_timer_clock_config(TimerA, TXCTL_CS_ACLK, TXCTL_PS_DIV);                   // Configure timer clock input for ACLK, divider
    pac5xxx_timer_base_config(TimerA, (TIMER_FREQ_HZ/(2*PWM_FREQ_HZ)), AUTO_RELOAD,
            TxCTL_MODE_UPDOWN, TIMER_SLAVE_SYNC_DISABLE);                               // Configure timer frequency and count mode

    // Configure Dead time generators
    PAC55XX_TIMERA->CTL.DTGCLK = BEFORE_ACLK_DIVIDER;                                   // 0--> The DTGCLK is the clock before the TACTL.CLKDIV clock divider.
                                                                                        // 1--> The DTGCLK is the clock after the TACTL.CLKDIV clock divider.

    pac5xxx_dtg_config2(&(PAC55XX_TIMERA->DTGCTL0), RED_DEATH_TIMET, FED_DEATH_TIMET);  // Configure DTGA0 for phase U
    pac5xxx_dtg_config2(&(PAC55XX_TIMERA->DTGCTL1), RED_DEATH_TIMET, FED_DEATH_TIMET);  // Configure DTGA1 for phase V
    pac5xxx_dtg_config2(&(PAC55XX_TIMERA->DTGCTL2), RED_DEATH_TIMET, FED_DEATH_TIMET);  // Configure DTGA2 for phase W

    PAC55XX_TIMERA->CCTR4.CTR = 0;
    PAC55XX_TIMERA->CCTR5.CTR = 0;
    PAC55XX_TIMERA->CCTR6.CTR = 0;
}
