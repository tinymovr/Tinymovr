
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

#include <src/common.h>
#include <src/motor/motor.h>
#include <src/sensors/ma7xx.h>
#include <src/sensors/hall.h>
#include <src/sensors/sensor.h>

static SensorConfig config = {0};
static SensorState state = {0};

void encoder_init(void)
{
#ifdef BOARD_REV_R5
    if (SENSOR_MA7XX == config.encoder_type)
    {
#endif
        ma7xx_init();
        state.current_encoder_type = SENSOR_MA7XX;
        state.get_error_ptr = &ma7xx_get_errors;
        state.get_calibrated_ptr = &ma7xx_rec_is_calibrated;
        state.get_angle_ptr = &ma7xx_get_angle_rectified;
        state.update_angle_ptr = &ma7xx_update;
        state.reset_encoder_ptr = &ma7xx_clear_rec_table;
        state.ticks = ENCODER_TICKS;
#ifdef BOARD_REV_R5
    }
    else if (SENSOR_HALL == config.encoder_type)
    {
        hall_init();
        state.current_encoder_type = SENSOR_HALL;
        state.get_error_ptr = &hall_get_errors;
        state.get_calibrated_ptr = &hall_sector_map_is_calibrated;
        state.get_angle_ptr = &hall_get_angle;
        state.update_angle_ptr = &hall_update;
        state.reset_encoder_ptr = &hall_clear_sector_map;
        state.ticks = HALL_SECTORS;
    }
#endif
}

void encoder_reset(void)
{
    state.reset_encoder_ptr();
}

TM_RAMFUNC int16_t encoder_get_angle(void)
{
    return state.get_angle_ptr();
}

TM_RAMFUNC void encoder_update(bool check_error)
{
    if (state.update_angle_ptr)
    {
        state.update_angle_ptr(check_error);
    }
}

TM_RAMFUNC uint16_t encoder_get_ticks(void)
{
    return state.ticks;
}

TM_RAMFUNC float encoder_ticks_to_eangle()
{
#ifdef BOARD_REV_R5
    // We need to derive this during call, because the motor pole pairs
    // may change after calibration, or after user input
    if (SENSOR_MA7XX == state.current_encoder_type)
    {
#endif
        return twopi_by_enc_ticks * motor_get_pole_pairs();
#ifdef BOARD_REV_R5
    }
    return  twopi_by_hall_sectors;
#endif
}

SensorType encoder_get_type(void)
{
    return state.current_encoder_type;
}

TM_RAMFUNC bool encoder_get_calibrated(void)
{
    return state.get_calibrated_ptr();
}

TM_RAMFUNC void encoder_set_type(SensorType enc_type)
{
#ifdef BOARD_REV_R5
    if (SENSOR_MA7XX == enc_type)
    {
#endif
        config.encoder_type = SENSOR_MA7XX;
#ifdef BOARD_REV_R5
    }
    else if (SENSOR_HALL == enc_type)
    {
        config.encoder_type = SENSOR_HALL;
    }
#endif
}

TM_RAMFUNC uint8_t encoder_get_errors(void)
{
    return state.get_error_ptr();
}

SensorConfig* encoder_get_config(void)
{
    return &config;
}

void encoder_restore_config(SensorConfig* config_)
{
    config = *config_;
}
