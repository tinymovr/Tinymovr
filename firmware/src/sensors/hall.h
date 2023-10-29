
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
} HallSensorState;

Sensor hall_init(void);
Sensor hall_init_with_config(SensorSpecificConfig *c);
void hall_deinit(Sensor *s);

void hall_clear_sector_map(Sensor *s);
void hall_set_sector_map_calibrated(Sensor *s);
bool hall_sector_map_is_calibrated(Sensor *s);
uint8_t *hall_get_sector_map_ptr(Sensor *s);

inline uint8_t hall_get_errors(Sensor *s)
{
    return s->state.hall_state.errors;
}

inline uint8_t *hall_get_error_ptr(Sensor *s)
{
    return &(s->state.hall_state.errors);
}

inline int16_t hall_get_angle(Sensor *s)
{
    return s->state.hall_state.angle;
}

inline void hall_update(Sensor *s, bool check_error)
{
    const uint8_t sector = (pac5xxx_tile_register_read(ADDR_DINSIG1) >> 1) & 0x07;
    s->state.hall_state.sector = sector;
    s->state.hall_state.angle = config.sector_map[state.sector];
}

inline uint8_t hall_get_sector(Sensor *s)
{
    return s->state.hall_state.sector;
}