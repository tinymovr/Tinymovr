
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

#ifndef UART_UART_LOWLEVEL_H_
#define UART_UART_LOWLEVEL_H_

#include <src/common.h>

// UART Protocol Constants
#define UART_SYNC_BYTE_LL       0xAA    // Binary protocol sync byte
#define UART_RX_BUFFER_SIZE     32      // Max receive buffer size
#define UART_TX_BUFFER_SIZE     32      // Max transmit buffer size

// Receive buffer and state
extern uint8_t uart_rx_msg[UART_RX_BUFFER_SIZE];
extern uint8_t uart_rx_msg_len;

// Transmit buffer and state
extern uint8_t uart_tx_msg[UART_TX_BUFFER_SIZE];
extern uint8_t uart_tx_byte_idx;
extern uint8_t uart_tx_frame_len;

// Initialize UART peripheral
void UART_Init(void);

// Message handler callback (called when a complete frame is received)
extern void UART_ReceiveMessageHandler(void);

#endif /* UART_UART_LOWLEVEL_H_ */
