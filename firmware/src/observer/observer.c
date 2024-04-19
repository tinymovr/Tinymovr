
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

bool observer_init_with_defaults(Observer *o, Sensor **s)
{
	ObserverConfig c = {.track_bw=350};
    return observer_init_with_config(o, s, &c);
}

bool observer_init_with_config(Observer *o, Sensor **s, ObserverConfig *c)
{
	o->sensor_ptr = s;
	o->config = *c;
    observer_set_bandwidth(o, c->track_bw);
	return true;
}

void observer_update_params(Observer *o)
{
    o->config.kp = 2.0f * o->config.track_bw;
	o->config.ki = 0.25f * (o->config.kp * o->config.kp);
	o->config.kp_period = o->config.kp * PWM_PERIOD_S;
	o->config.ki_period = o->config.ki * PWM_PERIOD_S;
}

void observer_reset_state(Observer *o)
{
	o->pos_sector = 0;
	o->pos_estimate_wrapped = 0;
	o->vel_estimate = 0;
	o->current = false;
}

float observer_get_bandwidth(Observer *o)
{
    return o->config.track_bw;
}

void observer_set_bandwidth(Observer *o, float bw)
{
    if (bw > 0.0f)
    {
        o->config.track_bw = bw;
		observer_update_params(o);
    }
}

void observers_init_with_defaults(void)
{
    observer_init_with_defaults(&commutation_observer, &commutation_sensor_p);
	observer_init_with_defaults(&position_observer, &position_sensor_p);
}

void observers_get_config(ObserversConfig *_config)
{
	_config->config_commutation = commutation_observer.config;
	_config->config_position = position_observer.config;
}

void observers_restore_config(ObserversConfig *_config)
{
	observer_init_with_config(&commutation_observer, &commutation_sensor_p, &(_config->config_commutation));
	observer_init_with_config(&position_observer, &position_sensor_p, &(_config->config_position));
	observer_update_params(&commutation_observer);
	observer_update_params(&position_observer);
}

void commutation_observer_set_bandwidth(float bw)
{
	observer_set_bandwidth(&commutation_observer, bw);
}

void position_observer_set_bandwidth(float bw)
{
	observer_set_bandwidth(&position_observer, bw);
}