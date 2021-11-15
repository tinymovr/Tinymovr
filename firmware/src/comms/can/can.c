
//  * This file is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * Copyright (c) 2020 Ioannis Chatzikonstantinou.
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

#include "string.h"

#include <src/system/system.h>
#include <src/comms/defs.h>
#include <src/comms/can/can_func.h>
#include <src/comms/can/can.h>

/* Alloc IsoTpLink statically in RAM */
static IsoTpLink g_link;

/* Alloc send and receive buffer statically in RAM */
static uint8_t g_isotpRecvBuf[ISOTP_BUFSIZE];
static uint8_t g_isotpSendBuf[ISOTP_BUFSIZE];

static struct CANConfig config = {
    .id = 1,
    .kbaud_rate = CAN_BAUD_1000KHz
};

extern Server s;

void CAN_init(void)
{

    if (system_board_revision() == BOARD_REV_5)
    {
		// Configure PF4 as GPIO input with pulldown
		PAC55XX_GPIOF->MODE.P4 = IO_HIGH_IMPEDENCE_INPUT;
		PAC55XX_GPIOF->OUTMASK.P4 = 1;
		PAC55XX_SCC->PFMUXSEL.P4 = 0;
		PAC55XX_SCC->PFPDEN.P4 = 1;
	}
	else
	{
		// Configure PF4 as GPIO input
		PAC55XX_GPIOF->MODE.P4 = IO_HIGH_IMPEDENCE_INPUT;
		PAC55XX_GPIOF->OUTMASK.P4 = 1;
		PAC55XX_SCC->PFMUXSEL.P4 = 0;
		PAC55XX_SCC->PFPUEN.P4 = 1;

		// Configure PE1 as a GPIO output
		PAC55XX_GPIOE->MODE.P1 = IO_PUSH_PULL_OUTPUT;   // PE1 GPIO configured as an output
		PAC55XX_SCC->PEMUXSEL.w &= 0xFFFFFF0F;          // Clear bits to select GPIO for PE1 via PE mux select
		PAC55XX_GPIOE->OUT.P1 = 1;                      // Set PE1 high to force transceiver into normal mode

		// Wait for CAN transceiver to enter normal mode
		while (PAC55XX_GPIOF->IN.P4 == 0)
		{
			// No action
		}
	}

    // CAN Bus initialization follows
    can_io_config(CAN_PE23);

    pac5xxx_can_reset_mode_set(1);	// CAN in reset mode, in order to configure CAN module
    PAC55XX_CAN->MR.AFM = 1;		// Single filter scheme

    // This below ensures a valid value is always assigned
    can_baud(CAN_IntToBaudType(CAN_BaudTypeToInt(config.kbaud_rate)));

    PAC55XX_CAN->BTR0.SJW = 1;		// Synchronization jump width
    PAC55XX_CAN->BTR1.SAM = 0;		// Bus is sampled once 

    //Filter bit7:0 => ID10:3
    //Filter bit15:13 => ID2:0
    PAC55XX_CAN->AMR = 0xFFFFFF87;
    PAC55XX_CAN->ACR = config.id << (CAN_EP_SIZE - 3);

    //PAC55XX_CAN->IMR.TIM = 1;		// Transmit Interrupt
    PAC55XX_CAN->IMR.RIM = 1;		// Receive Interrupt
    NVIC_SetPriority(CAN_IRQn, 3);
    NVIC_EnableIRQ(CAN_IRQn);

    pac5xxx_can_reset_mode_set(0);	// CAN reset mode inactive
    system_delay_us(100);
}

uint16_t CAN_get_kbit_rate(void)
{
    return CAN_BaudTypeToInt(config.kbaud_rate);
}

void CAN_set_kbit_rate(uint16_t rate)
{
    config.kbaud_rate = CAN_IntToBaudType(rate);
}

uint8_t CAN_get_ID(void)
{
    return config.id;
}

void CAN_set_ID(uint8_t id)
{
    pac5xxx_can_reset_mode_set(1);  // CAN in reset mode, in order to configure CAN module
    config.id = id;
    //Filter bit7:0 => ID10:3
    //Filter bit15:13 => ID2:0
    PAC55XX_CAN->AMR = 0xFFFFFF87;
    PAC55XX_CAN->ACR = config.id << (CAN_EP_SIZE - 3);
    pac5xxx_can_reset_mode_set(0);  // CAN reset mode inactive
    system_delay_us(100);
}

void CAN_process_interrupt(void)
{
    buffer = PAC55XX_CAN->RXBUF;	//  read RX buffer, RX buffer bit order same as TX buffer

    data_length = buffer & 0x0F;
    rx_id = ((buffer & 0xE00000) >> 21) | ((buffer & 0xFF00) >> 5);

    //uint8_t command_id = rx_id & 0x3F;
    //bool rtr = ((buffer >> 6) & 0x1) == 0x1;
    rx_data[0] = buffer>>24;    // data0
    if(data_length > 1u)
    {
        buffer = PAC55XX_CAN->RXBUF;    // buffer contains data1..data4
        rx_data[1] = buffer;
        rx_data[2] = buffer >> 8;
        rx_data[3] = buffer >> 16;
        rx_data[4] = buffer >> 24;
        if(data_length > 5u)
        {
            buffer = PAC55XX_CAN->RXBUF;    //  buffer contains data7..data5
            rx_data[5] = buffer;
            rx_data[6] = buffer >> 8;
            rx_data[7] = buffer >> 16;
        }
    }
    
    isotp_on_can_message(&g_link, rx_data, data_length);

    /* Poll link to handle multiple frame transmition */
    isotp_poll(&g_link);
    
    /* You can receive message with isotp_receive.
        payload is upper layer message buffer, usually UDS;
        payload_size is payload buffer size;
        out_size is the actuall read size;
        */
    uint16_t n_received;
    int ret = isotp_receive(&g_link, g_isotpRecvBuf, ISOTP_BUFSIZE, &n_received);
    if (ISOTP_RET_OK == ret) {
        /* Handle received message */
        const size_t response_size = handle(&s, g_isotpRecvBuf, g_isotpSendBuf);
        if (response_size > 0) {
            /* In case you want to send data w/ functional addressing, use isotp_send_with_id */
            ret = isotp_send_with_id(&g_link, 0x7df, g_isotpSendBuf, response_size);
            if (ISOTP_RET_OK == ret) {
                /* Send ok */
            } else {
                /* Error occur */
            }
        }
    }
}

struct CANConfig* CAN_get_config(void)
{
    return &config;
}

void CAN_restore_config(struct CANConfig* config_)
{
    config = *config_;
}

// ISO-TP-C shims
int isotp_user_send_can(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size)
{
    //can_transmit(size, (this->config.id << CAN_EP_SIZE) | command_id, can_msg_buffer);
    can_transmit(size, arbitration_id, data);
    return ISOTP_RET_OK;
}

// TODO: Ya.. could be better
extern volatile uint32_t msTicks;

uint32_t isotp_user_get_ms(void)
{
    return msTicks;
}

/* optional, provide to receive debugging log messages */
void isotp_user_debug(const char* message, ...) {
    // ...
}

void CAN_poll(void) {
    isotp_poll(&g_link);
}