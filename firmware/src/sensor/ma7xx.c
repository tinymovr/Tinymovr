
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
#include <src/sensor/ma7xx.h>

bool ma7xx_init_with_port(Sensor *s, const SSP_TYPE port, PAC55XX_SSP_TYPEDEF *ssp_struct)
{
    MA7xxSensorConfig c = {0};
    c.ssp_port = port;
    c.ssp_struct = ssp_struct;
    return ma7xx_init_with_config(s, &c);
}

 bool ma7xx_init_with_config(Sensor *s, const MA7xxSensorConfig *c)
{
    MA7xxSensor *ms = (MA7xxSensor *)s;
    ms->base.get_raw_angle_func = ma7xx_get_raw_angle;
    ms->base.update_func = ma7xx_update;
    ms->base.prepare_func = ma7xx_send_angle_cmd;
    ms->base.reset_func = ma7xx_reset;
    ms->base.deinit_func = ma7xx_deinit;
    ms->base.get_errors_func = ma7xx_get_errors;
    ms->base.is_calibrated_func = ma7xx_rec_is_calibrated;
    ms->base.calibrate_func = ma7xx_calibrate;
    ms->base.config.type = SENSOR_TYPE_MA7XX;
    ms->config = *c;
    ssp_init(ms->config.ssp_port, SSP_MS_MASTER, 0, 0);
    delay_us(16000); // ensure 16ms sensor startup time as per the datasheet
    ma7xx_send_angle_cmd(s);
    ma7xx_update(s, false);
    return true;
}

void ma7xx_deinit(Sensor *s)
{
    ssp_deinit(((MA7xxSensor *)s)->config.ssp_port);
}

void ma7xx_reset(Sensor *s)
{
    sensor_reset(s);
}

bool ma7xx_calibrate(Sensor *s, Observer *o)
{
    return sensor_calibrate_direction_and_pole_pair_count(s, o) && sensor_calibrate_offset_and_rectification(s, o);
}
