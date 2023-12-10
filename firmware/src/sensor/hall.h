
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

typedef struct Sensor Sensor;
typedef union SensorSpecificConfig SensorSpecificConfig;

typedef struct
{
	uint8_t sector_map[8];
    bool sector_map_calibrated;
} HallSensorConfig;

typedef struct
{
    uint8_t errors;
	uint16_t angle;
    uint8_t sector;
    uint8_t hw_defaults[3];
} HallSensorState;

bool hall_init_with_defaults(Sensor *s);
bool hall_init_with_config(Sensor *s, SensorSpecificConfig *c);
void hall_deinit(Sensor *s);
void hall_reset(Sensor *s);
uint8_t hall_get_errors(Sensor *s);
int16_t hall_get_angle(Sensor *s);
void hall_update(Sensor *s, bool check_error);
uint8_t hall_get_sector(Sensor *s);

bool hall_sector_map_is_calibrated(Sensor *s);
bool hall_calibrate_sequence(Sensor *s);


