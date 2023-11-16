
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

bool observer_init_with_defaults(Observer *o, Sensor *s)
{
	ObserverConfig c = {.track_bw=350};
    return observer_init_with_config(o, s, &c);
}

bool observer_init_with_config(Observer *o, Sensor *s, ObserverConfig *c)
{
	o->sensor_ptr = &(sensors[sensor_id]);
	o->config = *c;
    observer_set_bw(o, config->track_bw);
	// We keep local copies of a few key variables from
	// the encoder, because it is faster than calling
	// the encoder function pointer
	o->encoder_type = sensor_get_type(s);
	o->encoder_ticks = sensor_get_ticks(s);
	o->encoder_half_ticks = state.encoder_ticks/2;
}

void observer_reset(Observer *o)
{
	o->pos_sector = 0;
	o->pos_estimate_wrapped = 0;
	o->vel_estimate = 0;
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

void observers_init_with_defaults(void)
{
    observer_init(observers[0], sensor_commutation);
	observer_commutation = &(observers[0]);
	observer_position = &(observers[0]);
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

void observers_get_config(ObserversConfig *config_)
{

}

void observers_restore_config(ObserversConfig *config_)
{
	 
}