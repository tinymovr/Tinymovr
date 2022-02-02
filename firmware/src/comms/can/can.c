
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

#include <string.h>
#include <src/common.h>
#include <src/utils/utils.h>

#include <src/comms/defs.h>
#include <src/comms/can/can_func.h>
#include <src/comms/can/can.h>

/* Alloc IsoTpLink statically in RAM */
static IsoTpLink g_link;

/* Alloc send and receive buffer statically in RAM */
static uint8_t isotp_rx_buffer[ISOTP_RX_BUFSIZE];
static uint8_t isotp_tx_buffer[ISOTP_TX_BUFSIZE];

static const uint8_t heartbeat_byte = 0;

static CANConfig config = {
    .id = 1,
    .kbaud_rate = CAN_BAUD_1000KHz,
    .heartbeat_period = 1000
};

static CANState state ={0};

void CAN_init(void)
{
    isotp_init_link(&g_link, (config.id << CAN_EP_SIZE) | ISOTP_TX_ADDR,
						isotp_tx_buffer, ISOTP_TX_BUFSIZE, 
						isotp_rx_buffer, ISOTP_RX_BUFSIZE);

#if defined(BOARD_REV_T5)
    // Configure PD7 as GPIO output
    PAC55XX_GPIOD->MODE.P7 = IO_PUSH_PULL_OUTPUT;   // GPIO configured as an output
    PAC55XX_SCC->PDMUXSEL.w &= 0xFFFFFF0F;          // Clear bits to select GPIO function
    PAC55XX_GPIOD->OUT.P7 = 0;                      // Set high to force transceiver into normal mode
#elif defined(BOARD_REV_R3)
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
#endif

    can_io_config();

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
    pac5xxx_can_reset_mode_set(1);  // CAN in reset mode, in order to configure CAN module
    config.id = id;
    //Filter bit7:0 => ID10:3
    //Filter bit15:13 => ID2:0
    PAC55XX_CAN->AMR = 0xFFFFFF87;
    PAC55XX_CAN->ACR = config.id << (CAN_EP_SIZE - 3);
    pac5xxx_can_reset_mode_set(0);  // CAN reset mode inactive
    delay_us(100);
    isotp_init_link(&g_link, (config.id << CAN_EP_SIZE) | ISOTP_TX_ADDR,
						isotp_tx_buffer, ISOTP_TX_BUFSIZE, 
						isotp_rx_buffer, ISOTP_RX_BUFSIZE);
}

void CAN_process_interrupt(void)
{
    buffer = PAC55XX_CAN->RXBUF;	//  read RX buffer, RX buffer bit order same as TX buffer

    data_length = buffer & 0x0F;
    rx_id = ((buffer & 0xE00000) >> 21) | ((buffer & 0xFF00) >> 5);

    uint8_t command_id = rx_id & 0x3F;
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
    
    if (ISOTP_RX_ADDR == command_id) {
        isotp_on_can_message(&g_link, rx_data, data_length);
        /* Poll link to handle multiple frame transmission */
        isotp_poll(&g_link);
        
        if (ISOTP_RET_OK == isotp_full(&g_link))
        {
            uint8_t rx_payload[ISOTP_RX_BUFSIZE];
            uint16_t rx_payload_size = 0;
            int ret = isotp_receive(&g_link, rx_payload, 128, &rx_payload_size);
            if (ISOTP_RET_OK == ret)
            {
                /* Handle received message */
                const size_t tx_payload_size = handle_message(rx_payload, rx_payload_size, isotp_tx_buffer);
                if (tx_payload_size > 0)
                {
                    ret = isotp_send(&g_link, isotp_tx_buffer, tx_payload_size);
                    if (ISOTP_RET_OK != ret) 
                    {
                        state.faults |= CAN_FLT_ISOTP_TX_ERROR;
                    }
                }
            }
            else
            {
                state.faults |= CAN_FLT_ISOTP_RX_ERROR;
            }
        }
    }
}

CANConfig* CAN_get_config(void)
{
    return &config;
}

void CAN_restore_config(CANConfig* config_)
{
    config = *config_;
}

// TODO: Ya.. could be better
extern volatile uint32_t msTicks;

// ISO-TP-C shims
int isotp_user_send_can(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size)
{
    //can_transmit(size, (this->config.id << CAN_EP_SIZE) | command_id, can_msg_buffer);
    can_transmit(size, arbitration_id, data);
    state.last_msg_ms = msTicks;
    return ISOTP_RET_OK;
}

uint32_t isotp_user_get_ms(void)
{
    return msTicks;
}

/* optional, provide to receive debugging log messages */
void isotp_user_debug(const char* message, ...) {
    // ...
}

void CAN_task(void) {
    isotp_poll(&g_link);
    // Transmit heartbeat
    const uint32_t msg_diff = msTicks - state.last_msg_ms;
    if (msg_diff >= config.heartbeat_period &&
        ISOTP_SEND_STATUS_INPROGRESS != g_link.send_status &&
        ISOTP_RECEIVE_STATUS_INPROGRESS != g_link.receive_status &&
        PAC55XX_CAN->SR.TBS != 0)
    {
        state.last_msg_ms = msTicks;
        can_transmit(1, 0x700 | config.id, &heartbeat_byte);
    }
}

PAC5XXX_RAMFUNC uint8_t can_get_faults(void)
{
    return state.faults;
}