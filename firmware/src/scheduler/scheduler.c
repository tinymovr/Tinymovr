
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

#include <src/common.h>
#include <src/system/system.h>
#include <src/gatedriver/gatedriver.h>
#include <src/adc/adc.h>
#include <src/can/can.h>
#include <src/uart/uart_interface.h>
#include <src/uart/uart_lowlevel.h>
#include <src/encoder/encoder.h>
#include <src/encoder/ma7xx.h>
#include <src/observer/observer.h>
#include <src/can/can_endpoints.h>
#include <src/scheduler/scheduler.h>
#include <src/watchdog/watchdog.h>

volatile uint32_t msTicks = 0;

SchedulerState state = {0};

void WaitForControlLoopInterrupt(void)
{
	
	while (!state.adc_interrupt)
	{
		
		if (state.can_interrupt)
		{
			// Handle CAN
			CAN_process_interrupt();
			// Only clear the flag if all messages in the CAN RX buffer have been processed
			if (PAC55XX_CAN->SR.RBS == 0)
			{
				state.can_interrupt = false;
			}
		}
		else if (state.uart_message_interrupt)
		{
			// Handle UART
			state.uart_message_interrupt = false;
			UART_process_message();
		}
		else if (state.wwdt_interrupt)
		{
			state.wwdt_interrupt = false;
			WWDT_process_interrupt();
		}
		else
		{
			state.busy = false;
			state.busy_cycles = DWT->CYCCNT - state.busy_loop_start;
			// Go back to sleep
			__DSB();
			__ISB();
			__WFI();
		}
	}
	state.busy = true;
	state.adc_interrupt = false;
	state.busy_loop_start = DWT->CYCCNT;
	// We have to service the control loop by updating
	// current measurements and encoder estimates.
	if (ENCODER_MA7XX == encoder_get_type())
	{
		ma7xx_send_angle_cmd();
	}
	ADC_update();
	system_update();
	encoder_update(true);
	observer_update();
	// At this point control is returned to main loop.
}

void ADC_IRQHandler(void)
{
	PAC55XX_ADC->ADCINT.ADCIRQ0IF = 1;
	// Only in case the gate driver is enabled, ensure
	// the control deadline is not missed,
	// i.e. the previous control loop is complete prior
	// to the ADC triggering the next
	if ((gate_driver_is_enabled() == true) && (state.busy == true))
	{
		state.errors |= SCHEDULER_ERRORS_CONTROL_BLOCK_REENTERED;
		// We do not change the control state here, to
		// avoid any concurrency issues
	}
	else
	{
		state.adc_interrupt = true;
	}
	
    const uint32_t current_timestamp = DWT->CYCCNT;
	state.total_cycles = current_timestamp - state.total_loop_start;
	state.total_loop_start = current_timestamp;
}

void CAN_IRQHandler(void)
{
	pac5xxx_can_int_clear_RI();
	state.can_interrupt = true;
}

void SysTick_Handler(void)
{                               
    msTicks = msTicks + 1; 
    CAN_task();
}

void UART_ReceiveMessageHandler(void)
{
	state.uart_message_interrupt = true;
}

void Wdt_IRQHandler(void)
{
	state.wwdt_interrupt = true;
	PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_WRITE_AVALABLE;
    // Interrupt flag needs to be cleared here
    PAC55XX_WWDT->WWDTFLAG.IF = 1;
}

PAC5XXX_RAMFUNC uint32_t Scheduler_GetTotalCycles(void)
{
    return state.total_cycles;
}

PAC5XXX_RAMFUNC uint32_t Scheduler_GetBusyCycles(void)
{
    return state.busy_cycles;
}

PAC5XXX_RAMFUNC uint8_t scheduler_get_errors(void)
{
	return state.errors;
}
