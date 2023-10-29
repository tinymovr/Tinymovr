
#include <src/sensors/sensor.h>

static Sensor sensor;

void sensor_init_with_config(SensorConfig *c)
{
#ifdef BOARD_REV_R5
    if (SENSOR_MA7XX == c->type)
    {
#endif
        return ma7xx_init(c);
#ifdef BOARD_REV_R5
    }
    else if (SENSOR_HALL == s->type)
    {
        return hall_init(p);
        s->read_func = hall_get_angle;
        s->update_func = hall_update;
        s->reset_func = hall_clear_sector_map;
        s->get_error_func = hall_get_errors;
        s->is_calibrated_func = hall_sector_map_is_calibrated;
        // ... fill in other function pointers and configurations
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
        // ... fill in other function pointers and configurations
    }
#endif
    // ... (similar branches for other sensor types)
}

void sensor_reset(Sensor *s)
{
    s->reset_func();
}

SensorConfig* sensor_get_config(Sensor *s)
{
    return &(s->config);
}

void sensor_restore_config(Sensor *s, SensorConfig* config_)
{
    s->config = *config_;
    sensor_init(s);  // Re-initialize sensor with restored configuration
}