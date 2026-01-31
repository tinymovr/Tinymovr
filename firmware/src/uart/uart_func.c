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

#include "uart_func.h"

//==============================================================================
/// @brief Configure UARTB on PF2 (TX) and PF3 (RX)
//==============================================================================
static void UARTB_IO_Select_PF23(void)
{
    PAC55XX_GPIOF->MODE.P2 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOF->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PFPUEN.P2 = 0;
    PAC55XX_SCC->PFPUEN.P3 = 1;

    PAC55XX_SCC->PFMUXSEL.w &= 0xFFFF00FF;                      // Clear Port Pin selection
    PAC55XX_SCC->PFMUXSEL.w |= 0x00005500;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief Enable UARTB interrupt
//==============================================================================
static void uart_interrupt_enable(void)
{
    NVIC_ClearPendingIRQ(USARTB_IRQn);
    NVIC_SetPriority(USARTB_IRQn, 3);
    NVIC_EnableIRQ(USARTB_IRQn);
}

//==============================================================================
/// @brief Initialize UART peripheral
/// @param uart UART type (only UARTB is used, parameter kept for API compatibility)
/// @param baudrate Desired baud rate
//==============================================================================
void uart_init(UART_TYPE uart, uint32_t baudrate)
{
    (void)uart;  // Only UARTB is used
    
    PAC55XX_SCC->CCSCTL.USBMODE = USART_MODE_UART;  // UART mode

    pac5xxx_uart_config_LCR2(PAC55XX_UARTB,
                             UARTLCR_WL_BPC_8,
                             UART_STOP_BITS_1,
                             UART_PEN_DISABLE,
                             UART_PARITY_FORCE_STICK_1,
                             UART_BRKCTL_DISABLE);

    pac5xxx_uart_config_divisor_latch2(PAC55XX_UARTB, DF_UART_PCLK / 16 / baudrate);

    // RX FIFO setup
    PAC55XX_UARTB->FCR.FIFOEN = 1;
    PAC55XX_UARTB->FCR.RXFIFORST = 1;
    pac5xxx_uart_rx_fifo_threshold2(PAC55XX_UARTB, UARTFCR_TL_8B);
    pac5xxx_uart_tx_fifo_threshold2(PAC55XX_UARTB, UARTFCR_TL_8B);

    // Enable receive data available interrupt
    pac5xxx_uart_int_enable_RDAI2(PAC55XX_UARTB, UART_INT_ENABLE);

    UARTB_IO_Select_PF23();
    uart_interrupt_enable();
}
