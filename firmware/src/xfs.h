//  * This file is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * Copyright (c) 2020-2023 Ioannis Chatzikonstantinou.
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

#pragma once

#include <src/xf1.h>

typedef struct {
	FrameTransform position_sensor_to_user;
	FrameTransform user_to_position_sensor;
	FrameTransform position_sensor_to_motor;
	FrameTransform motor_to_position_sensor;
	FrameTransform commutation_sensor_to_motor;
	FrameTransform motor_to_commutation_sensor;
	FrameTransform motor_to_user;
	FrameTransform user_to_motor;
} FramesConfig;

extern FramesConfig frames;

static inline void frames_get_config(FramesConfig *_config)
{
	*_config = frames;
}

static inline void frames_restore_config(FramesConfig *_config)
{
    frames = *_config;
}

static inline FrameTransform *frame_position_sensor_to_user_p(void)
{
    return &(frames.position_sensor_to_user);
}

static inline FrameTransform *frame_user_to_position_sensor_p(void)
{
    return &(frames.user_to_position_sensor);
}

static inline FrameTransform *frame_position_sensor_to_motor_p(void)
{
    return &(frames.position_sensor_to_motor);
}

static inline FrameTransform *frame_motor_to_position_sensor_p(void)
{
    return &(frames.motor_to_position_sensor);
}

static inline FrameTransform *frame_commutation_sensor_to_motor_p(void)
{
    return &(frames.commutation_sensor_to_motor);
}

static inline FrameTransform *frame_motor_to_commutation_sensor_p(void)
{
    return &(frames.motor_to_commutation_sensor);
}

static inline FrameTransform *frame_motor_to_user_p(void)
{
    return &(frames.motor_to_user);
}

static inline FrameTransform *frame_user_to_motor_p(void)
{
    return &(frames.user_to_motor);
}

static inline float frame_user_to_position_sensor_get_offset(void)
{
	return frame_user_to_position_sensor_p()->offset;
}

static inline float frame_user_to_position_sensor_get_multiplier(void)
{
	return frame_user_to_position_sensor_p()->multiplier;
}

static inline void frame_user_to_position_sensor_set_offset(float value)
{
    frames.user_to_position_sensor.offset = value;
    frames.position_sensor_to_user = derive_inverse_transform(frame_user_to_position_sensor_p());
    frames.user_to_motor = combine_transforms(frame_user_to_position_sensor_p(), frame_position_sensor_to_motor_p());
    frames.motor_to_user = derive_inverse_transform(frame_user_to_motor_p());
}

static inline void frame_user_to_position_sensor_set_multiplier(float value)
{
    if (value != 0)
    {
        frames.user_to_position_sensor.multiplier = value;
        frames.position_sensor_to_user = derive_inverse_transform(frame_user_to_position_sensor_p());
        frames.user_to_motor = combine_transforms(frame_user_to_position_sensor_p(), frame_position_sensor_to_motor_p());
        frames.motor_to_user = derive_inverse_transform(frame_user_to_motor_p());
    }
}

static inline void frame_set_commutation_sensor_to_motor(const FrameTransform value)
{
    frames.commutation_sensor_to_motor = value;
    frames.motor_to_commutation_sensor = derive_inverse_transform(frame_commutation_sensor_to_motor_p());
}

static inline void frame_set_position_sensor_to_motor(const FrameTransform value)
{
    frames.position_sensor_to_motor = value;
    frames.motor_to_position_sensor = derive_inverse_transform(&value);
    frames.user_to_motor = combine_transforms(frame_user_to_position_sensor_p(), frame_position_sensor_to_motor_p());
    frames.motor_to_user = derive_inverse_transform(frame_user_to_motor_p());
}
