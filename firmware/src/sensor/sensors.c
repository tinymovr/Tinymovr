
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

GenSensor sensors[SENSOR_COUNT] = {
    {.sensor = {
        .config = { .type = SENSOR_TYPE_MA7XX },
        .is_calibrated_func = ma7xx_rec_is_calibrated,
        .calibrate_func = ma7xx_calibrate,
        .get_raw_angle_func = ma7xx_get_raw_angle,
        .deinit_func = ma7xx_deinit,
        .reset_func = ma7xx_reset,
        .update_func = ma7xx_update,
        .prepare_func = ma7xx_send_angle_cmd,
        .get_errors_func = ma7xx_get_errors
    }},
    {.sensor = {
        .config = { .type = SENSOR_TYPE_MA7XX },
        .is_calibrated_func = ma7xx_rec_is_calibrated,
        .calibrate_func = ma7xx_calibrate,
        .get_raw_angle_func = ma7xx_get_raw_angle,
        .deinit_func = ma7xx_deinit,
        .reset_func = ma7xx_reset,
        .update_func = ma7xx_update,
        .prepare_func = ma7xx_send_angle_cmd,
        .get_errors_func = ma7xx_get_errors
    }},
    {.sensor = {
        .config = { .type = SENSOR_TYPE_HALL },
        .is_calibrated_func = hall_sector_map_is_calibrated,
        .calibrate_func = hall_calibrate_sequence,
        .get_raw_angle_func = hall_get_angle,
        .deinit_func = hall_deinit,
        .reset_func = hall_reset,
        .update_func = hall_update,
        .prepare_func = 0x0,
        .get_errors_func = hall_get_errors
    }}
};

bool sensor_init_with_defaults(Sensor *s)
{
    // Here we check the sensor connection, either
    // ONBOARD, EXTERNAL_SPI or EXTERNAL_HALL. We need
    // to do this here in order to set up the config
    // struct accordingly.
    sensor_connection_t connection = sensor_get_connection(s);
    switch (connection)
    {
        case SENSOR_CONNECTION_ONBOARD_SPI:
            return ma7xx_init_with_port(s, ONBOARD_SENSOR_SSP_PORT, ONBOARD_SENSOR_SSP_STRUCT);
            break;
        case SENSOR_CONNECTION_EXTERNAL_SPI:
            return ma7xx_init_with_port(s, EXTERNAL_SENSOR_SSP_PORT, EXTERNAL_SENSOR_SSP_STRUCT);
            break;
        case SENSOR_CONNECTION_HALL:
            return hall_init_with_defaults(s);
            break;
        default:
            break;
    }
    return false;
}

bool sensor_init_with_configs(Sensor *s, SensorConfig *sc, GenSensorConfig *gsc)
{
    switch (sc->type)
    {
        case SENSOR_TYPE_MA7XX:
            ma7xx_init_with_config(s, &(gsc->ma7xx_sensor_config));
            break;
        case SENSOR_TYPE_HALL:
            hall_init_with_config(s, &(gsc->hall_sensor_config));
            break;
        default:
            break;
    }
}

void sensors_init_with_defaults(void)
{
    sensor_init_with_defaults(&(sensors[0].sensor));
    commutation_sensor_p = &(sensors[0].sensor);
    position_sensor_p = &(sensors[0].sensor);
}

void sensors_get_config(SensorsConfig *config_)
{
    // Serialize SensorConfig array
    for (int i = 0; i < SENSOR_COUNT; ++i) {
        config_->config[i] = sensors[i].sensor.config;
    }

    // Serialize GenSensorConfig array
    for (int i = 0; i < SENSOR_COUNT; ++i) {
        sensors[i].sensor.get_ss_config_func(sensors[i].sensor, &(config_->ss_config[i]));
    }
    config_->commutation_connection = sensor_get_connection(commutation_sensor_p);
    config_->position_connection = sensor_get_connection(position_sensor_p);
}

void sensors_restore_config(SensorsConfig *config_)
{
    // Assuming there are functions to set the commutation and position sensor pointers
    // based on the sensor_connection_t value. If not, this part needs to be adapted.
    sensor_set_pointer_with_connection(&commutation_sensor_p, config_->commutation_connection);
    sensor_set_pointer_with_connection(&position_sensor_p, config_->position_connection);

    // Restore SensorConfig array
    for (int i = 0; i < SENSOR_COUNT; ++i) {
        sensors[i].sensor.config = config_->config[i];
        if ((sensors[i].sensor.initialized == false) && 
            (commutation_sensor_p == &(sensors[i].sensor) || position_sensor_p == &(sensors[i].sensor)))
        {
            sensor_init_with_configs(commutation_sensor_p, &(config_->config[i]), &(config_->ss_config[i]));
        }
    }
}

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
        Sensor *s = &(sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor);
        s->deinit_func(s);
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
            (*target_sensor)->deinit_func(*target_sensor);
        }

        *target_sensor = &(sensors[new_connection].sensor);

        if (new_connection != sensor_get_connection(*other_sensor))
        {
            sensor_init_with_defaults(*target_sensor);
        }
    }
}
