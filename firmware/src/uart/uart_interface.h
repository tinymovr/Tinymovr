
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

#include "src/common.h"
#include "src/uart/uart_func.h"

#define UART_ENUM UARTB
#define UART_REF PAC55XX_UARTB
#define UART_BAUD_RATE 115200

 // . (dot)
#define UART_ASCII_PROT_START_BYTE 0x2E

// newline
#define UART_NEWLINE 0x0A

#define UART_BYTE_LIMIT 32

#define UART_I_SCALING_FACTOR ( 1000.0f )
#define UART_R_SCALING_FACTOR ( 1000.0f )
#define UART_L_SCALING_FACTOR ( 1000.0f )
#define ONE_OVER_UART_I_SCALING_FACTOR ( 0.001f )
#define UART_V_SCALING_FACTOR ( 1000.0f )

typedef enum {
    MSG_TYPE_UNKNOWN = 0,
    MSG_TYPE_ASCII = 1,
    MSG_TYPE_BINARY = 2
} SerialMessageType;

void UART_SendErrorMsg(void);

void UART_Init(void);
void UART_SendMessage(char *buffer);
void UART_ProcessTransmitInterrupt(void);
void UART_ProcessReceiveInterrupt(void);

#endif /* UART_UART_INTERFACE_H_ */
