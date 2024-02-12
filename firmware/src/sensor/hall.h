
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

#include <src/sensor/sensor.h>

#define HALL_BITS (3)
#define HALL_SECTORS (HALL_BITS - 2)
#define HALL_SECTOR_ANGLE (TWOPI / HALL_SECTORS)
#define CAL_DIR_LEN_PER_SECTOR (CAL_DIR_LEN / HALL_SECTORS)

static const float twopi_by_hall_sectors = TWOPI / HALL_SECTORS;

typedef struct Observer Observer;

typedef struct
{
	uint8_t sector_map[8];
    bool sector_map_calibrated;
} HallSensorConfig;

typedef struct
{
    Sensor base;
    HallSensorConfig config;
    uint8_t errors;
	int32_t angle;
    uint8_t sector;
    uint8_t hw_defaults[3];
} HallSensor;

void hall_make_blank_sensor(Sensor *s);
bool hall_init_with_defaults(Sensor *s);
bool hall_init_with_config(Sensor *s, const HallSensorConfig *c);
void hall_deinit(Sensor *s);
void hall_reset(Sensor *s);
bool hall_calibrate_sequence(Sensor *s, Observer *o);
void hall_get_ss_config(Sensor *s, void* buffer);

static inline uint8_t hall_get_errors(const Sensor *s)
{
    return ((HallSensor *)s)->errors;
}

static inline int32_t hall_get_angle(const Sensor *s)
{
    return ((HallSensor *)s)->angle;
}

static inline void hall_update(Sensor *s, bool check_error)
{
    HallSensor *ms = (HallSensor *)s;
    const uint8_t sector = (pac5xxx_tile_register_read(ADDR_DINSIG1) >> 1) & 0x07;
    ms->sector = sector;
    ms->angle = ms->config.sector_map[ms->sector];
}

static inline uint8_t hall_get_sector(const Sensor *s)
{
    return ((const HallSensor *)s)->sector;
}

static inline bool hall_sector_map_is_calibrated(const Sensor *s)
{
    return ((const HallSensor *)s)->config.sector_map_calibrated;
}

