
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

#include <src/common.h>

typedef enum {
	SENSOR_MA7XX = 0,
	SENSOR_HALL = 1
} SensorType;

typedef struct {
    SensorType desired_sensor_type;
} SensorConfig;

typedef struct {
    SensorType actual_sensor_type;
    uint8_getter get_error_ptr;
    bool_getter get_calibrated_ptr;
    bool_setter update_angle_ptr;
    int16_getter get_angle_ptr;
    void_func reset_sensor_ptr;
    uint16_t ticks;
    SensorConfig config;
} Sensor;

void sensor_init(Sensor *s);
void sensor_reset(Sensor *s);

int16_t sensor_get_angle(Sensor *s);
void sensor_update(Sensor *s, bool check_error);

uint16_t sensor_get_ticks(Sensor *s);
float sensor_ticks_to_eangle(Sensor *s);

SensorType sensor_get_type(Sensor *s);
void sensor_set_type(Sensor *s, SensorType sensor_type);

bool sensor_get_calibrated(Sensor *s);

uint8_t sensor_get_errors(Sensor *s);

SensorConfig* sensor_get_config(Sensor *s);
void sensor_restore_config(Sensor *s, SensorConfig* config_);

