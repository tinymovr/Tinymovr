
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

#include <src/motor/motor.h>
#include <src/common.h>
#include <src/utils/utils.h>
#include <src/system/system.h>
#include <src/observer/observer.h>

static ObserverState state = {0};

static ObserverConfig config = {
		.track_bw = 1500.0f,
		.kp = 0.0f,
		.ki = 0.0f,
};

void observer_init(void)
{
	observer_reset();
    observer_set_bw(config.track_bw);
	// We keep local copies of a few key variables from
	// the encoder, because it is faster than calling
	// the encoder function pointer
	state.encoder_type = encoder_get_type();
	state.encoder_ticks = encoder_get_ticks();
	state.encoder_half_ticks = state.encoder_ticks/2;
}

void observer_reset(void)
{
	state.pos_sector = 0;
	state.pos_estimate_wrapped = 0;
	state.vel_estimate = 0;
}

TM_RAMFUNC void observer_update(void)
{
	const int16_t angle_meas = encoder_get_angle();
	const float delta_pos_est = PWM_PERIOD_S * state.vel_estimate;
	float delta_pos_meas = angle_meas - state.pos_estimate_wrapped;
	if (delta_pos_meas < -state.encoder_half_ticks)
	{
		delta_pos_meas += state.encoder_ticks;
	}
	else if (delta_pos_meas >= state.encoder_half_ticks)
	{
		delta_pos_meas -= state.encoder_ticks;
	}
	const float delta_pos_error = delta_pos_meas - delta_pos_est;
	const float incr_pos = delta_pos_est + (PWM_PERIOD_S * config.kp * delta_pos_error);
	state.pos_estimate_wrapped += incr_pos;
	if (state.pos_estimate_wrapped < 0)
	{
		state.pos_estimate_wrapped += state.encoder_ticks;
		state.pos_sector -= 1;
	}
	else if (state.pos_estimate_wrapped >= state.encoder_ticks)
	{
		state.pos_estimate_wrapped -= state.encoder_ticks;
		state.pos_sector += 1;
	}
	state.vel_estimate += PWM_PERIOD_S * config.ki * delta_pos_error;
}

float observer_get_bw(void)
{
    return config.track_bw;
}

void observer_set_bw(float bw)
{
    if (bw > 0.0f)
    {
        config.track_bw = bw;
		config.kp = 2.0f * config.track_bw;
    	config.ki = 0.25f * (config.kp * config.kp);
    }
}

TM_RAMFUNC float observer_get_pos_estimate(void)
{
	const float primary = state.encoder_ticks * state.pos_sector;
	return primary + state.pos_estimate_wrapped;
}

TM_RAMFUNC float observer_get_diff(float target)
{
	const float primary = state.encoder_ticks * state.pos_sector;
	const float diff_sector = target - primary;
	return diff_sector - state.pos_estimate_wrapped;
}

TM_RAMFUNC float observer_get_vel_estimate(void)
{
	return state.vel_estimate;
}

TM_RAMFUNC float observer_get_epos(void)
{
	if (ENCODER_MA7XX == state.encoder_type)
	{
		return state.pos_estimate_wrapped * twopi_by_enc_ticks * motor_get_pole_pairs();
	}
	return state.pos_estimate_wrapped * twopi_by_hall_sectors;
}

TM_RAMFUNC float observer_get_evel(void)
{
	if (ENCODER_MA7XX == state.encoder_type)
	{
		return state.vel_estimate * twopi_by_enc_ticks * motor_get_pole_pairs();
	}
	return state.vel_estimate * twopi_by_hall_sectors;
}

TM_RAMFUNC float observer_get_pos_estimate_user_frame(void)
{
	return (observer_get_pos_estimate() - motor_get_user_offset()) * motor_get_user_direction();
}

TM_RAMFUNC float observer_get_vel_estimate_user_frame(void)
{
	return state.vel_estimate * motor_get_user_direction();
}

ObserverConfig* Observer_GetConfig(void)
{
	return &config;
}

void Observer_RestoreConfig(ObserverConfig* config_)
{
	config = *config_;
}
