
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
	STATE_IDLE = 0,
	STATE_CALIBRATE = 1,
    STATE_CL_CONTROL = 2
} ControlState;

typedef enum {
	CTRL_CURRENT = 0,
	CTRL_VELOCITY = 1,
    CTRL_POSITION = 2
} ControlMode;

typedef enum {
	ERROR_NO_ERROR = 0,
	ERROR_INVALID_STATE = 1,
	ERROR_ILLEGAL_VALUE = 2,
	ERROR_VBUS_UNDERVOLTAGE = 3,
	ERROR_OVERCURRENT = 4,
    ERROR_PWM_LIMIT_EXCEEDED = 5,
	// Keep below last to make comparisons for state change
	ERROR_PHASE_RESISTANCE_OUT_OF_RANGE = 6,
    ERROR_PHASE_INDUCTANCE_OUT_OF_RANGE = 7,
    ERROR_INVALID_POLE_PAIRS = 8
} ControlError;

struct ControllerState
{
	// TODO: State.state is very confusing, name appropriately
	ControlState state;
    ControlMode mode;
    ControlError error;

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

    uint32_t busy_cycles;
    uint32_t total_cycles;

    uint32_t last_timestamp;
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

float Controller_GetPosSetpoint(void);
void Controller_SetPosSetpoint(float value);
float Controller_GetVelSetpoint(void);
void Controller_SetVelSetpoint(float value);
float Controller_GetIqEstimate(void);
float Controller_GetIqSetpoint(void);
void Controller_SetIqSetpoint(float value);

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
uint8_t Controller_GetError(void);

uint32_t Controller_GetTotalCycles(void);
uint32_t Controller_GetBusyCycles(void);

PAC5XXX_RAMFUNC void Controller_UpdateCurrentGains(void);

struct ControllerConfig* Controller_GetConfig(void);
void Controller_RestoreConfig(struct ControllerConfig* config_);

#endif /* CONTROLLER_CONTROLLER_H_ */
