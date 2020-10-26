
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

#include <src/component.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include <src/comms/can/can_func.h>

#ifdef __cplusplus
}
#endif

typedef uint8_t (*CANEP_Callback)(uint8_t *);

typedef enum {
    CANRP_NoAction = 0,
    CANRP_Write = 1,
    CANRP_Read = 2,
    CANRP_ReadWrite = 3
} CAN_ResponseType;

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


class CAN : public Component
{
public:
	CAN(System sys_);
	uint16_t GetkBaudRate(void);
	void SetkBaudRate(uint16_t rate);
	uint8_t GetID(void);
	void SetID(uint8_t id);
	void ProcessInterrupt(void);
private:
	struct Config_t
	{
	    uint8_t id = 1;
	    CAN_BAUD_TYPE kbaud_rate = CAN_BAUD_1000KHz;
	};
	Config_t config;
	void ProcessMessage(uint8_t command_id, bool rtr);
	void InitEndpointMap(void);
	void AddEndpoint(CANEP_Callback callback, uint8_t id);
	CANEP_Callback GetEndpoint(uint8_t id);

	uint8_t CAN_EStop(uint8_t buffer[]);
	uint8_t CAN_GetState(uint8_t buffer[]);

	uint8_t CAN_GetCANConfig(uint8_t buffer[]);
	uint8_t CAN_SetCANConfig(uint8_t buffer[]);
	uint8_t CAN_SetState(uint8_t buffer[]);

	uint8_t CAN_GetEncoderEstimates(uint8_t buffer[]);
	uint8_t CAN_GetSetpoints(uint8_t buffer[]);

	uint8_t CAN_SetPosSetpoint(uint8_t buffer[]);
	uint8_t CAN_SetVelSetpoint(uint8_t buffer[]);
	uint8_t CAN_SetIqSetpoint(uint8_t buffer[]);
	uint8_t CAN_SetLimits(uint8_t buffer[]);
	uint8_t CAN_GetPhaseCurrents(uint8_t buffer[]);

	uint8_t CAN_GetIq(uint8_t buffer[]);
	uint8_t CAN_GetLimits(uint8_t buffer[]);
	uint8_t CAN_Reset(uint8_t buffer[]);
	uint8_t CAN_GetVBus(uint8_t buffer[]);
	uint8_t CAN_GetGains(uint8_t buffer[]);
	uint8_t CAN_SetGains(uint8_t buffer[]);
	uint8_t CAN_DeviceInfo(uint8_t buffer[]);
	uint8_t CAN_Timings(uint8_t buffer[]);
	uint8_t CAN_SaveConfig(uint8_t buffer[]);
	uint8_t CAN_EraseConfig(uint8_t buffer[]);
	uint8_t CAN_GetMotorInfo(uint8_t buffer[]);
};

#endif /* CAN_CAN_H_ */
