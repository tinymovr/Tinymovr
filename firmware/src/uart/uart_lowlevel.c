
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

#include <string.h>
#include <src/common.h>
#include "src/uart/uart_func.h"
#include <src/uart/uart_lowlevel.h>

char uart_rx_buf[96] = {0};
uint8_t uart_rx_byte_idx = 0;

typedef enum {
    MSG_TYPE_UNKNOWN = 0,
    MSG_TYPE_ASCII = 1,
    MSG_TYPE_BINARY = 2
} SerialMessageType;

SerialMessageType rx_msg_type = MSG_TYPE_UNKNOWN;

void ResetRxQueue(void)
{
    uart_rx_byte_idx = 0;
    rx_msg_type = MSG_TYPE_UNKNOWN;
}

void ResetTxQueue(void)
{
    uart_tx_byte_idx = 0;
}

void UART_Init()
{
    uart_init(UART_ENUM, UART_BAUD_RATE);
    ResetRxQueue();
    ResetTxQueue();
}

void USARTB_IRQHandler(void)
{
    uint8_t int_type = pac5xxx_uart_interrupt_identification2(UART_REF);
    uint8_t data = pac5xxx_uart_read2(UART_REF);

    if (int_type == UARTIIR_INTID_TX_HOLD_EMPTY)
    {
        pac5xxx_uart_write2(UART_REF, uart_tx_msg[uart_tx_byte_idx]);
        uart_tx_byte_idx++;

        // Terminate transmission upon newline or transmit overflow
        if ((uart_tx_msg[uart_tx_byte_idx - 1u] == UART_NEWLINE) ||
                (uart_tx_byte_idx > UART_BYTE_LIMIT))
        {
            // Disable transmit interrupt
            pac5xxx_uart_int_enable_THREI2(UART_REF, UART_INT_DISABLE);
            // Enable receive data interrupt for next incoming message
            // pac5xxx_uart_int_enable_RDAI2(UART_REF, UART_INT_ENABLE);
            ResetTxQueue();
        }
    }
    else
    {	
        // Check first byte or return
        if ((uart_rx_byte_idx == 0u) && (data == UART_ASCII_PROT_START_BYTE))
        {
            rx_msg_type = MSG_TYPE_ASCII;
        }

        if (rx_msg_type != MSG_TYPE_UNKNOWN)
        {
            uart_rx_buf[uart_rx_byte_idx] = data;
            if ((rx_msg_type == MSG_TYPE_ASCII) &&
                (uart_rx_buf[uart_rx_byte_idx] == UART_NEWLINE))
            {
                uart_rx_msg_len = uart_rx_byte_idx + 1u;
                memcpy(&uart_rx_msg, &uart_rx_buf, uart_rx_msg_len);
                ResetRxQueue();
                UART_ReceiveMessageHandler();
                // Disable receive data interrupt
                //pac5xxx_uart_int_enable_RDAI2(UART_REF, UART_INT_DISABLE);
                // Reset RX FIFO, to clear RDAI interrupt
                pac5xxx_uart_rx_fifo_reset2(UART_REF);

            }
            else if (uart_rx_byte_idx >= UART_BYTE_LIMIT)
            {
                ResetRxQueue();
            }
            else
            {
                uart_rx_byte_idx++;
            }

        }
    }
}
