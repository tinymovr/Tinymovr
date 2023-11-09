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
#include <src/sensors/sensor.h>

typedef struct 
{
	int32_t pos_sector;
	float pos_estimate_wrapped;
	float vel_estimate;
	uint16_t sensor_ticks;
	uint16_t sensor_half_ticks;
	sensor_type_t sensor_type : 4;
	bool current : 1;
} ObserverState;

typedef struct 
{
	float track_bw;
	float kp;
	float ki;
	uint8_t sensor_id;
} ObserverConfig;

typedef struct {
	ObserverConfig config;
	ObserverState state;
} Observer;

Observer observers[SENSOR_COUNT];
Observer *observer_commutation;
Observer *observer_position;

Observer observer_init(Sensor *s);
void observer_reset(Observer *o);

float observer_get_bw(Observer *o);
void observer_set_bw(Observer *o, float bw);

void make_default_observer_config(void);
uint32_t observers_config_length(void);
bool observers_serialize_config_to_buffer(uint8_t *buffer, uint32_t *len);
bool observers_initialize_with_config_buffer(const uint8_t *buffer, const uint32_t len);
 
inline void observer_update(Observer *o)
{
	if (o->state.current == false)
	{
		const int16_t angle_meas = sensors[o->config.sensor_idx]->get_angle_func();
		const float delta_pos_est = PWM_PERIOD_S * o->state.vel_estimate;
		float delta_pos_meas = angle_meas - o->state.pos_estimate_wrapped;
		if (delta_pos_meas < -(o->state.sensor_half_ticks))
		{
			delta_pos_meas += o->state.sensor_ticks;
		}
		else if (delta_pos_meas >= o->state.sensor_half_ticks)
		{
			delta_pos_meas -= o->state.sensor_ticks;
		}
		const float delta_pos_error = delta_pos_meas - delta_pos_est;
		const float incr_pos = delta_pos_est + (PWM_PERIOD_S * o->config.kp * delta_pos_error);
		o->state.pos_estimate_wrapped += incr_pos;
		if (o->state.pos_estimate_wrapped < 0)
		{
			o->state.pos_estimate_wrapped += o->state.sensor_ticks;
			o->state.pos_sector -= 1;
		}
		else if (o->state.pos_estimate_wrapped >= o->state.sensor_ticks)
		{
			o->state.pos_estimate_wrapped -= o->state.sensor_ticks;
			o->state.pos_sector += 1;
		}
		o->state.vel_estimate += PWM_PERIOD_S * o->config.ki * delta_pos_error;
		o->state.current = true;
	}
}

inline void observer_invalidate(Observer *o)
{
	o->state.current = false;
}

inline float observer_get_pos_estimate(Observer *o)
{
	const float primary = o->state.sensor_ticks * o->state.pos_sector;
	return primary + o->state.pos_estimate_wrapped;
}

inline float observer_get_diff(Observer *o, float target)
{
	const float primary = o->state.sensor_ticks * o->state.pos_sector;
	const float diff_sector = target - primary;
	return diff_sector - o->state.pos_estimate_wrapped;
}

inline float observer_get_vel_estimate(Observer *o)
{
	return o->state.vel_estimate;
}

inline float observer_get_epos(Observer *o)
{
	if (SENSOR_MA7XX == o->state.sensor_type)
	{
		return o->state.pos_estimate_wrapped * twopi_by_enc_ticks * motor_get_pole_pairs();
	}
	return o->state.pos_estimate_wrapped * twopi_by_hall_sectors;
}

inline float observer_get_evel(Observer *o)
{
	if (SENSOR_MA7XX == o->state.sensor_type)
	{
		return o->state.vel_estimate * twopi_by_enc_ticks * motor_get_pole_pairs();
	}
	return o->state.vel_estimate * twopi_by_hall_sectors;
}

inline float observer_get_pos_estimate_user_frame(Observer *o)
{
	return (observer_get_pos_estimate() - motor_get_user_offset()) * motor_get_user_direction();
}

inline float observer_get_vel_estimate_user_frame(Observer *o)
{
	return o->state.vel_estimate * motor_get_user_direction();
}
