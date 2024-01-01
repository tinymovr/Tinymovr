
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

#include <src/sensor/sensors.h>
#include <src/controller/controller.h>

bool sensor_init_with_defaults(Sensor *s)
{
    // Here we check the sensor connection, either
    // ONBOARD, EXTERNAL_SPI or EXTERNAL_HALL. We need
    // to do this here in order to set up the config
    // struct accordingly.
    sensor_connection_t connection = sensor_get_connection(s);
    if (connection == SENSOR_CONNECTION_ONBOARD_SPI)
    {
        return ma7xx_init_with_port(s, ONBOARD_SENSOR_SSP_PORT);
    }
    else if (connection == SENSOR_CONNECTION_EXTERNAL_SPI)
    {
        return ma7xx_init_with_port(s, EXTERNAL_SENSOR_SSP_PORT);
    }
    else if (connection == SENSOR_CONNECTION_HALL)
    {
        return hall_init_with_defaults(s);
    }
    return false;
}

void sensors_init_with_defaults(void)
{
    sensor_init_with_defaults(&(sensors[0].sensor));
    commutation_sensor_p = &(sensors[0].sensor);
    position_sensor_p = &(sensors[0].sensor);
}

void sensors_get_config(SensorsConfig *config_)
{
    // for (int i=0; i<SENSOR_COUNT; i++)
    // {
    //     config_->config[i] = sensors[i].config;
    // }
    // config_->commutation_connection = sensor_get_connection(commutation_sensor_p);
    // config_->position_connection = sensor_get_connection(position_sensor_p);
}

void sensors_restore_config(SensorsConfig *config_)
{
    // for (int i=0; i<SENSOR_COUNT; i++)
    // {
    //     sensors[i].config = config_->config[i];
    // }
    // commutation_sensor_p = config_->commutation_connection + sensors;
    // sensor_init_with_config(commutation_sensor_p, &(commutation_sensor_p->config));
    // position_sensor_p = config_->position_connection + sensors;
    // if (commutation_sensor_p != position_sensor_p)
    // {
    //     sensor_init_with_config(position_sensor_p, &(position_sensor_p->config));
    // }
}

// Interface functions

void commutation_sensor_set_connection(sensor_connection_t new_connection)
{
    sensor_set_connection(&(commutation_sensor_p), &(position_sensor_p), new_connection);
}

void position_sensor_set_connection(sensor_connection_t new_connection)
{
    sensor_set_connection(&(position_sensor_p), &(commutation_sensor_p), new_connection);
}

void sensor_external_spi_set_type(sensor_type_t type)
{
    if (controller_get_state() == STATE_IDLE
        && type > SENSOR_TYPE_INVALID && type < SENSOR_TYPE_MAX 
        && type != sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor.config.type)
    {
        sensor_deinit(&(sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor));
        sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor.config.type = type;
        sensors_init_with_defaults();
    }
}

void sensor_set_connection(Sensor** target_sensor, Sensor** other_sensor, sensor_connection_t new_connection)
{
    if (controller_get_state() == STATE_IDLE
        && new_connection != sensor_get_connection(*target_sensor) 
        && new_connection >= 0 && new_connection < SENSOR_CONNECTION_MAX)
    {
        if (sensor_get_connection(*target_sensor) != sensor_get_connection(*other_sensor))
        {
            sensor_deinit(*target_sensor);
        }

        *target_sensor = &(sensors[new_connection].sensor);

        if (new_connection != sensor_get_connection(*other_sensor))
        {
            sensor_init_with_defaults(*target_sensor);
        }
    }
}