
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

#pragma once

#include <src/ssp/ssp_func.h>
#include <src/can/can_endpoints.h>
#include <src/sensor/sensor.h>

typedef struct Observer Observer;

typedef enum {
    AS5047P_CMD_NOP              = 0x0000,
    AS5047P_CMD_READ_ANGLE       = 0x3FFF // Adjust based on the AS5047P datasheet
} AS5047PCommand;

typedef struct
{
    SSP_TYPE ssp_port;
    PAC55XX_SSP_TYPEDEF *ssp_struct;
    // Add any additional configuration parameters needed for AS5047P
} AS5047PSensorConfig;

typedef struct
{
    Sensor base;
    AS5047PSensorConfig config;
    uint8_t errors;
    int32_t angle;
} AS5047PSensor;

void as5047p_make_blank_sensor(Sensor *s);
bool as5047p_init_with_port(Sensor *s, const SSP_TYPE port, PAC55XX_SSP_TYPEDEF *ssp_struct);
bool as5047p_init_with_config(Sensor *s, const AS5047PSensorConfig *c);
void as5047p_deinit(Sensor *s);
void as5047p_reset(Sensor *s);
bool as5047p_calibrate(Sensor *s, Observer *o);

static inline bool as5047p_is_calibrated(const Sensor *s)
{
    return s->config.rec_calibrated;
}

static inline uint8_t as5047p_get_errors(const Sensor *s)
{
    return ((const AS5047PSensor *)s)->errors;
}

static inline void as5047p_send_angle_cmd(const Sensor *s)
{
    ssp_write_one(((const AS5047PSensor *)s)->config.ssp_struct, AS5047P_CMD_READ_ANGLE);
}

static inline int32_t as5047p_get_raw_angle(const Sensor *s)
{
    return ((const AS5047PSensor *)s)->angle;
}

static inline void as5047p_update(Sensor *s, bool check_error)
{
    AS5047PSensor *as = (AS5047PSensor *)s;
    const int32_t angle = ssp_read_one(as->config.ssp_struct) & 0x3FFF; // Mask to get the angle value
    if (check_error)
    {
    	const int32_t delta = as->angle - angle;
		if ( ((delta > MAX_ALLOWED_DELTA) || (delta < -MAX_ALLOWED_DELTA)) &&
		     ((delta > MAX_ALLOWED_DELTA_ADD) || (delta < MIN_ALLOWED_DELTA_ADD)) &&
		     ((delta > MAX_ALLOWED_DELTA_SUB) || (delta < MIN_ALLOWED_DELTA_SUB)) )
		{
            as->errors |= SENSORS_SETUP_ONBOARD_ERRORS_READING_UNSTABLE;
		}
    }
    as->angle = angle;
}