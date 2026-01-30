
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
#include <src/watchdog/watchdog.h>

// Protocol hash (low 8 bits) for version verification
static const uint8_t avlos_proto_hash_8 = (uint8_t)(avlos_proto_hash & 0xFF);
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
 * @brief Send a response frame over UART
 * @param ep_id Endpoint ID
 * @param payload Pointer to payload data
 * @param payload_len Length of payload
 */
static void UART_send_response(uint16_t ep_id, const uint8_t *payload, uint8_t payload_len)
{
    // Build response frame
    // Format: | Sync (1) | Length (1) | Hash (1) | EP_ID (2) | CMD (1) | Payload (0-8) | CRC16 (2) |
    uint8_t frame_len = UART_FRAME_HEADER_SIZE + payload_len;
    
    uart_tx_msg[0] = UART_SYNC_BYTE;
    uart_tx_msg[1] = UART_FRAME_HEADER_SIZE - 2 + payload_len;  // Length after length field, excluding CRC
    uart_tx_msg[2] = avlos_proto_hash_8;
    uart_tx_msg[3] = (uint8_t)(ep_id & 0xFF);         // EP_ID low byte
    uart_tx_msg[4] = (uint8_t)((ep_id >> 8) & 0xFF);  // EP_ID high byte
    uart_tx_msg[5] = AVLOS_CMD_READ;                  // Response is always a "read" result
    
    // Copy payload
    if (payload_len > 0 && payload != NULL)
    {
        memcpy(&uart_tx_msg[UART_FRAME_HEADER_SIZE], payload, payload_len);
    }
    
    // Calculate CRC over entire frame (excluding CRC field itself)
    uint16_t crc = UART_calculate_crc16((uint8_t *)uart_tx_msg, frame_len);
    uart_tx_msg[frame_len] = (uint8_t)(crc & 0xFF);         // CRC low byte
    uart_tx_msg[frame_len + 1] = (uint8_t)((crc >> 8) & 0xFF);  // CRC high byte
    
    // Set total frame length including CRC
    uart_tx_frame_len = frame_len + 2;
    uart_tx_byte_idx = 0;
    
    // Enable transmit interrupt to send response
    pac5xxx_uart_int_enable_THREI2(UART_REF, 1);
}

/**
 * @brief Process received UART message using Avlos protocol
 * 
 * Frame format:
 * | Sync (1) | Length (1) | Hash (1) | EP_ID (2) | CMD (1) | Payload (0-8) | CRC16 (2) |
 */
void UART_process_message(void)
{
    // Minimum frame size: Sync + Length + Hash + EP_ID(2) + CMD + CRC(2) = 8 bytes
    if (uart_rx_msg_len < UART_FRAME_MIN_SIZE)
    {
        return;  // Frame too short
    }
    
    // Verify sync byte
    if (uart_rx_msg[0] != UART_SYNC_BYTE)
    {
        return;  // Invalid sync byte
    }
    
    // Get length field
    uint8_t length = uart_rx_msg[1];
    uint8_t total_len = length + 2 + 2;  // +2 for Sync+Length, +2 for CRC
    
    // Validate total length
    if (total_len != uart_rx_msg_len || total_len > UART_FRAME_MAX_SIZE)
    {
        return;  // Length mismatch
    }
    
    // Verify CRC (over all bytes except CRC itself)
    uint8_t crc_offset = total_len - 2;
    uint16_t received_crc = (uint16_t)uart_rx_msg[crc_offset] | 
                            ((uint16_t)uart_rx_msg[crc_offset + 1] << 8);
    uint16_t calculated_crc = UART_calculate_crc16((uint8_t *)uart_rx_msg, crc_offset);
    
    if (received_crc != calculated_crc)
    {
        return;  // CRC mismatch
    }
    
    // Extract frame fields
    uint8_t frame_hash = uart_rx_msg[2];
    uint16_t ep_id = (uint16_t)uart_rx_msg[3] | ((uint16_t)uart_rx_msg[4] << 8);
    uint8_t cmd = uart_rx_msg[5];
    
    // Verify protocol hash (accept 0 for backward compatibility, like CAN does)
    if (frame_hash != avlos_proto_hash_8 && frame_hash != 0)
    {
        return;  // Protocol hash mismatch
    }
    
    // Validate endpoint ID
    if (ep_id >= endpoint_count)
    {
        return;  // Invalid endpoint
    }
    
    // Calculate payload length
    uint8_t payload_len = length - 4;  // length - (Hash + EP_ID(2) + CMD)
    
    // Reject frames with invalid payload size (defense in depth)
    if (payload_len > 8)
    {
        return;  // Payload too large
    }
    
    // Prepare buffer for endpoint call
    uint8_t buffer[8] = {0};
    uint8_t buffer_len = payload_len;
    
    // Copy payload to buffer
    if (payload_len > 0)
    {
        memcpy(buffer, &uart_rx_msg[UART_FRAME_HEADER_SIZE], payload_len);
    }
    
    // Map command to Avlos command type
    Avlos_Command avlos_cmd = (cmd == AVLOS_CMD_WRITE) ? AVLOS_CMD_WRITE : AVLOS_CMD_READ;
    
    // Call endpoint handler (same pattern as CAN)
    uint8_t (*callback)(uint8_t buffer[], uint8_t *buffer_length, Avlos_Command cmd) = avlos_endpoints[ep_id];
    uint8_t response_type = callback(buffer, &buffer_len, avlos_cmd);
    
    // Send response if needed
    if ((AVLOS_RET_READ == response_type || AVLOS_RET_CALL == response_type) && buffer_len > 0)
    {
        UART_send_response(ep_id, buffer, buffer_len);
    }
    
    // Reset watchdog on successful message
    Watchdog_reset();
}
