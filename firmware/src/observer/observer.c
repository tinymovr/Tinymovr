
#include <src/encoder/encoder.h>
#include "src/common.h"
#include "src/utils/utils.h"
#include "observer.h"

PAC5XXX_RAMFUNC static inline void Observer_UpdatePosEstimate(int newMeas);

static struct ObserverState state = {
		.pos_estimate = 0.0f,
		.pos_sector = 0,
		.pos_estimate_wrapped = 0.0f,
		.vel_estimate = 0.0f
};

static struct ObserverConfig config = {
		.track_bw = 1000.0f,
		.kp = 0.0f,
		.ki = 0.0f,
		.pos_offset = 0.0f,
		.offset_calibrated = 0,
		.direction_calibrated = 0,
		.direction = 1,
};

void Observer_Init(void)
{
    config.kp = 2.0f * config.track_bw;
    config.ki = 0.25f * (config.kp * config.kp);

	config.sector_half_interval = ENCODER_TICKS * 10;
}

void Observer_Reset(void)
{
    config.pos_offset = 0;
    config.offset_calibrated = 0;
    config.direction = 1;
    config.direction_calibrated = 0;
}

PAC5XXX_RAMFUNC static inline void Observer_UpdatePosEstimate(int new_pos_meas)
{
	const float delta_pos_est = PWM_TIMER_PERIOD * state.vel_estimate;
	const float delta_pos_meas = wrapf(new_pos_meas - state.pos_estimate, ENCODER_HALF_TICKS);
	const float delta_pos_error = delta_pos_meas - delta_pos_est;
	const float incr_pos = delta_pos_est + (PWM_TIMER_PERIOD * config.kp * delta_pos_error);
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
	state.vel_estimate += PWM_TIMER_PERIOD * config.ki * delta_pos_error;
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
	return config.direction * (primary + state.pos_estimate - config.pos_offset);
}

PAC5XXX_RAMFUNC float Observer_GetPosDiff(float target)
{
	const float primary = 2 * config.sector_half_interval * state.pos_sector;
	const float diff_sector = target - ((float)config.direction * primary);
	return diff_sector - ((float)config.direction * (state.pos_estimate - config.pos_offset));
}

PAC5XXX_RAMFUNC float Observer_GetPosEstimateWrapped(void)
{
	// FIXME: Due to offset, returned value interval is [-pi - offset, pi - offset)
	// However, it is correct with respect to electrical origin
	return config.direction * (state.pos_estimate_wrapped - config.pos_offset);
}

PAC5XXX_RAMFUNC float Observer_GetPosEstimateWrappedRadians(void)
{
	// FIXME: Same as above
	const float est = Observer_GetPosEstimateWrapped();
	return (est / ENCODER_TICKS ) * twopi;
}

PAC5XXX_RAMFUNC float Observer_GetVelEstimate(void)
{
	return config.direction * state.vel_estimate;
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

PAC5XXX_RAMFUNC float Observer_GetOffset(void)
{
	return config.pos_offset;
}

void Observer_CalibrateOffset(void)
{
	Observer_SetOffset(Observer_GetPosEstimateWrapped());
}

void Observer_SetOffset(float offset)
{
	config.offset_calibrated = true;
	config.pos_offset = offset;
}

PAC5XXX_RAMFUNC bool Observer_Calibrated(void)
{
	return config.offset_calibrated && config.direction_calibrated;
}

struct ObserverConfig* Observer_GetConfig(void)
{
	return &config;
}

void Observer_RestoreConfig(struct ObserverConfig* config_)
{
	config = *config_;
}

PAC5XXX_RAMFUNC void Observer_UpdatePos(void)
{
	MA_ReadAngle();
	Observer_UpdatePosEstimate(MA_GetAngle());
}
