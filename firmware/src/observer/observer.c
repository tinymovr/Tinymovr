
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

#include <string.h>
#include <src/encoder/encoder.h>
#include "src/common.h"
#include "src/utils/utils.h"
#include "observer.h"

static struct ObserverState state = {0};

static struct ObserverConfig config = {
		.track_bw = 1500.0f,
		.kp = 0.0f,
		.ki = 0.0f,
		.eccentricity_table = {0},
		.eccentricity_calibrated = 0
};

void Observer_Init(void)
{
    config.kp = 2.0f * config.track_bw;
    config.ki = 0.25f * (config.kp * config.kp);

	config.sector_half_interval = ENCODER_TICKS * 10;
}

PAC5XXX_RAMFUNC void Observer_UpdateEstimates(void)
{
	const int16_t raw = MA_GetAngle();
	const int16_t off_1 = config.eccentricity_table[raw>>ECN_BITS];
	const int16_t off_2 = config.eccentricity_table[((raw>>ECN_BITS) + 1) % ECN_SIZE];
	const int16_t off_interp = off_1 + ((off_2 - off_1)* (raw - ((raw>>ECN_BITS)<<ECN_BITS))>>ECN_BITS);
	const int16_t angle_meas = raw + off_interp;

	const float delta_pos_est = PWM_PERIOD_S * state.vel_estimate;
	const float delta_pos_meas = wrapf((float)angle_meas - state.pos_estimate, ENCODER_HALF_TICKS);
	const float delta_pos_error = delta_pos_meas - delta_pos_est;
	const float incr_pos = delta_pos_est + (PWM_PERIOD_S * config.kp * delta_pos_error);
	state.pos_estimate += incr_pos;
	state.pos_estimate_wrapped = wrapf(state.pos_estimate_wrapped + incr_pos, ENCODER_HALF_TICKS);
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

PAC5XXX_RAMFUNC float Observer_GetBandwidth(void)
{
    return config.track_bw;
}

void Observer_SetBandwidth(float bw)
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

PAC5XXX_RAMFUNC float Observer_GetPosDiff(float target)
{
	const float primary = 2 * config.sector_half_interval * state.pos_sector;
	const float diff_sector = target - primary;
	return diff_sector - state.pos_estimate;
}

PAC5XXX_RAMFUNC float Observer_GetPosEstimateWrapped(void)
{
	return state.pos_estimate_wrapped;
}

PAC5XXX_RAMFUNC float Observer_GetPosEstimateWrappedRadians(void)
{
	return (Observer_GetPosEstimateWrapped() / ENCODER_TICKS ) * twopi;
}

PAC5XXX_RAMFUNC float Observer_GetVelEstimate(void)
{
	return state.vel_estimate;
}

PAC5XXX_RAMFUNC float Observer_GetVelEstimateRadians(void)
{
	return (Observer_GetVelEstimate() / ENCODER_TICKS ) * twopi;

}

void Observer_ClearEccentricityTable(void)
{
    (void)memset(config.eccentricity_table, 0, sizeof(config.eccentricity_table));
	config.eccentricity_calibrated = false;
}

void Observer_SetEccentricityCalibrated(void)
{
	config.eccentricity_calibrated = true;
}

int16_t *Observer_GetEccentricityTablePointer(void)
{
    return config.eccentricity_table;
}

PAC5XXX_RAMFUNC bool Observer_Calibrated(void)
{
	return config.eccentricity_calibrated;
}

struct ObserverConfig* Observer_GetConfig(void)
{
	return &config;
}

void Observer_RestoreConfig(struct ObserverConfig* config_)
{
	config = *config_;
}
