//  * This file is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * Copyright (c) 2020-2023 Ioannis Chatzikonstantinou.
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

#include <string.h>

#include <src/utils/utils.h>
#include <src/watchdog/watchdog.h>
#include <src/can/can_endpoints.h>
#include <src/can/can_func.h>
#include <src/can/can.h>

#if defined BOARD_REV_R3 
#define CAN_BUS_PINS CAN_PE23
#elif defined BOARD_REV_R5
#define CAN_BUS_PINS CAN_PF67
#elif defined BOARD_REV_M5
#define CAN_BUS_PINS CAN_PD56
#endif

static CANConfig config = {
    .id = 1,
    .kbaud_rate = CAN_BAUD_1000KHz,
    .heartbeat_period = 1000};

static CANState state ={0};

extern volatile uint32_t msTicks;

const uint8_t avlos_proto_hash_8 = (uint8_t)(avlos_proto_hash & 0xFF);
const size_t endpoint_count = sizeof(avlos_endpoints) / sizeof(avlos_endpoints[0]);

void CAN_init(void)
{
#if defined(BOARD_REV_R52)
    PAC55XX_SCC->PDMUXSEL.w &= 0xFFFFFF0F;        // Clear bits to select GPIO function
    PAC55XX_GPIOD->MODE.P7 = IO_PUSH_PULL_OUTPUT; // GPIO configured as an output
    PAC55XX_GPIOD->OUT.P7 = 0;                    // Set low to force transceiver into normal mode
    PAC55XX_GPIOD->MODE.P6 = IO_PUSH_PULL_OUTPUT; // GPIO configured as an output
    PAC55XX_GPIOD->OUT.P6 = 1;                    // Set high to set IO voltage to 3V3
#elif defined(BOARD_REV_R5)
    // Configure PD7 as GPIO output
    PAC55XX_GPIOD->MODE.P7 = IO_PUSH_PULL_OUTPUT; // GPIO configured as an output
    PAC55XX_SCC->PDMUXSEL.w &= 0xFFFFFF0F;        // Clear bits to select GPIO function
    PAC55XX_GPIOD->OUT.P7 = 0;                    // Set low to force transceiver into normal mode
#elif defined(BOARD_REV_R3)
    // Configure PF4 as GPIO input
    PAC55XX_GPIOF->MODE.P4 = IO_HIGH_IMPEDENCE_INPUT;
    PAC55XX_GPIOF->OUTMASK.P4 = 1;
    PAC55XX_SCC->PFMUXSEL.P4 = 0;
    PAC55XX_SCC->PFPUEN.P4 = 1;

    // Configure PE1 as a GPIO output
    PAC55XX_GPIOE->MODE.P1 = IO_PUSH_PULL_OUTPUT; // PE1 GPIO configured as an output
    PAC55XX_SCC->PEMUXSEL.w &= 0xFFFFFF0F;        // Clear bits to select GPIO for PE1 via PE mux select
    PAC55XX_GPIOE->OUT.P1 = 1;                    // Set PE1 high to force transceiver into normal mode

    // Wait for CAN transceiver to enter normal mode
    while (PAC55XX_GPIOF->IN.P4 == 0)
    {
        // No action
    }
#elif defined(BOARD_REV_M5)
    PAC55XX_SCC->PDMUXSEL.w &= 0xFFFFFF0F;        // Clear bits to select GPIO function
    PAC55XX_GPIOD->MODE.P7 = IO_PUSH_PULL_OUTPUT; // GPIO configured as an output
    PAC55XX_GPIOD->OUT.P7 = 1;                    // Set high to set IO voltage to 3V3
    PAC55XX_GPIOD->MODE.P4 = IO_PUSH_PULL_OUTPUT; // GPIO configured as an output
    PAC55XX_GPIOD->OUT.P4 = 0;                    // Set low to force transceiver into normal mode

#endif

    can_io_config(CAN_BUS_PINS);

    pac5xxx_can_reset_mode_set(1); // CAN in reset mode, in order to configure CAN module
    PAC55XX_CAN->MR.AFM = 1;       // Single filter scheme

    // This below ensures a valid value is always assigned
    can_baud(CAN_IntToBaudType(CAN_BaudTypeToInt(config.kbaud_rate)));

    PAC55XX_CAN->BTR0.SJW = 1; // Synchronization jump width
    PAC55XX_CAN->BTR1.SAM = 0; // Bus is sampled once

    // PAC55XX_CAN->AMR = 0xFFFFFF87;
    //PAC55XX_CAN->AMR = 0xFFFFFFFF;
    // 11111111111111110000111100000000
    PAC55XX_CAN->AMR = 0xFFFFFF00;
    PAC55XX_CAN->ACR = config.id & 0xFF; // for now we only use 8 bit identifier

    // PAC55XX_CAN->IMR.TIM = 1; // Transmit Interrupt
    PAC55XX_CAN->IMR.RIM = 1; // Receive Interrupt
    NVIC_SetPriority(CAN_IRQn, 3);
    NVIC_EnableIRQ(CAN_IRQn);

    pac5xxx_can_reset_mode_set(0); // CAN reset mode inactive
    delay_us(100);
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
    if (id >= 1)
    {
        pac5xxx_can_reset_mode_set(1); // CAN in reset mode, in order to configure CAN module
        config.id = id;
        PAC55XX_CAN->ACR = config.id & 0xFF; // for now we only use 8 bit identifier
        pac5xxx_can_reset_mode_set(0); // CAN reset mode inactive
        delay_us(100);
    }
}

void CAN_process_interrupt(void)
{
    can_process_extended();

    if ((endpoint_count > can_ep_id) && 
       ((can_frame_hash == avlos_proto_hash_8) || (can_frame_hash == 0)))
    {
        uint8_t (*callback)(uint8_t buffer[], uint8_t * buffer_length, Avlos_Command cmd) = avlos_endpoints[can_ep_id];
        uint8_t can_msg_buffer[8];
        memcpy(can_msg_buffer, &rx_data, data_length);
        data_length = 0;
        uint8_t response_type = callback(can_msg_buffer, &data_length, (uint8_t)rtr);
        if ((AVLOS_RET_READ == response_type || AVLOS_RET_CALL == response_type) && (data_length > 0))
        {
            state.last_msg_ms = msTicks;
            can_transmit_extended(data_length, rx_id, can_msg_buffer);
        }
    }
    Watchdog_reset();
}

CANConfig *CAN_get_config(void)
{
    return &config;
}

void CAN_restore_config(CANConfig *config_)
{
    config = *config_;
}

void CAN_update(void) {
    // Transmit heartbeat
    const uint32_t msg_diff = msTicks - state.last_msg_ms;
    if (msg_diff >= config.heartbeat_period && PAC55XX_CAN->SR.TBS != 0)
    {
        state.last_msg_ms = msTicks;
        uint32_t proto_hash = _avlos_get_proto_hash();
        uint8_t buf[8];
        memcpy(buf, &proto_hash, sizeof(proto_hash));
        memcpy((buf+sizeof(proto_hash)), GIT_VERSION, 4);
        can_transmit_extended(sizeof(proto_hash)+4, 0x700 | config.id, buf);
    }
}
