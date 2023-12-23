
#include <src/controller/controller.h>
#include <src/sensor/sensor.h>

static uint32_t sensor_id = 1; // 0 implicitly treated as missing value

uint32_t get_next_sensor_id(void)
{
    return sensor_id++;
}

bool sensor_init_with_defaults(Sensor *s)
{
    SensorConfig c;
    sensor_connection_t connection = sensor_get_connection(s);
    if (connection == SENSOR_CONNECTION_ONBOARD_SPI)
    {

    }
    else if (connection == SENSOR_CONNECTION_EXTERNAL_SPI)
    {

    }
    else if (connection == SENSOR_CONNECTION_HALL)
    {
        
    }
    else
    {
        return false;
    }
    return sensor_init_with_config(s, &c);
}

bool sensor_init_with_config(Sensor *s, SensorConfig *c)
{
    if (s->initialized == false)
    {
#ifdef BOARD_REV_R5
        if (SENSOR_TYPE_MA7XX == c->type)
        {
#endif
            return ma7xx_init_with_config(s, &(c->ss_config));
#ifdef BOARD_REV_R5
        }
        else if (SENSOR_TYPE_HALL == c->type)
        {
            return hall_init_with_config(s, &(c->ss_config));
        }
#endif
    // ... (similar branches for other sensor types)
    }
    return false;
}

void sensor_deinit(Sensor *s)
{
#ifdef BOARD_REV_R5
    if (SENSOR_TYPE_MA7XX == s->config.type)
    {
#endif
        ma7xx_deinit(s);
#ifdef BOARD_REV_R5
    }
    else if (SENSOR_TYPE_HALL == s->config.type)
    {
        hall_deinit(s);
    }
#endif
    // ... (similar branches for other sensor types)
}

void sensor_reset(Sensor *s)
{
    s->reset_func(s);
}

void sensors_init_with_defaults(void)
{
    ma7xx_init_with_defaults(&(sensors[0]));
    commutation_sensor_p = &(sensors[0]);
    position_sensor_p = &(sensors[0]);
}

void sensors_get_config(SensorsConfig *config_)
{
#warning "Update implementation"
}

void sensors_restore_config(SensorsConfig *config_)
{
#warning "Update implementation"
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

        *target_sensor = &(sensors[new_connection]);

        if (new_connection != sensor_get_connection(*other_sensor))
        {
            sensor_init_with_defaults(*target_sensor);
        }
    }
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
        && type != sensors[SENSOR_CONNECTION_EXTERNAL_SPI].config.type)
    {
        sensor_deinit(&(sensors[SENSOR_CONNECTION_EXTERNAL_SPI]));
        sensors[SENSOR_CONNECTION_EXTERNAL_SPI].config.type = type;
        sensors_init_with_defaults();
    }
}

