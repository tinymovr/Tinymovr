
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
    MA_CMD_NOP              = 0x0000,
    MA_CMD_ANGLE            = 0x0000,
    MA_CMD_WRITE            = 0x8000,
    MA_CMD_READ             = 0x4000
} MA702Command;

typedef struct
{
    SSP_TYPE ssp_port;
    PAC55XX_SSP_TYPEDEF *ssp_struct;
} MA7xxSensorConfig;

typedef struct
{
    Sensor base;
    MA7xxSensorConfig config;
    uint8_t errors;
	int16_t angle;
} MA7xxSensor;

bool ma7xx_init_with_port(Sensor *s, const SSP_TYPE port, PAC55XX_SSP_TYPEDEF *ssp_struct);
bool ma7xx_init_with_config(Sensor *s, const MA7xxSensorConfig *c);
void ma7xx_deinit(Sensor *s);
void ma7xx_reset(Sensor *s);
bool ma7xx_calibrate(Sensor *s, Observer *o);

static inline bool ma7xx_rec_is_calibrated(const Sensor *s)
{
    return s->config.rec_calibrated;
}

static inline uint8_t ma7xx_get_errors(const Sensor *s)
{
    return ((const MA7xxSensor *)s)->errors;
}

static inline void ma7xx_send_angle_cmd(const Sensor *s)
{
	ssp_write_one(((const MA7xxSensor *)s)->config.ssp_struct, MA_CMD_ANGLE);
}

static inline int16_t ma7xx_get_raw_angle(const Sensor *s)
{
    return ((const MA7xxSensor *)s)->angle;
}

static inline void ma7xx_update(Sensor *s, bool check_error)
{
    MA7xxSensor *ms = (MA7xxSensor *)s;
    const int16_t angle = ssp_read_one(ms->config.ssp_struct);

    if (check_error)
    {
    	const int16_t delta = ms->angle - angle;
		if ( ((delta > MAX_ALLOWED_DELTA) || (delta < -MAX_ALLOWED_DELTA)) &&
		     ((delta > MAX_ALLOWED_DELTA_ADD) || (delta < MIN_ALLOWED_DELTA_ADD)) &&
		     ((delta > MAX_ALLOWED_DELTA_SUB) || (delta < MIN_ALLOWED_DELTA_SUB)) )
		{
            ms->errors |= SENSORS_SETUP_ONBOARD_ERRORS_READING_UNSTABLE;
		}
    }
    ms->angle = angle;
}

/**
 * @brief Write to a register of the sensor
 * 
 * @param reg The 5-bit register address
 * @param value The value to write
 * @return uint16_t 
 */
static inline uint16_t ma7xx_write_reg(const Sensor *s, uint8_t reg, uint8_t value)
{
    const MA7xxSensor *ms = (const MA7xxSensor *)s;
    uint16_t cmd = MA_CMD_WRITE | reg << 8 | value;
    uint32_t result = ssp_write_one(ms->config.ssp_struct, cmd);

    // Delay at least 20ms to let the encoder write to memory
    delay_us(20100);
    result |= ssp_write_one(ms->config.ssp_struct, 0);

    // The encoder returns the value written to memory, so check that it is the same as what we wrote
    uint8_t retval =  ssp_read_one(ms->config.ssp_struct) >> 8;
    if ((retval != value) || (result != 0))
    {
        return false;
    }
    return true;
}

/**
 * @brief Read from a register of the sensor
 * 
 * @param register The 5-bit register address
 * @return uint8_t 
 */
static inline uint8_t ma7xx_read_reg(const Sensor *s, uint8_t reg)
{
    const MA7xxSensor *ms = (const MA7xxSensor *)s;
    uint16_t cmd[2] = {MA_CMD_READ  | (reg << 8), 0};
    uint16_t result = ssp_write_multi(ms->config.ssp_struct, cmd, 2u);
    if (result != 0)
    {
        return false;
    }
    return ssp_read_one(ms->config.ssp_struct) >> 8;
}