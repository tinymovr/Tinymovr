
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
#include <src/motor/motor.h>
#include <src/sensor/ma7xx.h>
#include <src/sensor/hall.h>

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
typedef union SensorSpecificConfig SensorSpecificConfig;
typedef struct SensorConfig SensorConfig;
typedef union SensorSpecificState SensorSpecificState;
typedef struct SensorsConfig SensorsConfig;
typedef struct Observer Observer;

typedef bool (*sensor_is_calibrated_func_t)(Sensor *);
typedef bool (*sensor_calibrate_func_t)(Sensor *, Observer *);
typedef int16_t (*sensor_get_angle_func_t)(Sensor *);
typedef void (*sensor_reset_func_t)(Sensor *);
typedef void (*sensor_prepare_func_t)(Sensor *);
typedef void (*sensor_update_func_t)(Sensor *, bool);
typedef uint8_t (*sensor_get_errors_func_t)(Sensor *);

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

union SensorSpecificConfig {
    MA7xxSensorConfig ma7xx_config;
    HallSensorConfig hall_config;
    // AS504xSensorConfig as5047_config;
    // AMT22SensorConfig amt22_config;
};

struct SensorConfig {
    SensorSpecificConfig ss_config;
    uint32_t id;
    sensor_type_t type;
};

union SensorSpecificState{
    MA7xxSensorState ma7xx_state;
    HallSensorState hall_state;
    // AS504xSensorState as5047_state;
    // AMT22SensorState amt22_state;
};

struct Sensor { // typedefd earlier
    SensorConfig config;
    SensorSpecificState state;
    sensor_is_calibrated_func_t is_calibrated_func;
    sensor_calibrate_func_t calibrate_func;
    sensor_get_angle_func_t get_angle_func;
    sensor_reset_func_t reset_func;
    sensor_update_func_t update_func;
    sensor_prepare_func_t prepare_func;
    sensor_get_errors_func_t get_errors_func;
    bool initialized : 1;
    bool current : 1;
};

struct SensorsConfig {
    SensorConfig config[SENSOR_COUNT];
    sensor_connection_t commutation_connection;
    sensor_connection_t position_connection;
};

// The sequence in the `sensors` array is determined so that
// 0: onboard sensor, 1: external spi and 3: hall sensor
// index same as the members of `sensor_connection_t`
Sensor sensors[SENSOR_COUNT];

Sensor *commutation_sensor_p;
Sensor *position_sensor_p;

uint32_t get_next_sensor_id(void);
bool sensor_init_with_defaults(Sensor *s);
bool sensor_init_with_config(Sensor *s, SensorConfig *c);
void sensor_deinit(Sensor *s);
void sensor_reset(Sensor *s);

void sensors_init_with_defaults(void);
void sensors_get_config(SensorsConfig *config_);
void sensors_restore_config(SensorsConfig *config_);

static inline sensor_connection_t sensor_get_connection(Sensor *sensor)
{
    // Derive pointer array offset
    return sensor - sensors;
}

void sensor_set_connection(Sensor** target_sensor, Sensor** other_sensor, sensor_connection_t new_connection);

static inline int16_t sensor_get_angle(Sensor *s)
{
    return s->get_angle_func(s);
}

static inline void sensor_update(Sensor *s, bool check_error)
{
    if (s->current == false)
    {
        s->update_func(s, check_error);
        s->current = true;
    }
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

static inline sensor_type_t sensor_get_type(Sensor *s)
{
    return s->config.type;
}

static inline bool sensor_get_calibrated(Sensor *s)
{
    return s->is_calibrated_func(s);
}

static inline bool sensor_calibrate(Sensor *s, Observer *o)
{
    return s->calibrate_func(s, o);
}

static inline uint8_t sensor_get_errors(Sensor *s)
{
    return s->get_errors_func(s);
}

static inline void sensor_invalidate(Sensor *s)
{
    s->current = false;
}

static inline void sensor_prepare(Sensor *s)
{
    if (s->prepare_func && s->current == false)
    {
        s->prepare_func(s);
    }
}
