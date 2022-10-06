
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

#pragma once

#define MIN_PHASE_RESISTANCE (0.005f)
#define MAX_PHASE_RESISTANCE (1.0f)

#define MIN_PHASE_INDUCTANCE (2e-6f)
#define MAX_PHASE_INDUCTANCE (5000e-6f)

#include "src/common.h"

typedef struct
{
	uint8_t pole_pairs;
	float phase_resistance;
	float phase_inductance;

	float user_offset;
	float user_direction;

	float I_cal;

	bool resistance_calibrated;
	bool inductance_calibrated;
	bool poles_calibrated;

	bool phases_swapped;
	bool is_gimbal;
} MotorConfig;

typedef struct
{
	uint8_t errors;
} MotorState;

void motor_reset_calibration(void);

PAC5XXX_RAMFUNC uint8_t motor_get_pole_pairs(void);
PAC5XXX_RAMFUNC uint8_t motor_find_pole_pairs(uint16_t ticks, float mpos_start, float mpos_end, float epos_rad);
PAC5XXX_RAMFUNC void motor_set_pole_pairs(uint8_t pairs);

PAC5XXX_RAMFUNC float motor_get_phase_resistance(void);
PAC5XXX_RAMFUNC void motor_set_phase_resistance(float R);

PAC5XXX_RAMFUNC float motor_get_phase_inductance(void);
PAC5XXX_RAMFUNC void motor_set_phase_inductance(float L);

PAC5XXX_RAMFUNC void motor_set_phase_R_and_L(float R, float L);

PAC5XXX_RAMFUNC float motor_get_I_cal(void);
PAC5XXX_RAMFUNC void motor_set_I_cal(float I);

PAC5XXX_RAMFUNC bool motor_phases_swapped(void);
PAC5XXX_RAMFUNC void motor_set_phases_swapped(bool swapped);

PAC5XXX_RAMFUNC bool motor_get_calibrated(void);

PAC5XXX_RAMFUNC bool motor_get_is_gimbal(void);
PAC5XXX_RAMFUNC void motor_set_is_gimbal(bool gimbal);

PAC5XXX_RAMFUNC float motor_get_user_offset(void);
PAC5XXX_RAMFUNC void motor_set_user_offset(float offset);

PAC5XXX_RAMFUNC int8_t motor_get_user_direction(void);
PAC5XXX_RAMFUNC void motor_set_user_direction(int8_t dir);

PAC5XXX_RAMFUNC uint8_t motor_get_errors(void);
PAC5XXX_RAMFUNC uint8_t *motor_get_error_ptr(void);

MotorConfig *motor_get_config(void);
void motor_restore_config(MotorConfig *config_);

