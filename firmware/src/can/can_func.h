//=============================================================================
// Copyright (C) 2018-2019, Qorvo, Inc.
//
// THIS SOFTWARE IS SUBJECT TO A SOURCE CODE LICENSE AGREEMENT WHICH PROVIDES,
// AMONG OTHER THINGS:  (i) THAT IT CAN BE USED ONLY TO ADAPT THE LICENSEE'S
// APPLICATION TO PAC PROCESSORS SUPPLIED BY QORVO, INC.;
// (ii) THAT  IT IS PROVIDED "AS IS" WITHOUT WARRANTY;  (iii) THAT
// QORVO, INC. IS NOT LIABLE FOR ANY INDIRECT DAMAGES OR FOR DIRECT
// DAMAGES EXCEEDING US$1,500;  AND (iv) THAT IT CAN BE DISCLOSED TO AND USED
// ONLY BY CERTAIN AUTHORIZED PERSONS.
//
//=============================================================================

#pragma once

#include <stdbool.h>
#include "pac5xxx.h"
#include "pac5xxx_gpio.h"
#include "pac5xxx_can.h"

#define CAN_EP_SIZE 12
#define CAN_EP_MASK ((1 << CAN_EP_SIZE) - 1)
#define CAN_HASH_SIZE 9
#define CAN_HASH_MASK (((1 << CAN_HASH_SIZE) - 1) << CAN_EP_SIZE)
#define CAN_DEV_SIZE 8
#define CAN_DEV_MASK (((1 << CAN_DEV_SIZE) - 1) << (CAN_EP_SIZE + CAN_HASH_SIZE))

// #define CAN_SJW_1tq             ((uint8_t)0x00)     /*!< 1 time quantum */
// #define CAN_SJW_2tq             ((uint8_t)0x01)     /*!< 2 time quantum */
// #define CAN_SJW_3tq             ((uint8_t)0x02)     /*!< 3 time quantum */
// #define CAN_SJW_4tq             ((uint8_t)0x03)     /*!< 4 time quantum */

// #define CAN_TSEG1_1tq           ((uint8_t)0x00)     /*!< 1 time quantum */
// #define CAN_TSEG1_2tq           ((uint8_t)0x01)     /*!< 2 time quantum */
// #define CAN_TSEG1_3tq           ((uint8_t)0x02)     /*!< 3 time quantum */
// #define CAN_TSEG1_4tq           ((uint8_t)0x03)     /*!< 4 time quantum */
// #define CAN_TSEG1_5tq           ((uint8_t)0x04)     /*!< 5 time quantum */
// #define CAN_TSEG1_6tq           ((uint8_t)0x05)     /*!< 6 time quantum */
// #define CAN_TSEG1_7tq           ((uint8_t)0x06)     /*!< 7 time quantum */
// #define CAN_TSEG1_8tq           ((uint8_t)0x07)     /*!< 8 time quantum */
// #define CAN_TSEG1_9tq           ((uint8_t)0x08)     /*!< 9 time quantum */
// #define CAN_TSEG1_10tq          ((uint8_t)0x09)     /*!< 10 time quantum */
// #define CAN_TSEG1_11tq          ((uint8_t)0x0A)     /*!< 11 time quantum */
// #define CAN_TSEG1_12tq          ((uint8_t)0x0B)     /*!< 12 time quantum */
// #define CAN_TSEG1_13tq          ((uint8_t)0x0C)     /*!< 13 time quantum */
// #define CAN_TSEG1_14tq          ((uint8_t)0x0D)     /*!< 14 time quantum */
// #define CAN_TSEG1_15tq          ((uint8_t)0x0E)     /*!< 15 time quantum */
// #define CAN_TSEG1_16tq          ((uint8_t)0x0F)     /*!< 16 time quantum */

// #define CAN_TSEG2_1tq           ((uint8_t)0x00)     /*!< 1 time quantum */
// #define CAN_TSEG2_2tq           ((uint8_t)0x01)     /*!< 2 time quantum */
// #define CAN_TSEG2_3tq           ((uint8_t)0x02)     /*!< 3 time quantum */
// #define CAN_TSEG2_4tq           ((uint8_t)0x03)     /*!< 4 time quantum */
// #define CAN_TSEG2_5tq           ((uint8_t)0x04)     /*!< 5 time quantum */
// #define CAN_TSEG2_6tq           ((uint8_t)0x05)     /*!< 6 time quantum */
// #define CAN_TSEG2_7tq           ((uint8_t)0x06)     /*!< 7 time quantum */
// #define CAN_TSEG2_8tq           ((uint8_t)0x07)     /*!< 8 time quantum */

// CAN BUS Select BAUD Type
typedef enum
{
    CAN_BAUD_50KHz              = 0,
    CAN_BAUD_100KHz             = 1,
    CAN_BAUD_125KHz             = 2,
    CAN_BAUD_200KHz             = 3,
    CAN_BAUD_250KHz             = 4,
    CAN_BAUD_400KHz             = 5,
    CAN_BAUD_500KHz             = 6,
    CAN_BAUD_800KHz             = 7,
    CAN_BAUD_1000KHz            = 8,
} CAN_BAUD_TYPE;

extern uint8_t tx_data[8];
extern uint8_t rx_data[8];

extern uint8_t data_length;
extern uint32_t rx_id;
extern bool rtr;
extern uint32_t can_ep_id;
extern uint32_t can_frame_hash;

void can_baud(CAN_BAUD_TYPE baud);
void can_io_config(void);

static inline void ids_from_arbitration(uint32_t arb_id, uint32_t* ep_id, uint32_t* hash)
{
    *ep_id = arb_id & CAN_EP_MASK;
    *hash = (arb_id & CAN_HASH_MASK) >> CAN_EP_SIZE;
}

static inline void arbitration_from_ids(uint32_t* arb_id, uint32_t ep_id, uint32_t hash, uint32_t node_id)
{
    *arb_id = (ep_id & CAN_EP_MASK) | ((hash << CAN_EP_SIZE) & CAN_HASH_MASK) | ((node_id << (CAN_EP_SIZE + CAN_HASH_SIZE)) & CAN_DEV_MASK);
}

static inline void can_process_standard(void)
{
    uint32_t buffer = PAC55XX_CAN->RXBUF; //  read RX buffer, RX buffer bit order same as TX buffer

    data_length = buffer & 0x0F;
    rx_id = ((buffer & 0xE00000) >> 21) | ((buffer & 0xFF00) >> 5);

    can_ep_id = rx_id & 0x3F;
    rtr = ((buffer >> 6) & 0x1) == 0x1;
    rx_data[0] = buffer >> 24; // data0
    if (data_length > 1u)
    {
        buffer = PAC55XX_CAN->RXBUF; // buffer contains data1..data4
        rx_data[1] = buffer;
        rx_data[2] = buffer >> 8;
        rx_data[3] = buffer >> 16;
        rx_data[4] = buffer >> 24;
        if (data_length > 5u)
        {
            buffer = PAC55XX_CAN->RXBUF; //  buffer contains data7..data5
            rx_data[5] = buffer;
            rx_data[6] = buffer >> 8;
            rx_data[7] = buffer >> 16;
        }
    }
}

//  * The function "can_process_extended" is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * Copyright (c) 2022 Ioannis Chatzikonstantinou.
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

static inline void can_process_extended(void)
{
    uint32_t buffer = PAC55XX_CAN->RXBUF; //  read RX buffer, RX buffer bit order same as TX buffer

    data_length = buffer & 0x0F;
    rtr = ((buffer >> 6) & 0x1) == 0x1;
    rx_id = ((buffer & 0xFF000000) >> 19) | ((buffer & 0x00FF0000) >> 3) | ((buffer & 0x0000FF00) << 13);
    
    buffer = PAC55XX_CAN->RXBUF;

    rx_id |= (buffer & 0xFF) >> 3;
    
    ids_from_arbitration(rx_id, &can_ep_id, &can_frame_hash);

    rx_data[0] = (buffer >> 8) & 0xFF; // data0
    rx_data[1] = (buffer >> 16) & 0xFF; // data1
    rx_data[2] = (buffer >> 24) & 0xFF; // data2
    if (data_length > 3u)
    {
        buffer = PAC55XX_CAN->RXBUF; // buffer contains data3..data6
        rx_data[3] = buffer & 0xFF;
        rx_data[4] = (buffer >> 8) & 0xFF;
        rx_data[5] = (buffer >> 16) & 0xFF;
        rx_data[6] = (buffer >> 24) & 0xFF;
        if (data_length > 7u)
        {
            buffer = PAC55XX_CAN->RXBUF; //  buffer contains data7
            rx_data[7] = buffer & 0xFF;
        }
    }
}

static inline void can_transmit_standard(uint8_t dataLen, uint16_t id, const uint8_t * data)
{
    while (PAC55XX_CAN->SR.TBS == 0) {};           // wait for TX buffer free
    PAC55XX_CAN->TXBUF = (dataLen << 0)         |  // DLC - Data Length Code
                         (0u << 6)              |  // RTR = 0 Data Frame
                         (0u << 7)              |  // FF - Format Frame; 0=Std Frame
                         ((id>>3 & 0xFF) << 8)  |  // ID 10:3
                         ((id&0x07u) << 21)     |  // ID 2:0
                         (data[0] << 24);          // Data 0

    if (dataLen > 1u)
    {
        PAC55XX_CAN->TXBUF = (data[1] << 0)  |      // Data 1
                             (data[2] << 8)  |		// Data 2
                             (data[3] << 16) |      // Data 3
                             (data[4] << 24);       // Data 4
    }
    if (dataLen > 5u)
    {
    PAC55XX_CAN->TXBUF = (data[5] << 0)   |    // Data 5
                         (data[6] << 8)   |    // Data 6
                         (data[7] << 16);      // Data 7

    }
	
    PAC55XX_CAN->CMR.TR = 1;	// Request transmit
}

//  * The function "can_transmit_extended" is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * Copyright (c) 2022 Ioannis Chatzikonstantinou.
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

static inline void can_transmit_extended(uint8_t dataLen, uint32_t id, const uint8_t * data)
{
    while (PAC55XX_CAN->SR.TBS == 0) {};                 // wait for TX buffer free
    PAC55XX_CAN->TXBUF = (dataLen << 0)                | // DLC - Data Length Code
                         (0u << 6)                     | // RTR = 0 Data Frame
                         (1u << 7)                     | // FF - Format Frame; 1=Ext Frame
                         ((id&0x1FE00000u) >> (21-8))  |
                         ((id&0x001FE000u) << (16-13)) |
                         ((id&0x00001FE0u) << (24-5));

    PAC55XX_CAN->TXBUF = ((id & 0x1F) << 3)            |
                         ((data[0] & 0xFF) << 8)       |
                         ((data[1] & 0xFF) << 16)      |
                         ((data[2] & 0xFF) << 24);

    if (dataLen > 3u)
    {
        PAC55XX_CAN->TXBUF = ((data[3] & 0xFF) << 0)   |      // Data 3
                             ((data[4] & 0xFF) << 8)   |		// Data 4
                             ((data[5] & 0xFF) << 16)  |      // Data 5
                             ((data[6] & 0xFF) << 24);       // Data 6
    }
    if (dataLen > 7u)
    {
        PAC55XX_CAN->TXBUF = ((data[7] & 0xFF) << 0);        // Data 7
    }

    PAC55XX_CAN->CMR.TR = 1;	// Request transmit
}

static inline uint16_t CAN_BaudTypeToInt(CAN_BAUD_TYPE type)
{
    uint16_t ret = 0u;
    switch(type)
    {
        case CAN_BAUD_50KHz:
        ret = 50u;
        break;

        case CAN_BAUD_100KHz:
        ret = 100u;
        break;

        case CAN_BAUD_125KHz:
        ret = 125u;
        break;

        case CAN_BAUD_200KHz:
        ret = 200u;
        break;

        case CAN_BAUD_250KHz:
        ret = 250u;
        break;

        case CAN_BAUD_400KHz:
        ret = 400u;
        break;

        case CAN_BAUD_500KHz:
        ret = 500u;
        break;

        case CAN_BAUD_800KHz:
        ret = 800u;
        break;

        case CAN_BAUD_1000KHz:
        ret = 1000u;
        break;

        default:
        // ret already set
        break;
    }
    return ret;
}

static inline CAN_BAUD_TYPE CAN_IntToBaudType(uint16_t baud)
{
    CAN_BAUD_TYPE ret = CAN_BAUD_250KHz;
    switch(baud)
    {
        case 50u:
        ret = CAN_BAUD_50KHz;
        break;

        case 100u:
        ret = CAN_BAUD_100KHz;
        break;

        case 125u:
        ret = CAN_BAUD_125KHz;
        break;

        case 200u:
        ret = CAN_BAUD_200KHz;
        break;

        // 250 handled in default

        case 400u:
        ret = CAN_BAUD_400KHz;
        break;

        case 500u:
        ret = CAN_BAUD_500KHz;
        break;

        case 800u:
        ret = CAN_BAUD_800KHz;
        break;

        case 1000u:
        ret = CAN_BAUD_1000KHz;
        break;

        default:
        // ret already set
        break;
    }
    return ret;
}
