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
#include <src/sensor/sensors.h>
#include <src/xfs.h>

typedef struct Observer Observer;

typedef struct 
{
	float track_bw;
	float kp;
	float ki;
} ObserverConfig;

struct Observer {
	ObserverConfig config;
	Sensor **sensor_ptr;
	int32_t pos_sector;
	float pos_estimate_wrapped;
	float vel_estimate;
	bool initialized : 1;
	bool current : 1;
};

typedef struct {
    ObserverConfig config_commutation;
	ObserverConfig config_position;
} ObserversConfig;

Observer commutation_observer;
Observer position_observer;

bool observer_init_with_defaults(Observer *o, Sensor **s);
bool observer_init_with_config(Observer *o, Sensor **s, ObserverConfig *c);

float observer_get_bandwidth(Observer *o);
void observer_set_bandwidth(Observer *o, float bw);

void observers_init_with_defaults(void);
void observers_get_config(ObserversConfig *config_);
void observers_restore_config(ObserversConfig *config_);
 
static inline void observer_update(Observer *o)
{
	if (o->current == false)
	{
		const float sensor_res_norm_factor = SENSOR_COMMON_RES_TICKS_FLOAT / sensor_get_ticks(*(o->sensor_ptr));
		const float angle_meas = sensor_get_angle_rectified(*(o->sensor_ptr)) * sensor_res_norm_factor;
		const float delta_pos_est = PWM_PERIOD_S * o->vel_estimate;
		float delta_pos_meas = angle_meas - o->pos_estimate_wrapped;
		if (delta_pos_meas < -SENSOR_COMMON_RES_HALF_TICKS)
		{
			delta_pos_meas += SENSOR_COMMON_RES_TICKS;
		}
		else if (delta_pos_meas >= SENSOR_COMMON_RES_HALF_TICKS)
		{
			delta_pos_meas -= SENSOR_COMMON_RES_TICKS;
		}
		const float delta_pos_error = delta_pos_meas - delta_pos_est;
		const float incr_pos = delta_pos_est + (PWM_PERIOD_S * o->config.kp * delta_pos_error);
		o->pos_estimate_wrapped += incr_pos;
		if (o->pos_estimate_wrapped < 0)
		{
			o->pos_estimate_wrapped += SENSOR_COMMON_RES_TICKS;
			o->pos_sector -= 1;
		}
		else if (o->pos_estimate_wrapped >= SENSOR_COMMON_RES_TICKS)
		{
			o->pos_estimate_wrapped -= SENSOR_COMMON_RES_TICKS;
			o->pos_sector += 1;
		}
		o->vel_estimate += PWM_PERIOD_S * o->config.ki * delta_pos_error;
		o->current = true;
	}
}

static inline void observer_invalidate(Observer *o)
{
	o->current = false;
}

static inline float observer_get_pos_estimate(Observer *o)
{
	const float primary = SENSOR_COMMON_RES_TICKS * o->pos_sector;
	return primary + o->pos_estimate_wrapped;
}

static inline float get_diff_position_sensor_frame(float target)
{
	const float primary = SENSOR_COMMON_RES_TICKS * position_observer.pos_sector;
	const float diff_sector = target - primary;
	return diff_sector - position_observer.pos_estimate_wrapped;
}

static inline float observer_get_vel_estimate(Observer *o)
{
	return o->vel_estimate;
}

// Interface functions

static inline float get_position_observer_to_user_offset(void)
{
	return frame_position_sensor_to_user_p()->offset;
}

static inline float get_position_observer_to_user_multiplier(void)
{
	return frame_position_sensor_to_user_p()->multiplier;
}

static inline void set_position_observer_to_user_offset(float value)
{
	frame_position_sensor_to_user_p()->offset = value;
}

static inline void set_position_observer_to_user_multiplier(float value)
{
	frame_position_sensor_to_user_p()->offset = value;
}

static inline float commutation_observer_get_bandwidth(void)
{
	return observer_get_bandwidth(&commutation_observer);
}

void commutation_observer_set_bandwidth(float bw);

static inline float position_observer_get_bandwidth(void)
{
	return observer_get_bandwidth(&position_observer);
}

void position_observer_set_bandwidth(float bw);

static inline float commutation_observer_get_pos_estimate(void)
{
	return observer_get_pos_estimate(&commutation_observer);
}

static inline float commutation_observer_get_vel_estimate(void)
{
	return observer_get_vel_estimate(&commutation_observer);
}

static inline float position_observer_get_pos_estimate(void)
{
	return observer_get_pos_estimate(&position_observer);
}

static inline float position_observer_get_vel_estimate(void)
{
	return observer_get_vel_estimate(&position_observer);
}

static inline float user_frame_get_pos_estimate(void)
{
	return apply_transform(position_observer_get_pos_estimate(), frame_position_sensor_to_user_p());
}

static inline float user_frame_get_vel_estimate(void)
{
	return apply_velocity_transform(position_observer_get_vel_estimate(), frame_position_sensor_to_user_p());
}

static inline float motor_frame_get_pos_estimate(void)
{
	return apply_transform(commutation_observer_get_pos_estimate(), frame_commutation_sensor_to_motor_p());
}

static inline float motor_frame_get_vel_estimate(void)
{
	return apply_velocity_transform(commutation_observer_get_vel_estimate(), frame_commutation_sensor_to_motor_p());
}

static inline float observer_get_epos_motor_frame(void)
{
	if (SENSOR_TYPE_HALL == ((*(commutation_observer.sensor_ptr))->config.type))
	{
		return motor_frame_get_pos_estimate() * twopi_by_hall_sectors;
	}
	return motor_frame_get_pos_estimate() * twopi_by_enc_ticks * motor_get_pole_pairs();
}

static inline float observer_get_evel_motor_frame(void)
{
	if (SENSOR_TYPE_HALL == ((*(commutation_observer.sensor_ptr))->config.type))
	{
		return motor_frame_get_vel_estimate() * twopi_by_hall_sectors;
	}
	return motor_frame_get_vel_estimate() * twopi_by_enc_ticks * motor_get_pole_pairs();
}