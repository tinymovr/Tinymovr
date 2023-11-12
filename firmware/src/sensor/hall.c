
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

bool hall_init_with_defaults(Sensor *s)
{
    HallConfig c = {.id = get_next_sensor_id()};
    return hall_init_with_config(s, &c);
}

bool hall_init_with_config(Sensor *s, SensorSpecificConfig *c)
{
    s->get_angle_func = hall_get_angle;
    s->update_func = hall_update;
    s->reset_func = hall_clear_sector_map;
    s->get_error_func = hall_get_errors;
    s->is_calibrated_func = hall_sector_map_is_calibrated;
    s->calibrate_func = hall_calibrate_sequence;
    s->config.type = SENSOR_HALL;
    s->config.ss_config = *c;
    s->state.hall_state.hw_defaults[0] = pac5xxx_tile_register_read(ADDR_CFGAIO7);
    s->state.hall_state.hw_defaults[1] = pac5xxx_tile_register_read(ADDR_CFGAIO8);
    s->state.hall_state.hw_defaults[2] = pac5xxx_tile_register_read(ADDR_CFGAIO9);
    pac5xxx_tile_register_write(ADDR_CFGAIO7, AIO6789_IO_MODE | AIO_INPUT);
    pac5xxx_tile_register_write(ADDR_CFGAIO8, AIO6789_IO_MODE | AIO_INPUT);
    pac5xxx_tile_register_write(ADDR_CFGAIO9, AIO6789_IO_MODE | AIO_INPUT);
    return true;
}

void hall_deinit(Sensor *s)
{
    pac5xxx_tile_register_write(ADDR_CFGAIO7, s->state.hall_state.hw_defaults[0]);
    pac5xxx_tile_register_write(ADDR_CFGAIO8, s->state.hall_state.hw_defaults[1]);
    pac5xxx_tile_register_write(ADDR_CFGAIO9, s->state.hall_state.hw_defaults[2]);
}

bool hall_sector_map_is_calibrated(Sensor *s)
{
    return s->config.hall_config.sector_map_calibrated;
}

bool hall_calibrate_sequence(Sensor *s)
{
    (void)memset(s->config.hall_config.sector_map, 0, sizeof(config.sector_map));
	s->config.hall_config.sector_map_calibrated = false;
    uint8_t *sector_map = s->config.hall_config.sector_map;
    const float I_setpoint = motor_get_I_cal();
    bool success = true;

    // Stay a bit at starting epos
	for (uint32_t i=0; i<CAL_STAY_LEN; i++)
	{
		set_epos_and_wait(0, I_setpoint);
	}
    const uint8_t init_sector = hall_get_sector();
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
        current_sector = hall_get_sector();
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
        current_sector = hall_get_sector();
    }

    gate_driver_set_duty_cycle(&three_phase_zero);

    // Check that the number of sectors discovered is the same as expected
    if (sector_pos != HALL_SECTORS - 1)
    {
        success = false;
    }

    if (success)
    {
        s->config.hall_config.sector_map_calibrated = true;
    }
    else
    {
        s->state.hall_state.errors |= ENCODER_ERRORS_CALIBRATION_FAILED;
    }
    return success;
}