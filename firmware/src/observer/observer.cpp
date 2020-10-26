
#include <src/common.hpp>
#include <src/encoder/Encoder.hpp>
#include <src/observer/observer.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include "src/utils/utils.h"

#ifdef __cplusplus
}
#endif

void Observer::Reset(void)
{
    config.pos_offset = 0;
    config.offset_calibrated = 0;
    config.direction = 1;
    config.direction_calibrated = 0;
}

PAC5XXX_RAMFUNC void Observer::UpdatePosEstimate(void)
{
	int new_pos_meas = systm.encoder.GetAngle();
	const float delta_pos_est = PWM_TIMER_PERIOD * vel_estimate;
	const float delta_pos_meas = wrapf(new_pos_meas - pos_estimate, ENCODER_HALF_CPR);
	const float delta_pos_error = delta_pos_meas - delta_pos_est;
	const float incr_pos = delta_pos_est + (PWM_TIMER_PERIOD * config.kp * delta_pos_error);
	pos_estimate += incr_pos;
	pos_estimate_wrapped = wrapf(pos_estimate_wrapped + incr_pos, ENCODER_HALF_CPR);
	if (pos_estimate > config.sector_half_interval)
	{
		pos_estimate -= 2 * config.sector_half_interval;
		pos_sector += 1;
	}
	else if (pos_estimate < -(config.sector_half_interval) )
	{
		pos_estimate += 2 * config.sector_half_interval;
		pos_sector -= 1;
	}
	else
	{
		// No action
	}
	vel_estimate += PWM_TIMER_PERIOD * config.ki * delta_pos_error;
}

PAC5XXX_RAMFUNC float Observer::GetBandwidth(void)
{
    return config.track_bw;
}

void Observer::SetBandwidth(float bw)
{
    if (bw > 0.0f)
    {
        config.track_bw = bw;
    }
}

PAC5XXX_RAMFUNC float Observer::GetPosEstimate(void)
{
	const float primary = 2 * config.sector_half_interval * pos_sector;
	return config.direction * (primary + pos_estimate - config.pos_offset);
}

PAC5XXX_RAMFUNC float Observer::GetPosDiff(float target)
{
	const float primary = 2 * config.sector_half_interval * pos_sector;
	const float diff_sector = target - ((float)config.direction * primary);
	return diff_sector - ((float)config.direction * (pos_estimate - config.pos_offset));
}

PAC5XXX_RAMFUNC float Observer::GetPosEstimateWrapped(void)
{
	// FIXME: Due to offset, returned value interval is [-pi - offset, pi - offset)
	// However, it is correct with respect to electrical origin
	return config.direction * (pos_estimate_wrapped - config.pos_offset);
}

PAC5XXX_RAMFUNC float Observer::GetPosEstimateWrappedRadians(void)
{
	// FIXME: Same as above
	const float est = Observer::GetPosEstimateWrapped();
	return (est / ENCODER_CPR ) * twopi;
}

PAC5XXX_RAMFUNC float Observer::GetVelEstimate(void)
{
	return config.direction * vel_estimate;
}

PAC5XXX_RAMFUNC int Observer::GetDirection(void)
{
	return config.direction;
}

void Observer::CalibrateDirection(float ref_pos)
{
    SetDirection(1);
	if (GetPosEstimate() < ref_pos)
	{
	SetDirection(-1);
	}
}

void Observer::SetDirection(int direction)
{
	if ((direction == -1) || (direction == 1))
	{
		config.direction_calibrated = true;
		config.direction = direction;
	}
}

PAC5XXX_RAMFUNC float Observer::GetOffset(void)
{
	return config.pos_offset;
}

void Observer::CalibrateOffset(void)
{
	SetOffset(GetPosEstimateWrapped());
}

void Observer::SetOffset(float offset)
{
	config.offset_calibrated = true;
	config.pos_offset = offset;
}

PAC5XXX_RAMFUNC bool Observer::Calibrated(void)
{
	return config.offset_calibrated && config.direction_calibrated;
}
