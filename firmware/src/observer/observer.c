
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

#include <src/motor/motor.h>
#include <src/common.h>
#include <src/utils/utils.h>
#include <src/system/system.h>
#include <src/observer/observer.h>

static ObserverConfig config = {
	.track_bw = 350.0f,
	.kp = 0.0f,
	.ki = 0.0f,
};

Observer observer_init(Sensor *s)
{
	Observer *o = {0};
	o->config = config;
	o->config.sensor_idx = s->idx;
    observer_set_bw(config.track_bw);
	// We keep local copies of a few key variables from
	// the encoder, because it is faster than calling
	// the encoder function pointer
	o->state.encoder_type = sensor_get_type(s);
	o->state.encoder_ticks = sensor_get_ticks(s);
	o->state.encoder_half_ticks = state.encoder_ticks/2;
}

void observer_reset(Observer *o)
{
	o->state.pos_sector = 0;
	o->state.pos_estimate_wrapped = 0;
	o->state.vel_estimate = 0;
}

float observer_get_bw(Observer *o)
{
    return o->config.track_bw;
}

void observer_set_bw(Observer *o, float bw)
{
    if (bw > 0.0f)
    {
        o->config.track_bw = bw;
		o->config.kp = 2.0f * config.track_bw;
    	o->config.ki = 0.25f * (config.kp * config.kp);
    }
}

void make_default_observer_config(void)
{
    observers[0] = observer_init(sensor_commutation);
	observers[1] = observers[0];
}

uint32_t observers_config_length(void)
{
	uint32_t config_length = 0;
    for (uint8_t i=0; i<OBSERVER_COUNT; i++)
    {
        if (sensors[i].initialized)
        {
            config_length += len(SensorConfig);
        }
    }
    return config_length;
}

uint32_t observers_config_length(void)
{
    uint32_t config_length = sizeof(observer_commutation) + sizeof(observer_position);
    for (uint8_t i=0; i<SENSOR_COUNT; i++)
    {
        if (observers[i].initialized)
        {
            config_length += sizeof(ObserverConfig);
        }
    }
    return config_length;
}

bool observers_serialize_config_to_buffer(uint8_t *buffer, uint32_t *len)
{
    uint8_t *buffer_pos = buffer;
    memcpy(buffer, &(observer_commutation->id), sizeof(observer_commutation->id));
    buffer_pos += sizeof(observer_commutation->id);
    memcpy(buffer, &(observer_position->id), sizeof(observer_position->id));
    buffer_pos += sizeof(observer_position->id);

    for (uint8_t i=0; i<SENSOR_COUNT; i++)
    {
        if (observers[i].initialized == true)
        {
            if (buffer_pos - buffer <= *len - sizeof(ObserverConfig))
            {
                memcpy(buffer_pos, &(observers[i].config), sizeof(ObserverConfig));
                buffer_pos += sizeof(ObserverConfig);
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

bool observers_initialize_with_config_buffer(const uint8_t *buffer, const uint32_t len)
{
    uint8_t *buffer_pos = buffer;
    uint32_t commutation_id, position_id;
    memcpy(&commutation_id, buffer, sizeof(commutation_id));
    buffer_pos += sizeof(commutation_id);
    memcpy(&position_id, buffer, sizeof(position_id));
    buffer_pos += sizeof(position_id);

    ObserverConfig configs[SENSOR_COUNT] = {0};
    for (uint8_t i=0; i<SENSOR_COUNT; i++)
    {
        if (buffer_pos - buffer >= *len - sizeof(ObserverConfig))
        {
            memcpy(&(configs[i]), buffer_pos, sizeof(ObserverConfig));
            buffer_pos += sizeof(ObserverConfig);
        }
        else
        {
            return false;
        }
    }
    for (uint8_t i=0; i<SENSOR_COUNT; i++)
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
    return true;
}