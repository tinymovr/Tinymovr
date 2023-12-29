
#pragma once

#include <src/sensor/sensor.h>
#include <src/observer/observer.h>

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
        sensor_reset(&(sensors[i]));
    }
}

static inline void sensors_calibrate(void)
{
    sensor_calibrate(commutation_sensor_p, &commutation_observer);
    sensor_calibrate(position_sensor_p, &position_observer);
}

static inline sensor_type_t sensor_external_spi_get_type(void)
{
    return sensors[SENSOR_CONNECTION_EXTERNAL_SPI].config.type;
}

void sensor_external_spi_set_type(sensor_type_t type);

static inline bool sensor_onboard_get_is_calibrated(void)
{
    return sensors[SENSOR_CONNECTION_ONBOARD_SPI].is_calibrated_func(&(sensors[SENSOR_CONNECTION_ONBOARD_SPI]));
}

static inline bool sensor_external_spi_get_is_calibrated(void)
{
    return sensors[SENSOR_CONNECTION_EXTERNAL_SPI].is_calibrated_func(&(sensors[SENSOR_CONNECTION_EXTERNAL_SPI]));
}

static inline bool sensor_hall_get_is_calibrated(void)
{
    return sensors[SENSOR_CONNECTION_HALL].is_calibrated_func(&(sensors[SENSOR_CONNECTION_HALL]));
}

static inline uint8_t sensor_onboard_get_errors(void)
{
    return sensors[SENSOR_CONNECTION_ONBOARD_SPI].get_errors_func(&(sensors[SENSOR_CONNECTION_ONBOARD_SPI]));
}

static inline uint8_t sensor_external_spi_get_errors(void)
{
    return sensors[SENSOR_CONNECTION_EXTERNAL_SPI].get_errors_func(&(sensors[SENSOR_CONNECTION_EXTERNAL_SPI]));
}

static inline uint8_t sensor_hall_get_errors(void)
{
    return sensors[SENSOR_CONNECTION_HALL].get_errors_func(&(sensors[SENSOR_CONNECTION_HALL]));
}
