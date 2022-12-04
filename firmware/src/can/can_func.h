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
#define CAN_SEQ_SIZE 9
#define CAN_SEQ_MASK (((1 << CAN_SEQ_SIZE) - 1) << CAN_EP_SIZE)
#define CAN_DEV_SIZE 8
#define CAN_DEV_MASK (((1 << CAN_DEV_SIZE) - 1) << (CAN_EP_SIZE + CAN_SEQ_SIZE))

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
extern uint32_t can_seq_id;

void can_baud(CAN_BAUD_TYPE baud);
void can_io_config(void);
void can_process_standard(void);
void can_process_extended(void);
void can_transmit_standard(uint8_t dataLen, uint16_t id, const uint8_t * data);
void can_transmit_extended(uint8_t dataLen, uint32_t id, const uint8_t * data);

uint16_t CAN_BaudTypeToInt(CAN_BAUD_TYPE type);
CAN_BAUD_TYPE CAN_IntToBaudType(uint16_t baud);

void ids_from_arbitration(uint32_t arb_id, uint32_t* can_ep_id, uint32_t* can_seq_id);
void arbitration_from_ids(uint32_t* arb_id, uint32_t ep_id, uint32_t seq_id, uint32_t node_id);