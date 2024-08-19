
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

typedef enum {
    AMT22_CMD_READ_ANGLE       = 0xFFFF
} AMT22Command;

typedef struct
{
    SSP_TYPE ssp_port;
    PAC55XX_SSP_TYPEDEF *ssp_struct;
    sensors_setup_external_spi_rate_options rate;
} AMT22SensorConfig;

typedef struct
{
    Sensor base;
    AMT22SensorConfig config;
    uint8_t errors;
    int32_t angle;
    uint8_t val_l;
    uint8_t val_h;

} AMT22Sensor;

void amt22_make_blank_sensor(Sensor *s);
bool amt22_init_with_port_and_rate(Sensor *s, const SSP_TYPE port, PAC55XX_SSP_TYPEDEF *ssp_struct, sensors_setup_external_spi_rate_options rate);
bool amt22_init_with_config(Sensor *s, const AMT22SensorConfig *c);
bool amt22_init(Sensor *s);
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
    // AMT22 has specific timing requirements with respect to reading
    // the first and second bytes of the angle.
    // TODO: These delays make reading of the sensor really slow
    // refactor using interrupts
    AMT22Sensor *as = ((AMT22Sensor *)s);
    as->config.ssp_struct->SSCR.SWSS = 0;
    delay_us(3);
    ssp_write_one(as->config.ssp_struct, AMT22_CMD_READ_ANGLE);
    as->val_h = ssp_read_one(as->config.ssp_struct);
    delay_us(3);
    ssp_write_one(as->config.ssp_struct, AMT22_CMD_READ_ANGLE);
    as->val_l = ssp_read_one(as->config.ssp_struct);
    delay_us(3);
    as->config.ssp_struct->SSCR.SWSS = 1;
}

static inline int32_t amt22_get_raw_angle(const Sensor *s)
{
    return ((const AMT22Sensor *)s)->angle;
}

static inline void amt22_update(Sensor *s, bool check_error)
{
    AMT22Sensor *as = (AMT22Sensor *)s;

    const int32_t value = (((as->val_h & 0xff) << 8) | (as->val_l & 0xff)); 
    
    // TODO: This checksum calculation is sub-optimal
    bool binaryArray[16];

    for(int i = 0; i < 16; i++) binaryArray[i] = (0x01) & (value >> (i));

    if ((binaryArray[15] == !(binaryArray[13] ^ binaryArray[11] ^ binaryArray[9] ^ binaryArray[7] ^ binaryArray[5] ^ binaryArray[3] ^ binaryArray[1]))
          && (binaryArray[14] == !(binaryArray[12] ^ binaryArray[10] ^ binaryArray[8] ^ binaryArray[6] ^ binaryArray[4] ^ binaryArray[2] ^ binaryArray[0])))
    {
        const int32_t angle = value & 0x3FFF; 
        if (check_error)
        {
            const int32_t delta = as->angle - angle;
            if ( ((delta > AMT22_MAX_ALLOWED_DELTA) || (delta < -AMT22_MAX_ALLOWED_DELTA)) &&
                ((delta > AMT22_MAX_ALLOWED_DELTA_ADD) || (delta < AMT22_MIN_ALLOWED_DELTA_ADD)) &&
                ((delta > AMT22_MAX_ALLOWED_DELTA_SUB) || (delta < AMT22_MIN_ALLOWED_DELTA_SUB)) )
            {
                
            }
            else
            {
                as->angle = value & 0x3FFF;
            }
        }
        
    }
    
}


