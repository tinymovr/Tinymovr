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

#include "ssp_func.h"

//==============================================================================
/// @brief
///     SSPA_SCLK <--> PA3
///     SSPA_MOSI <--> PA4
///     SSPA_MISO <--> PA5
///     SSPA_SS   <--> PA6
///
//==============================================================================
void SSPA_IO_Select_PA3456(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOA->MODE.P3 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOA->MODE.P4 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOA->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;       // MISO
        PAC55XX_GPIOA->MODE.P6 = IO_PUSH_PULL_OUTPUT;           // SS

        PAC55XX_SCC->PAPUEN.P3 = 0;
        PAC55XX_SCC->PAPUEN.P4 = 0;
        PAC55XX_SCC->PAPUEN.P5 = 1;
        PAC55XX_SCC->PAPUEN.P6 = 0;
    }
    else
    {
        PAC55XX_GPIOA->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOA->MODE.P4 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOA->MODE.P5 = IO_PUSH_PULL_OUTPUT;           // MISO
        PAC55XX_GPIOA->MODE.P6 = IO_HIGH_IMPEDENCE_INPUT;       // SS

        PAC55XX_SCC->PAPUEN.P3 = 1;
        PAC55XX_SCC->PAPUEN.P4 = 1;
        PAC55XX_SCC->PAPUEN.P5 = 0;
        PAC55XX_SCC->PAPUEN.P6 = 1;
    }

    PAC55XX_SCC->PAMUXSEL.w &= 0xF0000FFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PAMUXSEL.w |= 0x01111000;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPB_SCLK <--> PA3
///     SSPB_MOSI <--> PA4
///     SSPB_MISO <--> PA5
///     SSPB_SS   <--> PA6
///
//==============================================================================
void SSPB_IO_Select_PA3456(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOA->MODE.P3 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOA->MODE.P4 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOA->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;       // MISO
        PAC55XX_GPIOA->MODE.P6 = IO_PUSH_PULL_OUTPUT;           // SS

        PAC55XX_SCC->PAPUEN.P3 = 0;
        PAC55XX_SCC->PAPUEN.P4 = 0;
        PAC55XX_SCC->PAPUEN.P5 = 1;
        PAC55XX_SCC->PAPUEN.P6 = 0;
    }
    else
    {
        PAC55XX_GPIOA->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOA->MODE.P4 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOA->MODE.P5 = IO_PUSH_PULL_OUTPUT;           // MISO
        PAC55XX_GPIOA->MODE.P6 = IO_HIGH_IMPEDENCE_INPUT;       // SS

        PAC55XX_SCC->PAPUEN.P3 = 1;
        PAC55XX_SCC->PAPUEN.P4 = 1;
        PAC55XX_SCC->PAPUEN.P5 = 0;
        PAC55XX_SCC->PAPUEN.P6 = 1;
    }

    PAC55XX_SCC->PAMUXSEL.w &= 0xF0000FFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PAMUXSEL.w |= 0x02222000;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPB_MOSI <--> PC0
///     SSPB_MISO <--> PC1
///     SSPB_SCLK <--> PC2
///     SSPB_SS   <--> PC3
///
//==============================================================================
void SSPB_IO_Select_PC0123(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOC->MODE.P0 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOC->MODE.P1 = IO_HIGH_IMPEDENCE_INPUT;       // MISO
        PAC55XX_GPIOC->MODE.P2 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOC->MODE.P3 = IO_PUSH_PULL_OUTPUT;           // SS

        PAC55XX_SCC->PCPUEN.P0 = 0;
        PAC55XX_SCC->PCPUEN.P1 = 1;
        PAC55XX_SCC->PCPUEN.P2 = 0;
        PAC55XX_SCC->PCPUEN.P3 = 0;
    }
    else
    {
        PAC55XX_GPIOC->MODE.P0 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOC->MODE.P1 = IO_PUSH_PULL_OUTPUT;           // MISO
        PAC55XX_GPIOC->MODE.P2 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOC->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;       // SS

        PAC55XX_SCC->PCPUEN.P0 = 1;
        PAC55XX_SCC->PCPUEN.P1 = 0;
        PAC55XX_SCC->PCPUEN.P2 = 1;
        PAC55XX_SCC->PCPUEN.P3 = 1;
    }

    PAC55XX_SCC->PCMUXSEL.w &= 0xFFFF0000;                      // Clear Port Pin selection
    PAC55XX_SCC->PCMUXSEL.w |= 0x00004444;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPB_MOSI <--> PC4
///     SSPB_MISO <--> PC5
///     SSPB_SCLK <--> PC6
///     SSPB_SS   <--> PC7
///
//==============================================================================
void SSPB_IO_Select_PC4567(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOC->MODE.P4 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOC->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;       // MISO
        PAC55XX_GPIOC->MODE.P6 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOC->MODE.P7 = IO_PUSH_PULL_OUTPUT;           // SS

        PAC55XX_SCC->PCPUEN.P4 = 0;
        PAC55XX_SCC->PCPUEN.P5 = 1;
        PAC55XX_SCC->PCPUEN.P6 = 0;
        PAC55XX_SCC->PCPUEN.P7 = 0;
    }
    else
    {
        PAC55XX_GPIOC->MODE.P4 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOC->MODE.P5 = IO_PUSH_PULL_OUTPUT;           // MISO
        PAC55XX_GPIOC->MODE.P6 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOC->MODE.P7 = IO_HIGH_IMPEDENCE_INPUT;       // SS

        PAC55XX_SCC->PCPUEN.P4 = 1;
        PAC55XX_SCC->PCPUEN.P5 = 0;
        PAC55XX_SCC->PCPUEN.P6 = 1;
        PAC55XX_SCC->PCPUEN.P7 = 1;
    }

    PAC55XX_SCC->PCMUXSEL.w &= 0x0000FFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PCMUXSEL.w |= 0x44440000;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPB_SCLK <--> PE4
///     SSPB_SS   <--> PE5
///     SSPB_MOSI <--> PE6
///     SSPB_MISO <--> PE7
///
//==============================================================================
void SSPB_IO_Select_PE4567(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOE->MODE.P4 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOE->MODE.P5 = IO_PUSH_PULL_OUTPUT;           // SS
        PAC55XX_GPIOE->MODE.P6 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOE->MODE.P7 = IO_HIGH_IMPEDENCE_INPUT;       // MISO

        PAC55XX_SCC->PEPUEN.P4 = 0;
        PAC55XX_SCC->PEPUEN.P5 = 0;
        PAC55XX_SCC->PEPUEN.P6 = 0;
        PAC55XX_SCC->PEPUEN.P7 = 1;
    }
    else
    {
        PAC55XX_GPIOE->MODE.P4 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOE->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;       // SS
        PAC55XX_GPIOE->MODE.P6 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOE->MODE.P7 = IO_PUSH_PULL_OUTPUT;           // MISO

        PAC55XX_SCC->PEPUEN.P4 = 1;
        PAC55XX_SCC->PEPUEN.P5 = 1;
        PAC55XX_SCC->PEPUEN.P6 = 1;
        PAC55XX_SCC->PEPUEN.P7 = 0;
    }

    PAC55XX_SCC->PEMUXSEL.w &= 0x0000FFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PEMUXSEL.w |= 0x44440000;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPB_SCLK <--> PF0
///     SSPB_SS   <--> PF1
///     SSPB_MOSI <--> PF2
///     SSPB_MISO <--> PF3
///
//==============================================================================
void SSPB_IO_Select_PF0123(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOF->MODE.P0 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOF->MODE.P1 = IO_PUSH_PULL_OUTPUT;           // SS
        PAC55XX_GPIOF->MODE.P2 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOF->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;       // MISO

        PAC55XX_SCC->PFPUEN.P0 = 0;
        PAC55XX_SCC->PFPUEN.P1 = 0;
        PAC55XX_SCC->PFPUEN.P2 = 0;
        PAC55XX_SCC->PFPUEN.P3 = 1;
    }
    else
    {
        PAC55XX_GPIOF->MODE.P0 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOF->MODE.P1 = IO_HIGH_IMPEDENCE_INPUT;       // SS
        PAC55XX_GPIOF->MODE.P2 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOF->MODE.P3 = IO_PUSH_PULL_OUTPUT;           // MISO

        PAC55XX_SCC->PFPUEN.P0 = 1;
        PAC55XX_SCC->PFPUEN.P1 = 1;
        PAC55XX_SCC->PFPUEN.P2 = 1;
        PAC55XX_SCC->PFPUEN.P3 = 0;
    }

    PAC55XX_SCC->PFMUXSEL.w &= 0xFFFF0000;                    // Clear Port Pin selection
    PAC55XX_SCC->PFMUXSEL.w |= 0x00005555;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPC_SCLK <--> PC0
///     SSPC_SS   <--> PC1
///     SSPC_MOSI <--> PC2
///     SSPC_MISO <--> PC3
///
//==============================================================================
void SSPC_IO_Select_PC0123(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOC->MODE.P0 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOC->MODE.P1 = IO_PUSH_PULL_OUTPUT;           // SS
        PAC55XX_GPIOC->MODE.P2 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOC->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;       // MISO

        PAC55XX_SCC->PCPUEN.P0 = 0;
        PAC55XX_SCC->PCPUEN.P1 = 0;
        PAC55XX_SCC->PCPUEN.P2 = 0;
        PAC55XX_SCC->PCPUEN.P3 = 1;
    }
    else
    {
        PAC55XX_GPIOC->MODE.P0 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOC->MODE.P1 = IO_HIGH_IMPEDENCE_INPUT;       // SS
        PAC55XX_GPIOC->MODE.P2 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOC->MODE.P3 = IO_PUSH_PULL_OUTPUT;           // MISO

        PAC55XX_SCC->PCPUEN.P0 = 1;
        PAC55XX_SCC->PCPUEN.P1 = 1;
        PAC55XX_SCC->PCPUEN.P2 = 1;
        PAC55XX_SCC->PCPUEN.P3 = 0;
    }

    PAC55XX_SCC->PCMUXSEL.w &= 0xFFFF0000;                      // Clear Port Pin selection
    PAC55XX_SCC->PCMUXSEL.w |= 0x00005555;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPC_SCLK <--> PC4
///     SSPC_SS   <--> PC5
///     SSPC_MOSI <--> PC6
///     SSPC_MISO <--> PC7
///
//==============================================================================
void SSPC_IO_Select_PC4567(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOC->MODE.P4 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOC->MODE.P5 = IO_PUSH_PULL_OUTPUT;           // SS
        PAC55XX_GPIOC->MODE.P6 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOC->MODE.P7 = IO_HIGH_IMPEDENCE_INPUT;       // MISO

        PAC55XX_SCC->PCPUEN.P4 = 0;
        PAC55XX_SCC->PCPUEN.P5 = 0;
        PAC55XX_SCC->PCPUEN.P6 = 0;
        PAC55XX_SCC->PCPUEN.P7 = 1;
    }
    else
    {
        PAC55XX_GPIOC->MODE.P4 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOC->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;       // SS
        PAC55XX_GPIOC->MODE.P6 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOC->MODE.P7 = IO_PUSH_PULL_OUTPUT;           // MISO

        PAC55XX_SCC->PCPUEN.P4 = 1;
        PAC55XX_SCC->PCPUEN.P5 = 1;
        PAC55XX_SCC->PCPUEN.P6 = 1;
        PAC55XX_SCC->PCPUEN.P7 = 0;
    }

    PAC55XX_SCC->PCMUXSEL.w &= 0x0000FFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PCMUXSEL.w |= 0x55550000;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPC_SCLK <--> PD0
///     SSPC_SS   <--> PD1
///     SSPC_MOSI <--> PD2
///     SSPC_MISO <--> PD3
///
//==============================================================================
void SSPC_IO_Select_PD0123(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOD->MODE.P0 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOD->MODE.P1 = IO_PUSH_PULL_OUTPUT;           // SS
        PAC55XX_GPIOD->MODE.P2 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOD->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;       // MISO

        PAC55XX_SCC->PDPUEN.P0 = 0;
        PAC55XX_SCC->PDPUEN.P1 = 0;
        PAC55XX_SCC->PDPUEN.P2 = 0;
        PAC55XX_SCC->PDPUEN.P3 = 1;
    }
    else
    {
        PAC55XX_GPIOD->MODE.P0 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOD->MODE.P1 = IO_HIGH_IMPEDENCE_INPUT;       // SS
        PAC55XX_GPIOD->MODE.P2 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOD->MODE.P3 = IO_PUSH_PULL_OUTPUT;           // MISO

        PAC55XX_SCC->PDPUEN.P0 = 1;
        PAC55XX_SCC->PDPUEN.P1 = 1;
        PAC55XX_SCC->PDPUEN.P2 = 1;
        PAC55XX_SCC->PDPUEN.P3 = 0;
    }

    PAC55XX_SCC->PDMUXSEL.w &= 0xFFFF0000;                      // Clear Port Pin selection
    PAC55XX_SCC->PDMUXSEL.w |= 0x00005555;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPC_SCLK <--> PE0
///     SSPC_SS   <--> PE1
///     SSPC_MOSI <--> PE2
///     SSPC_MISO <--> PE3
///
//==============================================================================
void SSPC_IO_Select_PE0123(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOE->MODE.P0 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOE->MODE.P1 = IO_PUSH_PULL_OUTPUT;           // SS
        PAC55XX_GPIOE->MODE.P2 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOE->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;       // MISO

        PAC55XX_SCC->PEPUEN.P0 = 0;
        PAC55XX_SCC->PEPUEN.P1 = 0;
        PAC55XX_SCC->PEPUEN.P2 = 0;
        PAC55XX_SCC->PEPUEN.P3 = 1;
    }
    else
    {
        PAC55XX_GPIOE->MODE.P0 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOE->MODE.P1 = IO_HIGH_IMPEDENCE_INPUT;       // SS
        PAC55XX_GPIOE->MODE.P2 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOE->MODE.P3 = IO_PUSH_PULL_OUTPUT;           // MISO

        PAC55XX_SCC->PEPUEN.P0 = 1;
        PAC55XX_SCC->PEPUEN.P1 = 1;
        PAC55XX_SCC->PEPUEN.P2 = 1;
        PAC55XX_SCC->PEPUEN.P3 = 0;
    }

    PAC55XX_SCC->PEMUXSEL.w &= 0xFFFF0000;                      // Clear Port Pin selection
    PAC55XX_SCC->PEMUXSEL.w |= 0x00005555;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPD_SCLK <--> PD4
///     SSPD_SS   <--> PD5
///     SSPD_MOSI <--> PD6
///     SSPD_MISO <--> PD7
///
//==============================================================================
void SSPD_IO_Select_PD4567(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOD->MODE.P4 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOD->MODE.P5 = IO_PUSH_PULL_OUTPUT;           // SS
        PAC55XX_GPIOD->MODE.P6 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOD->MODE.P7 = IO_HIGH_IMPEDENCE_INPUT;       // MISO

        PAC55XX_SCC->PDPUEN.P4 = 0;
        PAC55XX_SCC->PDPUEN.P5 = 0;
        PAC55XX_SCC->PDPUEN.P6 = 0;
        PAC55XX_SCC->PDPUEN.P7 = 1;
    }
    else
    {
        PAC55XX_GPIOD->MODE.P4 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOD->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;       // SS
        PAC55XX_GPIOD->MODE.P6 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOD->MODE.P7 = IO_PUSH_PULL_OUTPUT;           // MISO

        PAC55XX_SCC->PDPUEN.P4 = 1;
        PAC55XX_SCC->PDPUEN.P5 = 1;
        PAC55XX_SCC->PDPUEN.P6 = 1;
        PAC55XX_SCC->PDPUEN.P7 = 0;
    }

    PAC55XX_SCC->PDMUXSEL.w &= 0x0000FFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PDMUXSEL.w |= 0x55550000;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPD_MOSI <--> PE4
///     SSPD_MISO <--> PE5
///     SSPD_SCLK <--> PE6
///     SSPD_SS   <--> PE7
///
//==============================================================================
void SSPD_IO_Select_PE4567(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOE->MODE.P4 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOE->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;       // MISO
        PAC55XX_GPIOE->MODE.P6 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOE->MODE.P7 = IO_PUSH_PULL_OUTPUT;           // SS

        PAC55XX_SCC->PEPUEN.P4 = 0;
        PAC55XX_SCC->PEPUEN.P5 = 1;
        PAC55XX_SCC->PEPUEN.P6 = 0;
        PAC55XX_SCC->PEPUEN.P7 = 0;
    }
    else
    {
        PAC55XX_GPIOE->MODE.P4 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOE->MODE.P5 = IO_PUSH_PULL_OUTPUT;           // MISO
        PAC55XX_GPIOE->MODE.P6 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOE->MODE.P7 = IO_HIGH_IMPEDENCE_INPUT;       // SS

        PAC55XX_SCC->PEPUEN.P4 = 1;
        PAC55XX_SCC->PEPUEN.P5 = 0;
        PAC55XX_SCC->PEPUEN.P6 = 1;
        PAC55XX_SCC->PEPUEN.P7 = 1;
    }

    PAC55XX_SCC->PEMUXSEL.w &= 0x0000FFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PEMUXSEL.w |= 0x55550000;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPD_SCLK <--> PF4
///     SSPD_SS   <--> PF5
///     SSPD_MOSI <--> PF6
///     SSPD_MISO <--> PF7
///
//==============================================================================
void SSPD_IO_Select_PF4567(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOF->MODE.P4 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOF->MODE.P5 = IO_PUSH_PULL_OUTPUT;           // SS
        PAC55XX_GPIOF->MODE.P6 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOF->MODE.P7 = IO_HIGH_IMPEDENCE_INPUT;       // MISO

        PAC55XX_SCC->PFPUEN.P4 = 0;
        PAC55XX_SCC->PFPUEN.P5 = 0;
        PAC55XX_SCC->PFPUEN.P6 = 0;
        PAC55XX_SCC->PFPUEN.P7 = 1;
    }
    else
    {
        PAC55XX_GPIOF->MODE.P4 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOF->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;       // SS
        PAC55XX_GPIOF->MODE.P6 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOF->MODE.P7 = IO_PUSH_PULL_OUTPUT;           // MISO

        PAC55XX_SCC->PFPUEN.P4 = 1;
        PAC55XX_SCC->PFPUEN.P5 = 1;
        PAC55XX_SCC->PFPUEN.P6 = 1;
        PAC55XX_SCC->PFPUEN.P7 = 0;
    }

    PAC55XX_SCC->PFMUXSEL.w &= 0x0000FFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PFMUXSEL.w |= 0x55550000;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPD_SCLK <--> PG0
///     SSPD_SS   <--> PG1
///     SSPD_MOSI <--> PG2
///     SSPD_MISO <--> PG3
///
//==============================================================================
void SSPD_IO_Select_PG0123(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOG->MODE.P0 = IO_PUSH_PULL_OUTPUT;           // SCLK
        PAC55XX_GPIOG->MODE.P1 = IO_PUSH_PULL_OUTPUT;           // SS
        PAC55XX_GPIOG->MODE.P2 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOG->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;       // MISO

        PAC55XX_SCC->PGPUEN.P0 = 0;
        PAC55XX_SCC->PGPUEN.P1 = 0;
        PAC55XX_SCC->PGPUEN.P2 = 0;
        PAC55XX_SCC->PGPUEN.P3 = 1;
    }
    else
    {
        PAC55XX_GPIOG->MODE.P0 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK
        PAC55XX_GPIOG->MODE.P1 = IO_HIGH_IMPEDENCE_INPUT;       // SS
        PAC55XX_GPIOG->MODE.P2 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOG->MODE.P3 = IO_PUSH_PULL_OUTPUT;           // MISO

        PAC55XX_SCC->PGPUEN.P0 = 1;
        PAC55XX_SCC->PGPUEN.P1 = 1;
        PAC55XX_SCC->PGPUEN.P2 = 1;
        PAC55XX_SCC->PGPUEN.P3 = 0;
    }

    PAC55XX_SCC->PGMUXSEL.w &= 0xFFFF0000;                      // Clear Port Pin selection
    PAC55XX_SCC->PGMUXSEL.w |= 0x00005555;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     SSPD_SS   <--> PG4
///     SSPD_MOSI <--> PG5
///     SSPD_MISO <--> PG6
///     SSPD_SCLK <--> PG7
///
//==============================================================================
void SSPD_IO_Select_PG4567(SSP_MS_TYPE ms_mode)
{
    if (ms_mode == SSP_MS_MASTER)
    {
        PAC55XX_GPIOG->MODE.P4 = IO_PUSH_PULL_OUTPUT;           // SS
        PAC55XX_GPIOG->MODE.P5 = IO_PUSH_PULL_OUTPUT;           // MOSI
        PAC55XX_GPIOG->MODE.P6 = IO_HIGH_IMPEDENCE_INPUT;       // MISO
        PAC55XX_GPIOG->MODE.P7 = IO_PUSH_PULL_OUTPUT;           // SCLK

        PAC55XX_SCC->PGPUEN.P4 = 0;
        PAC55XX_SCC->PGPUEN.P5 = 0;
        PAC55XX_SCC->PGPUEN.P6 = 1;
        PAC55XX_SCC->PGPUEN.P7 = 0;
    }
    else
    {
        PAC55XX_GPIOG->MODE.P4 = IO_HIGH_IMPEDENCE_INPUT;       // SS
        PAC55XX_GPIOG->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;       // MOSI
        PAC55XX_GPIOG->MODE.P6 = IO_PUSH_PULL_OUTPUT;           // MISO
        PAC55XX_GPIOG->MODE.P7 = IO_HIGH_IMPEDENCE_INPUT;       // SCLK

        PAC55XX_SCC->PGPUEN.P4 = 1;
        PAC55XX_SCC->PGPUEN.P5 = 1;
        PAC55XX_SCC->PGPUEN.P6 = 0;
        PAC55XX_SCC->PGPUEN.P7 = 1;
    }

    PAC55XX_SCC->PGMUXSEL.w &= 0x0000FFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PGMUXSEL.w |= 0x55550000;                      // Set Port Pin as SSP
}

//==============================================================================
///@brief
///     Choose the SSP and Enable the IO you want
///
///@param[in]
///     SSP Type:
///          SSPA
///          SSPB
///          SSPC
///          SSPD
///     SSP_MS_TYPE:
///          MASTER
///          SLAVE
///
//==============================================================================
void ssp_io_config(SSP_TYPE ssp, SSP_MS_TYPE ms_mode)
{
    switch (ssp)
    {
        case SSPA:
            // Select ssp A peripheral choose one!
            SSPA_IO_Select_PA3456(ms_mode);
            break;

        case SSPB:
            // Select ssp B peripheral choose one!
            SSPB_IO_Select_PA3456(ms_mode);
//            SSPB_IO_Select_PC0123(ms_mode);
//            SSPB_IO_Select_PC4567(ms_mode);
//            SSPB_IO_Select_PE4567(ms_mode);
//            SSPB_IO_Select_PF0123(ms_mode);
            break;

        case SSPC:
            // Select ssp C peripheral choose one!
//            SSPC_IO_Select_PC0123(ms_mode);
//            SSPC_IO_Select_PC4567(ms_mode);
//            SSPC_IO_Select_PD0123(ms_mode);
            SSPC_IO_Select_PE0123(ms_mode);
            break;

        case SSPD:
            // Select ssp D peripheral choose one!
            SSPD_IO_Select_PD4567(ms_mode);
//            SSPD_IO_Select_PE4567(ms_mode);
//            SSPD_IO_Select_PF4567(ms_mode);
//            SSPD_IO_Select_PG0123(ms_mode);
//            SSPD_IO_Select_PG4567(ms_mode);
            break;

        default:
            break;
    }
}

//==============================================================================
///@brief
///     enable the interrupt
///
///@param[in]
///     SSP Type:
///         SSPA
///         SSPB
///         SSPC
///         SSPD
///
//==============================================================================
void ssp_interrupt_enable(SSP_TYPE ssp)
{
    switch (ssp)
    {
        case SSPA:
            NVIC_ClearPendingIRQ(USARTA_IRQn);
            NVIC_SetPriority(USARTA_IRQn, 3);
            NVIC_EnableIRQ(USARTA_IRQn);
            break;

        case SSPB:
            NVIC_ClearPendingIRQ(USARTB_IRQn);
            NVIC_SetPriority(USARTB_IRQn, 3);
            NVIC_EnableIRQ(USARTB_IRQn);
            break;

        case SSPC:
            NVIC_ClearPendingIRQ(USARTC_IRQn);
            NVIC_SetPriority(USARTC_IRQn, 3);
            NVIC_EnableIRQ(USARTC_IRQn);
            break;

        case SSPD:
            NVIC_ClearPendingIRQ(USARTD_IRQn);
            NVIC_SetPriority(USARTD_IRQn, 3);
            NVIC_EnableIRQ(USARTD_IRQn);
            break;

        default:
            break;
    }
}

void ssp_init(SSP_TYPE ssp, SSP_MS_TYPE ms_mode, uint8_t cph, uint8_t cpol)
{
    PAC55XX_SSP_TYPEDEF *ssp_ptr;

    switch (ssp)
    {
        case SSPA:
            ssp_ptr = PAC55XX_SSPA;
            PAC55XX_SCC->CCSCTL.USAMODE = USART_MODE_SSP;       // SSP mode
            break;

        case SSPB:
            ssp_ptr = PAC55XX_SSPB;
            PAC55XX_SCC->CCSCTL.USBMODE = USART_MODE_SSP;       // SSP mode
            break;

        case SSPC:
            ssp_ptr = PAC55XX_SSPC;
            PAC55XX_SCC->CCSCTL.USCMODE = USART_MODE_SSP;       // SSP mode
            break;

        case SSPD:
            ssp_ptr = PAC55XX_SSPD;
            PAC55XX_SCC->CCSCTL.USDMODE = USART_MODE_SSP;       // SSP mode
            break;

        default:
            ssp_ptr = PAC55XX_SSPD;
            break;
    }

    // SSPCLK = PCLK / ((SSPxCLK.M + 1)*SSPxCLK.N) = 150000000 / ((2+1) x 2) = 25MHz
	// SSPCLK = PCLK / ((SSPxCLK.M + 1)*SSPxCLK.N) = 150000000 / ((2+1) x 32) = 1.5875MHz
    // SSPCLK = PCLK / ((SSPxCLK.M + 1)*SSPxCLK.N) = 150000000 / ((2+1) x 16) = 3.175MHz
    // SSPCLK = PCLK / ((SSPxCLK.M + 1)*SSPxCLK.N) = 150000000 / ((2+1) x 8) = 6.35MHz
    // SSPCLK = PCLK / ((SSPxCLK.M + 1)*SSPxCLK.N) = 150000000 / ((2+1) x 6) = 8.47MHz
    // SSPCLK = PCLK / ((SSPxCLK.M + 1)*SSPxCLK.N) = 150000000 / ((2+1) x 4) = 12.5MHz
	ssp_ptr->CLK.M = 2;
	ssp_ptr->CLK.N = 4;                                     // N must be even value from 2 to 254
	ssp_ptr->CON.FRF = SSP_FRAME_FORMAT_SPI;                 // Frame Format, SPI frame format
	ssp_ptr->CON.MS = ms_mode;                               // Master/Slave mode, master mode
	ssp_ptr->CON.LSBFIRST = SSP_ENDIAN_MSB;                  // Endian Order, MSB transmit 1st
	ssp_ptr->CON.LBM = SSP_LP_NORMAL;                        // Loopback Mode, no loopback mode
	ssp_ptr->CON.CPH = cph;                      // Clock Out Phase, SPI captures data at 2nd edge transition of the frame
	ssp_ptr->CON.CPO = cpol;                // Clock Out Polarity, SPI clock active high
    ssp_ptr->CON.DSS = SSP_DATA_SIZE_16;                     // Data Size Select, 16 bit data
    ssp_ptr->CON.SOD = SSP_OUTPUT_NOT_DRIVE;                 // Slave Output Disable

    ssp_io_config(ssp, ms_mode);
    ssp_interrupt_enable(ssp);

    // Set the interrupt for Reading data
//    ssp_ptr->IMSC.TXIM = 1;                                  // Enable TX FIFO interrupt
//    ssp_ptr->ICLR.RORIC = 1;                                 // Clear RX Overrun interrupt
//    ssp_ptr->ICLR.RTIC = 1;                                  // Clear RX Timeout interrupt
//    ssp_ptr->IMSC.RXIM = 1;                                  // Enable RX FIFO interrupt
//    ssp_ptr->IMSC.RORIM = 1;                                 // Enable RX Overrun interrupt
//    ssp_ptr->IMSC.RTIM = 1;                                  // Enable RX Timeout interrupt

    ssp_ptr->CON.SSPEN = SSP_CONTROL_ENABLE;                 // SSP Enable
}

//==============================================================================
///@brief
///     Write a 16-bit data to SSP manually
///
///@param[in]
///     PAC55XX_SSP_TYPEDEF: pointer to SSP struct
///     data: The data to write.
///
///@param[out]
///     result: the result of the SSP write
///
///@retval
///     0: All is OK.
///     others: Some error occurs.
///
//==============================================================================
TM_RAMFUNC uint32_t ssp_write_one(PAC55XX_SSP_TYPEDEF *ssp_ptr, uint16_t data)
{
    uint32_t result = PAC5XXX_OK;
    uint32_t wait_tick = 0;

    // Write 16-bits Data
    ssp_ptr->DAT.DATA = data;

    while (ssp_ptr->STAT.TNF == 0)
    {
        wait_tick++;
        if (wait_tick > DF_SSP_BUSY_TICK)
        {
            result = PAC5XXX_ERROR;
            break;
        }
    }

    return result;
}

//==============================================================================
///@brief
///     Write a serial 16-bit data to SSP manually
///
///@param[in]
///     PAC55XX_SSP_TYPEDEF: pointer to SSP struct
///     data_p: The data pointer to write.
///     byte_num: the number to write.
///
///@param[out]
///     result: the result of the SSP byte write
///
///@retval
///     0: All is OK.
///     others: Some error occurs.
///
//==============================================================================
TM_RAMFUNC uint32_t ssp_write_multi(PAC55XX_SSP_TYPEDEF *ssp_ptr, uint16_t *data_p, uint32_t byte_num)
{
    uint16_t *data = data_p;

    uint32_t result = PAC5XXX_OK;
    uint32_t wait_tick = 0;
    uint32_t byte_num_temp;

    for (byte_num_temp=0; byte_num_temp<byte_num; byte_num_temp++)
    {
        // Write 16-bits data, increment pointer
        ssp_ptr->DAT.DATA = *data;
        data++;

        while (ssp_ptr->STAT.TNF == 0)
        {
            wait_tick++;
            if (wait_tick > DF_SSP_BUSY_TICK)
            {
                result = PAC5XXX_ERROR;
                break;
            }
        }

        wait_tick = 0;
    }

    return result;
}

TM_RAMFUNC uint16_t ssp_read_one(PAC55XX_SSP_TYPEDEF *ssp_ptr)
{
    // Might be worth adding a timeout
    while (!ssp_ptr->STAT.RNE) {};
    uint16_t data = (ssp_ptr->DAT.DATA);

    return data;
}