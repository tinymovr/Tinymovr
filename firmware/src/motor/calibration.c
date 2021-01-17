/*
 * calibration.c
 *
 *  Created on: 28 ??? 2020
 *      Author: conta
 */

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

bool CalibrateOffsetDirectionAndPolePairs(void)
{
	const float end_angle = CAL_PHASE_TURNS * twopi;
	float dir_initial_pos = 0.0f;
	struct FloatTriplet modulation_values = {0.0f};
	float I_setpoint = CAL_I_SETPOINT;
	if (motor_is_gimbal() == true)
	{
		I_setpoint = CAL_I_SETPOINT_GIMBAL;
	}
	bool success = true;
	for (uint32_t i=0; i<CAL_OFFSET_LEN; i++)
	{
		float pwm_setpoint = (I_setpoint * Motor_GetPhaseResistance()) / ADC_GetVBus();
		our_clamp(&pwm_setpoint, -PWM_LIMIT, PWM_LIMIT);
		SVM(pwm_setpoint, 0.0f, &modulation_values.A, &modulation_values.B, &modulation_values.C);
		GateDriver_SetDutyCycle(&modulation_values);
		Watchdog_Feed();
		WaitForControlLoopInterrupt();
	}
	Observer_Reset();
	Observer_CalibrateOffset();
	dir_initial_pos = Observer_GetPosEstimate();
	for (uint32_t i=0; i<CAL_DIR_LEN; i++)
	{
		// Ensure rotor stays at 2*Pi eangle a bit
		float factor = (float)i;
		float cur_angle = 1.2f * end_angle * (factor/CAL_DIR_LEN);
		our_clamp(&cur_angle, 0.0f, end_angle);
		float pwm_setpoint = (I_setpoint * Motor_GetPhaseResistance()) / ADC_GetVBus();
		our_clamp(&pwm_setpoint, -PWM_LIMIT, PWM_LIMIT);
		SVM(pwm_setpoint * fast_cos(cur_angle), pwm_setpoint * fast_sin(cur_angle),
			&modulation_values.A, &modulation_values.B, &modulation_values.C);
		GateDriver_SetDutyCycle(&modulation_values);
		Watchdog_Feed();
		WaitForControlLoopInterrupt();
	}
	if (!Motor_FindPolePairs(ENCODER_TICKS, dir_initial_pos, Observer_GetPosEstimate(), end_angle))
	{
		add_error_flag(ERROR_INVALID_POLE_PAIRS);
		success = false;
	}
	else
	{
		// Direction needs to be calibrated LAST
		Observer_CalibrateDirection(dir_initial_pos);
	}
	GateDriver_SetDutyCycle(&zeroDC);
	return success;
}
