
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

#include <src/common.h>
#include <src/comms/defs.h>
#include <src/comms/uart/uart_lowlevel.h>
#include <src/comms/uart/uart_interface.h>


void UART_ProcessMessage(void)
{
    // Ensure buffer is null-terminated
    uart_rx_msg[uart_rx_msg_len] = '\0';

    size_t tx_size = handle_message(uart_rx_msg, uart_rx_msg_len, uart_tx_msg);
    if (tx_size > 0)
    {
        // Ensure buffer is newline-terminated
        uart_tx_msg[tx_size] = UART_NEWLINE;
        pac5xxx_uart_int_enable_THREI2(UART_REF, 1);
    }
}
