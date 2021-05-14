
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

#include "src/common.h"

typedef uint8_t (*CANEP_Callback)(uint8_t *);

typedef enum {
    CANRP_NoAction = 0,
    CANRP_Write = 1,
    CANRP_Read = 2,
    CANRP_ReadWrite = 3
} CAN_ResponseType;

void CANEP_InitEndpointMap(void);
void CANEP_AddEndpoint(CANEP_Callback callback, uint8_t id);
CANEP_Callback CANEP_GetEndpoint(uint8_t id);

uint8_t CAN_GetOffsetAndDirection(uint8_t buffer[]);
uint8_t CAN_GetState(uint8_t buffer[]);
uint8_t CAN_GetMinStudioVersion(uint8_t buffer[]);
uint8_t CAN_GetCANConfig(uint8_t buffer[]);
uint8_t CAN_SetCANConfig(uint8_t buffer[]);
uint8_t CAN_SetState(uint8_t buffer[]);
uint8_t CAN_SetOffsetAndDirection(uint8_t buffer[]);
uint8_t CAN_GetEncoderEstimates(uint8_t buffer[]);
uint8_t CAN_GetSetpoints(uint8_t buffer[]);

uint8_t CAN_MoveToPos(uint8_t buffer[]);

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
uint8_t CAN_GetIntegratorGains(uint8_t buffer[]);
uint8_t CAN_SetIntegratorGains(uint8_t buffer[]);
uint8_t CAN_DeviceInfo(uint8_t buffer[]);
uint8_t CAN_Timings(uint8_t buffer[]);
uint8_t CAN_SaveConfig(uint8_t buffer[]);
uint8_t CAN_EraseConfig(uint8_t buffer[]);
uint8_t CAN_GetMotorConfig(uint8_t buffer[]);
uint8_t CAN_SetMotorConfig(uint8_t buffer[]);
