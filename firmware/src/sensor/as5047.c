
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
#include <src/utils/utils.h>
#include <src/can/can_endpoints.h>
#include <src/sensor/sensor.h>
#include <src/observer/observer.h>
#include <src/sensor/as5047.h>

void as5047p_make_blank_sensor(Sensor *s)
{
    s->config.type = SENSOR_TYPE_AS5047;
    s->bits = AS5047_BITS;
    s->ticks = AS5047_TICKS;
    s->normalization_factor = SENSOR_COMMON_RES_TICKS_FLOAT / s->ticks;
    s->get_raw_angle_func = as5047p_get_raw_angle;
    s->update_func = as5047p_update; 
    s->prepare_func = as5047p_send_angle_cmd; 
    s->reset_func = as5047p_reset; 
    s->deinit_func = as5047p_deinit; 
    s->get_errors_func = as5047p_get_errors; 
    s->is_calibrated_func = as5047p_is_calibrated; 
    s->get_ss_config_func = as5047p_get_ss_config;
}

bool as5047p_init_with_port(Sensor *s, const SSP_TYPE port, PAC55XX_SSP_TYPEDEF *ssp_struct) {
    AS5047PSensorConfig c = {0};
    c.ssp_port = port;
    c.ssp_struct = ssp_struct;
    return as5047p_init_with_config(s, &c);
}

bool as5047p_init_with_config(Sensor *s, const AS5047PSensorConfig *c) {
    AS5047PSensor *as = (AS5047PSensor *)s;
    as->config = *c;
    ssp_init(as->config.ssp_port, SSP_MS_MASTER, 16, 1, 0);
    delay_us(10000); // Example delay, adjust based on AS5047P datasheet

    as5047p_send_angle_cmd(s); 
    as5047p_update(s, false); 
    s->initialized = true;
    return true;
}

void as5047p_deinit(Sensor *s)
{
    ssp_deinit(((AS5047PSensor *)s)->config.ssp_port);
    s->initialized = false;
}

void as5047p_reset(Sensor *s)
{
    sensor_reset(s);
}

void as5047p_get_ss_config(Sensor *s, void* buffer)
{
    const AS5047PSensor *ss = ((const AS5047PSensor *)s);
    memcpy(buffer, &(ss->config), sizeof(ss->config));
}