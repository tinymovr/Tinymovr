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
#include "src/common.h"

struct ObserverState
{
	float pos_estimate;
	int32_t pos_sector;
	float pos_estimate_wrapped;
	float vel_estimate;
};

struct ObserverConfig
{
	float track_bw;
	float kp;
	float ki;

	int32_t sector_half_interval;

	bool eccentricity_calibrated;
	int16_t eccentricity_table[ECN_SIZE];
};

void Observer_Init(void);
PAC5XXX_RAMFUNC void observer_update_estimates(const int16_t raw_pos);
PAC5XXX_RAMFUNC float Observer_GetPosEstimate(void);
PAC5XXX_RAMFUNC float observer_get_diff(float target);
PAC5XXX_RAMFUNC float observer_get_vel_estimate(void);
PAC5XXX_RAMFUNC float observer_get_epos(void);
PAC5XXX_RAMFUNC float observer_get_evel(void);

PAC5XXX_RAMFUNC float observer_get_pos_estimate_user_frame(void);
PAC5XXX_RAMFUNC float observer_get_vel_estimate_user_frame(void);

PAC5XXX_RAMFUNC float Observer_GetFilterBandwidth(void);
void Observer_SetFilterBandwidth(float bw);
void Observer_ClearEccentricityTable(void);
void Observer_SetEccentricityCalibrated(void);
int16_t *Observer_GetEccentricityTablePointer(void);

PAC5XXX_RAMFUNC bool Observer_Calibrated(void);

struct ObserverConfig* Observer_GetConfig(void);
void Observer_RestoreConfig(struct ObserverConfig* config_);

#endif /* OBSERVER_OBSERVER_H_ */
