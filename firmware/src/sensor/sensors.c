
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

#include <src/sensor/sensors.h>
#include <src/observer/observer.h>
#include <src/controller/controller.h>

GenSensor sensors[SENSOR_COUNT] = {0};

bool sensor_init_with_defaults(Sensor *s)
{
    // Here we check the sensor connection, either
    // ONBOARD, EXTERNAL_SPI or EXTERNAL_HALL. We need
    // to do this here in order to set up the config
    // struct accordingly.
    sensor_connection_t connection = sensor_get_connection(s);
    switch (connection)
    {
        case SENSOR_CONNECTION_ONBOARD_SPI:
            ma7xx_make_blank_sensor(s);
            return ma7xx_init_with_port(s, ONBOARD_SENSOR_SSP_PORT, ONBOARD_SENSOR_SSP_STRUCT);
            break;
        case SENSOR_CONNECTION_EXTERNAL_SPI:
            switch (s->config.type)
            {
                case SENSOR_TYPE_MA7XX:
                    ma7xx_make_blank_sensor(s);
                    return ma7xx_init_with_port(s, EXTERNAL_SENSOR_SSP_PORT, EXTERNAL_SENSOR_SSP_STRUCT);
                    break;
                case SENSOR_TYPE_AS5047:
                    as5047p_make_blank_sensor(s);
                    return as5047p_init_with_port(s, EXTERNAL_SENSOR_SSP_PORT, EXTERNAL_SENSOR_SSP_STRUCT);
                    break;
                default:
                    break;
            }
            break;
        case SENSOR_CONNECTION_HALL:
            hall_make_blank_sensor(s);
            return hall_init_with_defaults(s);
            break;
        default:
            break;
    }
    return false;
}

bool sensor_init_with_configs(Sensor *s, SensorConfig *sc, GenSensorConfig *gsc)
{
    switch (sc->type)
    {
        case SENSOR_TYPE_MA7XX:
            ma7xx_make_blank_sensor(s);
            return ma7xx_init_with_config(s, &(gsc->ma7xx_sensor_config));
            break;
        case SENSOR_TYPE_HALL:
            hall_make_blank_sensor(s);
            return hall_init_with_config(s, &(gsc->hall_sensor_config));
            break;
        case SENSOR_TYPE_AS5047:
            as5047p_make_blank_sensor(s);
            return as5047p_init_with_config(s, &(gsc->as5047p_sensor_config));
            break;
        default:
            break;
    }
    return false;
}

void sensors_init_with_defaults(void)
{
    sensor_init_with_defaults(&(sensors[0].sensor));
    ma7xx_make_blank_sensor(&(sensors[1].sensor));
    hall_make_blank_sensor(&(sensors[2].sensor));
    commutation_sensor_p = &(sensors[0].sensor);
    position_sensor_p = &(sensors[0].sensor);
}

void sensors_get_config(SensorsConfig *config_)
{
    // Serialize SensorConfig array
    for (int i = 0; i < SENSOR_COUNT; ++i)
    {
        config_->config[i] = sensors[i].sensor.config;
    }

    // Serialize GenSensorConfig array
    for (int i = 0; i < SENSOR_COUNT; ++i)
    {
        sensors[i].sensor.get_ss_config_func(&(sensors[i].sensor), &(config_->ss_config[i]));
    }
    config_->commutation_connection = sensor_get_connection(commutation_sensor_p);
    config_->position_connection = sensor_get_connection(position_sensor_p);
}

void sensors_restore_config(SensorsConfig *config_)
{
    sensor_set_pointer_with_connection(&commutation_sensor_p, config_->commutation_connection);
    sensor_set_pointer_with_connection(&position_sensor_p, config_->position_connection);

    // Restore SensorConfig array
    for (int i = 0; i < SENSOR_COUNT; ++i)
    {
        sensors[i].sensor.config = config_->config[i];
        if ((sensors[i].sensor.initialized == false) 
            && (commutation_sensor_p == &(sensors[i].sensor)
            || position_sensor_p == &(sensors[i].sensor)))
        {
            sensor_init_with_configs(commutation_sensor_p, &(config_->config[i]), &(config_->ss_config[i]));
        }
    }
}

void commutation_sensor_set_connection(sensor_connection_t new_connection)
{
    sensor_set_connection(&(commutation_sensor_p), &(position_sensor_p), new_connection);
}

void position_sensor_set_connection(sensor_connection_t new_connection)
{
    sensor_set_connection(&(position_sensor_p), &(commutation_sensor_p), new_connection);
}

void sensor_external_spi_set_type_avlos(sensors_setup_external_spi_type_options type)
{
    sensor_type_t internal_type;
    bool valid = false;
    switch (type)
    {
        case SENSORS_SETUP_EXTERNAL_SPI_TYPE_MA7XX:
            internal_type = SENSOR_TYPE_MA7XX;
            valid = true;
            break;
        case SENSORS_SETUP_EXTERNAL_SPI_TYPE_AS5047:
            internal_type = SENSOR_TYPE_AS5047;
            valid = true;
            break;
        case SENSORS_SETUP_EXTERNAL_SPI_TYPE_AMT22:
            internal_type = SENSOR_TYPE_AMT22;
            valid = true;
            break;
        default:
            break;
    }
    if (valid 
        && (controller_get_state() == CONTROLLER_STATE_IDLE)
        && (internal_type != sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor.config.type))
    {
        Sensor *s = &(sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor);
        if (s->initialized)
        {
            s->deinit_func(s);
        }
        sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor.config.type = internal_type;
        sensor_init_with_defaults(s);
    }
}

void sensor_set_connection(Sensor** target_sensor, Sensor** other_sensor, sensor_connection_t new_connection)
{
    if (controller_get_state() == CONTROLLER_STATE_IDLE
        && new_connection != sensor_get_connection(*target_sensor) 
        && new_connection >= 0 && new_connection < SENSOR_CONNECTION_MAX)
    {
        if (sensor_get_connection(*target_sensor) != sensor_get_connection(*other_sensor))
        {
            (*target_sensor)->deinit_func(*target_sensor);
        }

        *target_sensor = &(sensors[new_connection].sensor);

        if (new_connection != sensor_get_connection(*other_sensor))
        {
            sensor_init_with_defaults(*target_sensor);
        }
    }
}

bool sensors_calibrate_pole_pair_count_and_transforms(void)
{
    const float I_setpoint = motor_get_I_cal();
    set_epos_and_wait(0, I_setpoint);
    wait_pwm_cycles(CAL_STAY_LEN);

    const float motor_frame_end = CAL_PHASE_TURNS * TWOPI;
    const float commutation_frame_start = commutation_observer_get_pos_estimate();
    const float position_frame_start = position_observer_get_pos_estimate();

    // Move to end
    for (uint32_t i = 0; i < CAL_DIR_LEN; i++)
    {
        set_epos_and_wait(motor_frame_end * ((float)i / CAL_DIR_LEN), I_setpoint);
    }

    set_epos_and_wait(motor_frame_end, I_setpoint);
    wait_pwm_cycles(CAL_STAY_LEN);
    const float commutation_frame_end = commutation_observer_get_pos_estimate();
    const float position_frame_end = position_observer_get_pos_estimate();

    // Find pole pairs if not Hall; otherwise have to be defined manually
    if (sensor_get_type(commutation_sensor_p) != SENSOR_TYPE_HALL)
    {
        if (!motor_find_pole_pairs(SENSOR_COMMON_RES_TICKS, commutation_frame_start, commutation_frame_end, motor_frame_end))
        {
            uint8_t *error_ptr = motor_get_error_ptr();
            *error_ptr |= MOTOR_ERRORS_INVALID_POLE_PAIRS;
            return false;
        }
    }

    const float motor_frame_end_ticks = SENSOR_COMMON_RES_TICKS * (motor_frame_end / (TWOPI * motor_get_pole_pairs()));

    // Move back to start epos
    for (uint32_t i = 0; i < CAL_DIR_LEN; i++)
    {
        set_epos_and_wait(motor_frame_end * (1.0f - ((float)i / CAL_DIR_LEN)), I_setpoint);
    }
    gate_driver_set_duty_cycle(&three_phase_zero);

    // Derive transforms
    frame_set_commutation_sensor_to_motor(derive_dir_transform_2p(commutation_frame_start, 0, commutation_frame_end, motor_frame_end_ticks));
    if (commutation_sensor_p == position_sensor_p)
    {
        frame_set_position_sensor_to_motor(frames.commutation_sensor_to_motor);
    }
    else
    {
        frame_set_position_sensor_to_motor(derive_transform_2p(position_frame_start, 0, position_frame_end, motor_frame_end_ticks));
    }

    return true;
}