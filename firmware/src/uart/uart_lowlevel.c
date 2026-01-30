
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
#include <src/uart/uart_func.h>
#include <src/uart/uart_lowlevel.h>

// Receive buffer
char uart_rx_buf[UART_RX_BUFFER_SIZE] = {0};
uint8_t uart_rx_byte_idx = 0;
char uart_rx_msg[UART_RX_BUFFER_SIZE];
uint8_t uart_rx_msg_len = 0;

// Transmit buffer
char uart_tx_msg[UART_TX_BUFFER_SIZE];
uint8_t uart_tx_byte_idx = 0;
uint8_t uart_tx_frame_len = 0;

// Receive state machine
typedef enum {
    RX_STATE_IDLE = 0,      // Waiting for sync byte
    RX_STATE_LENGTH,        // Waiting for length byte
    RX_STATE_DATA           // Receiving data bytes
} RxState;

static RxState rx_state = RX_STATE_IDLE;
static uint8_t rx_expected_len = 0;

static void ResetRxState(void)
{
    uart_rx_byte_idx = 0;
    rx_state = RX_STATE_IDLE;
    rx_expected_len = 0;
}

static void ResetTxState(void)
{
    uart_tx_byte_idx = 0;
    uart_tx_frame_len = 0;
}

void UART_Init(void)
{
    uart_init(UART_ENUM, UART_BAUD_RATE);
    ResetRxState();
    ResetTxState();
}

void USARTB_IRQHandler(void)
{
    uint8_t int_type = pac5xxx_uart_interrupt_identification2(UART_REF);
    uint8_t data = pac5xxx_uart_read2(UART_REF);

    if (int_type == UARTIIR_INTID_TX_HOLD_EMPTY)
    {
        // Transmit interrupt - send next byte
        if (uart_tx_byte_idx < uart_tx_frame_len)
        {
            pac5xxx_uart_write2(UART_REF, uart_tx_msg[uart_tx_byte_idx]);
            uart_tx_byte_idx++;
        }
        
        // Check if transmission complete
        if (uart_tx_byte_idx >= uart_tx_frame_len)
        {
            // Disable transmit interrupt
            pac5xxx_uart_int_enable_THREI2(UART_REF, UART_INT_DISABLE);
            ResetTxState();
        }
    }
    else
    {
        // Receive interrupt - process incoming byte
        switch (rx_state)
        {
            case RX_STATE_IDLE:
                // Looking for sync byte
                if (data == UART_SYNC_BYTE_LL)
                {
                    uart_rx_buf[0] = data;
                    uart_rx_byte_idx = 1;
                    rx_state = RX_STATE_LENGTH;
                }
                break;
                
            case RX_STATE_LENGTH:
                // Got length byte
                uart_rx_buf[1] = data;
                uart_rx_byte_idx = 2;
                // Total frame length = Sync(1) + Length(1) + data(length) + CRC(2)
                rx_expected_len = 2 + data + 2;
                
                // Validate length
                if (rx_expected_len > UART_RX_BUFFER_SIZE || data < 4)
                {
                    // Invalid length, reset
                    ResetRxState();
                }
                else
                {
                    rx_state = RX_STATE_DATA;
                }
                break;
                
            case RX_STATE_DATA:
                // Receiving data bytes
                uart_rx_buf[uart_rx_byte_idx] = data;
                uart_rx_byte_idx++;
                
                // Check if frame complete
                if (uart_rx_byte_idx >= rx_expected_len)
                {
                    // Copy to message buffer and signal handler
                    uart_rx_msg_len = uart_rx_byte_idx;
                    memcpy(uart_rx_msg, uart_rx_buf, uart_rx_msg_len);
                    ResetRxState();
                    UART_ReceiveMessageHandler();
                    
                    // Reset RX FIFO
                    pac5xxx_uart_rx_fifo_reset2(UART_REF);
                }
                else if (uart_rx_byte_idx >= UART_RX_BUFFER_SIZE)
                {
                    // Buffer overflow, reset
                    ResetRxState();
                }
                break;
                
            default:
                ResetRxState();
                break;
        }
    }
}
