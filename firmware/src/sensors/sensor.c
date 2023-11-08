
#include <src/sensors/ma7xx.h>
#include <src/sensors/sensor.h>

static uint8_t sensor_id = 0;

Sensor make_blank_sensor(void)
{
    Sensor s = {0};
    s.id = sensor_id;
    sensor_id++;
    return s;
}

Sensor sensor_init(void)
{
    SensorConfig c = {0};
    return sensor_init_with_config(&c);
}

Sensor sensor_init_with_config(SensorConfig *c)
{
#ifdef BOARD_REV_R5
    if (SENSOR_MA7XX == c->type)
    {
#endif
        return ma7xx_init_with_config(c);
#ifdef BOARD_REV_R5
    }
    else if (SENSOR_HALL == c->type)
    {
        return hall_init_with_config(c);
    }
#endif
    // ... (similar branches for other sensor types)
}

void sensor_deinit(Sensor *s)
{
#ifdef BOARD_REV_R5
    if (SENSOR_MA7XX == s->type)
    {
#endif
        ma7xx_deinit(s);
#ifdef BOARD_REV_R5
    }
    else if (SENSOR_HALL == s->type)
    {
        hall_deinit(s);
    }
#endif
    // ... (similar branches for other sensor types)
}

void sensor_reset(Sensor *s)
{
    s->reset_func();
}

void make_default_sensor_config(void)
{
    sensors[0] = ma7xx_init();
    sensor_commutation = &(sensors[0]);
    sensor_position = &(sensors[0]);
}

uint32_t sensors_config_length(void)
{
    uint32_t config_length = 0;
    for (uint8_t i=0; i<SENSOR_COUNT; i++)
    {
        if (sensors[i].initialized)
        {
            config_length += len(SensorConfig);
        }
    }
    return config_length;
}

bool serialize_sensors_config_to_buffer(uint8_t *buffer, uint32_t len)
{

}

bool initialize_sensors_with_config_buffer(uint8_t *buffer, uint32_t len)
{

}