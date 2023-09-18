
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

#include <src/adc/adc.h>
#include <src/observer/observer.h>
#include <src/gatedriver/gatedriver.h>
#include <src/scheduler/scheduler.h>
#include <src/utils/utils.h>
#include <src/encoder/ma7xx.h>
#include <src/encoder/hall.h>
#include <src/controller/controller.h>
#include <src/can/can_endpoints.h>
#include <src/system/system.h>
#include <src/motor/calibration.h>

static inline void set_epos_and_wait(float angle, float I_setpoint);
static inline void wait_a_while(void);

bool CalibrateADCOffset(void)
{
    // We only need to wait here, the ADC loop will
    // perform the offset calibration automatically
    wait_a_while();
    return true;
}

bool CalibrateResistance(void)
{
    if (!motor_get_is_gimbal())
    {
        float I_cal = motor_get_I_cal();
        float V_setpoint = 0.0f;
        FloatTriplet I_phase_meas = {0.0f};
        FloatTriplet modulation_values = {0.0f};
        for (uint32_t i = 0; i < CAL_R_LEN; i++)
        {
            ADC_GetPhaseCurrents(&I_phase_meas);
            V_setpoint += CAL_V_GAIN * (I_cal - I_phase_meas.A);
            const float pwm_setpoint = V_setpoint / system_get_Vbus();
            SVM(pwm_setpoint, 0.0f, &modulation_values.A, &modulation_values.B, &modulation_values.C);
            gate_driver_set_duty_cycle(&modulation_values);
            WaitForControlLoopInterrupt();
        }
        const float R = our_fabsf(V_setpoint / I_cal);
        gate_driver_set_duty_cycle(&three_phase_zero);
        if ((R <= MIN_PHASE_RESISTANCE) || (R >= MAX_PHASE_RESISTANCE))
        {
            uint8_t *error_ptr = motor_get_error_ptr();
            *error_ptr |= MOTOR_ERRORS_PHASE_RESISTANCE_OUT_OF_RANGE;
            return false;
        }
        else
        {
            motor_set_phase_resistance(R);
        }
    }
    return true;
}

bool CalibrateInductance(void)
{
    if (!motor_get_is_gimbal())
    {
        float V_setpoint = 0.0f;
        float I_low = 0.0f;
        float I_high = 0.0f;
        FloatTriplet I_phase_meas = {0.0f};
        FloatTriplet modulation_values = {0.0f};

        for (uint32_t i = 0; i < CAL_L_LEN; i++)
        {
            ADC_GetPhaseCurrents(&I_phase_meas);
            if ((i & 0x2u) == 0x2u)
            {
                I_high += I_phase_meas.A;
                V_setpoint = -CAL_V_INDUCTANCE;
            }
            else
            {
                I_low += I_phase_meas.A;
                V_setpoint = CAL_V_INDUCTANCE;
            }
            const float pwm_setpoint = V_setpoint / system_get_Vbus();
            SVM(pwm_setpoint, 0.0f, &modulation_values.A, &modulation_values.B, &modulation_values.C);
            gate_driver_set_duty_cycle(&modulation_values);
            WaitForControlLoopInterrupt();
        }
        const float num_cycles = CAL_L_LEN / 2;
        const float dI_by_dt = (I_high - I_low) / (PWM_PERIOD_S * num_cycles);
        const float L = CAL_V_INDUCTANCE / dI_by_dt;
        gate_driver_set_duty_cycle(&three_phase_zero);
        if ((L <= MIN_PHASE_INDUCTANCE) || (L >= MAX_PHASE_INDUCTANCE))
        {
            uint8_t *error_ptr = motor_get_error_ptr();
            *error_ptr |= MOTOR_ERRORS_PHASE_INDUCTANCE_OUT_OF_RANGE;
            return false;
        }
        else
        {
            motor_set_phase_inductance(L);
            controller_update_I_gains();
        }
    }
    return true;
}

bool CalibrateDirectionAndPolePairs(void)
{
    // Note that, in order to generate the error table,
    // we read the unwrapped positions given by the
    // observer, but we generate and use the error table
    // before the observer, at the encoder read.
    const float epos_target = CAL_PHASE_TURNS * TWOPI;
    const float I_setpoint = motor_get_I_cal();
    bool success = true;
    // Stay a bit at starting epos
    for (uint32_t i = 0; i < CAL_STAY_LEN; i++)
    {
        set_epos_and_wait(0, I_setpoint);
    }
    const float epos_start = observer_get_pos_estimate();
    float epos_end = 0;
    // Move to target epos
    for (uint32_t i = 0; i < CAL_DIR_LEN; i++)
    {
        set_epos_and_wait(epos_target * ((float)i / CAL_DIR_LEN), I_setpoint);
    }
    // Stay a bit at target epos
    for (uint32_t i = 0; i < CAL_STAY_LEN; i++)
    {
        set_epos_and_wait(epos_target, I_setpoint);
    }
    // Find pole pairs
    if (!motor_find_pole_pairs(ENCODER_TICKS, epos_start, observer_get_pos_estimate(), epos_target))
    {
        uint8_t *error_ptr = motor_get_error_ptr();
        *error_ptr |= MOTOR_ERRORS_INVALID_POLE_PAIRS;
        return false;
    }
    else
    {
        epos_end = observer_get_pos_estimate();
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

bool calibrate_hall_sequence(void)
{
    hall_clear_sector_map();
    uint8_t *sector_map = hall_get_sector_map_ptr();
    const float I_setpoint = motor_get_I_cal();
    bool success = true;

    // Stay a bit at starting epos
	for (uint32_t i=0; i<CAL_STAY_LEN; i++)
	{
		set_epos_and_wait(0, I_setpoint);
	}
    const uint8_t init_sector = hall_get_sector();
    uint8_t current_sector = init_sector;
    uint8_t last_sector = init_sector;
    uint8_t sector_pos = 0;
    const float increment = TWOPI / CAL_DIR_LEN;
    float angle = 0;

    sector_map[current_sector] = sector_pos;

    // Move to the 2nd sector
    while ((current_sector == init_sector) && (angle < TWOPI / HALL_SECTORS))
    {
        set_epos_and_wait(angle, I_setpoint);
        angle += increment;
        current_sector = hall_get_sector();
    }

    // Save the rest of the sectors
    while ((current_sector != init_sector) && (angle < TWOPI))
    {
        if (current_sector != last_sector)
        {
            last_sector = current_sector;
            sector_pos++;
            if (sector_pos >= HALL_SECTORS)
            {
                success = false;
                break;
            }
            sector_map[current_sector] = sector_pos;
        }
        set_epos_and_wait(angle, I_setpoint);
        angle += increment;
        current_sector = hall_get_sector();
    }

    gate_driver_set_duty_cycle(&three_phase_zero);

    // Check that the number of sectors discovered is the same as expected
    if (sector_pos != HALL_SECTORS - 1)
    {
        success = false;
    }

    if (success)
    {
        hall_set_sector_map_calibrated();
    }
    else
    {
        uint8_t *error_ptr = hall_get_error_ptr();
        *error_ptr |= ENCODER_ERRORS_CALIBRATION_FAILED;
    }
    return success;
}

bool calibrate_offset_and_rectification(void)
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
    int16_t *lut = ma7xx_get_rec_table_ptr();
    set_epos_and_wait(e_pos_ref, I_setpoint);
    wait_a_while();
    const uint16_t offset_idx = ma7xx_get_angle_raw() >> (ENCODER_BITS - ECN_BITS);

    for (uint32_t i = 0; i < n; i++)
    {
        for (uint8_t j = 0; j < nconv; j++)
        {
            e_pos_ref += delta;
            set_epos_and_wait(e_pos_ref, I_setpoint);
        }
        WaitForControlLoopInterrupt();
        const float pos_meas = observer_get_pos_estimate();
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
        const float pos_meas = observer_get_pos_estimate();
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
    wait_a_while();
    ma7xx_set_rec_calibrated();
    return true;
}

void reset_calibration(void)
{
    encoder_reset();
    observer_reset();
    motor_reset_calibration();
    wait_a_while();
}

static inline void set_epos_and_wait(float angle, float I_setpoint)
{
    FloatTriplet modulation_values = {0.0f};
    float pwm_setpoint = (I_setpoint * motor_get_phase_resistance()) / system_get_Vbus();
    our_clampc(&pwm_setpoint, -PWM_LIMIT, PWM_LIMIT);
    SVM(pwm_setpoint * fast_cos(angle), pwm_setpoint * fast_sin(angle),
        &modulation_values.A, &modulation_values.B, &modulation_values.C);
    gate_driver_set_duty_cycle(&modulation_values);
    WaitForControlLoopInterrupt();
}

static inline void wait_a_while(void)
{
    // Wait a while for the observer to settle
    // TODO: This is a bit of a hack, can be improved!
    for (int i = 0; i < 5000; i++)
    {
        WaitForControlLoopInterrupt();
    }
}
