//=============================================================================
// Copyright (C) 2018, Active-Semi International
//
// THIS SOFTWARE IS SUBJECT TO A SOURCE CODE LICENSE AGREEMENT WHICH PROVIDES,
// AMONG OTHER THINGS:  (i) THAT IT CAN BE USED ONLY TO ADAPT THE LICENSEE'S
// APPLICATION TO PAC PROCESSORS SUPPLIED BY ACTIVE-SEMI INTERNATIONAL;
// (ii) THAT  IT IS PROVIDED "AS IS" WITHOUT WARRANTY;  (iii) THAT
// ACTIVE-SEMICONDUCTOR IS NOT LIABLE FOR ANY INDIRECT DAMAGES OR FOR DIRECT
// DAMAGES EXCEEDING US$1,500;  AND (iv) THAT IT CAN BE DISCLOSED TO AND USED
// ONLY BY CERTAIN AUTHORIZED PERSONS.
//
//=============================================================================

#ifndef SSP_FUNC_H
#define SSP_FUNC_H

#include "src/common.h"

// SPI mode Clock polarity  Clock phase
//          (CPOL)          (CPHA)	
// 0	    0              	0	
// 1	    0	            1	
// 2	    1	            0	
// 3	    1	            1	

#define DF_SSP_BUSY_TICK (25000u)    // use to check the busy tick

// Device Select Enumeration Type
typedef enum
{
    SWSEL_SPI = 0,
    SWSEL_SW = 1
} SSP_SWSEL_TYPE;

// Interrupt Enable Enumeration Type
typedef enum
{
    SSPA = 0,
    SSPB = 1,
    SSPC = 2,
    SSPD = 3,
    SSPD_PD4567 = 4
} SSP_TYPE;


volatile uint16_t ssp_data[10];
volatile uint16_t data_num;

extern void ssp_init(SSP_TYPE ssp, SSP_MS_TYPE ms_mode, uint8_t clkn_div, uint32_t data_size, uint8_t swsel, uint8_t cph, uint8_t cpol);
extern void ssp_deinit(SSP_TYPE ssp);
extern uint32_t ssp_write_one(PAC55XX_SSP_TYPEDEF *ssp_ptr, uint16_t data);
extern uint32_t ssp_write_multi(PAC55XX_SSP_TYPEDEF *ssp_ptr, uint16_t *data, uint32_t byte_num);
extern uint16_t ssp_read_one(PAC55XX_SSP_TYPEDEF *ssp_ptr);

#endif
