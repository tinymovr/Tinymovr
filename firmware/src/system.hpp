
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

#ifndef SYSTEM_SYSTEM_HPP_
#define SYSTEM_SYSTEM_HPP_

#include <src/common.hpp>

class ADC;
class Encoder;
class Observer;
class Motor;
class CAN;
class UART;
class Controller;
class Watchdog;

typedef enum
{
    BEFORE_ACLK_DIVIDER                 = 0,        // The DTGCLK is the clock before the TACTL.CLKDIV clock divider
    AFTER_ACLK_DIVIDER                  = 1,        // The DTGCLK is the clock after the TACTL.CLKDIV clock divider
}TXCTL_DTCLK_Type;

typedef enum
{
    TIMER_SLAVE_SYNC_DISABLE            = 0,        // The timer auto reload
    TIMER_SLAVE_SYNC_ENABLE             = 1,        // The timer single shot
}TXCTL_SSYNC_Type;

typedef enum
{
    AUTO_RELOAD                         = 0,        // The timer auto reload
    SINGLE_SHOT                         = 1,        // The timer single shot
}TXCTL_SINGLE_Type;

class System {

public:
	ADC &adc;
	Encoder &encoder;
	Observer &observer;
	Motor &motor;
	UART &uart;
	CAN &can;
	Controller &controller;
	Watchdog &watchdog;

	System(void);
	void Spin(void);
	void Reset(void);
	void DelayUS(uint32_t us);
	void InitTimer(void);

	void HandleADCInterrupt();
	void HandleCANInterrupt();
	void HandleUARTInterrupt();

	void WaitForControlLoopInterrupt();

private:
	bool adc_interrupt = false;
	bool can_interrupt = false;
	bool uart_interrupt = false;
};

#endif /* SYSTEM_SYSTEM_HPP_ */
