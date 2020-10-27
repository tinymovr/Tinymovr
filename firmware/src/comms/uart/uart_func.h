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
 
#include <src/common.hpp>

#ifndef UART_FUNC_H
#define UART_FUNC_H

#define DF_isr_for_UART
#define DF_UART_PCLK            HCLK_FREQ_HZ
#define DF_UART_BUSY_TICK       (25000u)

extern volatile uint8_t uart_read_buf[50];
extern volatile uint8_t flag_uart_read_receive_done;
extern volatile uint8_t flag_uart_write_send_done;
extern volatile uint32_t uart_read_count;

extern void uart_init(UART_TYPE uart, uint32_t baudrate);
extern uint32_t uart_read_one(UART_TYPE uart, uint8_t *data);
extern uint32_t uart_write_one(UART_TYPE uart, uint8_t data);
extern uint32_t uart_write_multi(UART_TYPE uart, uint8_t *data, uint32_t byte_num);

#endif
