
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

#include <src/comms/can/can.hpp>
#include <src/comms/uart/uart.hpp>
#include <src/encoder/encoder.hpp>
#include <src/observer/observer.hpp>
#include <src/motor/motor.hpp>
#include <src/gatedriver/gatedriver.hpp>
#include <src/adc/adc.hpp>
#include <src/controller/controller.hpp>
#include <src/watchdog/watchdog.hpp>
#include <src/common.hpp>

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
	ADC adc = ADC();
	Encoder encoder = Encoder();
	Observer observer = Observer();
	Driver driver = Driver();
	Motor motor = Motor();
	UART uart = UART();
	CAN can = CAN();
	Controller controller = Controller();
	Watchdog watchdog = Watchdog();

	static System& getInstance()
	{
		static System instance; // Guaranteed to be destroyed.
								// Instantiated on first use.
		return instance;
	}
	System(System const&) = delete;
	void operator=(System const&) = delete;

	System(void);
	void Reset(void);
	void DelayUS(uint32_t us);
	void InitTimer(void);

	void HandleADCInterrupt(void);
	void HandleCANInterrupt(void);
	void HandleUARTInterrupt(void);

	void WaitForControlLoopInterrupt(void);

private:
	bool adc_interrupt = false;
	bool can_interrupt = false;
	bool uart_interrupt = false;
};

#endif /* SYSTEM_SYSTEM_HPP_ */
