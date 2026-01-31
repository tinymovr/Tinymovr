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

// Binary protocol frame markers
#define UART_BINARY_START_BYTE 0x02  // STX

// Buffer limits
#define UART_BYTE_LIMIT 32
#define UART_MAX_PAYLOAD 10  // endpoint_id + cmd + 8 data bytes

// Receive buffer and state
extern uint8_t uart_rx_msg[UART_BYTE_LIMIT];
extern uint8_t uart_rx_msg_len;

// Transmit buffer and state
extern uint8_t uart_tx_msg[UART_BYTE_LIMIT];
extern uint8_t uart_tx_msg_len;
extern uint8_t uart_tx_byte_idx;

void UART_Init(void);

// The following message handler is called when a UART
// binary frame is completely received
extern void UART_ReceiveMessageHandler(void);

#endif /* UART_UART_LOWLEVEL_H_ */
