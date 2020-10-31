
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

void UART_ProcessMessage(void);
void UART_SendInt32(int32_t val);

#endif /* UART_UART_INTERFACE_H_ */
