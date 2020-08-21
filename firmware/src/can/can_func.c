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

uint8_t tx_data[8] = {0,1,2,3,4,5,6,7};
uint8_t rx_data[8];

uint8_t rx_dataLen;
uint16_t rx_id;
uint32_t buffer;
uint8_t rx_flag;

void can_io_config(CAN_IO_TYPE can)
{
    switch (can)
    {
    case CAN_PC01:
        // Select CAN peripheral on PC0 and PC1
        PAC55XX_GPIOC->MODE.P0 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOC->MODE.P1 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PCMUXSEL.P0 = 6;                   //CANRXD
        PAC55XX_SCC->PCMUXSEL.P1 = 6;                   //CANTXD
        break;

    case CAN_PC45:
        // Select CAN peripheral on PC4 and PC5
        PAC55XX_GPIOC->MODE.P4 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOC->MODE.P5 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PCMUXSEL.P4 = 6;                   //CANRXD
        PAC55XX_SCC->PCMUXSEL.P5 = 6;                   //CANTXD
        break;

    case CAN_PD01:
        // Select CAN peripheral on PD0 and PD1
        PAC55XX_GPIOD->MODE.P1 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOD->MODE.P0 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PDMUXSEL.P1 = 6;                   //CANRXD
        PAC55XX_SCC->PDMUXSEL.P0 = 6;                   //CANTXD
        break;

    case CAN_PD56:
        // Select CAN peripheral on PD5 and PD6
        PAC55XX_GPIOD->MODE.P5 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOD->MODE.P6 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PDMUXSEL.P5 = 6;                   //CANRXD
        PAC55XX_SCC->PDMUXSEL.P6 = 6;                   //CANTXD
        break;

    case CAN_PD67:
        // Select CAN peripheral on PD6 and PD7
        PAC55XX_GPIOD->MODE.P7 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOD->MODE.P6 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PDMUXSEL.P7 = 6;                   //CANRXD
        PAC55XX_SCC->PDMUXSEL.P6 = 6;                   //CANTXD
        break;

    case CAN_PE23:
        // Select CAN peripheral on PE2 and PE3
        PAC55XX_GPIOE->MODE.P2 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOE->MODE.P3 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PEMUXSEL.P2 = 6;                   //CANRXD
        PAC55XX_SCC->PEMUXSEL.P3 = 6;                   //CANTXD
        break;

    case CAN_PE67:
        // Select CAN peripheral on PE6 and PE7
        PAC55XX_GPIOE->MODE.P6 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOE->MODE.P7 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PEMUXSEL.P6 = 6;                   //CANRXD
        PAC55XX_SCC->PEMUXSEL.P7 = 6;                   //CANTXD
        break;

    case CAN_PF67:
        // Select CAN peripheral on PF6 and PF7
        PAC55XX_GPIOF->MODE.P6 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOF->MODE.P7 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PFMUXSEL.P6 = 6;                   //CANRXD
        PAC55XX_SCC->PFMUXSEL.P7 = 6;                   //CANTXD
        break;

    case CAN_PG56:
        // Select CAN peripheral on PG5 and PG6
        PAC55XX_GPIOG->MODE.P5 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOG->MODE.P6 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PGMUXSEL.P5 = 6;                   //CANRXD
        PAC55XX_SCC->PGMUXSEL.P6 = 6;                   //CANTXD
        break;

    default:
        // No action
        break;
    }
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

void can_transmit(uint8_t dataLen, uint16_t id, uint8_t * data)
{
    while (PAC55XX_CAN->SR.TBS == 0) {};           // wait for TX buffer free
    PAC55XX_CAN->TXBUF = (dataLen << 0)    |       // DLC - Data Length Code
                         (0u << 6)         |       // RTR = 0 Data Frame
                         (0u << 7)         |       // FF - Format Frame; 0=Std Frame
                         ((id>>3) << 8)    |       // ID 10:3
                         ((id&0x07u) << 21) |      // ID 2:0
                         (data[0] << 24);          // Data 0

    if(dataLen > 1u)
    {
        PAC55XX_CAN->TXBUF = (data[1] << 0)  |      // Data 1
                             (data[2] << 8)  |		// Data 2
                             (data[3] << 16) |      // Data 3
                             (data[4] << 24);       // Data 4
    }
    if(dataLen > 5u)
    {
    PAC55XX_CAN->TXBUF = (data[5] << 0)   |    // Data 5
                         (data[6] << 8)   |    // Data 6
                         (data[7] << 16);      // Data 7

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
