
#include <src/sensor/iface.h>
#include <src/controller/controller.h>

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
        && type != sensors[SENSOR_CONNECTION_EXTERNAL_SPI].config.type)
    {
        sensor_deinit(&(sensors[SENSOR_CONNECTION_EXTERNAL_SPI]));
        sensors[SENSOR_CONNECTION_EXTERNAL_SPI].config.type = type;
        sensors_init_with_defaults();
    }
}
