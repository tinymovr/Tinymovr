
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

#include <src/sensor/sensor.h>

#if defined BOARD_REV_R3
#define ONBOARD_SENSOR_SSP_PORT SSPD
#define ONBOARD_SENSOR_SSP_STRUCT PAC55XX_SSPD
#elif defined BOARD_REV_R5 || defined BOARD_REV_M5
#define ONBOARD_SENSOR_SSP_PORT SSPC
#define ONBOARD_SENSOR_SSP_STRUCT PAC55XX_SSPC
#endif

#define MAX_ALLOWED_DELTA     (ENCODER_TICKS / 6)
#define MAX_ALLOWED_DELTA_ADD (MAX_ALLOWED_DELTA + ENCODER_TICKS)
#define MAX_ALLOWED_DELTA_SUB (MAX_ALLOWED_DELTA - ENCODER_TICKS)
#define MIN_ALLOWED_DELTA_ADD (-MAX_ALLOWED_DELTA + ENCODER_TICKS)
#define MIN_ALLOWED_DELTA_SUB (-MAX_ALLOWED_DELTA - ENCODER_TICKS)

typedef enum {
    MA_CMD_NOP              = 0x0000,
    MA_CMD_ANGLE            = 0x0000,
    MA_CMD_WRITE            = 0x8000,
    MA_CMD_READ             = 0x4000
} MA702Command;

typedef struct
{
	bool rec_calibrated;
	int16_t rec_table[ECN_SIZE];
} MA7xxSensorConfig;

typedef struct
{
    uint8_t errors;
	int16_t angle;
    uint8_t error;
} MA7xxSensorState;

Sensor ma7xx_init_with_ssp_port(SSP_TYPE ssp);
Sensor ma7xx_init_with_config(SensorSpecificConfig *c);
void ma7xx_deinit(Sensor *s);
void ma7xx_clear_rec_table(Sensor *s);
void ma7xx_set_rec_calibrated(Sensor *s);
bool ma7xx_rec_is_calibrated(Sensor *s);
int16_t *ma7xx_get_rec_table_ptr(Sensor *s);
uint16_t ma7xx_write_reg(Sensor *s, uint8_t, uint8_t);
uint8_t ma7xx_read_reg(Sensor *s, uint8_t);

inline uint8_t ma7xx_get_errors(Sensor *s)
{
    return state.errors;
}

inline void ma7xx_send_angle_cmd(Sensor *s)
{
	ssp_write_one(ONBOARD_SENSOR_SSP_STRUCT, MA_CMD_ANGLE);
}

inline int16_t ma7xx_get_angle_raw(Sensor *s)
{
    return state.angle;
}

inline int16_t ma7xx_get_angle_rectified(Sensor *s)
{
    const uint8_t offset_bits = (ENCODER_BITS - ECN_BITS);
    const int16_t angle = state.angle;
    const int16_t off_1 = config.rec_table[angle>>offset_bits];
	const int16_t off_2 = config.rec_table[((angle>>offset_bits) + 1) % ECN_SIZE];
	const int16_t off_interp = off_1 + ((off_2 - off_1)* (angle - ((angle>>offset_bits)<<offset_bits))>>offset_bits);
	return angle + off_interp;
}

inline void ma7xx_update(Sensor *s, bool check_error)
{
    const int16_t angle = ssp_read_one(ONBOARD_SENSOR_SSP_STRUCT) >> 3;

    if (check_error)
    {
    	const int16_t delta = state.angle - angle;
		if ( ((delta > MAX_ALLOWED_DELTA) || (delta < -MAX_ALLOWED_DELTA)) &&
		     ((delta > MAX_ALLOWED_DELTA_ADD) || (delta < MIN_ALLOWED_DELTA_ADD)) &&
		     ((delta > MAX_ALLOWED_DELTA_SUB) || (delta < MIN_ALLOWED_DELTA_SUB)) )
		{
            state.errors |= ENCODER_ERRORS_READING_UNSTABLE;
		}
    }
    state.angle = angle;
}