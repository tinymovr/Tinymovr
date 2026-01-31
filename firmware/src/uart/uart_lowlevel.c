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

// Receive buffers and state
static uint8_t uart_rx_buf[UART_BYTE_LIMIT] = {0};
static uint8_t uart_rx_byte_idx = 0;
static uint8_t uart_rx_expected_len = 0;

// Exported receive message buffer
uint8_t uart_rx_msg[UART_BYTE_LIMIT];
uint8_t uart_rx_msg_len = 0;

// Exported transmit buffer
uint8_t uart_tx_msg[UART_BYTE_LIMIT];
uint8_t uart_tx_msg_len = 0;
uint8_t uart_tx_byte_idx = 0;

static void ResetRxQueue(void)
{
    uart_rx_byte_idx = 0;
    uart_rx_expected_len = 0;
}

static void ResetTxQueue(void)
{
    uart_tx_byte_idx = 0;
    uart_tx_msg_len = 0;
}

void UART_Init(void)
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
        // Transmit path
        pac5xxx_uart_write2(UART_REF, uart_tx_msg[uart_tx_byte_idx]);
        uart_tx_byte_idx++;

        // Terminate transmission when all bytes sent
        if (uart_tx_byte_idx >= uart_tx_msg_len)
        {
            // Disable transmit interrupt
            pac5xxx_uart_int_enable_THREI2(UART_REF, UART_INT_DISABLE);
            ResetTxQueue();
        }
    }
    else
    {
        // Receive path - binary frame format:
        // [STX][LEN][EP_ID][CMD][DATA...][CRC16_LO][CRC16_HI]
        
        if (uart_rx_byte_idx == 0)
        {
            // Waiting for start byte
            if (data == UART_BINARY_START_BYTE)
            {
                uart_rx_buf[0] = data;
                uart_rx_byte_idx = 1;
            }
            // Ignore any other bytes when not in a frame
        }
        else if (uart_rx_byte_idx == 1)
        {
            // Length byte received
            uart_rx_buf[1] = data;
            // Total frame length: STX(1) + LEN(1) + payload(data) + CRC16(2)
            uart_rx_expected_len = 2 + data + 2;
            
            // Sanity check: ensure frame fits in buffer
            if (uart_rx_expected_len > UART_BYTE_LIMIT)
            {
                ResetRxQueue();
            }
            else
            {
                uart_rx_byte_idx = 2;
            }
        }
        else if (uart_rx_byte_idx < uart_rx_expected_len)
        {
            // Receiving payload and CRC bytes
            uart_rx_buf[uart_rx_byte_idx++] = data;
            
            if (uart_rx_byte_idx == uart_rx_expected_len)
            {
                // Frame complete - copy to message buffer and signal handler
                uart_rx_msg_len = uart_rx_byte_idx;
                memcpy(uart_rx_msg, uart_rx_buf, uart_rx_msg_len);
                ResetRxQueue();
                UART_ReceiveMessageHandler();
                // Reset RX FIFO to clear any pending data
                pac5xxx_uart_rx_fifo_reset2(UART_REF);
            }
        }
        else
        {
            // Overflow protection - should not reach here
            ResetRxQueue();
        }
    }
}
