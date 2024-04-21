
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
#include <src/sensor/amt22.h>

void amt22_make_blank_sensor(Sensor *s)
{
    AMT22Sensor *as = (AMT22Sensor *)s;
    as->config.rate = SENSORS_SETUP_EXTERNAL_SPI_RATE_3Mbps;
    s->config.type = SENSOR_TYPE_AMT22;
    s->bits = AMT22_BITS;
    s->ticks = AMT22_TICKS;
    s->normalization_factor = SENSOR_COMMON_RES_TICKS_FLOAT / s->ticks;
    s->get_raw_angle_func = amt22_get_raw_angle;
    s->update_func = amt22_update; 
    s->prepare_func = amt22_send_angle_cmd; 
    s->reset_func = amt22_reset; 
    s->init_func = amt22_init; 
    s->deinit_func = amt22_deinit;
    s->get_errors_func = amt22_get_errors; 
    s->is_calibrated_func = amt22_is_calibrated; 
    s->get_ss_config_func = amt22_get_ss_config;
}

bool amt22_init_with_port_and_rate(Sensor *s, const SSP_TYPE port, PAC55XX_SSP_TYPEDEF *ssp_struct, sensors_setup_external_spi_rate_options rate)
{
    AMT22SensorConfig c = {0};
    c.ssp_port = port;
    c.ssp_struct = ssp_struct;
    c.rate = rate;
    return amt22_init_with_config(s, &c);
}

bool amt22_init_with_config(Sensor *s, const AMT22SensorConfig *c)
{
    AMT22Sensor *as = (AMT22Sensor *)s;
    as->config = *c;
    return amt22_init(s);
}

bool amt22_init(Sensor *s)
{
    AMT22Sensor *as = (AMT22Sensor *)s;
    ssp_init(as->config.ssp_port, SSP_MS_MASTER, 16, SSP_DATA_SIZE_8, 0, 0);
    delay_us(10000); 

    amt22_send_angle_cmd(s); 
    amt22_update(s, false); 
    s->initialized = true;
    return true;
}

void amt22_deinit(Sensor *s)
{
    ssp_deinit(((AMT22Sensor *)s)->config.ssp_port);
    s->initialized = false;
}

void amt22_reset(Sensor *s)
{
    sensor_reset(s);
}

void amt22_get_ss_config(Sensor *s, void* buffer)
{
    const AMT22Sensor *ss = ((const AMT22Sensor *)s);
    memcpy(buffer, &(ss->config), sizeof(ss->config));
}
