
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

#include "src/observer/observer.h"
#include "motor.h"

static struct MotorConfig config = {
    .pole_pairs = 7u,
	.phase_resistance = 0.1f,
	.phase_inductance = 1e-5f,

	.resistance_calibrated = false,
	.inductance_calibrated = false,
	.poles_calibrated = false,

	.phases_swapped = false,
    .is_gimbal = false
};

void Motor_Init(void) {}

PAC5XXX_RAMFUNC uint8_t Motor_FindPolePairs(uint16_t ticks, float start_ticks, float end_ticks, float e_angle)
{
    const float pos = fabsf(end_ticks- start_ticks);
    float p_angle = twopi * pos/ticks;
    const float pairs_f = e_angle / p_angle;
    const uint8_t pairs_i = (uint8_t)floorf(pairs_f + 0.5f);
    const float residual = fabsf(pairs_f - (float)pairs_i);

    bool found = false;
    if (residual <= 0.30f)
    {
        found = true;
        Motor_SetPolePairs(pairs_i);
    }
    return found;
}

PAC5XXX_RAMFUNC uint8_t Motor_GetPolePairs(void)
{
    return config.pole_pairs;
}

PAC5XXX_RAMFUNC void Motor_SetPolePairs(uint8_t pairs)
{
    if (pairs > 1u)
    {
        config.pole_pairs = pairs;
        config.poles_calibrated = true;
    }
    // TODO: else error
}

PAC5XXX_RAMFUNC float Motor_GetPhaseResistance(void)
{
    return config.phase_resistance;
}
PAC5XXX_RAMFUNC void Motor_SetPhaseResistance(float R)
{
    if ((R > MIN_PHASE_RESISTANCE) && ((R < MAX_PHASE_RESISTANCE) || motor_is_gimbal()))
    {
        config.phase_resistance = R;
        config.resistance_calibrated = true;
    }
    // TODO: else error
}

PAC5XXX_RAMFUNC float Motor_GetPhaseInductance(void)
{
    return config.phase_inductance;
}
PAC5XXX_RAMFUNC void Motor_SetPhaseInductance(float L)
{
    if ((L > MIN_PHASE_INDUCTANCE) && ((L < MAX_PHASE_INDUCTANCE) || motor_is_gimbal()))
    {
        config.phase_inductance = L;
        config.inductance_calibrated = true;
    }
    // TODO: else error
}

PAC5XXX_RAMFUNC bool motor_phases_swapped(void)
{
	return config.phases_swapped;
}

PAC5XXX_RAMFUNC void motor_set_phases_swapped(bool swapped)
{
	config.phases_swapped = swapped;
}

PAC5XXX_RAMFUNC bool motor_is_calibrated(void)
{
    return config.resistance_calibrated && config.inductance_calibrated && config.poles_calibrated;
}

PAC5XXX_RAMFUNC bool motor_is_gimbal(void)
{
    return config.is_gimbal;
}

PAC5XXX_RAMFUNC void motor_set_is_gimbal(bool gimbal)
{
    config.is_gimbal = gimbal;
}

struct MotorConfig* Motor_GetConfig(void)
{
    return &config;
}

void Motor_RestoreConfig(struct MotorConfig* config_)
{
    config = *config_;
}
