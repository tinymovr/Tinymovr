
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

void sensor_make_blank(Sensor *s)
{
    // Here we check the sensor connection, either
    // ONBOARD, EXTERNAL_SPI or EXTERNAL_HALL. We need
    // to do this here in order to set up the config
    // struct accordingly.
    s->initialized = false;
    sensor_connection_t connection = sensor_get_connection(s);
    switch (connection)
    {
        case SENSOR_CONNECTION_ONBOARD_SPI:
            ma7xx_make_blank_sensor(s);
            break;
        case SENSOR_CONNECTION_EXTERNAL_SPI:
            switch (s->config.type)
            {
                case SENSOR_TYPE_MA7XX:
                    ma7xx_make_blank_sensor(s);
                    break;
                case SENSOR_TYPE_AS5047:
                    as5047p_make_blank_sensor(s);
                    break;
                case SENSOR_TYPE_AMT22:
                    amt22_make_blank_sensor(s);
                    break;
                default:
                    ma7xx_make_blank_sensor(s);
                    break;
            }
            break;
        case SENSOR_CONNECTION_HALL:
            hall_make_blank_sensor(s);
            break;
        default:
            break;
    }
}

bool sensor_init_with_defaults(Sensor *s)
{
    sensor_make_blank(s);
    // Here we check the sensor connection, either
    // ONBOARD, EXTERNAL_SPI or EXTERNAL_HALL. We need
    // to do this here in order to set up the config
    // struct accordingly.
    sensor_connection_t connection = sensor_get_connection(s);
    switch (connection)
    {
        case SENSOR_CONNECTION_ONBOARD_SPI:
            return ma7xx_init_with_port_and_rate(s, ONBOARD_SENSOR_SSP_PORT, ONBOARD_SENSOR_SSP_STRUCT, SENSORS_SETUP_EXTERNAL_SPI_RATE_12Mbps);
            break;
#if defined BOARD_REV_R5 || defined BOARD_REV_M5
        case SENSOR_CONNECTION_EXTERNAL_SPI:
            switch (s->config.type)
            {
                case SENSOR_TYPE_MA7XX:
                    return ma7xx_init_with_port_and_rate(s, EXTERNAL_SENSOR_SSP_PORT, EXTERNAL_SENSOR_SSP_STRUCT, SENSORS_SETUP_EXTERNAL_SPI_RATE_3Mbps);
                    break;
                case SENSOR_TYPE_AS5047:
                    return as5047p_init_with_port_and_rate(s, EXTERNAL_SENSOR_SSP_PORT, EXTERNAL_SENSOR_SSP_STRUCT, SENSORS_SETUP_EXTERNAL_SPI_RATE_3Mbps);
                    break;
                case SENSOR_TYPE_AMT22:
                    return amt22_init_with_port_and_rate(s, EXTERNAL_SENSOR_SSP_PORT, EXTERNAL_SENSOR_SSP_STRUCT, SENSORS_SETUP_EXTERNAL_SPI_RATE_3Mbps);
                    break;
                default:
                    break;
            }
            break;

        case SENSOR_CONNECTION_HALL:
            return hall_init_with_defaults(s);
            break;
#endif
        default:
            break;
    }
    return false;
}

bool sensor_init_with_configs(Sensor *s, SensorConfig *sc, GenSensorConfig *gsc)
{
    sensor_make_blank(s);
    switch (sc->type)
    {
        case SENSOR_TYPE_MA7XX:
            return ma7xx_init_with_config(s, &(gsc->ma7xx_sensor_config));
            break;
        case SENSOR_TYPE_HALL:
            return hall_init_with_config(s, &(gsc->hall_sensor_config));
            break;
        case SENSOR_TYPE_AS5047:
            return as5047p_init_with_config(s, &(gsc->as5047p_sensor_config));
            break;
        case SENSOR_TYPE_AMT22:
            return amt22_init_with_config(s, &(gsc->amt22_sensor_config));
            break;
        default:
            break;
    }
    return false;
}

void sensors_init_with_defaults(void)
{
    sensor_init_with_defaults(&(sensors[0].sensor));
    sensor_make_blank(&(sensors[1].sensor));
    sensor_make_blank(&(sensors[2].sensor));
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
            sensor_init_with_configs(&(sensors[i].sensor), &(config_->config[i]), &(config_->ss_config[i]));
        }
        else if (sensors[i].sensor.initialized == false)
        {
            sensor_make_blank(&(sensors[i].sensor));
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
    if (type < SENSORS_SETUP_EXTERNAL_SPI_TYPE__MAX
        && (controller_get_state() == CONTROLLER_STATE_IDLE))
    {
        sensor_type_t internal_type;
        switch (type)
        {
            case SENSORS_SETUP_EXTERNAL_SPI_TYPE_MA7XX:
                internal_type = SENSOR_TYPE_MA7XX;
                break;
            case SENSORS_SETUP_EXTERNAL_SPI_TYPE_AS5047:
                internal_type = SENSOR_TYPE_AS5047;
                break;
            case SENSORS_SETUP_EXTERNAL_SPI_TYPE_AMT22:
                internal_type = SENSOR_TYPE_AMT22;
                break;
            default:
                internal_type = SENSOR_TYPE_MA7XX;
                break;
        }
        frames_reset_calibrated();
        
        Sensor *s = &(sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor);
        if (s->initialized)
        {
            s->deinit_func(s);
        }
        sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor.config.type = internal_type;
        sensor_init_with_defaults(s);
    }
}

void sensor_external_spi_set_rate_avlos(sensors_setup_external_spi_rate_options rate)
{
    if (rate < SENSORS_SETUP_EXTERNAL_SPI_RATE__MAX
        && (controller_get_state() == CONTROLLER_STATE_IDLE))
    {
        sensors_setup_external_spi_rate_options current_rate;
        switch (sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor.config.type)
        {
            case SENSOR_TYPE_MA7XX:
                current_rate = sensors[SENSOR_CONNECTION_EXTERNAL_SPI].ma7xx_sensor.config.rate;
                break;
            case SENSOR_TYPE_AS5047:
                current_rate = sensors[SENSOR_CONNECTION_EXTERNAL_SPI].as5047p_sensor.config.rate;
                break;
            case SENSOR_TYPE_AMT22:
                current_rate = sensors[SENSOR_CONNECTION_EXTERNAL_SPI].amt22_sensor.config.rate;
                break;
            default:
                current_rate = sensors[SENSOR_CONNECTION_EXTERNAL_SPI].ma7xx_sensor.config.rate;
                break;
        }
        if (rate != current_rate)
        {
            sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor.deinit_func(&(sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor));
            switch (sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor.config.type)
            {
                case SENSOR_TYPE_MA7XX:
                    sensors[SENSOR_CONNECTION_EXTERNAL_SPI].ma7xx_sensor.config.rate = rate;
                    break;
                case SENSOR_TYPE_AS5047:
                    sensors[SENSOR_CONNECTION_EXTERNAL_SPI].as5047p_sensor.config.rate = rate;
                    break;
                case SENSOR_TYPE_AMT22:
                    current_rate = sensors[SENSOR_CONNECTION_EXTERNAL_SPI].amt22_sensor.config.rate = rate;
                    break;
                default:
                    break;
            }
            sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor.init_func(&(sensors[SENSOR_CONNECTION_EXTERNAL_SPI].sensor));
        }
    }
}

void sensor_set_connection(Sensor** target_sensor_p, Sensor** other_sensor_p, sensor_connection_t new_connection)
{
    if (controller_get_state() == CONTROLLER_STATE_IDLE
        && new_connection != sensor_get_connection(*target_sensor_p) 
        && new_connection >= 0 && new_connection < SENSOR_CONNECTION_MAX)
    {
        frames_reset_calibrated();

        if (sensor_get_connection(*target_sensor_p) != sensor_get_connection(*other_sensor_p))
        {
            observer_reset_state(observer_get_for_sensor(*target_sensor_p));
            (*target_sensor_p)->deinit_func(*target_sensor_p);
        }

        *target_sensor_p = &(sensors[new_connection].sensor);

        if (new_connection != sensor_get_connection(*other_sensor_p))
        {
            observer_reset_state(observer_get_for_sensor(*target_sensor_p));
            sensor_init_with_defaults(*target_sensor_p);
        }
    }
}

bool sensors_calibrate_pole_pair_count_and_transforms(void)
{
    observer_reset_state(&commutation_observer);
    observer_reset_state(&position_observer);

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
        return motor_find_pole_pairs(SENSOR_COMMON_RES_TICKS, commutation_frame_start, commutation_frame_end, motor_frame_end);
    }

    const float motor_frame_end_ticks = SENSOR_COMMON_RES_TICKS * (motor_frame_end / (TWOPI * motor_get_pole_pairs()));

    // Move back to start epos
    for (uint32_t i = 0; i < CAL_DIR_LEN; i++)
    {
        set_epos_and_wait(motor_frame_end * (1.0f - ((float)i / CAL_DIR_LEN)), I_setpoint);
    }
    gate_driver_set_duty_cycle(&three_phase_zero);

    // Derive transforms
    if (sensor_get_type(commutation_sensor_p) != SENSOR_TYPE_HALL)
    {
        frame_set_commutation_sensor_to_motor(derive_dir_transform_2p(commutation_frame_start, 0, commutation_frame_end, motor_frame_end_ticks));
    }
    else // commutation sensor is Hall
    {
        const FrameTransform t = {.offset = 0.0f, .multiplier = 1.0f};
        frame_set_commutation_sensor_to_motor(t);
    }

    if (commutation_sensor_p == position_sensor_p)
    {
        frame_set_position_sensor_to_motor(*(frame_commutation_sensor_to_motor_p()));
    }
    else if (sensor_get_type(position_sensor_p) != SENSOR_TYPE_HALL)
    {
        frame_set_position_sensor_to_motor(derive_transform_2p(position_frame_start, 0, position_frame_end, motor_frame_end_ticks));
    }
    else // position sensor is Hall
    {
        const FrameTransform t = {.offset = 0.0f, .multiplier = 1.0f};
        frame_set_position_sensor_to_motor(t);
    }

    return true;
}