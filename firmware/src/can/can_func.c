// Except where otherwise noted, this file is:
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

#include "can_func.h"
#include <src/common.h>

uint8_t tx_data[8] = {0,1,2,3,4,5,6,7};
uint8_t rx_data[8];

uint8_t data_length;
uint32_t rx_id;
bool rtr;
uint32_t can_ep_id;
uint32_t can_seq_id;

//  * The function "can_io_config" is part of the Tinymovr-Firmware distribution
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

void can_io_config(void)
{
#ifdef CAN_PC01
        // Select CAN peripheral on PC0 and PC1
        PAC55XX_GPIOC->MODE.P0 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOC->MODE.P1 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PCMUXSEL.P0 = 6;                   //CANRXD
        PAC55XX_SCC->PCMUXSEL.P1 = 6;                   //CANTXD
#elif defined CAN_PC45
        // Select CAN peripheral on PC4 and PC5
        PAC55XX_GPIOC->MODE.P4 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOC->MODE.P5 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PCMUXSEL.P4 = 6;                   //CANRXD
        PAC55XX_SCC->PCMUXSEL.P5 = 6;                   //CANTXD
#elif defined CAN_PD01
        // Select CAN peripheral on PD0 and PD1
        PAC55XX_GPIOD->MODE.P1 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOD->MODE.P0 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PDMUXSEL.P1 = 6;                   //CANRXD
        PAC55XX_SCC->PDMUXSEL.P0 = 6;                   //CANTXD
#elif defined CAN_PD56
        // Select CAN peripheral on PD5 and PD6
        PAC55XX_GPIOD->MODE.P5 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOD->MODE.P6 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PDMUXSEL.P5 = 6;                   //CANRXD
        PAC55XX_SCC->PDMUXSEL.P6 = 6;                   //CANTXD
#elif defined CAN_PD67
        // Select CAN peripheral on PD6 and PD7
        PAC55XX_GPIOD->MODE.P7 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOD->MODE.P6 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PDMUXSEL.P7 = 6;                   //CANRXD
        PAC55XX_SCC->PDMUXSEL.P6 = 6;                   //CANTXD
#elif defined CAN_PE23
        // Select CAN peripheral on PE2 and PE3
        PAC55XX_GPIOE->MODE.P2 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOE->MODE.P3 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PEMUXSEL.P2 = 6;                   //CANRXD
        PAC55XX_SCC->PEMUXSEL.P3 = 6;                   //CANTXD
#elif defined CAN_PE67
        // Select CAN peripheral on PE6 and PE7
        PAC55XX_GPIOE->MODE.P6 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOE->MODE.P7 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PEMUXSEL.P6 = 6;                   //CANRXD
        PAC55XX_SCC->PEMUXSEL.P7 = 6;                   //CANTXD
#elif defined CAN_PF67
        // Select CAN peripheral on PF6 and PF7
        PAC55XX_GPIOF->MODE.P6 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOF->MODE.P7 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PFMUXSEL.P6 = 6;                   //CANRXD
        PAC55XX_SCC->PFMUXSEL.P7 = 6;                   //CANTXD
#elif defined CAN_PG56
        // Select CAN peripheral on PG5 and PG6
        PAC55XX_GPIOG->MODE.P5 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOG->MODE.P6 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PGMUXSEL.P5 = 6;                   //CANRXD
        PAC55XX_SCC->PGMUXSEL.P6 = 6;                   //CANTXD
#else
    #error "No CAN interface constant defined"
#endif
}

void can_baud(CAN_BAUD_TYPE baud)
{
    // CAN bit rate = PCLK/(2x(BRP+1)x(TSEG1+TSEG2+3).
    // For example: 150/(2x(19+1)x(10+2+3)) = 0.250 Mbps
    switch (baud)
    {
    case CAN_BAUD_50KHz:
        PAC55XX_CAN->BTR0.BRP = 59;
        PAC55XX_CAN->BTR1.TSEG1 = 15;
        PAC55XX_CAN->BTR1.TSEG2 = 7;
        break;

    case CAN_BAUD_100KHz:
        PAC55XX_CAN->BTR0.BRP = 49;
        PAC55XX_CAN->BTR1.TSEG1 = 10;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_125KHz:
        PAC55XX_CAN->BTR0.BRP = 39;
        PAC55XX_CAN->BTR1.TSEG1 = 10;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_200KHz:
        PAC55XX_CAN->BTR0.BRP = 24;
        PAC55XX_CAN->BTR1.TSEG1 = 10;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_250KHz:
        PAC55XX_CAN->BTR0.BRP = 19;
        PAC55XX_CAN->BTR1.TSEG1 = 10;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_400KHz:
        PAC55XX_CAN->BTR0.BRP = 11;
        PAC55XX_CAN->BTR1.TSEG1 = 10;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_500KHz:
        PAC55XX_CAN->BTR0.BRP = 18;
        PAC55XX_CAN->BTR1.TSEG1 = 3;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_800KHz:
        PAC55XX_CAN->BTR0.BRP = 11;
        PAC55XX_CAN->BTR1.TSEG1 = 3;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_1000KHz:
        PAC55XX_CAN->BTR0.BRP = 14;
        PAC55XX_CAN->BTR1.TSEG1 = 1;
        PAC55XX_CAN->BTR1.TSEG2 = 1;
        break;

    default:
        PAC55XX_CAN->BTR0.BRP = 14;
        PAC55XX_CAN->BTR1.TSEG1 = 1;
        PAC55XX_CAN->BTR1.TSEG2 = 1;
        break;
    }
}

PAC5XXX_RAMFUNC void can_process_standard(void)
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

PAC5XXX_RAMFUNC void can_process_extended(void)
{
    uint32_t buffer = PAC55XX_CAN->RXBUF; //  read RX buffer, RX buffer bit order same as TX buffer

    data_length = buffer & 0x0F;
    rtr = ((buffer >> 6) & 0x1) == 0x1;
    rx_id = ((buffer & 0xFF000000) >> 19) | ((buffer & 0x00FF0000) >> 3) | ((buffer & 0x0000FF00) << 13);
    
    buffer = PAC55XX_CAN->RXBUF;

    rx_id |= (buffer & 0xFF) >> 3;
    
    ids_from_arbitration(rx_id, &can_ep_id, &can_seq_id);

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

PAC5XXX_RAMFUNC void can_transmit_standard(uint8_t dataLen, uint16_t id, const uint8_t * data)
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

PAC5XXX_RAMFUNC void can_transmit_extended(uint8_t dataLen, uint32_t id, const uint8_t * data)
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

uint16_t CAN_BaudTypeToInt(CAN_BAUD_TYPE type)
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

CAN_BAUD_TYPE CAN_IntToBaudType(uint16_t baud)
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

void ids_from_arbitration(uint32_t arb_id, uint32_t* ep_id, uint32_t* seq_id)
{
    *ep_id = arb_id & CAN_EP_MASK;
    *seq_id = (arb_id & CAN_SEQ_MASK) >> CAN_EP_SIZE;
}

void arbitration_from_ids(uint32_t* arb_id, uint32_t ep_id, uint32_t seq_id, uint32_t node_id)
{
    *arb_id = (ep_id & CAN_EP_MASK) | ((seq_id << CAN_EP_SIZE) & CAN_SEQ_MASK) | ((node_id << (CAN_EP_SIZE + CAN_SEQ_SIZE)) & CAN_DEV_MASK);
}
