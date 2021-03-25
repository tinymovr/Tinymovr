
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
#include <src/encoder/encoder.h>
#include "string.h"

#include "src/system/system.h"
#include "src/adc/adc.h"
#include "src/observer/observer.h"
#include "src/controller/controller.h"
#include "src/scheduler/scheduler.h"
#include "src/nvm/nvm.h"
#include "src/watchdog/watchdog.h"

#include "src/can/can_endpoints.h"

#define EP_LIST_SIZE 64
#define EP_MAP_SIZE 256

CANEP_Callback EPList[EP_LIST_SIZE];
uint8_t currentIndex = 0;
uint8_t EPMap[EP_MAP_SIZE] = {0};

void CANEP_InitEndpointMap(void)
{
    (void)memset(EPList, '\0', sizeof(EPList));

    CANEP_AddEndpoint(&CAN_EStop, 0x002);
    CANEP_AddEndpoint(&CAN_GetState, 0x003);
    CANEP_AddEndpoint(&CAN_GetMinStudioVersion, 0x004);
    CANEP_AddEndpoint(&CAN_GetCANConfig, 0x005);
    CANEP_AddEndpoint(&CAN_SetCANConfig, 0x006);
    CANEP_AddEndpoint(&CAN_SetState, 0x007);
    // 0x008 AVAIL
    CANEP_AddEndpoint(&CAN_GetEncoderEstimates, 0x009);
    CANEP_AddEndpoint(&CAN_GetSetpoints, 0x00A);
    // 0x00B Reserved: Move To Pos
    CANEP_AddEndpoint(&CAN_SetPosSetpoint, 0x00C);
    CANEP_AddEndpoint(&CAN_SetVelSetpoint, 0x00D);
    CANEP_AddEndpoint(&CAN_SetIqSetpoint, 0x00E);
    CANEP_AddEndpoint(&CAN_SetLimits, 0x00F);
    CANEP_AddEndpoint(&CAN_GetPhaseCurrents, 0x010);
    // 0x011 Reserved: Move To Pos
    CANEP_AddEndpoint(&CAN_GetIntegratorGains, 0x012);
    CANEP_AddEndpoint(&CAN_SetIntegratorGains, 0x013);
    CANEP_AddEndpoint(&CAN_GetIq, 0x014);
    CANEP_AddEndpoint(&CAN_GetLimits, 0x015);
    CANEP_AddEndpoint(&CAN_Reset, 0x016);
    CANEP_AddEndpoint(&CAN_GetVBus, 0x017);
    CANEP_AddEndpoint(&CAN_GetGains, 0x018);
    CANEP_AddEndpoint(&CAN_SetGains, 0x019);
    CANEP_AddEndpoint(&CAN_DeviceInfo, 0x01A);
    CANEP_AddEndpoint(&CAN_Timings, 0x01B);
    CANEP_AddEndpoint(&CAN_SaveConfig, 0x01C);
    CANEP_AddEndpoint(&CAN_EraseConfig, 0x01D);
    CANEP_AddEndpoint(&CAN_GetMotorConfig, 0x01E);
    CANEP_AddEndpoint(&CAN_SetMotorConfig, 0x01F);
}

void CANEP_AddEndpoint(CANEP_Callback callback, uint8_t id)
{
    if (EPMap[id] == 0u)
    {
        currentIndex += 1u;
        EPMap[id] = currentIndex;
        EPList[currentIndex] = callback;
    }
}

CANEP_Callback CANEP_GetEndpoint(uint8_t id)
{
    CANEP_Callback cb = NULL;
    if (EPMap[id] != 0u)
    {
        cb = EPList[EPMap[id]];
    }
    return cb;
}

// Endpoint handlers

uint8_t CAN_EStop(uint8_t buffer[])
{
    Controller_SetState(STATE_IDLE);
    return CANRP_Write;
}

uint8_t CAN_GetState(uint8_t buffer[])
{
    uint8_t state = Controller_GetState();
    uint8_t mode = Controller_GetMode();
    uint8_t *error_flags = get_error_flags();
    //memcpy(&buffer[0], &error, sizeof(uint8_t));
    memcpy(&buffer[1], &state, sizeof(uint8_t));
    memcpy(&buffer[2], &mode, sizeof(uint8_t));
    memcpy(&buffer[3], error_flags, sizeof(uint8_t) * ERROR_FLAG_MAX_SIZE);
    return CANRP_Read;
}

uint8_t CAN_GetMinStudioVersion(uint8_t buffer[])
{
    static const uint8_t v_major = STUDIO_MIN_VERSION_MAJOR;
    static const uint8_t v_minor = STUDIO_MIN_VERSION_MINOR;
    static const uint8_t v_patch = STUDIO_MIN_VERSION_PATCH;
    memcpy(&buffer[0], &v_major, sizeof(uint8_t));
    memcpy(&buffer[1], &v_minor, sizeof(uint8_t));
    memcpy(&buffer[2], &v_patch, sizeof(uint8_t));
}

uint8_t CAN_GetCANConfig(uint8_t buffer[])
{
    uint8_t id = CAN_GetID();
    uint16_t baudrate = CAN_GetkBaudRate();
    memcpy(&buffer[0], &id, sizeof(uint8_t));
    memcpy(&buffer[1], &baudrate, sizeof(uint16_t));
    return CANRP_Read;
}

uint8_t CAN_SetCANConfig(uint8_t buffer[])
{
    uint8_t id;
    uint16_t baudrate;
    memcpy(&id, &buffer[0], sizeof(uint8_t));
    memcpy(&baudrate, &buffer[1], sizeof(uint16_t));
    CAN_ResponseType response = CANRP_NoAction;
    if (id >= 1u)
    {
        CAN_SetID(id);
        response = CANRP_Write;
    }
    if ((baudrate >= 50u) && (baudrate <= 1000u))
    {
        CAN_SetkBaudRate(baudrate);
        response = CANRP_Write;
    }
    return response;
}

uint8_t CAN_SetState(uint8_t buffer[])
{
    uint8_t requested_state;
    uint8_t requested_mode;
    memcpy(&requested_state, &buffer[0], sizeof(uint8_t));
    memcpy(&requested_mode, &buffer[1], sizeof(uint8_t));
    CAN_ResponseType response = CANRP_NoAction;
    if (requested_state <= STATE_CL_CONTROL)
    {
        Controller_SetState(requested_state);
        response = CANRP_Write;
    }
    if (requested_mode <= CTRL_POSITION)
    {
        Controller_SetMode(requested_mode);
        response = CANRP_Write;
    }
    return response;
}

uint8_t CAN_GetEncoderEstimates(uint8_t buffer[])
{
    float pos = Observer_GetPosEstimate();
    float vel = Observer_GetVelEstimate();
    memcpy(&buffer[0], &pos, sizeof(float));
    memcpy(&buffer[4], &vel, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_GetSetpoints(uint8_t buffer[])
{
    float pos = Controller_GetPosSetpoint();
    float vel = Controller_GetVelSetpoint();
    memcpy(&buffer[0], &pos, sizeof(float));
    memcpy(&buffer[4], &vel, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_SetPosSetpoint(uint8_t buffer[])
{
    float pos;
    int16_t vel_ff;
    int16_t Iq_ff;
    memcpy(&pos, &buffer[0], sizeof(float));
    memcpy(&vel_ff, &buffer[4], sizeof(int16_t));
    memcpy(&Iq_ff, &buffer[6], sizeof(int16_t));
    float velFF_float = vel_ff * 10.0f;
    float iqFF_float = Iq_ff * 0.01f;
    Controller_SetPosSetpoint(pos);
    Controller_SetVelSetpoint(velFF_float);
    Controller_SetIqSetpoint(iqFF_float);
    return CANRP_Write;
}

uint8_t CAN_SetVelSetpoint(uint8_t buffer[])
{
    float vel;
    float Iq_ff;
    memcpy(&vel, &buffer[0], sizeof(float));
    memcpy(&Iq_ff, &buffer[4], sizeof(float));
    Controller_SetVelSetpoint(vel);
    Controller_SetIqSetpoint(Iq_ff);
    return CANRP_Write;
}

uint8_t CAN_SetIqSetpoint(uint8_t buffer[])
{
    float Iq;
    memcpy(&Iq, &buffer[0], sizeof(float));
    Controller_SetIqSetpoint(Iq);
    return CANRP_Write;
}

uint8_t CAN_GetLimits(uint8_t buffer[])
{
    float vel_limit = Controller_GetVelLimit();
    float iq_limit = Controller_GetIqLimit();
    memcpy(&buffer[0], &vel_limit, sizeof(float));
    memcpy(&buffer[4], &iq_limit, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_SetLimits(uint8_t buffer[])
{
    float vel_limit;
    float iq_limit;
    memcpy(&vel_limit, &buffer[0], sizeof(float));
    memcpy(&iq_limit, &buffer[4], sizeof(float));
    Controller_SetVelLimit(vel_limit);
    Controller_SetIqLimit(iq_limit);
    return CANRP_Write;
}

uint8_t CAN_GetPhaseCurrents(uint8_t buffer[])
{
    struct FloatTriplet I_phase;
    ADC_GetPhaseCurrents(&I_phase);
    int16_t IA = (int16_t)(I_phase.A * 1000.0f);
    int16_t IB = (int16_t)(I_phase.B * 1000.0f);
    int16_t IC = (int16_t)(I_phase.C * 1000.0f);
    memcpy(&buffer[0], &IA, sizeof(int16_t));
    memcpy(&buffer[2], &IB, sizeof(int16_t));
    memcpy(&buffer[4], &IC, sizeof(int16_t));
    return CANRP_Read;
}

uint8_t CAN_GetIq(uint8_t buffer[])
{
    float Iq_set = Controller_GetIqSetpoint();
    float Iq_est = Controller_GetIqEstimate();
    memcpy(&buffer[0], &Iq_set, sizeof(float));
    memcpy(&buffer[4], &Iq_est, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_Reset(uint8_t buffer[])
{
    system_reset();
    return CANRP_Read;
}

uint8_t CAN_GetVBus(uint8_t buffer[])
{
    const float VBus = ADC_GetVBus();
    memcpy(&buffer[0], &VBus, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_GetGains(uint8_t buffer[])
{
    const float pos_gain = Controller_GetPosGain();
    const float vel_P_gain = Controller_GetVelGain();
    memcpy(&buffer[0], &pos_gain, sizeof(float));
    memcpy(&buffer[4], &vel_P_gain, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_SetGains(uint8_t buffer[])
{
    float pos_gain;
    float vel_P_gain;
    memcpy(&pos_gain, &buffer[0], sizeof(float));
    memcpy(&vel_P_gain, &buffer[4], sizeof(float));
    CAN_ResponseType response = CANRP_NoAction;
    if (pos_gain > 0.0f)
    {
        Controller_SetPosGain(pos_gain);
        response = CANRP_Write;
    }
    if (vel_P_gain > 0.0f)
    {
        Controller_SetVelGain(vel_P_gain);
        response = CANRP_Write;
    }
    return response;
}

uint8_t CAN_GetIntegratorGains(uint8_t buffer[])
{
    const float vel_I_gain = Controller_GetVelIntegratorGain();
    memcpy(&buffer[0], &vel_I_gain, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_SetIntegratorGains(uint8_t buffer[])
{
    float vel_I_gain;
    memcpy(&vel_I_gain, &buffer[0], sizeof(float));
    CAN_ResponseType response = CANRP_NoAction;
    if (vel_I_gain >= 0.0f)
    {
        Controller_SetVelIntegratorGain(vel_I_gain);
        response = CANRP_Write;
    }
    return response;
}

uint8_t CAN_DeviceInfo(uint8_t buffer[])
{
    const uint32_t idr = PAC55XX_INFO2->PACIDR;
    static const uint8_t v_major = VERSION_MAJOR;
    static const uint8_t v_minor = VERSION_MINOR;
    static const uint8_t v_patch = VERSION_PATCH;
    const int8_t temp = (int8_t)ADC_GetMCUTemp();
    memcpy(&buffer[0], &idr, sizeof(uint32_t));
    memcpy(&buffer[4], &v_major, sizeof(uint8_t));
    memcpy(&buffer[5], &v_minor, sizeof(uint8_t));
    memcpy(&buffer[6], &v_patch, sizeof(uint8_t));
    memcpy(&buffer[7], &temp, sizeof(int8_t));
    return CANRP_Read;
}

uint8_t CAN_Timings(uint8_t buffer[])
{
    const uint32_t total = Scheduler_GetTotalCycles();
    const uint32_t busy = Scheduler_GetBusyCycles();
    memcpy(&buffer[0], &total, sizeof(uint32_t));
    memcpy(&buffer[4], &busy, sizeof(uint32_t));
    return CANRP_Read;
}

uint8_t CAN_SaveConfig(uint8_t buffer[])
{
    NVM_SaveConfig();
    return CANRP_Write;
}

uint8_t CAN_EraseConfig(uint8_t buffer[])
{
    NVM_Erase();
    return CANRP_Write;
}

uint8_t CAN_GetMotorConfig(uint8_t buffer[])
{
    uint8_t flags = (motor_is_calibrated() == true) | ((motor_is_gimbal() == true) << 1);
    uint16_t R = (uint16_t)(Motor_GetPhaseResistance() * 1e+3f);
    uint8_t pole_pairs = Motor_GetPolePairs();
    uint16_t L = (uint16_t)(Motor_GetPhaseInductance() * 1e+6f);
    uint16_t ticks = (uint16_t)ENCODER_TICKS;
    memcpy(&buffer[0], &flags, sizeof(uint8_t));
    memcpy(&buffer[1], &R, sizeof(uint16_t));
    memcpy(&buffer[3], &pole_pairs, sizeof(uint8_t));
    memcpy(&buffer[4], &L, sizeof(uint16_t));
    memcpy(&buffer[6], &ticks, sizeof(uint16_t));
    return CANRP_Read;
}

uint8_t CAN_SetMotorConfig(uint8_t buffer[])
{
    uint8_t flags;
    uint16_t R;
    uint16_t L;
    memcpy(&flags, &buffer[0], sizeof(uint8_t));
    memcpy(&R, &buffer[1], sizeof(uint16_t));
    memcpy(&L, &buffer[3], sizeof(uint16_t));

    CAN_ResponseType response = CANRP_NoAction;

    if ((R > 0) && (L > 0))
    {
        bool is_gimbal = (bool)(flags & 0x1);
        motor_set_is_gimbal(is_gimbal);
        Motor_SetPhaseResistance(((float)R) * 1e-3f);
        Motor_SetPhaseInductance(((float)L) * 1e-6f);

        response = CANRP_Write;
    }
    return response;
}
