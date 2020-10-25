/* 
 * This file is part of the Tinymovr-Firmware distribution
 * (https://github.com/yconst/tinymovr-firmware).
 * Copyright (c) 2020 Ioannis Chatzikonstantinou.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OBSERVER_OBSERVER_H_
#define OBSERVER_OBSERVER_H_

#include <stdint.h>
#include <src/common.hpp>

class Observer
{
public:
	void Observer_Reset(void);
	PAC5XXX_RAMFUNC void UpdatePosEstimate(void);
	PAC5XXX_RAMFUNC float GetPosEstimate(void);
	PAC5XXX_RAMFUNC float GetPosDiff(float target);
	PAC5XXX_RAMFUNC float GetPosEstimateWrapped(void);
	PAC5XXX_RAMFUNC float GetPosEstimateWrappedRadians(void);
	PAC5XXX_RAMFUNC float GetVelEstimate(void);
	PAC5XXX_RAMFUNC int GetDirection(void);
	void Observer_CalibrateDirection(float ref_pos); // Considers POSITIVE electrical phase
	void Observer_SetDirection(int dir);
	PAC5XXX_RAMFUNC float Observer_GetOffset(void);
	void Observer_CalibrateOffset(void);
	void Observer_SetOffset(float offset);
	bool Calibrated(void);
private:
	struct ObserverConfig
	{
		float track_bw = OBSERVER_TRACK_BW;
		float kp = 2.0f * OBSERVER_TRACK_BW;
		float ki = OBSERVER_TRACK_BW * OBSERVER_TRACK_BW; //0.25f * (config.kp * config.kp);
		int32_t sector_half_interval = ENCODER_CPR * 10;
		float pos_offset = 0.0f;     // offset array for each sensor
		bool offset_calibrated = false;
		int direction = 0;      // direction 1 positive, -1 negative
		bool direction_calibrated = false;
	};
	ObserverConfig config;
	float pos_estimate = 0.0f;
	int32_t pos_sector = 0;
	float pos_estimate_wrapped = 0.0f;
	float vel_estimate = 0.0f;
}

#endif /* OBSERVER_OBSERVER_H_ */
