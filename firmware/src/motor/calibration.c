
//  * This file is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * Copyright (c) 2020 Ioannis Chatzikonstantinou.
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
#include <src/watchdog/watchdog.h>
#include <src/utils/utils.h>
#include <src/encoder/encoder.h>
#include <src/controller/controller.h>
#include <src/system/system.h>
#include <src/motor/calibration.h>

static inline void set_epos_and_wait(float angle, float I_setpoint);
static inline float get_I_setpoint(void);
static inline void wait_a_while(void);
static struct FloatTriplet zeroDC = {0.5f, 0.5f, 0.5f};

bool CalibrateResistance(void)
{
    bool success = true;
    if (!motor_is_gimbal())
    {
        float V_setpoint = 0.0f;
        struct FloatTriplet I_phase_meas = {0.0f};
        struct FloatTriplet modulation_values = {0.0f};
        for (uint32_t i=0; i<CAL_R_LEN; i++)
        {
            ADC_GetPhaseCurrents(&I_phase_meas);
            V_setpoint += CAL_V_GAIN * (CAL_I_SETPOINT - I_phase_meas.A);
            const float pwm_setpoint = V_setpoint / ADC_GetVBus();
            SVM(pwm_setpoint, 0.0f, &modulation_values.A, &modulation_values.B, &modulation_values.C);
            GateDriver_SetDutyCycle(&modulation_values);
            Watchdog_Feed();
            WaitForControlLoopInterrupt();
        }
#ifndef DRY_RUN
        const float R = fabsf(V_setpoint / CAL_I_SETPOINT);
        if ((R <= MIN_PHASE_RESISTANCE) || (R >= MAX_PHASE_RESISTANCE))
        {
            add_error_flag(ERROR_PHASE_RESISTANCE_OUT_OF_RANGE);
            success = false;
        }
        else
        {
            Motor_SetPhaseResistance(R);
        }
#else
        Motor_SetPhaseResistance(0.5);
#endif
        GateDriver_SetDutyCycle(&zeroDC);
    }
    return success;
}

bool CalibrateInductance(void)
{
    bool success = true;
    if (!motor_is_gimbal())
    {
        float V_setpoint = 0.0f;
        float I_low = 0.0f;
        float I_high = 0.0f;
        struct FloatTriplet I_phase_meas = {0.0f};
        struct FloatTriplet modulation_values = {0.0f};
        
        for (uint32_t i=0; i<CAL_L_LEN; i++)
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
            const float pwm_setpoint = V_setpoint / ADC_GetVBus();
            SVM(pwm_setpoint, 0.0f, &modulation_values.A, &modulation_values.B, &modulation_values.C);
            GateDriver_SetDutyCycle(&modulation_values);
            Watchdog_Feed();
            WaitForControlLoopInterrupt();
        }
#ifndef DRY_RUN
        const float num_cycles = CAL_L_LEN / 2;
        const float dI_by_dt = (I_high - I_low) / (PWM_PERIOD_S * num_cycles);
        const float L = CAL_V_INDUCTANCE / dI_by_dt;
        if ((L <= MIN_PHASE_INDUCTANCE) || (L >= MAX_PHASE_INDUCTANCE))
        {
            add_error_flag(ERROR_PHASE_INDUCTANCE_OUT_OF_RANGE);
            success = false;
        }
        else
        {
            Motor_SetPhaseInductance(L);
            Controller_UpdateCurrentGains();
        }
#else
        Motor_SetPhaseInductance(0.005);
#endif
        GateDriver_SetDutyCycle(&zeroDC);
    }
    return success;
}

bool CalibrateDirectionAndPolePairs(void)
{
    // Note that, in order to generate the error table,
    // we read the unwrapped positions given by the
    // observer, but we generate and use the error table
    // before the observer, at the encoder read.
    const float epos_target = CAL_PHASE_TURNS * twopi;
    const float I_setpoint = get_I_setpoint();
    bool success = true;
    // Stay a bit at starting epos
	for (uint32_t i=0; i<CAL_STAY_LEN; i++)
	{
		set_epos_and_wait(0, I_setpoint);
	}
    const float epos_start = Observer_GetPosEstimate();
    float epos_end = 0;
    // Move to target epos
    for (uint32_t i=0; i<CAL_DIR_LEN; i++)
    {
        set_epos_and_wait(epos_target * ((float)i/CAL_DIR_LEN), I_setpoint);
    }
    // Stay a bit at target epos
	for (uint32_t i=0; i<CAL_STAY_LEN; i++)
	{
		set_epos_and_wait(epos_target, I_setpoint);
	}
    // Try to calibrate
    if (!Motor_FindPolePairs(ENCODER_TICKS, epos_start, Observer_GetPosEstimate(), epos_target))
    {
        add_error_flag(ERROR_INVALID_POLE_PAIRS);
        success = false;
    }
    else
    {
    	epos_end = Observer_GetPosEstimate();
    }
    // Go back to start epos
    for (uint32_t i=0; i<CAL_DIR_LEN; i++)
    {
        set_epos_and_wait(epos_target * (1.0f - ((float)i/CAL_DIR_LEN)), I_setpoint);
    }
    GateDriver_SetDutyCycle(&zeroDC);
    if (success && epos_start > epos_end)
	{
    	motor_set_phases_swapped(true);
	}
    return success;
}

bool CalibrateOffsetAndEccentricity(void)
{
    // Size below is an arbitrary large number ie > ECN_SIZE * npp
    int16_t error_ticks[ECN_SIZE * 24];
    const int16_t npp = Motor_GetPolePairs();
    const int16_t n = ECN_SIZE * npp;
    const int16_t nconv = 50;
    const float delta = 2 * PI * npp / (n * nconv);
    const float e_pos_to_ticks = ((float)ENCODER_TICKS)/(2 * PI * npp);
    float e_pos_ref = 0.f;
    const float I_setpoint = get_I_setpoint();
    Observer_ClearEccentricityTable();
    int16_t *lut = Observer_GetEccentricityTablePointer();
    wait_a_while();
    int16_t offset_raw = MA_GetAngle();
    // Perform measuerments, store only mean of F + B error
    for (uint32_t i=0; i<n; i++)
    {
        for (uint8_t j=0; j<nconv; j++)
        {
            e_pos_ref += delta;
            set_epos_and_wait(e_pos_ref, I_setpoint);
        }
        WaitForControlLoopInterrupt();
        const float pos_meas = Observer_GetPosEstimate();
        error_ticks[i] = (int16_t)(e_pos_ref * e_pos_to_ticks - pos_meas);
    }
    offset_raw = (offset_raw + MA_GetAngle()) / 2;
    for (uint32_t i=0; i<n; i++)
    {
        for (uint8_t j=0; j<nconv; j++)
        {
            e_pos_ref -= delta;
            set_epos_and_wait(e_pos_ref, I_setpoint);
        }
        WaitForControlLoopInterrupt();
        const float pos_meas = Observer_GetPosEstimate();
        error_ticks[n-i-1] = (int16_t)(0.5f * ((float)error_ticks[n-i-1] + e_pos_ref * e_pos_to_ticks - pos_meas));
    }
    GateDriver_SetDutyCycle(&zeroDC);

    // FIR and map measurements to lut
    for (int16_t i=0; i<ECN_SIZE; i++)
    {
        int32_t acc = 0;
        for (int16_t j=0; j<ECN_SIZE; j++)
        {
            int16_t read_idx = -ECN_SIZE/2 + j + i*npp;
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
        acc /= ECN_SIZE;
        int16_t write_idx = i + (offset_raw>>ECN_BITS);
        if (write_idx > (ECN_SIZE - 1))
        {
            write_idx -= ECN_SIZE;
        }
        lut[write_idx] = (int16_t)acc;
    }
    wait_a_while();
    Observer_SetEccentricityCalibrated();
    return true;
}

static inline void set_epos_and_wait(float angle, float I_setpoint)
{
	struct FloatTriplet modulation_values = {0.0f};
	float pwm_setpoint = (I_setpoint * Motor_GetPhaseResistance()) / ADC_GetVBus();
	our_clamp(&pwm_setpoint, -PWM_LIMIT, PWM_LIMIT);
	SVM(pwm_setpoint * fast_cos(angle), pwm_setpoint * fast_sin(angle),
		&modulation_values.A, &modulation_values.B, &modulation_values.C);
	GateDriver_SetDutyCycle(&modulation_values);
	Watchdog_Feed();
	WaitForControlLoopInterrupt();
}

static inline float get_I_setpoint(void)
{
	float I_setpoint = CAL_I_SETPOINT;
	if (motor_is_gimbal() == true)
	{
		I_setpoint = CAL_I_SETPOINT_GIMBAL;
	}
	return I_setpoint;
}

static inline void wait_a_while(void)
{
	// Wait a while for the observer to settle
	// TODO: This is a bit of a hack, can be improved!
	for (int i=0; i<5000; i++)
	{
		Watchdog_Feed();
		WaitForControlLoopInterrupt();
	}
}
