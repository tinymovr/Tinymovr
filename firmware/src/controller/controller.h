
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

#ifndef CONTROLLER_CONTROLLER_H_
#define CONTROLLER_CONTROLLER_H_

#include <src/common.h>
#include <src/controller/trajectory_planner.h>
#include <src/controller/homing_planner.h>

typedef enum
{
    STATE_IDLE = 0,
    STATE_CALIBRATE = 1,
    STATE_CL_CONTROL = 2
} ControlState;

typedef enum
{
    CTRL_CURRENT = 0,
    CTRL_VELOCITY = 1,
    CTRL_POSITION = 2,
    CTRL_TRAJECTORY = 3,
    CTRL_HOMING = 4
} ControlMode;

typedef struct
{
    // TODO: State.state is very confusing, name appropriately
    ControlState state;
    ControlMode mode;
    uint8_t warnings;
    uint8_t errors;
    bool is_calibrating;
    struct FloatTriplet I_phase_meas;
    struct FloatTriplet modulation_values;
    float Iq_est;
    float Id_est;
    float Ibus_est;
    float power_est;
    float pos_setpoint;
    float vel_setpoint;
    float vel_ramp_setpoint;
    float Iq_setpoint;
    float Id_setpoint;
    float Vq_setpoint;
    float vel_integrator_Iq;
    float Iq_integrator_Vq;
    float Id_integrator_Vd;
    float t_plan;
} ControllerState;

typedef struct
{
    float vel_limit;
    float vel_ramp_limit;
    float I_limit;
    float pos_gain;
    float vel_gain;
    float vel_integrator_gain;
    float vel_integrator_deadband;
    float I_bw;
    float I_gain;
    float Iq_integrator_gain;
    float Id_integrator_gain;
    float I_k;
    float vel_increment;
    float max_Ibus_regen;
    float max_Ibrake;
} ControllerConfig;

void Controller_ControlLoop(void);

ControlState controller_get_state(void);
void controller_set_state(ControlState new_state);

ControlMode controller_get_mode(void);
void controller_set_mode(ControlMode mode);

inline void controller_calibrate(void) {controller_set_state(STATE_CALIBRATE);}
inline void controller_idle(void) {controller_set_state(STATE_IDLE);}
inline void controller_position_mode(void) {controller_set_mode(CTRL_POSITION);controller_set_state(STATE_CL_CONTROL);}
inline void controller_velocity_mode(void) {controller_set_mode(CTRL_VELOCITY);controller_set_state(STATE_CL_CONTROL);}
inline void controller_current_mode(void) {controller_set_mode(CTRL_CURRENT);controller_set_state(STATE_CL_CONTROL);}

float controller_get_pos_setpoint_user_frame(void);
void controller_set_pos_setpoint_user_frame(float value);
float controller_get_vel_setpoint_user_frame(void);
void controller_set_vel_setpoint_user_frame(float value);

float controller_get_Iq_estimate(void);
float controller_get_Iq_setpoint(void);
void controller_set_Iq_setpoint(float value);

float controller_get_Iq_estimate_user_frame(void);
float controller_get_Iq_setpoint_user_frame(void);
void controller_set_Iq_setpoint_user_frame(float value);
float controller_get_Id_setpoint_user_frame(void);

float controller_get_Vq_setpoint_user_frame(void);

float controller_set_pos_vel_setpoints(float pos_setpoint, float vel_setpoint);

void controller_get_modulation_values(struct FloatTriplet *dc);

float controller_get_pos_gain(void);
void controller_set_pos_gain(float gain);
float controller_get_vel_gain(void);
void controller_set_vel_gain(float gain);
float controller_get_vel_integrator_gain(void);
void controller_set_vel_integrator_gain(float gain);
float controller_get_vel_integrator_deadband(void);
void controller_set_vel_integrator_deadband(float gain);
float controller_get_Iq_gain(void);
float controller_get_I_bw(void);
void controller_set_I_bw(float bw);

float controller_get_Ibus_est(void);
float controller_get_power_est(void);

float controller_get_vel_limit(void);
void controller_set_vel_limit(float limit);
float controller_get_Iq_limit(void);
void controller_set_Iq_limit(float limit);
float controller_get_vel_increment(void);
void controller_set_vel_increment(float inc);

float controller_get_max_Ibus_regen(void);
void controller_set_max_Ibus_regen(float value);
float controller_get_max_Ibrake(void);
void controller_set_max_Ibrake(float value);

void controller_set_motion_plan(MotionPlan mp);

void controller_update_I_gains(void);

uint8_t controller_get_warnings(void);
uint8_t controller_get_errors(void);

ControllerConfig *Controller_GetConfig(void);
void Controller_RestoreConfig(ControllerConfig *config_);

#endif /* CONTROLLER_CONTROLLER_H_ */
