
#include <src/sensors/ma7xx.h>
#include <src/sensors/sensor.h>

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
        if (SENSOR_MA7XX == c->type)
        {
#endif
            return ma7xx_init_with_config(s, c);
#ifdef BOARD_REV_R5
        }
        else if (SENSOR_HALL == c->type)
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
    ma7xx_init(&(sensors[0]));
    sensor_commutation = &(sensors[0]);
    sensor_position = &(sensors[0]);
}

uint32_t sensors_config_length(void)
{
    uint32_t config_length = sizeof(sensor_commutation) + sizeof(sensor_position);
    for (uint8_t i=0; i<SENSOR_COUNT; i++)
    {
        if (sensors[i].initialized)
        {
            config_length += sizeof(SensorConfig);
        }
    }
    return config_length;
}

bool sensors_serialize_config_to_buffer(uint8_t *buffer, uint32_t *len)
{
    uint8_t *buffer_pos = buffer;
    memcpy(buffer, &(sensor_commutation->id), sizeof(sensor_commutation->id));
    buffer_pos += sizeof(sensor_commutation->id);
    memcpy(buffer, &(sensor_position->id), sizeof(sensor_position->id));
    buffer_pos += sizeof(sensor_position->id);

    for (uint8_t i=0; i<SENSOR_COUNT; i++)
    {
        if (sensors[i].config.type != SENSOR_INVALID && sensors[i].initialized == true)
        {
            if (buffer_pos - buffer <= *len - sizeof(SensorConfig))
            {
                memcpy(buffer_pos, &(sensors[i].config), sizeof(SensorConfig));
                buffer_pos += sizeof(SensorConfig);
            }
            else
            {
                return false;
            }
        }
    }
    *len = buffer_pos - buffer;
    return true;
}

bool sensors_initialize_with_config_buffer(const uint8_t *buffer, const uint32_t len)
{
    uint8_t *buffer_pos = buffer;
    uint32_t commutation_id, position_id;
    memcpy(&commutation_id, buffer, sizeof(commutation_id));
    buffer_pos += sizeof(commutation_id);
    memcpy(&position_id, buffer, sizeof(position_id));
    buffer_pos += sizeof(position_id);

    SensorConfig configs[SENSOR_COUNT] = {0};
    for (uint8_t i=0; i<SENSOR_COUNT; i++)
    {
        if (buffer_pos - buffer >= *len - sizeof(SensorConfig))
        {
            memcpy(&(configs[i]), buffer_pos, sizeof(SensorConfig));
            buffer_pos += sizeof(SensorConfig);
        }
        else
        {
            return false;
        }
    }
    for (uint8_t i=0; i<SENSOR_COUNT; i++)
    {
        if (configs[i].type != SENSOR_INVALID)
        {
            sensor_init_with_config(&(sensors[i]), &(configs[i]));
            if (sensors[i].config.id == commutation_id)
            {
                sensor_commutation = &(sensors[i]);
            }
            else if (sensors[i].config.id == position_id)
            {
                sensor_position = &(sensors[i]);
            }
        }
    }
    return true;
}