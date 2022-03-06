
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

#include <src/encoder/ma7xx.h>
#include <src/motor/motor.h>
#include <src/common.h>
#include <src/utils/utils.h>
#include <src/observer/observer.h>

static ObserverState state = {0};

static ObserverConfig config = {
		.track_bw = 1500.0f,
		.kp = 0.0f,
		.ki = 0.0f,
};

void Observer_Init(void)
{
    config.kp = 2.0f * config.track_bw;
    config.ki = 0.25f * (config.kp * config.kp);

	config.sector_half_interval = ENCODER_TICKS * 10;
}

PAC5XXX_RAMFUNC void observer_update_estimates(const int16_t angle_meas)
{
	const float delta_pos_est = PWM_PERIOD_S * state.vel_estimate;
	const float delta_pos_meas = wrapf_min_max((float)angle_meas - state.pos_estimate, -ENCODER_HALF_TICKS, ENCODER_HALF_TICKS);
	const float delta_pos_error = delta_pos_meas - delta_pos_est;
	const float incr_pos = delta_pos_est + (PWM_PERIOD_S * config.kp * delta_pos_error);
	state.pos_estimate += incr_pos;
	state.pos_estimate_wrapped = wrapf_min_max(state.pos_estimate_wrapped + incr_pos, -ENCODER_HALF_TICKS, ENCODER_HALF_TICKS);
	if (state.pos_estimate > config.sector_half_interval)
	{
		state.pos_estimate -= 2 * config.sector_half_interval;
		state.pos_sector += 1;
	}
	else if (state.pos_estimate < -(config.sector_half_interval) )
	{
		state.pos_estimate += 2 * config.sector_half_interval;
		state.pos_sector -= 1;
	}
	state.vel_estimate += PWM_PERIOD_S * config.ki * delta_pos_error;
}

PAC5XXX_RAMFUNC float Observer_GetFilterBandwidth(void)
{
    return config.track_bw;
}

void Observer_SetFilterBandwidth(float bw)
{
    if (bw > 0.0f)
    {
        config.track_bw = bw;
    }
}

PAC5XXX_RAMFUNC float Observer_GetPosEstimate(void)
{
	const float primary = 2 * config.sector_half_interval * state.pos_sector;
	return primary + state.pos_estimate;
}

PAC5XXX_RAMFUNC float observer_get_diff(float target)
{
	const float primary = 2 * config.sector_half_interval * state.pos_sector;
	const float diff_sector = target - primary;
	return diff_sector - state.pos_estimate;
}

PAC5XXX_RAMFUNC float observer_get_vel_estimate(void)
{
	return state.vel_estimate;
}

PAC5XXX_RAMFUNC float observer_get_epos(void)
{
	return state.pos_estimate_wrapped * twopi_by_enc_ticks * motor_get_pole_pairs();
}

PAC5XXX_RAMFUNC float observer_get_evel(void)
{
	return state.vel_estimate * twopi_by_enc_ticks * motor_get_pole_pairs();
}

PAC5XXX_RAMFUNC float observer_get_pos_estimate_user_frame(void)
{
	return (state.pos_estimate - motor_get_user_offset()) * motor_get_user_direction();
}

PAC5XXX_RAMFUNC float observer_get_vel_estimate_user_frame(void)
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
