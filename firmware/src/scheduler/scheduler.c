/*
 * scheduler.c
 *
 *  Created on: 28 ??? 2020
 *      Author: conta
 */

#include <src/common.h>
#include <src/adc/adc.h>
#include <src/can/can.h>
#include <src/uart/uart_interface.h>
#include <src/uart/uart_lowlevel.h>
#include <src/observer/observer.h>

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
	const uint32_t current_timestamp = ARM_CM_DWT_CYCCNT;
	state.total_cycles = current_timestamp - state.total_loop_start;
	state.busy_cycles = current_timestamp - state.busy_loop_start;
	state.total_loop_start = current_timestamp;

	// Control loop is synced to ADC measurements
	while (!state.adc_interrupt)
	{
		// Received an interrupt but it's not ADC.
		// If there are any tasks pending from other interrupts, do them now.
		if (state.can_interrupt)
		{
			CAN_ProcessInterrupt();
			state.can_interrupt = false;
		}
		if (state.uart_message_interrupt)
		{
			UART_ProcessMessage();
			state.uart_message_interrupt = false;
		}
		// Go back to sleep
		__WFI();
	}
	state.busy_loop_start = ARM_CM_DWT_CYCCNT;
	// We have to service the control loop by updating
	// current measurements and encoder estimates.
	ADC_UpdateMeasurements();
	Observer_UpdatePos();
	// Finally, we clear the interrupt flag and return
	// control to main loop.
	state.adc_interrupt = false;
}

void ADC_IRQHandler(void)
{
	PAC55XX_ADC->ADCINT.ADCIRQ0IF = 1;
    state.adc_interrupt = true;
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

uint32_t Scheduler_GetTotalCycles(void)
{
    return state.total_cycles;
}

uint32_t Scheduler_GetBusyCycles(void)
{
    return state.busy_cycles;
}
