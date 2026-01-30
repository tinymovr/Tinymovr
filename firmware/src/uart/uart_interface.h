
//  * This file is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * Copyright (c) 2020-2023 Ioannis Chatzikonstantinou.
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


#ifndef UART_UART_INTERFACE_H_
#define UART_UART_INTERFACE_H_

#include <src/common.h>

// UART Avlos Binary Protocol Constants
#define UART_SYNC_BYTE          0xAA    // Frame start marker
#define UART_FRAME_HEADER_SIZE  6       // Sync + Length + Hash + EP_ID(2) + CMD
#define UART_FRAME_MIN_SIZE     9       // Header + CRC(2), no payload
#define UART_FRAME_MAX_SIZE     17      // Header + 8 bytes payload + CRC(2)

// Process received UART message (called from scheduler)
void UART_process_message(void);

#endif /* UART_UART_INTERFACE_H_ */
