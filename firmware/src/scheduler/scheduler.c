
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
#include <src/adc/adc.h>
#include <src/comms/can/can.h>
#include <src/comms/uart/uart_interface.h>
#include <src/comms/uart/uart_lowlevel.h>
#include <src/encoder/encoder.h>
#include <src/observer/observer.h>

volatile uint32_t msTicks = 0;

struct SchedulerState
{
	bool adc_interrupt;
	bool can_interrupt;
	bool uart_message_interrupt;

    uint32_t busy_cycles;
    uint32_t total_cycles;
    uint32_t busy_loop_start;
    uint32_t total_loop_start;
};

struct SchedulerState state = {0};

void WaitForControlLoopInterrupt(void)
{
	state.busy_cycles = DWT->CYCCNT - state.busy_loop_start;
	while (!state.adc_interrupt)
	{
		if (state.can_interrupt)
		{
			// Handle CAN
			state.can_interrupt = false;
			CAN_process_interrupt();
		}
		else if (state.uart_message_interrupt)
		{
			// Handle UART
			state.uart_message_interrupt = false;
			UART_ProcessMessage();
		}
		else
		{
			// Go back to sleep
			__DSB();
			__ISB();
			__WFI();
		}
	}
	state.adc_interrupt = false;
	state.busy_loop_start = DWT->CYCCNT;
	// We have to service the control loop by updating
	// current measurements and encoder estimates.
	encoder_send_angle_cmd();
	ADC_UpdateMeasurements();
	encoder_update_angle(true);
	observer_update_estimates(encoder_get_angle());
	// At this point control is returned to main loop.
}

void ADC_IRQHandler(void)
{
	PAC55XX_ADC->ADCINT.ADCIRQ0IF = 1;
    state.adc_interrupt = true;
    const uint32_t current_timestamp = DWT->CYCCNT;
    state.total_cycles = current_timestamp - state.total_loop_start;
    state.total_loop_start = current_timestamp;
}

void CAN_IRQHandler(void)
{
	pac5xxx_can_int_clear_RI();
	state.can_interrupt = true;
}

void UART_ReceiveMessageHandler(void)
{
	state.uart_message_interrupt = true;
}

void SysTick_Handler(void)  {                               /* SysTick interrupt Handler. */
    msTicks = msTicks + 1;                                  /* See startup file startup_LPC17xx.s for SysTick vector */ 
    CAN_task();
}

uint32_t Scheduler_GetTotalCycles(void)
{
    return state.total_cycles;
}

uint32_t Scheduler_GetBusyCycles(void)
{
    return state.busy_cycles;
}
