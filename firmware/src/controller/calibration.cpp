/*
 * calibrateion.cpp
 *
 *  Created on: 27 ??? 2020
 *      Author: conta
 */

#include <src/common.hpp>
#include <src/adc/adc.hpp>
#include <src/controller/controller.hpp>
#include <src/encoder/encoder.hpp>
#include <src/gatedriver/gatedriver.hpp>
#include <src/motor/motor.hpp>
#include <src/observer/observer.hpp>
#include <src/watchdog/watchdog.hpp>
#include <src/system.hpp>
#include <src/utils/utils.hpp>

struct CalibrateResistance: public ControlAction
{
public:
	static constexpr float V_gain = 0.0005f;
	static constexpr float I_cal_R_setpoint = 5.0f;
	float V_setpoint = 0;
	virtual void operator () ()
	{
		for (int i=0; i< CAL_R_LEN; i++)
		{
			System::getInstance().watchdog.Feed();
			struct FloatTriplet I_phase_meas = System::getInstance().adc.GetPhaseCurrents();
			V_setpoint += V_gain * (I_cal_R_setpoint - I_phase_meas.A);
			const float pwm_setpoint = V_setpoint / System::getInstance().adc.GetVBus();
			struct FloatTriplet modulation_values;
			SVM(pwm_setpoint, 0.0f, &modulation_values.A, &modulation_values.B, &modulation_values.C);
			System::getInstance().driver.SetDutyCycle(&modulation_values);
			System::getInstance().WaitForControlLoopInterrupt();
		}
#ifndef DRY_RUN
        const float R = fabsf(V_setpoint / I_cal_R_setpoint);
        if ((R <= MIN_PHASE_RESISTANCE) || (R >= MAX_PHASE_RESISTANCE))
        {
            throw ERROR_PHASE_RESISTANCE_OUT_OF_RANGE;
        }
        System::getInstance().motor.SetPhaseResistance(R);
#else
        Motor_SetPhaseResistance(0.5);
#endif
	}
};


struct CalibrateInductance: public ControlAction
{
public:
	static constexpr float V_inductance = 2.0f;
	float V_setpoint;
	float I_low;
	float I_high;
	virtual void operator () ()
	{
		for (int i=0; i< CAL_L_LEN; i++)
		{
			System::getInstance().watchdog.Feed();
			struct FloatTriplet I_phase_meas = System::getInstance().adc.GetPhaseCurrents();
			if ((i & 0x2u) == 0x2u)
			{
				I_high += I_phase_meas.A;
				V_setpoint = -V_inductance;
			}
			else
			{
				I_low += I_phase_meas.A;
				V_setpoint = V_inductance;
			}
			const float pwm_setpoint = V_setpoint / System::getInstance().adc.GetVBus();
			struct FloatTriplet modulation_values;
			SVM(pwm_setpoint, 0.0f, &modulation_values.A, &modulation_values.B, &modulation_values.C);
			System::getInstance().driver.SetDutyCycle(&modulation_values);
			System::getInstance().WaitForControlLoopInterrupt();
		}
#ifndef DRY_RUN
        const float num_cycles = CAL_L_LEN / 2;
        const float dI_by_dt = (I_high - I_low) / (PWM_TIMER_PERIOD * num_cycles);
        const float L = (V_inductance) / dI_by_dt;
        if ((L <= MIN_PHASE_INDUCTANCE) || (L >= MAX_PHASE_INDUCTANCE))
        {
            throw ERROR_PHASE_INDUCTANCE_OUT_OF_RANGE;
        }
        else
        {
            System::getInstance().motor.SetPhaseInductance(L);
            System::getInstance().controller.UpdateCurrentGains();
        }
#else
        System::getInstance().motor.SetPhaseInductance(0.005);
#endif
	}
};


struct CalibrateOffsetDirectionAndPoles: public ControlAction
{
public:
	static constexpr float I_setpoint = 8.0f;
	float dir_initial_pos;
	virtual void operator () ()
	{
		for (int i=0; i< CAL_OFFSET_LEN; i++)
		{
			System::getInstance().watchdog.Feed();
			float pwm_setpoint = (I_setpoint * System::getInstance().motor.GetPhaseResistance()) / System::getInstance().adc.GetVBus();
			clamp(&pwm_setpoint, -PWM_LIMIT, PWM_LIMIT);
			struct FloatTriplet modulation_values;
			SVM(pwm_setpoint, 0.0f, &modulation_values.A, &modulation_values.B, &modulation_values.C);
			System::getInstance().driver.SetDutyCycle(&modulation_values);
			System::getInstance().WaitForControlLoopInterrupt();
		}
		System::getInstance().observer.Reset();
		System::getInstance().observer.CalibrateOffset();
		dir_initial_pos = System::getInstance().observer.GetPosEstimate();
		for (int i=0; i< CAL_DIR_LEN; i++)
		{
			// Ensure rotor stays at 2*Pi eangle a bit
			static const float end_angle = CAL_PHASE_TURNS * twopi;
			float cur_angle = 1.2f * end_angle * (i/CAL_DIR_LEN);
			clamp(&cur_angle, 0.0f, end_angle);
			float pwm_setpoint = (I_setpoint * System::getInstance().motor.GetPhaseResistance()) / System::getInstance().adc.GetVBus();
			clamp(&pwm_setpoint, -PWM_LIMIT, PWM_LIMIT);
			struct FloatTriplet modulation_values;
			SVM(pwm_setpoint * fast_cos(cur_angle), pwm_setpoint * fast_sin(cur_angle),
					&modulation_values.A, &modulation_values.B, &modulation_values.C);
		}
		// Set neutral pwm immediately
		static const float end_angle = CAL_PHASE_TURNS * twopi;
		if (!System::getInstance().motor.FindPolePairs(ENCODER_CPR, dir_initial_pos, System::getInstance().observer.GetPosEstimate(), end_angle))
		{
			throw ERROR_INVALID_POLE_PAIRS;
		}
		// Direction needs to be calibrated LAST
		System::getInstance().observer.CalibrateDirection(dir_initial_pos);
	}
};

