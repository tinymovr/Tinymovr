
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

#include <string.h>
#include <src/system/system.h>
#include <src/sensors/hall.h>

#define AIO6789_IO_MODE                 0x00
#define AIO_INPUT                       0x00

static HallConfig config = {0};
static HallState state = {0};

void hall_init(void)
{
    pac5xxx_tile_register_write(ADDR_CFGAIO7, AIO6789_IO_MODE | AIO_INPUT);
    pac5xxx_tile_register_write(ADDR_CFGAIO8, AIO6789_IO_MODE | AIO_INPUT);
    pac5xxx_tile_register_write(ADDR_CFGAIO9, AIO6789_IO_MODE | AIO_INPUT);
}

TM_RAMFUNC uint8_t hall_get_errors(void)
{
    return state.errors;
}

TM_RAMFUNC uint8_t *hall_get_error_ptr(void)
{
	return &(state.errors);
}

TM_RAMFUNC int16_t hall_get_angle(void)
{
    return state.angle;
}

TM_RAMFUNC void hall_update(bool check_error)
{
    const uint8_t sector = (pac5xxx_tile_register_read(ADDR_DINSIG1) >> 1) & 0x07;
    state.sector = sector;
    state.angle = config.sector_map[state.sector];
}

TM_RAMFUNC uint8_t hall_get_sector(void)
{
    return state.sector;
}

void hall_clear_sector_map(void)
{
    (void)memset(config.sector_map, 0, sizeof(config.sector_map));
	config.sector_map_calibrated = false;
}

void hall_set_sector_map_calibrated(void)
{
    config.sector_map_calibrated = true;
}

bool hall_sector_map_is_calibrated(void)
{
    return config.sector_map_calibrated;
}

uint8_t *hall_get_sector_map_ptr(void)
{
    return config.sector_map;
}

HallConfig* hall_get_config(void)
{
    return &config;
}

void hall_restore_config(HallConfig* config_)
{
    config = *config_;
}