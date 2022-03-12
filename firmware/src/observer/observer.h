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

typedef struct 
{
	int32_t pos_sector;
	float pos_estimate_wrapped;
	float vel_estimate;
} ObserverState;

typedef struct 
{
	float track_bw;
	float kp;
	float ki;
} ObserverConfig;

void Observer_Init(void);
PAC5XXX_RAMFUNC void observer_update_estimates(void);
PAC5XXX_RAMFUNC float Observer_GetPosEstimate(void);
PAC5XXX_RAMFUNC float observer_get_diff(float target);
PAC5XXX_RAMFUNC float observer_get_vel_estimate(void);
PAC5XXX_RAMFUNC float observer_get_epos(void);
PAC5XXX_RAMFUNC float observer_get_evel(void);

PAC5XXX_RAMFUNC float observer_get_pos_estimate_user_frame(void);
PAC5XXX_RAMFUNC float observer_get_vel_estimate_user_frame(void);

float observer_get_bw(void);
void observer_set_bw(float bw);

ObserverConfig* Observer_GetConfig(void);
void Observer_RestoreConfig(ObserverConfig* config_);

#endif /* OBSERVER_OBSERVER_H_ */
