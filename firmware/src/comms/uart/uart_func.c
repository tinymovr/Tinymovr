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
 
#include <src/comms/uart/uart_func.h>

//==============================================================================
/// @brief
///     UARTA_Tx <--> PA4
///     UARTA_Rx <--> PA5
///
//==============================================================================
void UARTA_IO_Select_PA45(void)
{
    PAC55XX_GPIOA->MODE.P4 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOA->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PAPUEN.P4 = 0;
    PAC55XX_SCC->PAPUEN.P5 = 1;

    PAC55XX_SCC->PAMUXSEL.w &= 0xFFF00FFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PAMUXSEL.w |= 0x00011000;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTB_Tx <--> PA4
///     UARTB_Rx <--> PA5
///
//==============================================================================
void UARTB_IO_Select_PA45(void)
{
    PAC55XX_GPIOA->MODE.P4 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOA->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PAPUEN.P4 = 0;
    PAC55XX_SCC->PAPUEN.P5 = 1;

    PAC55XX_SCC->PAMUXSEL.w &= 0xFFF00FFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PAMUXSEL.w |= 0x00022000;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTB_Tx <--> PC0
///     UARTB_Rx <--> PC1
///
//==============================================================================
void UARTB_IO_Select_PC01(void)
{
    PAC55XX_GPIOC->MODE.P0 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOC->MODE.P1 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PCPUEN.P0 = 0;
    PAC55XX_SCC->PCPUEN.P1 = 1;

    PAC55XX_SCC->PCMUXSEL.w &= 0xFFFFFF00;                      // Clear Port Pin selection
    PAC55XX_SCC->PCMUXSEL.w |= 0x00000044;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTB_Tx <--> PC4
///     UARTB_Rx <--> PC5
///
//==============================================================================
void UARTB_IO_Select_PC45(void)
{
    PAC55XX_GPIOC->MODE.P4 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOC->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PCPUEN.P4 = 0;
    PAC55XX_SCC->PCPUEN.P5 = 1;

    PAC55XX_SCC->PCMUXSEL.w &= 0xFF00FFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PCMUXSEL.w |= 0x00440000;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTB_Tx <--> PE6
///     UARTB_Rx <--> PE7
///
//==============================================================================
void UARTB_IO_Select_PE67(void)
{
    PAC55XX_GPIOE->MODE.P6 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOE->MODE.P7 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PEPUEN.P6 = 0;
    PAC55XX_SCC->PEPUEN.P7 = 1;

    PAC55XX_SCC->PEMUXSEL.w &= 0x00FFFFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PEMUXSEL.w |= 0x44000000;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTB_Tx <--> PF2
///     UARTB_Rx <--> PF3
///
//==============================================================================
void UARTB_IO_Select_PF23(void)
{
    PAC55XX_GPIOF->MODE.P2 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOF->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PFPUEN.P2 = 0;
    PAC55XX_SCC->PFPUEN.P3 = 1;

    PAC55XX_SCC->PFMUXSEL.w &= 0xFFFF00FF;                      // Clear Port Pin selection
    PAC55XX_SCC->PFMUXSEL.w |= 0x00005500;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTC_Tx <--> PC2
///     UARTC_Rx <--> PC3
///
//==============================================================================
void UARTC_IO_Select_PC23(void)
{
    PAC55XX_GPIOC->MODE.P2 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOC->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PCPUEN.P2 = 0;
    PAC55XX_SCC->PCPUEN.P3 = 1;

    PAC55XX_SCC->PCMUXSEL.w &= 0xFFFF00FF;                      // Clear Port Pin selection
    PAC55XX_SCC->PCMUXSEL.w |= 0x00005500;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTC_Tx <--> PC6
///     UARTC_Rx <--> PC7
///
//==============================================================================
void UARTC_IO_Select_PC67(void)
{
    PAC55XX_GPIOC->MODE.P6 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOC->MODE.P7 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PCPUEN.P6 = 0;
    PAC55XX_SCC->PCPUEN.P7 = 1;

    PAC55XX_SCC->PCMUXSEL.w &= 0x00FFFFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PCMUXSEL.w |= 0x55000000;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTC_Tx <--> PD2
///     UARTC_Rx <--> PD3
///
//==============================================================================
void UARTC_IO_Select_PD23(void)
{
    PAC55XX_GPIOD->MODE.P2 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOD->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PDPUEN.P2 = 0;
    PAC55XX_SCC->PDPUEN.P3 = 1;

    PAC55XX_SCC->PDMUXSEL.w &= 0xFFFF00FF;                      // Clear Port Pin selection
    PAC55XX_SCC->PDMUXSEL.w |= 0x00005500;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTC_Tx <--> PE2
///     UARTC_Rx <--> PE3
///
//==============================================================================
void UARTC_IO_Select_PE23(void)
{
    PAC55XX_GPIOE->MODE.P2 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOE->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PEPUEN.P2 = 0;
    PAC55XX_SCC->PEPUEN.P3 = 1;

    PAC55XX_SCC->PEMUXSEL.w &= 0xFFFF00FF;                      // Clear Port Pin selection
    PAC55XX_SCC->PEMUXSEL.w |= 0x00005500;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTD_Tx <--> PD4
///     UARTD_Rx <--> PD5
///
//==============================================================================
void UARTD_IO_Select_PD45(void)
{
    PAC55XX_GPIOD->MODE.P4 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOD->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PDPUEN.P4 = 0;
    PAC55XX_SCC->PDPUEN.P5 = 1;

    PAC55XX_SCC->PDMUXSEL.w &= 0xFF00FFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PDMUXSEL.w |= 0x00770000;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTD_Tx <--> PD6
///     UARTD_Rx <--> PD7
///
//==============================================================================
void UARTD_IO_Select_PD67(void)
{
    PAC55XX_GPIOD->MODE.P6 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOD->MODE.P7 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PDPUEN.P6 = 0;
    PAC55XX_SCC->PDPUEN.P7 = 1;

    PAC55XX_SCC->PDMUXSEL.w &= 0x00FFFFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PDMUXSEL.w |= 0x55000000;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTD_Tx <--> PE4
///     UARTD_Rx <--> PE5
///
//==============================================================================
void UARTD_IO_Select_PE45(void)
{
    PAC55XX_GPIOE->MODE.P4 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOE->MODE.P5 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PEPUEN.P4 = 0;
    PAC55XX_SCC->PEPUEN.P5 = 1;

    PAC55XX_SCC->PEMUXSEL.w &= 0xFF00FFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PEMUXSEL.w |= 0x00550000;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTD_Tx <--> PF6
///     UARTD_Rx <--> PF7
///
//==============================================================================
void UARTD_IO_Select_PF67(void)
{
    PAC55XX_GPIOF->MODE.P6 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOF->MODE.P7 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PFPUEN.P6 = 0;
    PAC55XX_SCC->PFPUEN.P7 = 1;

    PAC55XX_SCC->PFMUXSEL.w &= 0x00FFFFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PFMUXSEL.w |= 0x55000000;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTD_Tx <--> PG2
///     UARTD_Rx <--> PG3
///
//==============================================================================
void UARTD_IO_Select_PG23(void)
{
    PAC55XX_GPIOG->MODE.P2 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOG->MODE.P3 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PGPUEN.P2 = 0;
    PAC55XX_SCC->PGPUEN.P3 = 1;

    PAC55XX_SCC->PGMUXSEL.w &= 0xFFFF00FF;                      // Clear Port Pin selection
    PAC55XX_SCC->PGMUXSEL.w |= 0x00005500;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     UARTD_Tx <--> PG5
///     UARTD_Rx <--> PG6
///
//==============================================================================
void UARTD_IO_Select_PG56(void)
{
    PAC55XX_GPIOG->MODE.P5 = IO_PUSH_PULL_OUTPUT;               // TX
    PAC55XX_GPIOG->MODE.P6 = IO_HIGH_IMPEDENCE_INPUT;           // RX

    PAC55XX_SCC->PGPUEN.P5 = 0;
    PAC55XX_SCC->PGPUEN.P6 = 1;

    PAC55XX_SCC->PGMUXSEL.w &= 0xF00FFFFF;                      // Clear Port Pin selection
    PAC55XX_SCC->PGMUXSEL.w |= 0x05500000;                      // Set Port Pin as UART
}

//==============================================================================
/// @brief
///     Choose the Uart and Enable the IO you want
///
///@param[in]
///     UART Type:
///         UARTA
///         UARTB
///         UARTC
///         UARTD
///
//==============================================================================
void uart_io_config(UART_TYPE uart)
{
    switch (uart)
    {
        case UARTA:
            // Select UART A peripheral choose one!
            UARTA_IO_Select_PA45();
            break;

        case UARTB:
            // Select UART B peripheral choose one!
//            UARTB_IO_Select_PA45();
//            UARTB_IO_Select_PC01();
//            UARTB_IO_Select_PC45();
//            UARTB_IO_Select_PE67();
            UARTB_IO_Select_PF23();
            break;

        case UARTC:
            // Select UART C peripheral choose one!
//            UARTC_IO_Select_PC23();
//            UARTC_IO_Select_PC67();
//            UARTC_IO_Select_PD23();
            UARTC_IO_Select_PE23();
            break;

        case UARTD:
            // Select UART D peripheral choose one!
            UARTD_IO_Select_PD45();
//            UARTD_IO_Select_PD67();
//            UARTD_IO_Select_PE45();
//            UARTD_IO_Select_PF67();
//            UARTD_IO_Select_PG23();
//            UARTD_IO_Select_PG56();
            break;

        default:
            break;
    }
}

//==============================================================================
/// @brief
///     enable the interrupt
///
///@param[in]
///     UART Type:
///         UARTA
///         UARTB
///         UARTC
///         UARTD
///
//==============================================================================
void uart_interrupt_enable(UART_TYPE uart)
{
    switch (uart)
    {
        case UARTA:
            NVIC_ClearPendingIRQ(USARTA_IRQn);
            NVIC_SetPriority(USARTA_IRQn, 3);
            NVIC_EnableIRQ(USARTA_IRQn);
            break;

        case UARTB:
            NVIC_ClearPendingIRQ(USARTB_IRQn);
            NVIC_SetPriority(USARTB_IRQn, 3);
            NVIC_EnableIRQ(USARTB_IRQn);
            break;

        case UARTC:
            NVIC_ClearPendingIRQ(USARTC_IRQn);
            NVIC_SetPriority(USARTC_IRQn, 3);
            NVIC_EnableIRQ(USARTC_IRQn);
            break;

        case UARTD:
            NVIC_ClearPendingIRQ(USARTD_IRQn);
            NVIC_SetPriority(USARTD_IRQn, 3);
            NVIC_EnableIRQ(USARTD_IRQn);
            break;

        default:
            break;
    }
}

//==============================================================================
///@brief
///     the whole uart setting function is here
///
///@param[in]
///     UART Type:
///         UARTA
///         UARTB
///         UARTC
///         UARTD
///     baudrate: The baudrate you want
///
//==============================================================================
void uart_init(UART_TYPE uart, uint32_t baudrate)
{
    PAC55XX_UART_TYPEDEF *uart_ptr;

    switch (uart)
    {
        case UARTA:
            uart_ptr = PAC55XX_UARTA;
            PAC55XX_SCC->CCSCTL.USAMODE = USART_MODE_UART;      // UART mode
            break;

        case UARTB:
            uart_ptr = PAC55XX_UARTB;
            PAC55XX_SCC->CCSCTL.USBMODE = USART_MODE_UART;      // UART mode
            break;

        case UARTC:
            uart_ptr = PAC55XX_UARTC;
            PAC55XX_SCC->CCSCTL.USCMODE = USART_MODE_UART;      // UART mode
            break;

        case UARTD:
            uart_ptr = PAC55XX_UARTD;
            PAC55XX_SCC->CCSCTL.USDMODE = USART_MODE_UART;      // UART mode
            break;

        default:
            uart_ptr = PAC55XX_UARTD;
            break;
    }

    pac5xxx_uart_config_LCR2(uart_ptr,
                             UARTLCR_WL_BPC_8,
                             UART_STOP_BITS_1,
                             UART_PEN_DISABLE,
                             UART_PARITY_FORCE_STICK_1,
                             UART_BRKCTL_DISABLE);

    pac5xxx_uart_config_divisor_latch2(uart_ptr, DF_UART_PCLK/16/baudrate);     // 115kbps = PCLK / (16///DLR)

    //rx FIFO set
    uart_ptr->FCR.FIFOEN = 1;
    uart_ptr->FCR.RXFIFORST = 1;
    pac5xxx_uart_rx_fifo_threshold2(uart_ptr, UARTFCR_TL_8B);
    pac5xxx_uart_tx_fifo_threshold2(uart_ptr, UARTFCR_TL_8B);

    pac5xxx_uart_int_enable_RDAI2(uart_ptr, UART_INT_ENABLE);
    // pac5xxx_uart_int_enable_THREI2(uart_ptr, UART_INT_ENABLE);

    uart_io_config(uart);
    uart_interrupt_enable(uart);
}

//==============================================================================
/// @brief
///     read a byte from UART manually
///
/// @param[in]
///      UART Type:
///         UARTA
///         UARTB
///         UARTC
///         UARTD
///     data: The data read back.
///
/// @param[out]
///     result: the result of the UART byte read
///
///@retval
///     0            All is OK.
///     others        Some error occurs.
///
//==============================================================================
uint32_t uart_read_one(UART_TYPE uart, uint8_t *data)
{
    uint32_t result = PAC5XXX_OK;
    uint32_t wait_tick = 0;

    PAC55XX_UART_TYPEDEF *uart_ptr;

    switch (uart)
    {
        case UARTA:
            uart_ptr = PAC55XX_UARTA;
            break;

        case UARTB:
            uart_ptr = PAC55XX_UARTB;
            break;

        case UARTC:
            uart_ptr = PAC55XX_UARTC;
            break;

        case UARTD:
            uart_ptr = PAC55XX_UARTD;
            break;

        default:
            uart_ptr = PAC55XX_UARTA;
            break;
    }

    while (uart_ptr->LSR.RDR == 0)
    {
        wait_tick++;
        if (wait_tick > DF_UART_BUSY_TICK)
        {
            result = PAC5XXX_ERROR;
            break;
        }
    }

    *data = (uint8_t)uart_ptr->RBR.RBR;

    return result;
}

//==============================================================================
///@brief
///     Write a byte to UART manually
///
///@param[in]
///     UART Type:
///         UARTA
///         UARTB
///         UARTC
///         UARTD
///     data: The data to write.
///
///@param[out]
///     result: the result of the UART byte write
///
///@retval
///     0            All is OK.
///     others        Some error occurs.
///
//==============================================================================
uint32_t uart_write_one(UART_TYPE uart, uint8_t data)
{
    uint32_t result = PAC5XXX_OK;
    uint32_t wait_tick = 0;

    PAC55XX_UART_TYPEDEF *uart_ptr;

    switch (uart)
    {
        case UARTA:
            uart_ptr = PAC55XX_UARTA;
            break;

        case UARTB:
            uart_ptr = PAC55XX_UARTB;
            break;

        case UARTC:
            uart_ptr = PAC55XX_UARTC;
            break;

        case UARTD:
            uart_ptr = PAC55XX_UARTD;
            break;

        default:
            uart_ptr = PAC55XX_UARTA;
            break;
    }

    uart_ptr->THR.THR = data;

    while (uart_ptr->LSR.TEMT == 0)
    {
        wait_tick++;
        if (wait_tick > DF_UART_BUSY_TICK)
        {
            result = PAC5XXX_ERROR;
            break;
        }
    }

    return result;
}

//==============================================================================
///@brief
///     Write a serial bytes to UART manually
///
///@param[in]
///     UART Type:
///         UARTA
///         UARTB
///         UARTC
///         UARTD
///     data_p: The data pointer to write.
///     byte_num: the number to write.
///
///@param[out]
///     result: the result of the UART byte write
///
///@retval
///     0            All is OK.
///     others        Some error occurs.
///
//==============================================================================
uint32_t uart_write_multi(UART_TYPE uart, uint8_t *data_p, uint32_t byte_num)
{
    uint8_t *data;
    data = data_p;

    uint32_t result = PAC5XXX_OK;
    uint32_t wait_tick = 0;
    uint32_t byte_num_temp;

    PAC55XX_UART_TYPEDEF *uart_ptr;

    switch (uart)
    {
        case UARTA:
            uart_ptr = PAC55XX_UARTA;
            break;

        case UARTB:
            uart_ptr = PAC55XX_UARTB;
            break;

        case UARTC:
            uart_ptr = PAC55XX_UARTC;
            break;

        case UARTD:
            uart_ptr = PAC55XX_UARTD;
            break;

        default:
            uart_ptr = PAC55XX_UARTA;
            break;
    }

    for (byte_num_temp=0; byte_num_temp<byte_num; byte_num_temp++)
    {
        uart_ptr->THR.THR = *data;
        data++;

        while (uart_ptr->LSR.TEMT == 0)
        {
            wait_tick++;
            if (wait_tick > DF_UART_BUSY_TICK)
            {
                result = PAC5XXX_ERROR;
                break;
            }
        }

        wait_tick = 0;
    }

    return result;
}
