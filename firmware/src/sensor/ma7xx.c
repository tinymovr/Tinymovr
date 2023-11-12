
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

#include <string.h>
#include <src/system/system.h>
#include <src/ssp/ssp_func.h>
#include <src/utils/utils.h>
#include <src/can/can_endpoints.h>
#include <src/sensors/ma7xx.h>

bool ma7xx_init_with_defaults(Sensor *s)
{
    MA7xxConfig c = {.id = get_next_sensor_id(), .ssp_port = ONBOARD_SENSOR_SSP_PORT};
    return ma7xx_init_with_config(s, &c);
}

bool ma7xx_init_with_config(Sensor *s, SensorSpecificConfig *c)
{
    s->get_angle_func = ma7xx_get_angle_rectified;
    s->update_func = ma7xx_update;
    s->reset_func = ma7xx_clear_rec_table;
    s->get_error_func = ma7xx_get_errors;
    s->is_calibrated_func = ma7xx_rec_is_calibrated;
    s->calibrate_func = ma7xx_calibrate;
    s->config.type = SENSOR_TYPE_MA7XX;
    s->config.ss_config = *c;
    ssp_init(s->config.ss_config.ma7xx_config.ssp_port, SSP_MS_MASTER, 0, 0);
    delay_us(16000); // ensure 16ms sensor startup time as per the datasheet
    ma7xx_send_angle_cmd(&s);
    ma7xx_update(&s, false);
    return true;
}

void ma7xx_deinit(Sensor *s)
{
    ssp_deinit(c->ssp_port);
}

void ma7xx_clear_rec_table(Sensor *s)
{
    MA7xxConfig *c = s->config.ss_config.ma7xx_config;
    (void)memset(c->rec_table, 0, sizeof(c->rec_table));
	c->rec_calibrated = false;
}

bool ma7xx_rec_is_calibrated(Sensor *s)
{
    return s->config.ss_config.ma7xx_config.rec_calibrated;
}

int16_t *ma7xx_get_rec_table_ptr(Sensor *s)
{
    return s->config.ss_config.ma7xx_config.rec_table;
}

bool ma7xx_calibrate_offset_and_rectification(Sensor *s)
{
    // Size below is an arbitrary large number ie > ECN_SIZE * npp
    float error_ticks[ECN_SIZE * 20];
    const int16_t npp = motor_get_pole_pairs();
    const int16_t n = ECN_SIZE * npp;
    const int16_t nconv = 100;
    const float delta = 2 * PI * npp / (n * nconv);
    const float e_pos_to_ticks = ((float)ENCODER_TICKS) / (2 * PI * npp);
    float e_pos_ref = 0.f;
    const float I_setpoint = motor_get_I_cal();
    int16_t *lut = s->config.ma7xx_config.rec_table;
    set_epos_and_wait(e_pos_ref, I_setpoint);
    wait_pwm_cycles(5000);
    const uint16_t offset_idx = ma7xx_get_angle_raw(s) >> (ENCODER_BITS - ECN_BITS);

    for (uint32_t i = 0; i < n; i++)
    {
        for (uint8_t j = 0; j < nconv; j++)
        {
            e_pos_ref += delta;
            set_epos_and_wait(e_pos_ref, I_setpoint);
        }
        WaitForControlLoopInterrupt();
        const float pos_meas = observer_get_pos_estimate(observers[s->idx]);
        error_ticks[i] = (int16_t)(e_pos_ref * e_pos_to_ticks - pos_meas);
    }
    for (uint32_t i = 0; i < n; i++)
    {
        for (uint8_t j = 0; j < nconv; j++)
        {
            e_pos_ref -= delta;
            set_epos_and_wait(e_pos_ref, I_setpoint);
        }
        WaitForControlLoopInterrupt();
        const float pos_meas = observer_get_pos_estimate(observers[s->idx]);
        error_ticks[n - i - 1] += (int16_t)(e_pos_ref * e_pos_to_ticks - pos_meas);
    }
    gate_driver_set_duty_cycle(&three_phase_zero);
    gate_driver_disable();

    // FIR filtering and map measurements to lut
    for (int16_t i=0; i<ECN_SIZE; i++)
    {
        float acc = 0;
        for (int16_t j = 0; j < ECN_SIZE; j++)
        {
            int16_t read_idx = -ECN_SIZE / 2 + j + i * npp;
            if (read_idx < 0)
            {
                read_idx += n;
            }
            else if (read_idx > n - 1)
            {
                read_idx -= n;
            }
            acc += error_ticks[read_idx];
        }
        acc = acc / ((float)(ECN_SIZE * 2));
        int16_t write_idx = i + offset_idx;
        if (write_idx > (ECN_SIZE - 1))
        {
            write_idx -= ECN_SIZE;
        }
        lut[write_idx] = (int16_t)acc;
    }
    wait_pwm_cycles(5000);
    s->config.ss_config.ma7xx_config.rec_calibrated = true;
    return true;
}

bool ma7xx_calibrate_direction_and_pole_pair_count(Sensor *s)
{
    const float epos_target = CAL_PHASE_TURNS * TWOPI;
    const float I_setpoint = motor_get_I_cal();
    bool success = true;
    set_epos_and_wait(0, I_setpoint);
    wait_pwm_cycles(CAL_STAY_LEN);
    const float epos_start = observer_get_pos_estimate(observers[s->idx]);
    float epos_end = 0;
    // Move to target epos
    for (uint32_t i = 0; i < CAL_DIR_LEN; i++)
    {
        set_epos_and_wait(epos_target * ((float)i / CAL_DIR_LEN), I_setpoint);
    }
    set_epos_and_wait(epos_target, I_setpoint);
    wait_pwm_cycles(CAL_STAY_LEN);
    // Find pole pairs
    if (!motor_find_pole_pairs(ENCODER_TICKS, epos_start, observer_get_pos_estimate(observers[s->idx]), epos_target))
    {
        uint8_t *error_ptr = motor_get_error_ptr();
        *error_ptr |= MOTOR_ERRORS_INVALID_POLE_PAIRS;
        return false;
    }
    else
    {
        epos_end = observer_get_pos_estimate(observers[s->idx]);
    }
    // Move back to start epos
    for (uint32_t i = 0; i < CAL_DIR_LEN; i++)
    {
        set_epos_and_wait(epos_target * (1.0f - ((float)i / CAL_DIR_LEN)), I_setpoint);
    }
    gate_driver_set_duty_cycle(&three_phase_zero);
    if (success && epos_start > epos_end)
    {
        motor_set_phases_swapped(true);
    }
    return success;
}

/**
 * @brief Write to a register of the sensor
 * 
 * @param reg The 5-bit register address
 * @param value The value to write
 * @return uint16_t 
 */
uint16_t ma7xx_write_reg(Sensor *s, uint8_t reg, uint8_t value)
{
    uint16_t cmd = MA_CMD_WRITE | reg << 8 | value;
    uint32_t result = ssp_write_one(s->config.ss_config.ma7xx_config.ssp_struct, cmd);

    // Delay at least 20ms to let the encoder write to memory
    delay_us(20100);
    result |= ssp_write_one(s->config.ss_config.ma7xx_config.ssp_struct, 0);

    // The encoder returns the value written to memory, so check that it is the same as what we wrote
    uint8_t retval =  ssp_read_one(s->config.ss_config.ma7xx_config.ssp_struct) >> 8;
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
uint8_t ma7xx_read_reg(Sensor *s, uint8_t reg)
{
    uint16_t cmd[2] = {MA_CMD_READ  | (reg << 8), 0};
    uint16_t result = ssp_write_multi(s->params.ma7xx_params.ssp_struct, cmd, 2u);
    if (result != 0)
    {
        return false;
    }
    return ssp_read_one(s->params.ma7xx_params.ssp_struct) >> 8;
}