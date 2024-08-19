
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
#include <src/xfs.h>
#include <src/ssp/ssp_func.h>
#include <src/motor/motor.h>

#if defined(BOARD_REV_R53)
#define ONBOARD_SENSOR_SSP_PORT SSPC
#define ONBOARD_SENSOR_SSP_STRUCT PAC55XX_SSPC
#define EXTERNAL_SENSOR_SSP_PORT SSPD_PD4567
#define EXTERNAL_SENSOR_SSP_STRUCT PAC55XX_SSPD
#elif defined BOARD_REV_R5 || defined BOARD_REV_M5
#define ONBOARD_SENSOR_SSP_PORT SSPC
#define ONBOARD_SENSOR_SSP_STRUCT PAC55XX_SSPC
#define EXTERNAL_SENSOR_SSP_PORT SSPD
#define EXTERNAL_SENSOR_SSP_STRUCT PAC55XX_SSPD
#elif defined BOARD_REV_R3
#define ONBOARD_SENSOR_SSP_PORT SSPD
#define ONBOARD_SENSOR_SSP_STRUCT PAC55XX_SSPD
#endif

typedef struct Sensor Sensor;
typedef struct SensorConfig SensorConfig;
typedef struct Observer Observer;

typedef bool (*sensor_is_calibrated_func_t)(const Sensor *);
typedef bool (*sensor_calibrate_func_t)(Sensor *, Observer *);
typedef int32_t (*sensor_get_raw_angle_func_t)(const Sensor *);
typedef bool (*sensor_init_func_t)(Sensor *);
typedef void (*sensor_deinit_func_t)(Sensor *);
typedef void (*sensor_reset_func_t)(Sensor *);
typedef void (*sensor_prepare_func_t)(const Sensor *);
typedef void (*sensor_update_func_t)(Sensor *, bool);
typedef void (*sensor_get_ss_config_func_t)(Sensor* sensor, void* buffer);
typedef uint8_t (*sensor_get_errors_func_t)(const Sensor *);

typedef enum {
    SENSOR_TYPE_INVALID = 0,
    SENSOR_TYPE_MA7XX = 1,
    SENSOR_TYPE_AS5047 = 2,
    SENSOR_TYPE_AMT22 = 3,
    SENSOR_TYPE_HALL = 4,
    SENSOR_TYPE_MAX
} sensor_type_t;

typedef enum {
    SENSOR_CONNECTION_ONBOARD_SPI = 0,
    SENSOR_CONNECTION_EXTERNAL_SPI = 1,
    SENSOR_CONNECTION_HALL = 2,
    SENSOR_CONNECTION_MAX
} sensor_connection_t;

struct SensorConfig {
    uint32_t id;
    sensor_type_t type;
    int32_t rec_table[ECN_SIZE];
    bool rec_calibrated;
};

struct Sensor { // typedefd earlier
    SensorConfig config;
    sensor_is_calibrated_func_t is_calibrated_func;
    sensor_calibrate_func_t calibrate_func;
    sensor_get_raw_angle_func_t get_raw_angle_func;
    sensor_init_func_t init_func;
    sensor_deinit_func_t deinit_func;
    sensor_reset_func_t reset_func;
    sensor_update_func_t update_func;
    sensor_prepare_func_t prepare_func;
    sensor_get_ss_config_func_t get_ss_config_func;
    sensor_get_errors_func_t get_errors_func;
    uint8_t bits;
    uint32_t ticks;
    float normalization_factor;
    bool initialized : 1;
    bool prepared : 1;
    bool updated : 1;
};

void sensor_reset(Sensor *s);
bool sensor_calibrate_eccentricity_compensation(Sensor *s, Observer *o, FrameTransform *xf_motor_to_sensor);


static inline void sensor_update(Sensor *s, bool check_error)
{
    if (s->updated == false)
    {
        s->update_func(s, check_error);
        s->updated = true;
    }
}

static inline void sensor_invalidate(Sensor *s)
{
    s->updated = false;
    s->prepared = false;
}

static inline uint8_t sensor_get_bits(const Sensor *s)
{
    return s->bits;
}

static inline uint32_t sensor_get_ticks(const Sensor *s)
{
    return s->ticks;
}

static inline sensor_type_t sensor_get_type(const Sensor *s)
{
    return s->config.type;
}

static inline void sensor_prepare(Sensor *s)
{
    if (s->prepare_func && s->prepared == false)
    {
        s->prepare_func(s);
        s->prepared = true;
    }
}

static inline int32_t sensor_get_angle_rectified(const Sensor *s)
{
    const uint8_t offset_bits = (sensor_get_bits(s) - ECN_BITS);
    const int32_t angle = s->get_raw_angle_func(s);
    const int32_t off_1 = s->config.rec_table[angle>>offset_bits];
	const int32_t off_2 = s->config.rec_table[((angle>>offset_bits) + 1) % ECN_SIZE];
	const int32_t off_interp = off_1 + ((off_2 - off_1)* (angle - ((angle>>offset_bits)<<offset_bits))>>offset_bits);
	return angle + off_interp;
}

static inline float sensor_get_angle_rectified_normalized(const Sensor *s)
{
    return sensor_get_angle_rectified(s) * s->normalization_factor;
}