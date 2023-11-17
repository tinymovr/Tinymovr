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

#pragma once

#include <stdint.h>
#include <src/common.h>
#include <src/sensor/sensor.h>

typedef struct 
{
	float track_bw;
	float kp;
	float ki;
	uint8_t sensor_id;
} ObserverConfig;

typedef struct {
	ObserverConfig config;
	Sensor *sensor_ptr;
	int32_t pos_sector;
	float pos_estimate_wrapped;
	float vel_estimate;
	uint16_t sensor_ticks;
	uint16_t sensor_half_ticks;
	sensor_type_t sensor_type : 4;
	bool initialized : 1;
	bool current : 1;
} Observer;

typedef struct {
    ObserverConfig config_commutation;
	ObserverConfig config_position;
} ObserversConfig;

Observer observer_commutation;
Observer observer_position;

bool observer_init_with_defaults(Observer *o, Sensor *s);
bool observer_init_with_config(Observer *o, Sensor *s, ObserverConfig *c);
void observer_reset(Observer *o);

float observer_get_bw(Observer *o);
void observer_set_bw(Observer *o, float bw);

void observers_init_with_defaults(void);
void observers_get_config(ObserversConfig *config_);
void observers_restore_config(ObserversConfig *config_);
 
inline void observer_update(Observer *o)
{
	if (o->current == false)
	{
		const int16_t angle_meas = o->sensor_ptr->get_angle_func(o->sensor_ptr);
		const float delta_pos_est = PWM_PERIOD_S * o->vel_estimate;
		float delta_pos_meas = angle_meas - o->pos_estimate_wrapped;
		if (delta_pos_meas < -(o->sensor_half_ticks))
		{
			delta_pos_meas += o->sensor_ticks;
		}
		else if (delta_pos_meas >= o->sensor_half_ticks)
		{
			delta_pos_meas -= o->sensor_ticks;
		}
		const float delta_pos_error = delta_pos_meas - delta_pos_est;
		const float incr_pos = delta_pos_est + (PWM_PERIOD_S * o->config.kp * delta_pos_error);
		o->pos_estimate_wrapped += incr_pos;
		if (o->pos_estimate_wrapped < 0)
		{
			o->pos_estimate_wrapped += o->sensor_ticks;
			o->pos_sector -= 1;
		}
		else if (o->pos_estimate_wrapped >= o->sensor_ticks)
		{
			o->pos_estimate_wrapped -= o->sensor_ticks;
			o->pos_sector += 1;
		}
		o->vel_estimate += PWM_PERIOD_S * o->config.ki * delta_pos_error;
		o->current = true;
	}
}

inline void observer_invalidate(Observer *o)
{
	o->current = false;
}

inline float observer_get_pos_estimate(Observer *o)
{
	const float primary = o->sensor_ticks * o->pos_sector;
	return primary + o->pos_estimate_wrapped;
}

inline float observer_get_diff(Observer *o, float target)
{
	const float primary = o->sensor_ticks * o->pos_sector;
	const float diff_sector = target - primary;
	return diff_sector - o->pos_estimate_wrapped;
}

inline float observer_get_vel_estimate(Observer *o)
{
	return o->vel_estimate;
}

static inline float observer_get_epos(Observer *o)
{
	if (SENSOR_TYPE_MA7XX == o->sensor_type)
	{
		return o->pos_estimate_wrapped * twopi_by_enc_ticks * motor_get_pole_pairs();
	}
	return o->pos_estimate_wrapped * twopi_by_hall_sectors;
}

static inline float observer_get_evel(Observer *o)
{
	if (SENSOR_TYPE_MA7XX == o->sensor_type)
	{
		return o->vel_estimate * twopi_by_enc_ticks * motor_get_pole_pairs();
	}
	return o->vel_estimate * twopi_by_hall_sectors;
}

inline float observer_get_pos_estimate_user_frame(Observer *o)
{
	return (observer_get_pos_estimate(o) - motor_get_user_offset()) * motor_get_user_direction();
}

inline float observer_get_vel_estimate_user_frame(Observer *o)
{
	return o->vel_estimate * motor_get_user_direction();
}
