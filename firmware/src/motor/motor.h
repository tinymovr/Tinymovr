
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

#define CAL_R_LEN             (2 * PWM_FREQ_HZ)
#define CAL_L_LEN             (1 * PWM_FREQ_HZ)
#define CAL_OFFSET_LEN        (1 * PWM_FREQ_HZ)
#define CAL_STAY_LEN          (PWM_FREQ_HZ / 2)
#define CAL_DIR_LEN           (3 * PWM_FREQ_HZ)
#define CAL_PHASE_TURNS       (8)
#if defined BOARD_REV_R32 || BOARD_REV_R33 || defined BOARD_REV_R5
#define CAL_V_GAIN            (0.0005f)
#define CAL_V_INDUCTANCE      (2.0f)
#elif defined BOARD_REV_M5
#define CAL_V_GAIN            (0.002f)
#define CAL_V_INDUCTANCE      (5.0f)
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
bool motor_calibrate_resistance(void);
bool motor_calibrate_inductance(void);

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

static inline void set_epos_and_wait(float angle, float I_setpoint)
{
    FloatTriplet modulation_values = {0.0f};
    float pwm_setpoint = (I_setpoint * motor_get_phase_resistance()) / system_get_Vbus();
    our_clampc(&pwm_setpoint, -PWM_LIMIT, PWM_LIMIT);
    SVM(pwm_setpoint * fast_cos(angle), pwm_setpoint * fast_sin(angle),
        &modulation_values.A, &modulation_values.B, &modulation_values.C);
    gate_driver_set_duty_cycle(&modulation_values);
    WaitForControlLoopInterrupt();
}

