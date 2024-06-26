
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

#pragma once

typedef struct 
{
    uint8_t id;
    uint8_t kbaud_rate;
    uint16_t heartbeat_period_ms;
    uint16_t can_reset_period_ms;
} CANConfig;

typedef struct 
{
    uint8_t faults;
    uint32_t last_msg_ms;
    uint32_t can_reset_counter_ms;
    bool send_heartbeat;
} CANState;

void CAN_init(void);
uint16_t CAN_get_kbit_rate(void);
void CAN_set_kbit_rate(uint16_t rate);
uint8_t CAN_get_ID(void);
void CAN_set_ID(uint8_t id);
void CAN_process_interrupt(void);

bool CAN_get_send_heartbeat(void);
void CAN_set_send_heartbeat(bool value);

CANConfig *CAN_get_config(void);
void CAN_restore_config(CANConfig *config_);

void CAN_update(void);
