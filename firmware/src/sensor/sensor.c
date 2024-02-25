
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

#include <src/controller/controller.h>
#include <src/observer/observer.h>
#include <src/sensor/sensor.h>

void sensor_reset(Sensor *s)
{
    (void)memset(s->config.rec_table, 0, sizeof(s->config.rec_table));
	s->config.rec_calibrated = false;
}

bool sensor_calibrate_eccentricity_compensation(Sensor *s, Observer *o)
{
    // Size below is an arbitrary large number ie > ECN_SIZE * npp
    float error_ticks[ECN_SIZE * 24];
    const int16_t npp = motor_get_pole_pairs();
    const int16_t n = ECN_SIZE * npp;
    const int16_t nconv = 100;
    const float delta = 2 * PI * npp / (n * nconv);
    const float e_pos_to_ticks = ((float)SENSOR_COMMON_RES_TICKS) / (2 * PI * npp);
    float e_pos_ref = 0;
    const float I_setpoint = motor_get_I_cal();
    int32_t *lut = s->config.rec_table;
    set_epos_and_wait(e_pos_ref, I_setpoint);
    wait_pwm_cycles(5000);
    const int32_t offset_idx = (s->get_raw_angle_func(s)) >> (SENSOR_COMMON_RES_BITS - ECN_BITS);

    for (uint32_t i = 0; i < n; i++)
    {
        for (int16_t j = 0; j < nconv; j++)
        {
            e_pos_ref += delta;
            set_epos_and_wait(e_pos_ref, I_setpoint);
        }
        wait_for_control_loop_interrupt();
        const float pos_meas = observer_get_pos_estimate(o);
        error_ticks[i] = e_pos_ref * e_pos_to_ticks - pos_meas;
    }
    for (uint32_t i = 0; i < n; i++)
    {
        for (int16_t j = 0; j < nconv; j++)
        {
            e_pos_ref -= delta;
            set_epos_and_wait(e_pos_ref, I_setpoint);
        }
        wait_for_control_loop_interrupt();
        const float pos_meas = observer_get_pos_estimate(o);
        error_ticks[n - i - 1] += e_pos_ref * e_pos_to_ticks - pos_meas;
    }
    gate_driver_set_duty_cycle(&three_phase_zero);
    gate_driver_disable();

    // FIR filtering and map measurements to lut
    for (int16_t i=0; i<ECN_SIZE; i++)
    {
        float acc = 0;
        for (int16_t j = 0; j < ECN_SIZE; j++)
        {
            int32_t read_idx = -ECN_SIZE / 2 + j + i * npp;
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
        int32_t write_idx = i + offset_idx;
        if (write_idx > (ECN_SIZE - 1))
        {
            write_idx -= ECN_SIZE;
        }
        lut[write_idx] = (int32_t)acc;
    }
    wait_pwm_cycles(5000);
    s->config.rec_calibrated = true;
    return true;
}
