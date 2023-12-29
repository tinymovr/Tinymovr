

#include <src/sensor/sensor.h>
#include <src/controller/controller.h>

static uint32_t sensor_id = 1; // 0 implicitly treated as missing value

uint32_t get_next_sensor_id(void)
{
    return sensor_id++;
}

bool sensor_init_with_defaults(Sensor *s)
{
    // Here we check the sensor connection, either
    // ONBOARD, EXTERNAL_SPI or EXTERNAL_HALL. We need
    // to do this here in order to set up the config
    // struct accordingly.
    SensorConfig c;
    sensor_connection_t connection = sensor_get_connection(s);
    if (connection == SENSOR_CONNECTION_ONBOARD_SPI)
    {
        c.type = SENSOR_TYPE_MA7XX;
        c.ss_config.ma7xx_config.ssp_port = ONBOARD_SENSOR_SSP_PORT;
    }
    else if (connection == SENSOR_CONNECTION_EXTERNAL_SPI)
    {
        c.type = SENSOR_TYPE_MA7XX;
        c.ss_config.ma7xx_config.ssp_port = EXTERNAL_SENSOR_SSP_PORT;
    }
    else if (connection == SENSOR_CONNECTION_HALL)
    {
        c.type = SENSOR_TYPE_HALL;
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
    sensor_init_with_defaults(&(sensors[0]));
    commutation_sensor_p = &(sensors[0]);
    position_sensor_p = &(sensors[0]);
}

void sensors_get_config(SensorsConfig *config_)
{
    for (int i=0; i<SENSOR_COUNT; i++)
    {
        config_->config[i] = sensors[i].config;
    }
    config_->commutation_connection = sensor_get_connection(commutation_sensor_p);
    config_->position_connection = sensor_get_connection(position_sensor_p);
}

void sensors_restore_config(SensorsConfig *config_)
{
    for (int i=0; i<SENSOR_COUNT; i++)
    {
        sensors[i].config = config_->config[i];
    }
    commutation_sensor_p = config_->commutation_connection + sensors;
    sensor_init_with_config(commutation_sensor_p, &(commutation_sensor_p->config));
    position_sensor_p = config_->position_connection + sensors;
    if (commutation_sensor_p != position_sensor_p)
    {
        sensor_init_with_config(position_sensor_p, &(position_sensor_p->config));
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

        *target_sensor = &(sensors[new_connection]);

        if (new_connection != sensor_get_connection(*other_sensor))
        {
            sensor_init_with_defaults(*target_sensor);
        }
    }
}

