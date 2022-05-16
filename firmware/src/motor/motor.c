
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

#include <src/controller/controller.h>
#include <src/utils/utils.h>
#include <src/motor/motor.h>

static MotorConfig config = {
	.pole_pairs = 7u,
	.phase_resistance = 0.1f,
	.phase_inductance = 1e-5f,

	.user_offset = 0.0f,
	.user_direction = 1,

	.I_cal = 6.0f,

	.resistance_calibrated = false,
	.inductance_calibrated = false,
	.poles_calibrated = false,

	.phases_swapped = false,
	.is_gimbal = false};

void motor_reset_calibration()
{
	config.pole_pairs = 7;
	// Important! We only reset resistance and 
	// inductance measurements if the motor is
	// not a gimbal, otherwise they will not
	// be recalibrated and will stay at default
	// values!
	if (!motor_is_gimbal())
	{
		config.phase_resistance = 0.1f;
		config.phase_inductance = 1e-5f;
		config.resistance_calibrated = false;
		config.inductance_calibrated = false;
	}
	config.user_offset = 0.0f;
	config.user_direction = 1;
	config.poles_calibrated = false;
	config.phases_swapped = false;
}

PAC5XXX_RAMFUNC uint8_t motor_find_pole_pairs(uint16_t ticks, float mpos_start, float mpos_end, float epos_rad)
{
	const float mpos_diff = our_fabsf(mpos_end - mpos_start);
	float mpos_diff_rad = TWOPI * mpos_diff / ticks;
	const float pairs_f = epos_rad / mpos_diff_rad;
	const uint8_t pairs_i = (uint8_t)our_floorf(pairs_f + 0.5f);
	const float residual = our_fabsf(pairs_f - (float)pairs_i);

	bool found = false;
	if (residual <= 0.30f)
	{
		found = true;
		motor_set_pole_pairs(pairs_i);
	}
	return found;
}

PAC5XXX_RAMFUNC uint8_t motor_get_pole_pairs(void)
{
	return config.pole_pairs;
}

PAC5XXX_RAMFUNC void motor_set_pole_pairs(uint8_t pairs)
{
	if (pairs > 1u)
	{
		config.pole_pairs = pairs;
		config.poles_calibrated = true;
	}
}

PAC5XXX_RAMFUNC float motor_get_phase_resistance(void)
{
	return config.phase_resistance;
}

PAC5XXX_RAMFUNC void motor_set_phase_resistance(float R)
{
	if ((R > MIN_PHASE_RESISTANCE) && ((R < MAX_PHASE_RESISTANCE) || motor_is_gimbal()))
	{
		config.phase_resistance = R;
		config.resistance_calibrated = true;
		if (config.resistance_calibrated && config.inductance_calibrated)
		{
			Controller_UpdateCurrentGains();
		}
	}
}

PAC5XXX_RAMFUNC float motor_get_phase_inductance(void)
{
	return config.phase_inductance;
}

PAC5XXX_RAMFUNC void motor_set_phase_inductance(float L)
{
	if ((L > MIN_PHASE_INDUCTANCE) && ((L < MAX_PHASE_INDUCTANCE) || motor_is_gimbal()))
	{
		config.phase_inductance = L;
		config.inductance_calibrated = true;
		if (config.resistance_calibrated && config.inductance_calibrated)
		{
			Controller_UpdateCurrentGains();
		}
	}
}

PAC5XXX_RAMFUNC void motor_set_phase_R_and_L(float R, float L)
{
	if ((R > MIN_PHASE_RESISTANCE) && ((R < MAX_PHASE_RESISTANCE) || motor_is_gimbal()))
	{
		config.phase_resistance = R;
		config.resistance_calibrated = true;
	}
	if ((L > MIN_PHASE_INDUCTANCE) && ((L < MAX_PHASE_INDUCTANCE) || motor_is_gimbal()))
	{
		config.phase_inductance = L;
		config.inductance_calibrated = true;
	}
	if (config.resistance_calibrated && config.inductance_calibrated)
	{
		Controller_UpdateCurrentGains();
	}
}

PAC5XXX_RAMFUNC float motor_get_I_cal(void)
{
	return config.I_cal;
}

PAC5XXX_RAMFUNC void motor_set_I_cal(float I)
{
	if (I > 0)
	{
		config.I_cal = I;
	}
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

PAC5XXX_RAMFUNC float motor_get_user_offset(void)
{
	return config.user_offset;
}

PAC5XXX_RAMFUNC void motor_set_user_offset(float offset)
{
	config.user_offset = offset;
}

PAC5XXX_RAMFUNC int8_t motor_get_user_direction(void)
{
	return config.user_direction;
}

PAC5XXX_RAMFUNC void motor_set_user_direction(int8_t dir)
{
	if ((dir == -1) || (dir == 1))
	{
		config.user_direction = dir;
	}
}

MotorConfig *motor_get_config(void)
{
	return &config;
}

void motor_restore_config(MotorConfig *config_)
{
	config = *config_;
}
