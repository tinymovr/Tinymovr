
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

#include <src/adc/adc.hpp>
#include <src/comms/can/can.hpp>
#include <src/comms/uart/uart.hpp>
#include <src/controller/controller.hpp>
#include <src/encoders/MA702.hpp>
#include <src/motor/motor.hpp>
#include <src/nvm/nvm.hpp>
#include <src/observer/observer.hpp>
#include <src/system.hpp>
#include <src/watchdog/watchdog.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include <src/comms/uart/uart_func.h>
#include "src/utils/utils.h"

#ifdef __cplusplus
}
#endif

UART::UART(System sys_)
{
	uart_init(UART_ENUM, UART_BAUD_RATE);
}

void UART::WriteAddr(uint8_t addr, int32_t data)
{
	switch (addr)
	{
		case 'P': // pos setpoint
		Controller_SetPosSetpoint(data);
		break;

		case 'V': // vel setpoint
		Controller_SetVelSetpoint((float)data);
		break;

		case 'I': // current setpoint
		Controller_SetIqSetpoint(data * ONE_OVER_UART_I_SCALING_FACTOR);
		break;

		case 'U': // CAN Baud Rate
		systm.can.SetkBaudRate(data);
		break;

		case 'C': // CAN ID
        systm.can.SetID(data);
        break;

		default:
		// No action
		break;
	}
}

int32_t UART::ReadAddr(uint8_t addr)
{
	int32_t ret_val = 0;
	switch (addr)
	{
		case 'b': // vbus value
		ret_val = (int32_t)(ADC_GetVBus() * UART_V_SCALING_FACTOR);
		break;

		case 'e': // controller error
		ret_val = Controller_GetError();
		break;

		case 'o': // encoder pos
		ret_val =  MA_GetAngle();
		break;

		case 'p': // pos estimate
		ret_val = Observer_GetPosEstimate();
		break;

		case 'P': // pos setpoint
		ret_val = Controller_GetPosSetpoint();
		break;

		case 'v': // vel estimate
		ret_val = (int32_t)Observer_GetVelEstimate();
		break;

		case 'V': // vel setpoint
		ret_val = (int32_t)Controller_GetVelSetpoint();
		break;

		case 'i': // current estimate
		ret_val = (int32_t)(Controller_GetIqEstimate() * UART_I_SCALING_FACTOR);
		break;

		case 'I': // current setpoint
		ret_val = (int32_t)(Controller_GetIqSetpoint() * UART_I_SCALING_FACTOR);
		break;

		case 'd': // observer direction
		ret_val = Observer_GetDirection();
		break;

		case 'h': // phase resistance
		ret_val = Motor_GetPhaseResistance() * UART_R_SCALING_FACTOR;
		break;

		case 'l': // phase inductance
		ret_val = Motor_GetPhaseInductance() * UART_L_SCALING_FACTOR;
		break;

		case 'U': // CAN Baud Rate
		ret_val = systm.can.GetkBaudRate();
		break;

		case 'C': // CAN ID
        ret_val = systm.can.GetID();
        break;

		case 'Q': // calibrate
		Controller_SetState(STATE_CALIBRATE);
		break;

		case 'A': // closed loop
		Controller_SetState(STATE_CL_CONTROL);
		break;

		case 'Z': // idle
		Controller_SetState(STATE_IDLE);
		break;

		case 'R': // reset mcu
		systm.Reset();
		break;

		case 'S': // save config
		NVM_SaveConfig();
		break;

		case 'X': // erase config
		NVM_Erase();
		break;

		default:
		// No action
		break;

	}
	return ret_val;
}

void UART::ProcessASCIIMessage()
{
	// We know the first byte is a start byte

	int8_t addr = rx_buffer[1];
	int8_t len = ((int8_t)rx_byte_count) - 3;

	// Ensure buffer is null-terminated
	rx_buffer[rx_byte_count] = '\0';

	if (len > 0)
	{
		// Write operation
		int32_t n = atol(&(rx_buffer)[2]);
		WriteAddr(addr, n);
		Watchdog_Feed();
	}
	else if (len == 0)
	{
		// Read operation
		char msg[64];
		int32_t val = ReadAddr(rx_buffer[1]);
		(void)itoa(val, msg, 10);
		SendMessage(msg);
		Watchdog_Feed();
	}
	else
	{
		// Error
	}
}

// TODO: Add protection for unsent messages
void UART::SendMessage(char *buffer)
{
	tx_byte_count = 0;
	for (uint8_t i=0; i<UART_BYTE_LIMIT; i++)
	{
		if (buffer[i] == '\0')
		{
			tx_buffer[i] = UART_NEWLINE;
			break;
		}
		tx_buffer[i] = buffer[i];
	}
	// Enable transmit interrupt to send reponse to host
	pac5xxx_uart_int_enable_THREI2(UART_REF, 1);
}

void UART::InterruptHandler(void)
{
	uint8_t int_type = pac5xxx_uart_interrupt_identification2(UART_REF);
    uint8_t data = pac5xxx_uart_read2(UART_REF);

    if (int_type == UARTIIR_INTID_TX_HOLD_EMPTY)
	{
		pac5xxx_uart_write2(UART_REF, tx_buffer[tx_byte_count]);
		tx_byte_count++;

		// Terminate transmission upon newline or transmit overflow
        if ((tx_buffer[tx_byte_count - 1u] == UART_NEWLINE) ||
				(tx_byte_count > UART_BYTE_LIMIT))
		{
			// Disable transmit interrupt
	        pac5xxx_uart_int_enable_THREI2(UART_REF, UART_INT_DISABLE);
			// Enable receive data interrupt for next incoming message
			pac5xxx_uart_int_enable_RDAI2(UART_REF, UART_INT_ENABLE);
		  	tx_byte_count = 0;
		}
	}
    else
	{	
		// Check first byte or return
        if (rx_byte_count == 0u)
		{
        	if (data == UART_ASCII_PROT_START_BYTE)
			{
        		msg_type = MSG_TYPE_ASCII;
			}
        	else
        	{
        		msg_type = MSG_TYPE_UNKNOWN;
        	}
		}

		if (msg_type != MSG_TYPE_UNKNOWN)
		{
			// Store data in buffer and increment index
			rx_buffer[rx_byte_count] = data;
			rx_byte_count++;

			if ((msg_type == MSG_TYPE_ASCII) &&
				(rx_buffer[rx_byte_count - 1u] == UART_NEWLINE))
			{
				ProcessASCIIMessage();
				ResetRxQueue();
				// Disable receive data interrupt
				pac5xxx_uart_int_enable_RDAI2(UART_REF, UART_INT_DISABLE);
				// Reset RX FIFO, to clear RDAI interrupt
				pac5xxx_uart_rx_fifo_reset2(UART_REF);
			}
			else if (rx_byte_count > UART_BYTE_LIMIT)
			{
				ResetRxQueue();
			}
			else
			{
				// No action
			}
		}
	}
}

void UART::ResetRxQueue()
{
	rx_byte_count = 0;
	tx_byte_count = 0;
	msg_type = MSG_TYPE_UNKNOWN;
}
