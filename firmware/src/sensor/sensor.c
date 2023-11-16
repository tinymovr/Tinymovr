
#include <src/sensor/sensor.h>

static uint32_t sensor_id = 1; // 0 implicitly treated as missing value

uint32_t get_next_sensor_id(void)
{
    return sensor_id++;
}

bool sensor_init_with_config(Sensor *s, SensorConfig *c)
{
    if (s->initialized == false)
    {
#ifdef BOARD_REV_R5
        if (SENSOR_TYPE_MA7XX == c->type)
        {
#endif
            return ma7xx_init_with_config(s, c);
#ifdef BOARD_REV_R5
        }
        else if (SENSOR_TYPE_HALL == c->type)
        {
            return hall_init_with_config(s, c);
        }
#endif
    // ... (similar branches for other sensor types)
    }
    else
    {
        return false;
    }
}

void sensor_deinit(Sensor *s)
{
#ifdef BOARD_REV_R5
    if (SENSOR_TYPE_MA7XX == s->type)
    {
#endif
        ma7xx_deinit(s);
#ifdef BOARD_REV_R5
    }
    else if (SENSOR_TYPE_HALL == s->type)
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

void sensors_init_with_defaults(void)
{
    ma7xx_init_with_defaults(&(sensors[0]));
    sensor_commutation = &(sensors[0]);
    sensor_position = &(sensors[0]);
}

void sensors_get_config(SensorsConfig *config_)
{

}

void sensors_restore_config(SensorsConfig *config_)
{

}
