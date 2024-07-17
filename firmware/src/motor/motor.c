
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
#include <src/controller/controller.h>
#include <src/observer/observer.h>
#include <src/scheduler/scheduler.h>
#include <src/utils/utils.h>
#include <src/can/can_endpoints.h>
#include <src/motor/motor.h>

#if defined BOARD_REV_R32 || defined BOARD_REV_R33 || defined BOARD_REV_R5

static MotorConfig config = {
	.pole_pairs = 7u,
	.phase_resistance = MIN_PHASE_RESISTANCE,
	.phase_inductance = MIN_PHASE_INDUCTANCE,

	.I_cal = 6.0f,

	.resistance_calibrated = false,
	.inductance_calibrated = false,
	.poles_calibrated = false,

	.is_gimbal = false};

#elif defined BOARD_REV_M5

static MotorConfig config = {
	.pole_pairs = 7u,
	.phase_resistance = MIN_PHASE_RESISTANCE,
	.phase_inductance = MIN_PHASE_INDUCTANCE,

	.I_cal = 1.2f,

	.resistance_calibrated = false,
	.inductance_calibrated = false,
	.poles_calibrated = false,

	.is_gimbal = false};

#endif

static MotorState state = {0};

void motor_reset_calibration()
{
	config.pole_pairs = 7;
	// Important! We only reset resistance and 
	// inductance measurements if the motor is
	// not a gimbal, otherwise they will not
	// be recalibrated and will stay at default
	// values!
	if (!motor_get_is_gimbal())
	{
		config.phase_resistance = 0.1f;
		config.phase_inductance = 1e-5f;
		config.resistance_calibrated = false;
		config.inductance_calibrated = false;
	}
	config.poles_calibrated = false;
}

bool motor_calibrate_resistance(void)
{
    if (!motor_get_is_gimbal())
    {
        float I_cal = motor_get_I_cal();
        float V_setpoint = 0.0f;
        FloatTriplet I_phase_meas = {0.0f};
        FloatTriplet modulation_values = {0.0f};
        
        for (uint32_t i = 0; i < CAL_R_LEN; i++)
        {
            ADC_get_phase_currents(&I_phase_meas);
            
            if (powf(V_setpoint, 2) / MAX_PHASE_RESISTANCE > MAX_CALIBRATION_POWER)
            {
                uint8_t *error_ptr = motor_get_error_ptr();
                *error_ptr |= MOTOR_ERRORS_ABNORMAL_CALIBRATION_VOLTAGE;
                gate_driver_set_duty_cycle(&three_phase_zero);
                return false;
            }
            
            V_setpoint += CAL_V_GAIN * (I_cal - I_phase_meas.A);
            const float pwm_setpoint = V_setpoint / system_get_Vbus();
            SVM(pwm_setpoint, 0.0f, &modulation_values.A, &modulation_values.B, &modulation_values.C);
            gate_driver_set_duty_cycle(&modulation_values);
            wait_for_control_loop_interrupt();
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


bool motor_calibrate_inductance(void)
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
            ADC_get_phase_currents(&I_phase_meas);
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
            wait_for_control_loop_interrupt();
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

TM_RAMFUNC uint8_t motor_find_pole_pairs(uint32_t ticks, float mpos_start, float mpos_end, float epos_rad)
{
	const float mpos_diff = our_fabsf(mpos_end - mpos_start);
	float mpos_diff_rad = TWOPI * mpos_diff / ticks;
	const float pairs_f = epos_rad / mpos_diff_rad;
	const uint8_t pairs_i = (uint8_t)our_floorf(pairs_f + 0.5f);
	const float residual = our_fabsf(pairs_f - (float)pairs_i);

	if (pairs_i > MOTOR_MAX_POLE_PAIRS)
	{
		uint8_t *error_ptr = motor_get_error_ptr();
		*error_ptr |= MOTOR_ERRORS_POLE_PAIRS_OUT_OF_RANGE;
		return false;
	}
	else if (residual > 0.30f)
	{
		uint8_t *error_ptr = motor_get_error_ptr();
		*error_ptr |= MOTOR_ERRORS_POLE_PAIRS_CALCULATION_DID_NOT_CONVERGE;
		return false;
	}
	motor_set_pole_pairs(pairs_i);
	return true;
}

TM_RAMFUNC uint8_t motor_get_pole_pairs(void)
{
	return config.pole_pairs;
}

TM_RAMFUNC void motor_set_pole_pairs(uint8_t pairs)
{
	if (pairs >= 1u && pairs <= MOTOR_MAX_POLE_PAIRS)
	{
		config.pole_pairs = pairs;
		config.poles_calibrated = true;
	}
}

TM_RAMFUNC float motor_get_phase_resistance(void)
{
	return config.phase_resistance;
}

TM_RAMFUNC void motor_set_phase_resistance(float R)
{
	if ((R > MIN_PHASE_RESISTANCE) && ((R < MAX_PHASE_RESISTANCE) || motor_get_is_gimbal()))
	{
		config.phase_resistance = R;
		config.resistance_calibrated = true;
		if (config.resistance_calibrated && config.inductance_calibrated)
		{
			controller_update_I_gains();
		}
	}
}

TM_RAMFUNC float motor_get_phase_inductance(void)
{
	return config.phase_inductance;
}

TM_RAMFUNC void motor_set_phase_inductance(float L)
{
	if ((L > MIN_PHASE_INDUCTANCE) && ((L < MAX_PHASE_INDUCTANCE) || motor_get_is_gimbal()))
	{
		config.phase_inductance = L;
		config.inductance_calibrated = true;
		if (config.resistance_calibrated && config.inductance_calibrated)
		{
			controller_update_I_gains();
		}
	}
}

TM_RAMFUNC void motor_set_phase_R_and_L(float R, float L)
{
	if ((R > MIN_PHASE_RESISTANCE) && ((R < MAX_PHASE_RESISTANCE) || motor_get_is_gimbal()))
	{
		config.phase_resistance = R;
		config.resistance_calibrated = true;
	}
	if ((L > MIN_PHASE_INDUCTANCE) && ((L < MAX_PHASE_INDUCTANCE) || motor_get_is_gimbal()))
	{
		config.phase_inductance = L;
		config.inductance_calibrated = true;
	}
	if (config.resistance_calibrated && config.inductance_calibrated)
	{
		controller_update_I_gains();
	}
}

TM_RAMFUNC float motor_get_I_cal(void)
{
	return config.I_cal;
}

TM_RAMFUNC void motor_set_I_cal(float I)
{
	if (I > 0)
	{
		config.I_cal = I;
	}
}

TM_RAMFUNC bool motor_get_calibrated(void)
{
	return config.resistance_calibrated && config.inductance_calibrated && config.poles_calibrated;
}

TM_RAMFUNC bool motor_get_is_gimbal(void)
{
	return config.is_gimbal;
}

TM_RAMFUNC void motor_set_is_gimbal(bool gimbal)
{
	config.is_gimbal = gimbal;
}

TM_RAMFUNC uint8_t motor_get_errors(void)
{
	return state.errors;
}

TM_RAMFUNC uint8_t *motor_get_error_ptr(void)
{
	return &(state.errors);
}

MotorConfig *motor_get_config(void)
{
	return &config;
}

void motor_restore_config(MotorConfig *config_)
{
	config = *config_;
}
