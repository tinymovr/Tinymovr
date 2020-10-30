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
};

struct SchedulerState state;

void WaitForControlLoopInterrupt(void)
{
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
