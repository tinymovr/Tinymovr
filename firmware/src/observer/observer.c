
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

bool observers_serialize_config_to_buffer(uint8_t *buffer, uint32_t *len)
{

}

bool observers_initialize_with_config_buffer(const uint8_t *buffer, const uint32_t len)
{

}