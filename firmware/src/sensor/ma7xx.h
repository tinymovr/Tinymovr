
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

#include <src/common.h>
#include <src/ssp/ssp_func.h>

typedef struct Sensor Sensor;
typedef struct Observer Observer;
typedef union SensorSpecificConfig SensorSpecificConfig;

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
    SSP_TYPE ssp_port;
    PAC55XX_SSP_TYPEDEF *ssp_struct;
	int16_t rec_table[ECN_SIZE];
    bool rec_calibrated;
} MA7xxSensorConfig;

typedef struct
{
    uint8_t errors;
	int16_t angle;
} MA7xxSensorState;

bool ma7xx_init_with_defaults(Sensor *s);
bool ma7xx_init_with_config(Sensor *s, SensorSpecificConfig *c);
void ma7xx_deinit(Sensor *s);
void ma7xx_reset(Sensor *s);
bool ma7xx_rec_is_calibrated(Sensor *s);
int16_t *ma7xx_get_rec_table_ptr(Sensor *s);
uint8_t ma7xx_get_errors(Sensor *s);
void ma7xx_send_angle_cmd(Sensor *s);
int16_t ma7xx_get_angle_raw(Sensor *s);
int16_t ma7xx_get_angle_rectified(Sensor *s);
void ma7xx_calibrate(Sensor *s, Observer *o);
void ma7xx_update(Sensor *s, bool check_error);
uint16_t ma7xx_write_reg(Sensor *s, uint8_t, uint8_t);
uint8_t ma7xx_read_reg(Sensor *s, uint8_t);
bool ma7xx_calibrate_offset_and_rectification(Sensor *s, Observer *o);
bool ma7xx_calibrate_direction_and_pole_pair_count(Sensor *s, Observer *o);
