
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

#include "src/system/system.h"
#include "src/motor/motor.h"
#include "src/encoders/MA702.h"
#include "src/observer/observer.h"
#include "src/controller/controller.h"
#include "src/adc/adc.h"
#include "src/nvm/nvm.h"
#include "src/can/can.h"
#include "src/uart/uart_func.h"
#include "src/utils/utils.h"
#include "src/watchdog/watchdog.h"
#include "src/uart/uart_interface.h"

struct UART {
	SerialMessageType msg_type;

	uint8_t rx_byte_count;
	uint8_t tx_byte_count;

	char rx_buffer[64];
	char tx_buffer[64];
};

static struct UART state = {
	.msg_type = 0,
	.rx_byte_count = 0,
	.tx_byte_count = 0,

	.rx_buffer = {0},
	.tx_buffer = {0}
};

bool UART_ProcessASCIIMessage(void);
void ResetRxQueue(void);

void UART_WriteAddr(uint8_t addr, int32_t data)
{
	switch (addr)
	{
		case 'P': // pos setpoint
			Controller_SetIqSetpoint(0);
			Controller_SetVelSetpoint(0);
			Controller_SetPosSetpoint(data);
			Controller_SetMode(CTRL_POSITION);
		break;

		case 'V': // vel setpoint
			Controller_SetIqSetpoint(0);
			Controller_SetVelSetpoint(data);
			Controller_SetMode(CTRL_VELOCITY);
			Controller_SetVelSetpoint((float)data);
		break;

		case 'I': // current setpoint
			Controller_SetMode(CTRL_CURRENT);
			Controller_SetIqSetpoint(data * ONE_OVER_UART_I_SCALING_FACTOR);
		break;

		case 'U': // CAN Baud Rate
			CAN_SetkBaudRate(data);
		break;

		case 'C': // CAN ID
			CAN_SetID(data);
        break;

		default:
		// No action
		break;
	}
}

int32_t UART_ReadAddr(uint8_t addr)
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
			ret_val = CAN_GetkBaudRate();
		break;

		case 'C': // CAN ID
			ret_val = CAN_GetID();
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
			System_Reset();
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

void UART_Init()
{
    uart_init(UART_ENUM, UART_BAUD_RATE);
}

// True if message will be returned
bool UART_ProcessASCIIMessage()
{
	// We know the first byte is a start byte

	int8_t addr = state.rx_buffer[1];
	int8_t len = ((int8_t)state.rx_byte_count) - 3;

	// Ensure buffer is null-terminated
	state.rx_buffer[state.rx_byte_count] = '\0';

	bool returned = false;
	if (len > 0)
	{
		// Write operation
		int32_t n = atol(&(state.rx_buffer)[2]);
		UART_WriteAddr(addr, n);
		Watchdog_Feed();
	}
	else if (len == 0)
	{
		// Read operation
		returned = true;
		char msg[64];
		int32_t val = UART_ReadAddr(state.rx_buffer[1]);
		(void)itoa(val, msg, 10);
		UART_SendMessage(msg);
		Watchdog_Feed();
	}
	else
	{
		// Error
	}
	return returned;
}

// TODO: Add protection for unsent messages
void UART_SendMessage(char *buffer)
{
	state.tx_byte_count = 0;
	for (uint8_t i=0; i<UART_BYTE_LIMIT; i++)
	{
		if (buffer[i] == '\0')
		{
			state.tx_buffer[i] = UART_NEWLINE;
			break;
		}
		state.tx_buffer[i] = buffer[i];
	}
	// Enable transmit interrupt to send reponse to host
	pac5xxx_uart_int_enable_THREI2(UART_REF, 1);
}

void USARTB_IRQHandler(void)
{
	uint8_t int_type = pac5xxx_uart_interrupt_identification2(UART_REF);
    uint8_t data = pac5xxx_uart_read2(UART_REF);

    if (int_type == UARTIIR_INTID_TX_HOLD_EMPTY)
	{
		pac5xxx_uart_write2(UART_REF, state.tx_buffer[state.tx_byte_count]);
		state.tx_byte_count++;

		// Terminate transmission upon newline or transmit overflow
        if ((state.tx_buffer[state.tx_byte_count - 1u] == UART_NEWLINE) ||
				(state.tx_byte_count > UART_BYTE_LIMIT))
		{
			// Disable transmit interrupt
	        pac5xxx_uart_int_enable_THREI2(UART_REF, UART_INT_DISABLE);
			// Enable receive data interrupt for next incoming message
			pac5xxx_uart_int_enable_RDAI2(UART_REF, UART_INT_ENABLE);
		  	state.tx_byte_count = 0;
		}
	}
    else
	{	
		// Check first byte or return
        if (state.rx_byte_count == 0u)
		{
        	if (data == UART_ASCII_PROT_START_BYTE)
			{
        		state.msg_type = MSG_TYPE_ASCII;
			}
        	else
        	{
        		state.msg_type = MSG_TYPE_UNKNOWN;
        	}
		}

		if (state.msg_type != MSG_TYPE_UNKNOWN)
		{
			// Store data in buffer and increment index
			state.rx_buffer[state.rx_byte_count] = data;
			state.rx_byte_count++;

			if ((state.msg_type == MSG_TYPE_ASCII) &&
				(state.rx_buffer[state.rx_byte_count - 1u] == UART_NEWLINE))
			{
				if (UART_ProcessASCIIMessage())
				{
					// Will return message, disable receive data interrupt
					pac5xxx_uart_int_enable_RDAI2(UART_REF, UART_INT_DISABLE);
				}
				ResetRxQueue();
				// Reset RX FIFO, to clear RDAI interrupt
				pac5xxx_uart_rx_fifo_reset2(UART_REF);
			}
			else if (state.rx_byte_count > UART_BYTE_LIMIT)
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

void ResetRxQueue()
{
	state.rx_byte_count = 0;
	state.tx_byte_count = 0;
	state.msg_type = MSG_TYPE_UNKNOWN;
}
