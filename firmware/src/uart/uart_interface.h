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

/**
 * @brief Process a received UART binary frame using Avlos endpoints
 * 
 * Frame format: [STX][LEN][EP_ID][CMD][DATA...][CRC16_LO][CRC16_HI]
 * - STX: Start byte (0x02)
 * - LEN: Payload length (EP_ID + CMD + DATA)
 * - EP_ID: Avlos endpoint index
 * - CMD: 0=WRITE, 1=READ
 * - DATA: 0-8 bytes of payload
 * - CRC16: CRC-16-CCITT over LEN through DATA (little-endian)
 */
void UART_process_message(void);

#endif /* UART_UART_INTERFACE_H_ */
