
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

#define AMT22_BITS (14) // Assuming a 14-bit resolution for consistency
#define AMT22_TICKS (1 << AMT22_BITS)
#define AMT22_MAX_ALLOWED_DELTA     (AMT22_TICKS / 6)
#define AMT22_MAX_ALLOWED_DELTA_ADD (AMT22_MAX_ALLOWED_DELTA + AMT22_TICKS)
#define AMT22_MAX_ALLOWED_DELTA_SUB (AMT22_MAX_ALLOWED_DELTA - AMT22_TICKS)
#define AMT22_MIN_ALLOWED_DELTA_ADD (-AMT22_MAX_ALLOWED_DELTA + AMT22_TICKS)
#define AMT22_MIN_ALLOWED_DELTA_SUB (-AMT22_MAX_ALLOWED_DELTA - AMT22_TICKS)

typedef struct Observer Observer;

// Define commands relevant to the AMT22 here. For simplicity, removed commands specific to the AS5047.
typedef enum {
    AMT22_CMD_READ_ANGLE       = 0xFFFF // Placeholder command; update based on the AMT22 datasheet
} AMT22Command;

typedef struct
{
    SSP_TYPE ssp_port;
    PAC55XX_SSP_TYPEDEF *ssp_struct;
    // Add any additional configuration parameters needed for AMT22
} AMT22SensorConfig;

typedef struct
{
    Sensor base;
    AMT22SensorConfig config;
    uint8_t errors;
    int32_t angle;
} AMT22Sensor;

void amt22_make_blank_sensor(Sensor *s);
bool amt22_init_with_port(Sensor *s, const SSP_TYPE port, PAC55XX_SSP_TYPEDEF *ssp_struct);
bool amt22_init_with_config(Sensor *s, const AMT22SensorConfig *c);
void amt22_deinit(Sensor *s);
void amt22_reset(Sensor *s);
void amt22_get_ss_config(Sensor *s, void* buffer);

static inline bool amt22_is_calibrated(const Sensor *s)
{
    return s->config.rec_calibrated;
}

static inline uint8_t amt22_get_errors(const Sensor *s)
{
    return ((const AMT22Sensor *)s)->errors;
}

static inline void amt22_send_angle_cmd(const Sensor *s)
{
    ssp_write_one(((const AMT22Sensor *)s)->config.ssp_struct, AMT22_CMD_READ_ANGLE);
}

static inline int32_t amt22_get_raw_angle(const Sensor *s)
{
    return ((const AMT22Sensor *)s)->angle;
}

static inline void amt22_update(Sensor *s, bool check_error)
{
    AMT22Sensor *as = (AMT22Sensor *)s;
    volatile uint16_t read_value = ssp_read_one(as->config.ssp_struct);
    const int32_t angle = read_value & 0x3FFF; // Mask to get the angle value, assuming 14-bit resolution
    if (check_error)
    {
        // Implement error checking specific to the AMT22, if necessary
    }
    as->angle = angle;
}
