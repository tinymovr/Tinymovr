
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

#include <src/common.h>
#include <string.h>

#include <src/system/system.h>
#include <src/adc/adc.h>
#include <src/motor/motor.h>
#include <src/encoder/encoder.h>
#include <src/encoder/hall.h>
#include <src/observer/observer.h>
#include <src/controller/controller.h>
#include <src/controller/trajectory_planner.h>
#include <src/scheduler/scheduler.h>
#include "src/nvm/nvm.h"

#include <src/can/can_endpoints.h>

#define EP_LIST_SIZE 64
#define EP_MAP_SIZE 64

CANEP_Callback EPList[EP_LIST_SIZE] = {0};
uint8_t currentIndex = 0;
uint8_t EPMap[EP_MAP_SIZE] = {0};

void CANEP_InitEndpointMap(void)
{
    // 0x001 AVAIL
    CANEP_AddEndpoint(&CAN_GetOffsetAndDirection, 0x002);
    CANEP_AddEndpoint(&CAN_GetState, 0x003);
    CANEP_AddEndpoint(&CAN_GetMinStudioVersion, 0x004);
    CANEP_AddEndpoint(&CAN_GetCANConfig, 0x005);
    CANEP_AddEndpoint(&CAN_SetCANConfig, 0x006);
    CANEP_AddEndpoint(&CAN_SetState, 0x007);
    CANEP_AddEndpoint(&CAN_SetOffsetAndDirection, 0x008);
    CANEP_AddEndpoint(&CAN_GetEncoderEstimates, 0x009);
    CANEP_AddEndpoint(&CAN_GetSetpoints, 0x00A);
    CANEP_AddEndpoint(&CAN_GetEncoderConfig, 0x00B);
    CANEP_AddEndpoint(&CAN_SetPosSetpoint, 0x00C);  
    CANEP_AddEndpoint(&CAN_SetVelSetpoint, 0x00D);
    CANEP_AddEndpoint(&CAN_SetIqSetpoint, 0x00E);
    CANEP_AddEndpoint(&CAN_SetLimits, 0x00F);
    CANEP_AddEndpoint(&CAN_GetPhaseCurrents, 0x010);
    CANEP_AddEndpoint(&CAN_SetEncoderConfig, 0x011); 
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
    CANEP_AddEndpoint(&CAN_MoveToPosWithTimeLimit, 0x020);
    CANEP_AddEndpoint(&CAN_MoveToPosWithVelLimit, 0x021);
    CANEP_AddEndpoint(&CAN_SetMaxPlanAccelDecel, 0x022);
    CANEP_AddEndpoint(&CAN_GetMaxPlanAccelDecel, 0x023);
    CANEP_AddEndpoint(&CAN_GetSetPosVel, 0x025);
    CANEP_AddEndpoint(&CAN_GetSetPosVelIq, 0x026);
    CANEP_AddEndpoint(&CAN_GetMotorRL, 0x027);
    CANEP_AddEndpoint(&CAN_SetMotorRL, 0x028);
    CANEP_AddEndpoint(&CAN_GetHallSectorMap, 0x029);
    // 0x02A AVAIL
    // 0x02B AVAIL
    // 0x02C AVAIL
    // 0x02D AVAIL
    // 0x02E AVAIL
    // 0x02F AVAIL
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

uint8_t CAN_GetOffsetAndDirection(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
	float offset = motor_get_user_offset();
	int8_t direction = motor_get_user_direction();
	*buffer_len = 8;
	memcpy(&buffer[0], &offset, sizeof(float));
	memcpy(&buffer[4], &direction, sizeof(int8_t));
	return CANRP_Read;
}

uint8_t CAN_GetState(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    uint8_t state = Controller_GetState();
    uint8_t mode = Controller_GetMode();
    uint8_t *error_flags = get_error_flags();
    //memcpy(&buffer[0], &error, sizeof(uint8_t));
    *buffer_len = 8;
    memcpy(&buffer[1], &state, sizeof(uint8_t));
    memcpy(&buffer[2], &mode, sizeof(uint8_t));
    memcpy(&buffer[3], error_flags, sizeof(uint8_t) * ERROR_FLAG_MAX_SIZE);
    return CANRP_Read;
}

uint8_t CAN_GetMinStudioVersion(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    static const uint8_t v_major = STUDIO_MIN_VERSION_MAJOR;
    static const uint8_t v_minor = STUDIO_MIN_VERSION_MINOR;
    static const uint8_t v_patch = STUDIO_MIN_VERSION_PATCH;
    *buffer_len = 3 * sizeof(uint8_t);
    memcpy(&buffer[0], &v_major, sizeof(uint8_t));
    memcpy(&buffer[1], &v_minor, sizeof(uint8_t));
    memcpy(&buffer[2], &v_patch, sizeof(uint8_t));
    return CANRP_Read;
}

uint8_t CAN_GetCANConfig(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    uint8_t id = CAN_get_ID();
    uint16_t baudrate = CAN_get_kbit_rate();
    *buffer_len = sizeof(uint8_t) + sizeof(uint16_t);
    memcpy(&buffer[0], &id, sizeof(uint8_t));
    memcpy(&buffer[1], &baudrate, sizeof(uint16_t));
    return CANRP_Read;
}

uint8_t CAN_SetCANConfig(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    uint8_t id;
    uint16_t baudrate;
    memcpy(&id, &buffer[0], sizeof(uint8_t));
    memcpy(&baudrate, &buffer[1], sizeof(uint16_t));
    CAN_ResponseType response = CANRP_NoAction;
    if (id >= 1u)
    {
        CAN_set_ID(id);
        response = CANRP_Write;
    }
    if ((baudrate >= 50u) && (baudrate <= 1000u))
    {
        CAN_set_kbit_rate(baudrate);
        response = CANRP_Write;
    }
    return response;
}

uint8_t CAN_SetState(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
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

uint8_t CAN_SetOffsetAndDirection(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
	float offset;
	int8_t direction;
	memcpy(&offset, &buffer[0], sizeof(float));
	memcpy(&direction, &buffer[4], sizeof(int8_t));
	motor_set_user_offset(offset);
	motor_set_user_direction(direction);
	return CANRP_Write;
}

uint8_t CAN_GetEncoderEstimates(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    const float pos = observer_get_pos_estimate_user_frame();
    const float vel = observer_get_vel_estimate_user_frame();
    *buffer_len = 2 * sizeof(float);
    memcpy(&buffer[0], &pos, sizeof(float));
    memcpy(&buffer[4], &vel, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_GetSetpoints(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    const float pos = controller_get_pos_setpoint_user_frame();
    const float vel = controller_get_vel_setpoint_user_frame();
    *buffer_len = 2 * sizeof(float);
    memcpy(&buffer[0], &pos, sizeof(float));
    memcpy(&buffer[4], &vel, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_GetEncoderConfig(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    const uint8_t enc_type = encoder_get_type();
    const float bw = observer_get_bw();
    *buffer_len = sizeof(enc_type) + sizeof(bw);
    memcpy(&buffer[0], &enc_type, sizeof(uint8_t));
    memcpy(&buffer[1], &bw, sizeof(bw));
    return CANRP_Read;
}

uint8_t CAN_SetPosSetpoint(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    float pos;
    int16_t vel_ff;
    int16_t Iq_ff;
    memcpy(&pos, &buffer[0], sizeof(pos));
    memcpy(&vel_ff, &buffer[4], sizeof(vel_ff));
    memcpy(&Iq_ff, &buffer[6], sizeof(Iq_ff));
    float velFF_float = vel_ff * 10.0f;
    float iqFF_float = Iq_ff * 0.01f;
    controller_set_pos_setpoint_user_frame(pos);
    controller_set_vel_setpoint_user_frame(velFF_float);
    controller_set_Iq_setpoint_user_frame(iqFF_float);
    return CANRP_Write;
}

uint8_t CAN_SetVelSetpoint(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    float vel;
    float Iq_ff;
    memcpy(&vel, &buffer[0], sizeof(float));
    memcpy(&Iq_ff, &buffer[4], sizeof(float));
    controller_set_vel_setpoint_user_frame(vel);
    controller_set_Iq_setpoint_user_frame(Iq_ff);
    return CANRP_Write;
}

uint8_t CAN_SetIqSetpoint(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    float Iq;
    memcpy(&Iq, &buffer[0], sizeof(float));
    controller_set_Iq_setpoint_user_frame(Iq);
    return CANRP_Write;
}

uint8_t CAN_GetLimits(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    const float vel_limit = Controller_GetVelLimit();
    const float iq_limit = Controller_GetIqLimit();
    *buffer_len = 2 * sizeof(float);
    memcpy(&buffer[0], &vel_limit, sizeof(float));
    memcpy(&buffer[4], &iq_limit, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_SetLimits(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    float vel_limit;
    float iq_limit;
    memcpy(&vel_limit, &buffer[0], sizeof(float));
    memcpy(&iq_limit, &buffer[4], sizeof(float));
    Controller_SetVelLimit(vel_limit);
    Controller_SetIqLimit(iq_limit);
    return CANRP_Write;
}

uint8_t CAN_GetPhaseCurrents(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    struct FloatTriplet I_phase;
    ADC_GetPhaseCurrents(&I_phase);
    const int16_t IA = (int16_t)(I_phase.A * 1000.0f);
    const int16_t IB = (int16_t)(I_phase.B * 1000.0f);
    const int16_t IC = (int16_t)(I_phase.C * 1000.0f);
    *buffer_len = 3 * sizeof(int16_t);
    memcpy(&buffer[0], &IA, sizeof(int16_t));
    memcpy(&buffer[2], &IB, sizeof(int16_t));
    memcpy(&buffer[4], &IC, sizeof(int16_t));
    return CANRP_Read;
}

uint8_t CAN_SetEncoderConfig(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    uint8_t enc_type;
    float bw;
    memcpy(&enc_type, &buffer[0], sizeof(enc_type));
    memcpy(&bw, &buffer[1], sizeof(bw));
    encoder_set_type(enc_type); // check done in setter
    observer_set_bw(bw); // check done in setter
    return CANRP_Write;
}

uint8_t CAN_GetIq(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    const float Iq_set = controller_get_Iq_setpoint_user_frame();
    const float Iq_est = controller_get_Iq_estimate_user_frame();
    *buffer_len = 2 * sizeof(float);
    memcpy(&buffer[0], &Iq_set, sizeof(float));
    memcpy(&buffer[4], &Iq_est, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_Reset(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    system_reset();
    return CANRP_Write;
}

uint8_t CAN_GetVBus(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    const float VBus = ADC_GetVBus();
    *buffer_len = sizeof(float);
    memcpy(&buffer[0], &VBus, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_GetGains(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    const float pos_gain = Controller_GetPosGain();
    const float vel_P_gain = Controller_GetVelGain();
    *buffer_len = 2 * sizeof(float);
    memcpy(&buffer[0], &pos_gain, sizeof(float));
    memcpy(&buffer[4], &vel_P_gain, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_SetGains(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
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

uint8_t CAN_GetIntegratorGains(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    const float vel_I_gain = Controller_GetVelIntegratorGain();
    const float vel_I_deadband = controller_get_vel_integrator_deadband();
    *buffer_len = 2*sizeof(float);
    memcpy(&buffer[0], &vel_I_gain, sizeof(float));
    memcpy(&buffer[4], &vel_I_deadband, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_SetIntegratorGains(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    float vel_I_gain;
    float vel_I_deadband;
    memcpy(&vel_I_gain, &buffer[0], sizeof(float));
    memcpy(&vel_I_deadband, &buffer[4], sizeof(float));
    CAN_ResponseType response = CANRP_NoAction;
    if (vel_I_gain >= 0.0f && vel_I_deadband >= 0.0f)
    {
        Controller_SetVelIntegratorGain(vel_I_gain);
        controller_set_vel_integrator_deadband(vel_I_deadband);
        response = CANRP_Write;
    }
    return response;
}

uint8_t CAN_DeviceInfo(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    const uint32_t idr = PAC55XX_INFO1->UNIQUEID[0];
    const uint8_t v_major = VERSION_MAJOR;
    const uint8_t v_minor = VERSION_MINOR;
    const uint8_t v_patch = VERSION_PATCH;
    const int8_t temp = (int8_t)ADC_GetMCUTemp();
    *buffer_len = sizeof(uint32_t) + 4 * sizeof(uint8_t);
    memcpy(&buffer[0], &idr, sizeof(uint32_t));
    memcpy(&buffer[4], &v_major, sizeof(uint8_t));
    memcpy(&buffer[5], &v_minor, sizeof(uint8_t));
    memcpy(&buffer[6], &v_patch, sizeof(uint8_t));
    memcpy(&buffer[7], &temp, sizeof(int8_t));
    return CANRP_Read;
}

uint8_t CAN_Timings(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    const uint32_t total = Scheduler_GetTotalCycles();
    const uint32_t busy = Scheduler_GetBusyCycles();
    *buffer_len = 2 * sizeof(uint32_t);
    memcpy(&buffer[0], &total, sizeof(uint32_t));
    memcpy(&buffer[4], &busy, sizeof(uint32_t));
    return CANRP_Read;
}

uint8_t CAN_SaveConfig(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    nvm_save_config();
    return CANRP_Write;
}

uint8_t CAN_EraseConfig(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    nvm_erase();
    return CANRP_Write;
}

uint8_t CAN_GetMotorConfig(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    const uint8_t flags = (motor_is_calibrated() == true) | ((motor_is_gimbal() == true) << 1);
    const uint8_t pole_pairs = motor_get_pole_pairs();
    const float I_cal = motor_get_I_cal();
    *buffer_len = 2 * sizeof(uint8_t) + sizeof(float);
    memcpy(&buffer[0], &flags, sizeof(uint8_t));
    memcpy(&buffer[1], &pole_pairs, sizeof(uint8_t));
    memcpy(&buffer[2], &I_cal, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_SetMotorConfig(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    uint8_t flags;
    uint8_t pole_pairs;
    float I_cal;
    memcpy(&flags, &buffer[0], sizeof(uint8_t));
    memcpy(&pole_pairs, &buffer[1], sizeof(uint8_t));
    memcpy(&I_cal, &buffer[2], sizeof(float));

    if (I_cal > 0 && pole_pairs > 0)
    {
        bool is_gimbal = (bool)(flags & 0x1);
        motor_set_is_gimbal(is_gimbal);
        motor_set_pole_pairs(pole_pairs);
        motor_set_I_cal(I_cal);
        return CANRP_Write;
    }
    return CANRP_NoAction;
}

uint8_t CAN_GetMotorRL(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    const float R = motor_get_phase_resistance();
    const float L = motor_get_phase_inductance();
    *buffer_len = 2 * sizeof(float);
    memcpy(&buffer[0], &R, sizeof(float));
    memcpy(&buffer[4], &L, sizeof(float));
    return CANRP_Read;
}

uint8_t CAN_SetMotorRL(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    float R;
    float L;
    memcpy(&R, &buffer[0], sizeof(float));
    memcpy(&L, &buffer[4], sizeof(float));

    if ((R > 0) && (L > 0))
    {
        motor_set_phase_R_and_L(R, L);
        return CANRP_Write;
    }
    return CANRP_NoAction;
}

// -----

uint8_t CAN_MoveToPosWithTimeLimit(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    float target_pos;
    uint16_t t_tot_int;
    uint8_t p_acc;
    uint8_t p_dec;
    uint8_t response = CANRP_NoAction;
    memcpy(&target_pos, &buffer[0], sizeof(float));
    memcpy(&t_tot_int, &buffer[4], sizeof(uint16_t));
    memcpy(&p_acc, &buffer[6], sizeof(uint8_t));
    memcpy(&p_dec, &buffer[7], sizeof(uint8_t));
    float t_tot = ((float)t_tot_int) * 1e-3f;
    float t_acc = t_tot * ((float)p_acc/256.f);
    float t_dec = t_tot * ((float)p_dec/256.f);
    if (planner_move_to_tlimit(target_pos, t_tot, t_acc, t_dec))
    {
        response = CANRP_Write;
    }
    return response;
}

uint8_t CAN_MoveToPosWithVelLimit(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    float target_pos;
    float max_vel;
    uint8_t response = CANRP_NoAction;
    memcpy(&target_pos, &buffer[0], sizeof(float));
    memcpy(&max_vel, &buffer[4], sizeof(float));
    if (planner_set_max_vel(max_vel) && planner_move_to_vlimit(target_pos))
    {
        response = CANRP_Write;
    }
    return response;
}

uint8_t CAN_SetMaxPlanAccelDecel(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
	float max_accel;
	float max_decel;
	uint8_t response = CANRP_NoAction;
	memcpy(&max_accel, &buffer[0], sizeof(float));
	memcpy(&max_decel, &buffer[4], sizeof(float));
	if (planner_set_max_accel(max_accel) && planner_set_max_decel(max_decel))
	{
		response = CANRP_Write;
	}
	return response;
}

uint8_t CAN_GetMaxPlanAccelDecel(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
	const float max_accel = planner_get_max_accel();
	const float max_decel = planner_get_max_decel();
	memcpy(&buffer[0], &max_accel, sizeof(float));
	memcpy(&buffer[4], &max_decel, sizeof(float));
	return CANRP_Read;
}

// -----

uint8_t CAN_GetSetPosVel(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    float pos;
    float vel_ff;
    memcpy(&pos, &buffer[0], sizeof(float));
    memcpy(&vel_ff, &buffer[4], sizeof(float));
    controller_set_pos_setpoint_user_frame(pos);
    controller_set_vel_setpoint_user_frame(vel_ff);
    *buffer_len = 2 * sizeof(float);
    pos = observer_get_pos_estimate_user_frame();
    vel_ff = observer_get_vel_estimate_user_frame();
    memcpy(&buffer[0], &pos, sizeof(float));
    memcpy(&buffer[4], &vel_ff, sizeof(float));
    return CANRP_ReadWrite;
}

uint8_t CAN_GetSetPosVelIq(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    float pos;
    int16_t vel_ff;
    int16_t Iq_ff;
    memcpy(&pos, &buffer[0], sizeof(float));
    memcpy(&vel_ff, &buffer[4], sizeof(int16_t));
    memcpy(&Iq_ff, &buffer[6], sizeof(int16_t));
    float velFF_float = vel_ff * 10.0f;
    float iqFF_float = Iq_ff * 0.01f;
    controller_set_pos_setpoint_user_frame(pos);
    controller_set_vel_setpoint_user_frame(velFF_float);
    controller_set_Iq_setpoint_user_frame(iqFF_float);
    *buffer_len = sizeof(float) + 2 * sizeof(int16_t);
    pos = observer_get_pos_estimate_user_frame();
    vel_ff = (int16_t)(observer_get_vel_estimate_user_frame() * 0.1f);
    Iq_ff = (int16_t)(controller_get_Iq_estimate() * 100.0f);
    memcpy(&buffer[0], &pos, sizeof(float));
    memcpy(&buffer[4], &vel_ff, sizeof(int16_t));
    memcpy(&buffer[6], &Iq_ff, sizeof(int16_t));
    return CANRP_ReadWrite;
}

uint8_t CAN_GetHallSectorMap(uint8_t buffer[], uint8_t *buffer_len, bool rtr)
{
    const uint8_t *sector_map = hall_get_sector_map_ptr();
	const uint8_t sector = hall_get_sector();
    
    *buffer_len = sizeof(uint8_t);
    memcpy(&buffer[0], sector_map, sizeof(uint8_t) * 7);
	memcpy(&buffer[8], &sector, sizeof(uint8_t));
	return CANRP_Read;
}
