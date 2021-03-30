
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

#define CAN_EP_SIZE 6

typedef enum {
    MSG_TINYMOVR_HEARTBEAT = 0x0001,

    // -- Setters
    MSG_TINYMOVR_ESTOP = 0x0040,
    MSG_SET_CTRL_STATE,
    MSG_SET_CTRL_MODE,
    MSG_SET_POS_SETPOINT,
    MSG_SET_VEL_SETPOINT,
    MSG_SET_IQ_SETPOINT,
    MSG_SET_POS_MIN_LIMIT,
    MSG_SET_POS_MAX_LIMIT,
    MSG_SET_VEL_LIMIT,
    MSG_SET_IQ_LIMIT,
    MSG_SET_POS_GAIN,
    MSG_SET_VEL_GAIN,
    MSG_SET_VEL_INT_GAIN,
	MSG_SET_CAN_ID,
    MSG_SET_CAN_KBAUD,
    MSG_SET_OBSERVER_BW,
    MSG_SET_IQ_BW,
    MSG_SET_WATCHDOG_ENABLED,
    MSG_SET_WATCHDOG_TIMEOUT,

    // -- Getters
    MSG_GET_CTRL_STATE = 0x0080,
    MSG_GET_CTRL_MODE,
    MSG_GET_POS_SETPOINT,
    MSG_GET_POS_ESTIMATE,
    MSG_GET_POS_OFFSET,
    MSG_GET_VEL_SETPOINT,
    MSG_GET_VEL_ESTIMATE,
    MSG_GET_IQ_SETPOINT,
    MSG_GET_IQ_ESTIMATE,
    MSG_GET_POS_MIN_LIMIT,
    MSG_GET_POS_MAX_LIMIT,
    MSG_GET_VEL_LIMIT,
    MSG_GET_IQ_LIMIT,
    MSG_GET_POS_GAIN,
    MSG_GET_VEL_GAIN,
    MSG_GET_VEL_INT_GAIN,
    MSG_GET_IQ_GAIN,
	MSG_GET_CALIBRATED,
    MSG_GET_VBUS,
	MSG_GET_CAN_ID,
    MSG_GET_CAN_KBAUD,
    MSG_GET_OBSERVER_BW,
    MSG_GET_IQ_BW,
    MSG_GET_WATCHDOG_ENABLED,
    MSG_GET_WATCHDOG_TIMEOUT,

    MSG_GET_ERROR = 0x00C0,
    MSG_SAVE_CONFIG,
    MSG_ERASE_CONFIG,
    MSG_RESET,
    
} CANMsgType;

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

#endif /* CAN_CAN_H_ */
