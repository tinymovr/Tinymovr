
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
#include <src/motor/motor.h>

#define SENSOR_COUNT 3

#if defined BOARD_REV_R3
#define ONB§OARD_SENSOR_SSP_PORT SSPD
#define ONBOARD_SENSOR_SSP_STRUCT PAC55XX_SSPD
#elif defined BOARD_REV_R5 || defined BOARD_REV_M5
#define ONBOARD_SENSOR_SSP_PORT SSPC
#define ONBOARD_SENSOR_SSP_STRUCT PAC55XX_SSPC
#define EXTERNAL_SENSOR_SSP_PORT SSPC
#define EXTERNAL_SENSOR_SSP_STRUCT PAC55XX_SSPC
#endif

typedef struct Sensor Sensor;
typedef struct SensorConfig SensorConfig;
typedef struct Observer Observer;

typedef bool (*sensor_is_calibrated_func_t)(const Sensor *);
typedef bool (*sensor_calibrate_func_t)(Sensor *, Observer *);
typedef int16_t (*sensor_get_angle_func_t)(const Sensor *);
typedef void (*sensor_deinit_func_t)(Sensor *);
typedef void (*sensor_reset_func_t)(Sensor *);
typedef void (*sensor_prepare_func_t)(const Sensor *);
typedef void (*sensor_update_func_t)(Sensor *, bool);
typedef uint8_t (*sensor_get_errors_func_t)(const Sensor *);

typedef enum {
    SENSOR_TYPE_INVALID = 0,
    SENSOR_TYPE_MA7XX = 1,
    SENSOR_TYPE_HALL = 2,
    SENSOR_TYPE_AS5047 = 3,
    SENSOR_TYPE_AMT22 = 4,
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
};

struct Sensor { // typedefd earlier
    SensorConfig config;
    sensor_is_calibrated_func_t is_calibrated_func;
    sensor_calibrate_func_t calibrate_func;
    sensor_get_angle_func_t get_angle_func;
    sensor_deinit_func_t deinit_func;
    sensor_reset_func_t reset_func;
    sensor_update_func_t update_func;
    sensor_prepare_func_t prepare_func;
    sensor_get_errors_func_t get_errors_func;
    bool initialized : 1;
    bool current : 1;
};

uint32_t get_next_sensor_id(void);
void sensor_deinit(Sensor *s);
void sensor_reset(Sensor *s);

static inline void sensor_update(Sensor *s, bool check_error)
{
    if (s->current == false)
    {
        s->update_func(s, check_error);
        s->current = true;
    }
}

static inline void sensor_invalidate(Sensor *s)
{
    s->current = false;
}

static inline uint16_t sensor_get_ticks(Sensor *s) {
#ifdef BOARD_REV_R5
    if (SENSOR_TYPE_MA7XX == s->config.type) {
#endif
        // We need to derive this during call, because the motor pole pairs
        // may change after calibration, or after user input
        return 6 * motor_get_pole_pairs();
#ifdef BOARD_REV_R5
    }
    return twopi_by_hall_sectors;
#endif
}

static inline float sensor_ticks_to_eangle(Sensor *s)
{
#ifdef BOARD_REV_R5
    if (SENSOR_TYPE_MA7XX == s->config.type) {
#endif
        // We need to derive this during call, because the motor pole pairs
        // may change after calibration, or after user input
        return twopi_by_enc_ticks * motor_get_pole_pairs();
#ifdef BOARD_REV_R5
    }
    return twopi_by_hall_sectors;
#endif
}

static inline void sensor_prepare(Sensor *s)
{
    if (s->prepare_func && s->current == false)
    {
        s->prepare_func(s);
    }
}
