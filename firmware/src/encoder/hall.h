
//  * This file is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * Copyright (c) 2020 Ioannis Chatzikonstantinou.
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

typedef struct
{
	uint8_t sector_map[8];
    bool sector_map_calibrated;
} HallConfig;

typedef struct
{
    uint8_t errors;
	uint16_t angle;
    uint8_t sector;
} HallState;

void hall_init(void);

PAC5XXX_RAMFUNC uint8_t hall_get_errors(void);
PAC5XXX_RAMFUNC int16_t hall_get_angle(void);
PAC5XXX_RAMFUNC void hall_update(bool check_error);
PAC5XXX_RAMFUNC uint8_t hall_get_sector(void);

void hall_clear_sector_map(void);
void hall_set_sector_map_calibrated(void);
bool hall_sector_map_is_calibrated(void);
uint8_t *hall_get_sector_map_ptr(void);

HallConfig* hall_get_config(void);
void hall_restore_config(HallConfig* config_);