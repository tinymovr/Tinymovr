
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

void timers_init(void)
{
    // Timer A -- PWM

    // Timer clock input for ACLK, divider
    pac5xxx_timer_clock_config(TimerA, TXCTL_CS_ACLK, TXCTL_PS_DIV);       
    // Timer frequency and count mode            
    pac5xxx_timer_base_config(TimerA, (TIMER_FREQ_HZ/(2*PWM_FREQ_HZ)), AUTO_RELOAD,
            TxCTL_MODE_UPDOWN, TIMER_SLAVE_SYNC_DISABLE);                               

    // Configure Dead time generators
    // 0--> The DTGCLK is the clock before the TACTL.CLKDIV clock divider.
    // 1--> The DTGCLK is the clock after the TACTL.CLKDIV clock divider.
    PAC55XX_TIMERA->CTL.DTGCLK = BEFORE_ACLK_DIVIDER;                                   
                                                                                        
    // Configure DTGA0, 1, 2 for phase U, V, W
    pac5xxx_dtg_config2(&(PAC55XX_TIMERA->DTGCTL0), RED_DEATH_TIMET, FED_DEATH_TIMET);  
    pac5xxx_dtg_config2(&(PAC55XX_TIMERA->DTGCTL1), RED_DEATH_TIMET, FED_DEATH_TIMET); 
    pac5xxx_dtg_config2(&(PAC55XX_TIMERA->DTGCTL2), RED_DEATH_TIMET, FED_DEATH_TIMET);

    PAC55XX_TIMERA->CCTR4.CTR = 0;
    PAC55XX_TIMERA->CCTR5.CTR = 0;
    PAC55XX_TIMERA->CCTR6.CTR = 0;

#if defined(DEBUG)

    // Timer B -- Performance Instrumentation (ddebug only)

    // Configure timer clock input for ACLK, divider
    pac5xxx_timer_clock_config(TimerB, TXCTL_CS_ACLK, TXCTL_PS_DIV);      
    // Configure timer frequency and count mode             
    pac5xxx_timer_base_config(TimerB, (TIMER_FREQ_HZ/(PWM_FREQ_HZ/100)), AUTO_RELOAD,
            TxCTL_MODE_UPDOWN, TIMER_SLAVE_SYNC_DISABLE);    

    PAC55XX_TIMERB->CCTR4.CTR = 0;
    PAC55XX_TIMERB->CCTR5.CTR = 0;
    PAC55XX_TIMERB->CCTR6.CTR = 0;                           

#endif
}
