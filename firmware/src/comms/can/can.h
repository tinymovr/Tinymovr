
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

#ifndef CAN_CAN_H_
#define CAN_CAN_H_

#include <isotp/isotp.h>

#define CAN_EP_SIZE 6
#define ISOTP_RX_ADDR 0x3E
#define ISOTP_TX_ADDR 0x3F
#define ISOTP_BUFSIZE 512

struct CANConfig
{
    uint8_t id;
    uint8_t kbaud_rate;
};

void CAN_init(void);
uint16_t CAN_get_kbit_rate(void);
void CAN_set_kbit_rate(uint16_t rate);
uint8_t CAN_get_ID(void);
void CAN_set_ID(uint8_t id);
void CAN_process_interrupt(void);

struct CANConfig* CAN_get_config(void);
void CAN_restore_config(struct CANConfig* config_);
void CAN_poll(void);

#endif /* CAN_CAN_H_ */
