
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
#include <src/can/can_endpoints.h>
#include <src/sensor/sensor.h>
#include <src/sensor/hall.h>

#define AIO6789_IO_MODE                 0x00
#define AIO_INPUT                       0x00

bool hall_init_with_defaults(Sensor *s)
{
    HallSensorConfig c = {0};
    return hall_init_with_config(s, &c);
}

bool hall_init_with_config(Sensor *s, const HallSensorConfig *c)
{
    HallSensor *ms = (HallSensor *)s;
    ms->base.get_raw_angle_func = hall_get_angle;
    ms->base.update_func = hall_update;
    ms->base.reset_func = hall_reset;
    ms->base.deinit_func = hall_deinit;
    ms->base.get_errors_func = hall_get_errors;
    ms->base.is_calibrated_func = hall_sector_map_is_calibrated;
    ms->base.calibrate_func = hall_calibrate_sequence;
    ms->base.config.type = SENSOR_TYPE_HALL;
    ms->config = *c;
    ms->hw_defaults[0] = pac5xxx_tile_register_read(ADDR_CFGAIO7);
    ms->hw_defaults[1] = pac5xxx_tile_register_read(ADDR_CFGAIO8);
    ms->hw_defaults[2] = pac5xxx_tile_register_read(ADDR_CFGAIO9);
    pac5xxx_tile_register_write(ADDR_CFGAIO7, AIO6789_IO_MODE | AIO_INPUT);
    pac5xxx_tile_register_write(ADDR_CFGAIO8, AIO6789_IO_MODE | AIO_INPUT);
    pac5xxx_tile_register_write(ADDR_CFGAIO9, AIO6789_IO_MODE | AIO_INPUT);
    return true;
}

void hall_deinit(Sensor *s)
{
    HallSensor *ms = (HallSensor *)s;
    pac5xxx_tile_register_write(ADDR_CFGAIO7, ms->hw_defaults[0]);
    pac5xxx_tile_register_write(ADDR_CFGAIO8, ms->hw_defaults[1]);
    pac5xxx_tile_register_write(ADDR_CFGAIO9, ms->hw_defaults[2]);
}

void hall_reset(Sensor *s)
{
    HallSensor *ms = (HallSensor *)s;
    memset(ms->config.sector_map, 0, sizeof(ms->config.sector_map));
    ms->config.sector_map_calibrated = false;
}

bool hall_calibrate_sequence(Sensor *s, Observer *o)
{
    HallSensor *ms = (HallSensor *)s;
    HallSensorConfig *c = &(ms->config);
    (void)memset(c->sector_map, 0, sizeof(c->sector_map));
	c->sector_map_calibrated = false;
    uint8_t *sector_map = c->sector_map;
    const float I_setpoint = motor_get_I_cal();
    bool success = true;

    // Stay a bit at starting epos
	for (uint32_t i=0; i<CAL_STAY_LEN; i++)
	{
		set_epos_and_wait(0, I_setpoint);
	}
    const uint8_t init_sector = hall_get_sector(s);
    uint8_t current_sector = init_sector;
    uint8_t last_sector = init_sector;
    uint8_t sector_pos = 0;
    const float increment = TWOPI / CAL_DIR_LEN;
    float angle = 0;

    sector_map[current_sector] = sector_pos;

    // Move to the 2nd sector
    while ((current_sector == init_sector) && (angle < TWOPI / HALL_SECTORS))
    {
        set_epos_and_wait(angle, I_setpoint);
        angle += increment;
        current_sector = hall_get_sector(s);
    }

    // Save the rest of the sectors
    while ((current_sector != init_sector) && (angle < TWOPI))
    {
        if (current_sector != last_sector)
        {
            last_sector = current_sector;
            sector_pos++;
            if (sector_pos >= HALL_SECTORS)
            {
                success = false;
                break;
            }
            sector_map[current_sector] = sector_pos;
        }
        set_epos_and_wait(angle, I_setpoint);
        angle += increment;
        current_sector = hall_get_sector(s);
    }

    gate_driver_set_duty_cycle(&three_phase_zero);

    // Check that the number of sectors discovered is the same as expected
    if (sector_pos != HALL_SECTORS - 1)
    {
        success = false;
    }

    if (success)
    {
        c->sector_map_calibrated = true;
    }
    else
    {
        ms->errors |= SENSORS_SETUP_HALL_ERRORS_CALIBRATION_FAILED;
    }
    return success;
}