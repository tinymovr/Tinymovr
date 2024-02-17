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
uint32_t can_frame_hash;

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



