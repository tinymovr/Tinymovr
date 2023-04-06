
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

#include <src/common.h>

#if defined BOARD_REV_R32 || BOARD_REV_R33 || defined BOARD_REV_R5
#define MIN_PHASE_RESISTANCE (0.01f)
#define MAX_PHASE_RESISTANCE (1.0f)
#define MIN_PHASE_INDUCTANCE (5e-6f)
#define MAX_PHASE_INDUCTANCE (1e-3f)
#elif defined BOARD_REV_M5
#define MIN_PHASE_RESISTANCE (0.5f)
#define MAX_PHASE_RESISTANCE (20.0f)
#define MIN_PHASE_INDUCTANCE (1e-5f)
#define MAX_PHASE_INDUCTANCE (1e-2f)
#endif

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

uint8_t motor_get_pole_pairs(void);
uint8_t motor_find_pole_pairs(uint16_t ticks, float mpos_start, float mpos_end, float epos_rad);
void motor_set_pole_pairs(uint8_t pairs);

float motor_get_phase_resistance(void);
void motor_set_phase_resistance(float R);

float motor_get_phase_inductance(void);
void motor_set_phase_inductance(float L);

void motor_set_phase_R_and_L(float R, float L);

float motor_get_I_cal(void);
void motor_set_I_cal(float I);

bool motor_phases_swapped(void);
void motor_set_phases_swapped(bool swapped);

bool motor_get_calibrated(void);

bool motor_get_is_gimbal(void);
void motor_set_is_gimbal(bool gimbal);

float motor_get_user_offset(void);
void motor_set_user_offset(float offset);

int8_t motor_get_user_direction(void);
void motor_set_user_direction(int8_t dir);

uint8_t motor_get_errors(void);
uint8_t *motor_get_error_ptr(void);

MotorConfig *motor_get_config(void);
void motor_restore_config(MotorConfig *config_);

