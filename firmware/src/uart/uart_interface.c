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
#include <src/can/can_endpoints.h>
#include <src/uart/uart_lowlevel.h>
#include <src/uart/uart_interface.h>

// Number of endpoints in the Avlos endpoint array
static const size_t endpoint_count = sizeof(avlos_endpoints) / sizeof(avlos_endpoints[0]);

/**
 * @brief Calculate CRC-16-CCITT using hardware CRC peripheral
 * @param data Pointer to data buffer
 * @param len Length of data in bytes
 * @return 16-bit CRC value
 */
static uint16_t UART_calculate_crc16(const uint8_t *data, uint8_t len)
{
    // Configure CRC peripheral for CRC-16-CCITT
    PAC55XX_CRC->CTL.POLYSEL = CRC16_CCITT;
    PAC55XX_CRC->CTL.DATAWIDTH = CRC_DATA_WIDTH_8BITS;
    PAC55XX_CRC->CTL.INREF = 0;
    PAC55XX_CRC->CTL.OUTREF = 0;
    
    // Set seed value
    PAC55XX_CRC->SEED.CRCSEED = 0xFFFF;
    
    // Feed data bytes
    for (uint8_t i = 0; i < len; i++)
    {
        PAC55XX_CRC->DATAIN = data[i];
    }
    
    // Return computed CRC
    return (uint16_t)PAC55XX_CRC->OUT.CRCOUT;
}

/**
 * @brief Send a binary response frame over UART
 * @param data Pointer to response data
 * @param len Length of response data in bytes
 */
static void UART_send_response(const uint8_t *data, uint8_t len)
{
    // Build response frame: [STX][LEN][DATA...][CRC16_LO][CRC16_HI]
    uart_tx_msg[0] = UART_BINARY_START_BYTE;
    uart_tx_msg[1] = len;
    memcpy(&uart_tx_msg[2], data, len);
    
    // Calculate CRC over [LEN][DATA...]
    uint16_t crc = UART_calculate_crc16(&uart_tx_msg[1], len + 1);
    uart_tx_msg[2 + len] = crc & 0xFF;
    uart_tx_msg[3 + len] = (crc >> 8) & 0xFF;
    
    // Set total frame length: STX + LEN + data + CRC16
    uart_tx_msg_len = 4 + len;
    
    // Enable transmit interrupt to start sending
    pac5xxx_uart_int_enable_THREI2(UART_REF, 1);
}

/**
 * @brief Process a received UART binary frame
 * 
 * Frame format: [STX][LEN][EP_ID][CMD][DATA...][CRC16_LO][CRC16_HI]
 * 
 * LEN = number of payload bytes (EP_ID + CMD + DATA)
 * CRC16 covers: [LEN][EP_ID][CMD][DATA...]
 */
void UART_process_message(void)
{
    // Minimum frame: STX + LEN + EP_ID + CMD + CRC16 = 6 bytes
    if (uart_rx_msg_len < 6)
    {
        return;
    }
    
    // Extract frame fields
    uint8_t len = uart_rx_msg[1];           // Payload length
    uint8_t ep_id = uart_rx_msg[2];         // Endpoint ID
    uint8_t cmd = uart_rx_msg[3];           // Command (READ=1, WRITE=0)
    
    // Verify CRC16 over [LEN][EP_ID][CMD][DATA...]
    // CRC is at position: 2 + len (after STX + LEN + payload)
    uint16_t rx_crc = uart_rx_msg[2 + len] | ((uint16_t)uart_rx_msg[3 + len] << 8);
    uint16_t calc_crc = UART_calculate_crc16(&uart_rx_msg[1], (uint8_t)(len + 1));
    
    if (rx_crc != calc_crc)
    {
        // CRC mismatch - discard frame silently
        return;
    }
    
    // Validate endpoint ID
    if (ep_id >= endpoint_count)
    {
        return;
    }
    
    // Validate payload length
    if (len < 2 || len > UART_MAX_PAYLOAD)
    {
        return;
    }

    // Prepare data buffer for endpoint callback
    uint8_t buffer[8] = {0};
    uint8_t data_len = len - 2;  // Subtract EP_ID and CMD from payload length
    
    if (data_len > sizeof(buffer))
    {
        return;
    }
    
    if (data_len > 0)
    {
        memcpy(buffer, &uart_rx_msg[4], data_len);
    }
    
    // Call the Avlos endpoint
    uint8_t response_len = 0;
    uint8_t (*callback)(uint8_t *, uint8_t *, Avlos_Command) = avlos_endpoints[ep_id];
    uint8_t ret = callback(buffer, &response_len, (Avlos_Command)cmd);
    
    // Send response if this was a read or a function call with return value
    if ((ret == AVLOS_RET_READ || ret == AVLOS_RET_CALL) && response_len > 0)
    {
        UART_send_response(buffer, response_len);
    }
}
