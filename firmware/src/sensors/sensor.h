
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
#include <src/sensors/ma7xx.h>
#include <src/sensors/hall.h>
#include <src/sensors/sensor.h>

typedef enum {
    SENSOR_MA7XX = 0,
    SENSOR_HALL = 1,
    SENSOR_AS5047 = 2,
    SENSOR_AMT22 = 3
} sensor_type_t;

typedef union {
    MA7xxSensorConfig ma7xx_config;
    HallSensorConfig hall_config;
    AS5047SensorConfig as5047_config;
    AMT22SensorConfig amt22_config;
} SensorSpecificConfig;

typedef struct {
    SensorType configured_sensor_type;
    SensorSpecificConfig ss_config;
} SensorConfig;

typedef union {
    MA7xxSensorState ma7xx_state;
    HallSensorState hall_state;
    AS5047SensorState as5047_state;
    AMT22SensorState amt22_state;
} SensorSpecificState;

// Generic sensor struct
typedef struct {
    sensor_type_t type;
    sensor_init_func_t init_func;
    sensor_is_calibrated_func_t is_calibrated_func;
    sensor_read_func_t read_func;
    sensor_reset_func_t reset_func;
    sensor_update_func_t update_func;
    sensor_get_error_func_t get_error_func;
    SensorConfig config;
    SensorSpecificState state;
} Sensor;

extern Sensor sensor;

void sensor_init_with_config(SensorConfig *c);
void sensor_deinit(Sensor *s);
void sensor_reset(Sensor *s);
SensorConfig* sensor_get_config(Sensor *s);
void sensor_restore_config(Sensor *s, SensorConfig* config_);

inline int16_t sensor_get_angle(Sensor *s)
{
    return s->read_func();
}

inline void sensor_update(Sensor *s, bool check_error)
{
    s->update_func(check_error);
}

inline uint16_t sensor_get_ticks(Sensor *s) {
#ifdef BOARD_REV_R5
    if (SENSOR_MA7XX == s->type) {
#endif
        // We need to derive this during call, because the motor pole pairs
        // may change after calibration, or after user input
        return 6 * motor_get_pole_pairs();
#ifdef BOARD_REV_R5
    }
    return twopi_by_hall_sectors;
#endif
}

inline float sensor_ticks_to_eangle(Sensor *s)
{
#ifdef BOARD_REV_R5
    if (SENSOR_MA7XX == s->type) {
#endif
        // We need to derive this during call, because the motor pole pairs
        // may change after calibration, or after user input
        return twopi_by_enc_ticks * motor_get_pole_pairs();
#ifdef BOARD_REV_R5
    }
    return twopi_by_hall_sectors;
#endif
}

inline SensorType sensor_get_type(Sensor *s)
{
    return s->type;
}

inline void sensor_set_type(Sensor *s, SensorType sensor_type)
{
#ifdef BOARD_REV_R5
    if (SENSOR_MA7XX == sensor_type || SENSOR_HALL == sensor_type) {
#endif
        s->type = sensor_type;
        sensor_init(s);  // Re-initialize sensor with new type
#ifdef BOARD_REV_R5
    }
#endif
}

inline bool sensor_get_calibrated(Sensor *s)
{
    return s->is_calibrated_func();
}

inline uint8_t sensor_get_errors(Sensor *s)
{
    return s->get_error_func();
}

