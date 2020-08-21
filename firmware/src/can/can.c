
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

#include "src/system/system.h"
#include "src/watchdog/watchdog.h"

#include "src/can/can_endpoints.h"
#include "src/can/can_func.h"
#include "src/can/can.h"

void CAN_ProcessMessage(uint8_t command_id, bool rtr);

static struct CANConfig config = {
    .id = 1,
    .kbaud_rate = CAN_BAUD_1000KHz
};

void CAN_Init(void)
{
    CANEP_InitEndpointMap();

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
    System_DelayUS(100);
}

uint16_t CAN_GetkBaudRate(void)
{
    return CAN_BaudTypeToInt(config.kbaud_rate);
}

void CAN_SetkBaudRate(uint16_t rate)
{
    config.kbaud_rate = CAN_IntToBaudType(rate);
}

uint8_t CAN_GetID(void)
{
    return config.id;
}

void CAN_SetID(uint8_t id)
{
    pac5xxx_can_reset_mode_set(1);  // CAN in reset mode, in order to configure CAN module
    config.id = id;
    //Filter bit7:0 => ID10:3
    //Filter bit15:13 => ID2:0
    PAC55XX_CAN->AMR = 0xFFFFFF87;
    PAC55XX_CAN->ACR = config.id << (CAN_EP_SIZE - 3);
    pac5xxx_can_reset_mode_set(0);  // CAN reset mode inactive
    System_DelayUS(100);
}

void CAN_ProcessMessage(uint8_t command_id, bool rtr)
{
    uint8_t (*callback)(uint8_t buffer[]) = CANEP_GetEndpoint(command_id);
    if (callback != NULL)
    {
        uint8_t can_msg_buffer[8] = {0};
        if ((rtr == false) && (rx_dataLen > 0u))
        {
            memcpy(can_msg_buffer, &rx_data, rx_dataLen);
        }
        uint8_t response_type = callback(can_msg_buffer);
        if (rtr && (response_type >= CANRP_Read))
        {
            can_transmit(8, (config.id << CAN_EP_SIZE) | command_id, can_msg_buffer);
        }
        Watchdog_Feed();
    }
    rx_flag = 0;
}

void CAN_IRQHandler(void)
{
    buffer = PAC55XX_CAN->RXBUF;	//  read RX buffer, RX buffer bit order same as TX buffer

    rx_dataLen = buffer & 0x0F;
    rx_id = ((buffer & 0xE00000) >> 21) | ((buffer & 0xFF00) >> 5);

    uint8_t command_id = rx_id & 0x3F;
    bool rtr = ((buffer >> 6) & 0x1) == 0x1;
    rx_data[0] = buffer>>24;    // data0
    if(rx_dataLen > 1u)
    {
        buffer = PAC55XX_CAN->RXBUF;    // buffer contains data1..data4
        rx_data[1] = buffer;
        rx_data[2] = buffer >> 8;
        rx_data[3] = buffer >> 16;
        rx_data[4] = buffer >> 24;
        if(rx_dataLen > 5u)
        {
            buffer = PAC55XX_CAN->RXBUF;    //  buffer contains data7..data5
            rx_data[5] = buffer;
            rx_data[6] = buffer >> 8;
            rx_data[7] = buffer >> 16;
        }
    }
    rx_flag = 1;
    CAN_ProcessMessage(command_id, rtr);

    pac5xxx_can_int_clear_RI(); // Clear RX interrupt
}

struct CANConfig* CAN_GetConfig(void)
{
    return &config;
}

void CAN_RestoreConfig(struct CANConfig* config_)
{
    config = *config_;
}

