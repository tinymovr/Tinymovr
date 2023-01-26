
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

#define UART_NEWLINE 0x0A
#define UART_ASCII_PROT_START_BYTE 0x2E
#define UART_BYTE_LIMIT 32

char uart_rx_msg[96];
uint8_t uart_rx_msg_len;

char uart_tx_msg[96];
uint8_t uart_tx_byte_idx;

void UART_Init(void);

// The following message handler is called when a UART
// message completes being received i.e.  with a newline
// character
extern void UART_ReceiveMessageHandler(void);

#endif /* UART_UART_LOWLEVEL_H_ */
