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

#include <stdbool.h>
#include <src/xf1.h>

typedef enum {
    none_calibrated = 0,
    position_sensor_to_user_calibrated = 1,
    user_to_position_sensor_calibrated = 1 << 1,
    position_sensor_to_motor_calibrated = 1 << 2,
    motor_to_position_sensor_calibrated = 1 << 3,
    commutation_sensor_to_motor_calibrated = 1 << 4,
    motor_to_commutation_sensor_calibrated = 1 << 5,
    motor_to_user_calibrated = 1 << 6,
    user_to_motor_calibrated = 1 << 7,
    default_calibrated = (position_sensor_to_user_calibrated | user_to_position_sensor_calibrated),
    all_calibrated = 0xFF
} FramesOrder;


typedef struct {
	FrameTransform position_sensor_to_user;
	FrameTransform user_to_position_sensor;
	FrameTransform position_sensor_to_motor;
	FrameTransform motor_to_position_sensor;
	FrameTransform commutation_sensor_to_motor;
	FrameTransform motor_to_commutation_sensor;
	FrameTransform motor_to_user;
	FrameTransform user_to_motor;
    FramesOrder calibrated;
} FramesConfig;

extern FramesConfig frames;

static inline bool frames_get_calibrated(void)
{
    return (frames.calibrated == all_calibrated);
}

static inline void frames_reset_calibrated(void)
{
    frames.calibrated = default_calibrated;
}

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
    frames.calibrated |= (position_sensor_to_user_calibrated | 
        user_to_position_sensor_calibrated | motor_to_user_calibrated | user_to_motor_calibrated);
}

static inline void frame_user_to_position_sensor_set_multiplier(float value)
{
    if (value != 0)
    {
        frames.user_to_position_sensor.multiplier = value;
        frames.position_sensor_to_user = derive_inverse_transform(frame_user_to_position_sensor_p());
        frames.user_to_motor = combine_transforms(frame_user_to_position_sensor_p(), frame_position_sensor_to_motor_p());
        frames.motor_to_user = derive_inverse_transform(frame_user_to_motor_p());
        frames.calibrated |= (position_sensor_to_user_calibrated | 
            user_to_position_sensor_calibrated | motor_to_user_calibrated | user_to_motor_calibrated);
    }
}

static inline void frame_set_commutation_sensor_to_motor(const FrameTransform value)
{
    frames.commutation_sensor_to_motor = value;
    frames.motor_to_commutation_sensor = derive_inverse_transform(frame_commutation_sensor_to_motor_p());
    frames.calibrated |= (commutation_sensor_to_motor_calibrated | motor_to_commutation_sensor_calibrated);
}

static inline void frame_set_position_sensor_to_motor(const FrameTransform value)
{
    frames.position_sensor_to_motor = value;
    frames.motor_to_position_sensor = derive_inverse_transform(&value);
    frames.user_to_motor = combine_transforms(frame_user_to_position_sensor_p(), frame_position_sensor_to_motor_p());
    frames.motor_to_user = derive_inverse_transform(frame_user_to_motor_p());
    frames.calibrated |= (position_sensor_to_motor_calibrated |
        motor_to_position_sensor_calibrated | motor_to_user_calibrated | user_to_motor_calibrated);
}
