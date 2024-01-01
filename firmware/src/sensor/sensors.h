
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

#pragma once

#include <src/sensor/ma7xx.h>
#include <src/sensor/hall.h>
#include <src/sensor/sensor.h>

typedef struct Observer Observer;

extern Observer commutation_observer;
extern Observer position_observer;

typedef union 
{
    Sensor sensor;
    MA7xxSensor ma7xx_sensor;
    HallSensor hall_sensor;
} GenSensor;

typedef union 
{
    SensorConfig sensor_config;
    MA7xxSensorConfig ma7xx_sensor_config;
    HallSensorConfig hall_sensor_config;
} GenSensorConfig;

typedef struct
{
    GenSensorConfig config[SENSOR_COUNT];
    sensor_connection_t commutation_connection;
    sensor_connection_t position_connection;
} SensorsConfig;

// The sequence in the `sensors` array is determined so that
// 0: onboard sensor, 1: external spi and 3: hall sensor
// index same as the members of `sensor_connection_t`
GenSensor sensors[SENSOR_COUNT];

Sensor *commutation_sensor_p;
Sensor *position_sensor_p;

static inline sensor_connection_t sensor_get_connection(const Sensor *sensor)
{
    // Derive pointer array offset
    const GenSensor *s = (const GenSensor *)sensor;
    return s - sensors;
}

void sensor_set_connection(Sensor** target_sensor, Sensor** other_sensor, sensor_connection_t new_connection);

bool sensor_init_with_defaults(Sensor *s);
void sensors_init_with_defaults(void);
void sensors_get_config(SensorsConfig *config_);
void sensors_restore_config(SensorsConfig *config_);

// Interface functions

static inline sensor_connection_t commutation_sensor_get_connection(void)
{
    return sensor_get_connection(commutation_sensor_p);
}

void commutation_sensor_set_connection(sensor_connection_t new_connection);

static inline sensor_connection_t position_sensor_get_connection(void)
{
    return sensor_get_connection(position_sensor_p);
}

void position_sensor_set_connection(sensor_connection_t new_connection);

static inline void sensors_reset(void)
{
    for (int i=0; i<SENSOR_COUNT; i++)
    {
        sensor_reset(&(sensors[i].sensor));
    }
}

static inline void sensors_calibrate(void)
{
    commutation_sensor_p->calibrate_func(commutation_sensor_p, &commutation_observer);
    position_sensor_p->calibrate_func(position_sensor_p, &position_observer);
}

static inline sensor_type_t sensor_external_spi_get_type(void)
{
    return sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor.config.type;
}

void sensor_external_spi_set_type(sensor_type_t type);

static inline bool sensor_onboard_get_is_calibrated(void)
{
    return sensors[SENSOR_CONNECTION_ONBOARD_SPI].sensor.is_calibrated_func(&(sensors[SENSOR_CONNECTION_ONBOARD_SPI].sensor));
}

static inline bool sensor_external_spi_get_is_calibrated(void)
{
    return sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor.is_calibrated_func(&(sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor));
}

static inline bool sensor_hall_get_is_calibrated(void)
{
    return sensors[SENSOR_CONNECTION_HALL].sensor.is_calibrated_func(&(sensors[SENSOR_CONNECTION_HALL].sensor));
}

static inline uint8_t sensor_onboard_get_errors(void)
{
    return sensors[SENSOR_CONNECTION_ONBOARD_SPI].sensor.get_errors_func(&(sensors[SENSOR_CONNECTION_ONBOARD_SPI].sensor));
}

static inline uint8_t sensor_external_spi_get_errors(void)
{
    return sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor.get_errors_func(&(sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor));
}

static inline uint8_t sensor_hall_get_errors(void)
{
    return sensors[SENSOR_CONNECTION_HALL].sensor.get_errors_func(&(sensors[SENSOR_CONNECTION_HALL].sensor));
}
