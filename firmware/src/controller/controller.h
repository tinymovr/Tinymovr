
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

#ifndef CONTROLLER_CONTROLLER_H_
#define CONTROLLER_CONTROLLER_H_

#include "src/common.h"

typedef enum {
    CONTROLLER_ERR_NO_ERROR        = 0x0000,
	CONTROLLER_ERR_INVALID_STATE   = 0x0001,
	CONTROLLER_ERR_OVERCURRENT     = 0x0001
} ControllerError;

typedef enum {
	STATE_IDLE = 0,
	STATE_CALIBRATE = 1,
    STATE_CL_CONTROL = 2
} ControlState;

typedef enum {
	CTRL_CURRENT = 0,
	CTRL_VELOCITY = 1,
    CTRL_POSITION = 2
} ControlMode;

struct ControllerState
{
	// TODO: State.state is very confusing, name appropriately
	ControlState state;
    ControlMode mode;
    bool is_calibrating;

    struct FloatTriplet I_phase_meas;
    struct FloatTriplet modulation_values;

    float Iq_meas;
    float Id_meas;

    float pos_setpoint;
    float vel_setpoint;
    float Iq_setpoint;

    float vel_integrator_Iq;

    float Iq_integrator_Vq;
    float Id_integrator_Vd;
};

struct ControllerConfig
{
    float vel_limit;
    float vel_ramp_limit;
    float I_limit;

    float pos_gain;
    float vel_gain;
    float vel_integrator_gain;
    float I_bw;
    float I_gain;
    float Iq_integrator_gain;
    float Id_integrator_gain;
    float I_k;
};

void Controller_ControlLoop(void);

PAC5XXX_RAMFUNC ControlState Controller_GetState(void);
PAC5XXX_RAMFUNC void Controller_SetState(ControlState new_state);

ControlMode Controller_GetMode(void);
void Controller_SetMode(ControlMode mode);

float controller_get_pos_setpoint_user_frame(void);
void controller_set_pos_setpoint_user_frame(float value);
float controller_get_vel_setpoint_user_frame(void);
void controller_set_vel_setpoint_user_frame(float value);

PAC5XXX_RAMFUNC float controller_get_Iq_estimate(void);
float controller_get_Iq_setpoint(void);
void controller_set_Iq_setpoint(float value);

PAC5XXX_RAMFUNC float controller_get_Iq_estimate_user_frame(void);
float controller_get_Iq_setpoint_user_frame(void);
void controller_set_Iq_setpoint_user_frame(float value);

void Controller_GetModulationValues(struct FloatTriplet *dc);

float Controller_GetPosGain(void);
void Controller_SetPosGain(float gain);
float Controller_GetVelGain(void);
void Controller_SetVelGain(float gain);
float Controller_GetVelIntegratorGain(void);
void Controller_SetVelIntegratorGain(float gain);
float Controller_GetIqGain(void);
float Controller_GetIqBandwidth(void);
void Controller_SetIqBandwidth(float bw);

float Controller_GetVelLimit(void);
void Controller_SetVelLimit(float limit);
float Controller_GetIqLimit(void);
void Controller_SetIqLimit(float limit);

PAC5XXX_RAMFUNC bool Controller_Calibrated(void);

PAC5XXX_RAMFUNC void Controller_UpdateCurrentGains(void);

struct ControllerConfig* Controller_GetConfig(void);
void Controller_RestoreConfig(struct ControllerConfig* config_);

#endif /* CONTROLLER_CONTROLLER_H_ */
