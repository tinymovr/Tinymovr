
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

#include <src/controller/controller.h>
#include <src/sensor/sensor.h>

static uint32_t sensor_id = 1; // 0 implicitly treated as missing value

uint32_t get_next_sensor_id(void)
{
    return sensor_id++;
}



void sensor_deinit(Sensor *s)
{
    s->deinit_func(s);
}

void sensor_reset(Sensor *s)
{
    s->reset_func(s);
}

