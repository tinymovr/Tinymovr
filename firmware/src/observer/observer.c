
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
		.track_bw = 1000.0f,
		.kp = 0.0f,
		.ki = 0.0f,
		.direction_calibrated = 0,
		.direction = 1,
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
	int16_t raw = MA_GetAngle();
	int16_t off_1 = config.eccentricity_table[raw>>ECN_BITS];
	int16_t off_2 = config.eccentricity_table[((raw>>ECN_BITS) + 1) % ECN_SIZE];
	int16_t off_interp = off_1 + ((off_2 - off_1)* (raw - ((raw>>ECN_BITS)<<ECN_BITS))>>ECN_BITS);
	float angle_meas = (float)(raw + off_interp);

	const float delta_pos_est = PWM_PERIOD_S * state.vel_estimate;
	const float delta_pos_meas = wrapf(angle_meas - state.pos_estimate, ENCODER_HALF_TICKS);
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
	else
	{
		// No action
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
	return config.direction * (primary + state.pos_estimate);
}

PAC5XXX_RAMFUNC float Observer_GetPosDiff(float target)
{
	const float primary = 2 * config.sector_half_interval * state.pos_sector;
	const float diff_sector = target - ((float)config.direction * primary);
	return diff_sector - ((float)config.direction * state.pos_estimate);
}

PAC5XXX_RAMFUNC float Observer_GetPosEstimateWrapped(void)
{
	return config.direction * state.pos_estimate_wrapped;
}

PAC5XXX_RAMFUNC float Observer_GetPosEstimateWrappedRadians(void)
{
	return (Observer_GetPosEstimateWrapped() / ENCODER_TICKS ) * twopi;
}

PAC5XXX_RAMFUNC float Observer_GetVelEstimate(void)
{
	return config.direction * state.vel_estimate;
}

PAC5XXX_RAMFUNC float Observer_GetVelEstimateRadians(void)
{
	return (Observer_GetVelEstimate() / ENCODER_TICKS ) * twopi;

}

PAC5XXX_RAMFUNC int Observer_GetDirection(void)
{
	return config.direction;
}

void Observer_CalibrateDirection(float ref_pos)
{
    Observer_SetDirection(1);
	if (Observer_GetPosEstimate() < ref_pos)
	{
		Observer_SetDirection(-1);
	}
}

void Observer_SetDirection(int direction)
{
	if ((direction == -1) || (direction == 1))
	{
		config.direction_calibrated = true;
		config.direction = direction;
	}
}

void Observer_ClearDirection(void)
{
	config.direction_calibrated = false;
	config.direction = 1;
}

void Observer_ClearEccentricityTable(void)
{
    memset(config.eccentricity_table, 0, sizeof(config.eccentricity_table));
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
	return config.direction_calibrated & config.eccentricity_calibrated;
}

struct ObserverConfig* Observer_GetConfig(void)
{
	return &config;
}

void Observer_RestoreConfig(struct ObserverConfig* config_)
{
	config = *config_;
}
