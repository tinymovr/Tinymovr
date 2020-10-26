
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

//
// Commands
// b - read vbus voltage *
// P - read/write position setpoint
// p - read position meas
// V - read/write velocity setpoint
// v - read velocity meas
// I - read/write current setpoint **
// i - read current meas **
// Q - read/write current tolerance **
//
// * scaled by UART_V_SCALING_FACTOR
// ** scaled by UART_I_SCALING_FACTOR


#ifndef UART_UART_INTERFACE_H_
#define UART_UART_INTERFACE_H_

#include <src/component.hpp>

typedef enum {
    MSG_TYPE_UNKNOWN = 0,
    MSG_TYPE_ASCII = 1,
    MSG_TYPE_BINARY = 2
} SerialMessageType;

class UART : public Component
{
public:
	UART(System sys_);
	void SendMessage(char *buffer);
private:
	SerialMessageType msg_type = MSG_TYPE_UNKNOWN;
	uint8_t rx_byte_count = 0;
	uint8_t tx_byte_count = 0;
	char rx_buffer[64] = {0};
	char tx_buffer[64] = {0};
	void WriteAddr(uint8_t addr, int32_t data);
	int32_t ReadAddr(uint8_t addr);
	void SendMessage(char *buffer);
	void ProcessASCIIMessage();
	void ResetRxQueue();
	void ProcessInterrupt(void);
};



#endif /* UART_UART_INTERFACE_H_ */
