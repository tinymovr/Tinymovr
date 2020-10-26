/*
 * calibration.cpp
 *
 *  Created on: 26 ??? 2020
 *      Author: conta
 */


void CalibrateStep(void)
{
    Watchdog_Feed();

    // Resistance
    if (calState.current_cal_index < CAL_R_END_INDEX)
    {
        calState.V_setpoint += config.V_calib_gain *
                (config.I_cal_R_setpoint - state.I_phase_meas.A);
        const float pwm_setpoint = calState.V_setpoint / ADC_GetVBus();
        SVM(pwm_setpoint, 0.0f, &state.modulation_values.A,
                    &state.modulation_values.B, &state.modulation_values.C);
    }
    else if (calState.current_cal_index == CAL_R_END_INDEX)
    {
#ifndef DRY_RUN
        const float R = fabsf(calState.V_setpoint / config.I_cal_R_setpoint);
        if ((R <= MIN_PHASE_RESISTANCE) || (R >= MAX_PHASE_RESISTANCE))
        {
            state.error = ERROR_PHASE_RESISTANCE_OUT_OF_RANGE;
            calState.current_cal_index = 0;
            calState.I_high = 0.0f;
            calState.I_low = 0.0f;
            calState.V_setpoint = 0.0f;
            Controller_SetState(STATE_IDLE);
        }
        else
        {
            Motor_SetPhaseResistance(R);
        }
#else
        Motor_SetPhaseResistance(0.5);
#endif
        calState.V_setpoint = 0.0f;
    }

    // Inductance
    else if (calState.current_cal_index < CAL_L_END_INDEX)
    {
        if ((calState.current_cal_index & 0x2u) == 0x2u)
        {
            calState.I_high += state.I_phase_meas.A;
            calState.V_setpoint = -config.V_calib_inductance;
        }
        else
        {
            calState.I_low += state.I_phase_meas.A;
            calState.V_setpoint = config.V_calib_inductance;
        }
        const float pwm_setpoint = calState.V_setpoint / ADC_GetVBus();
        SVM(pwm_setpoint, 0.0f, &state.modulation_values.A,
                    &state.modulation_values.B, &state.modulation_values.C);
    }
    else if (calState.current_cal_index == CAL_L_END_INDEX)
    {
#ifndef DRY_RUN
        const float num_cycles = (CAL_L_END_INDEX - CAL_R_END_INDEX) / 2;
        const float dI_by_dt = (calState.I_high - calState.I_low) / (PWM_TIMER_PERIOD * num_cycles);
        const float L = (config.V_calib_inductance) / dI_by_dt;
        if ((L <= MIN_PHASE_INDUCTANCE) || (L >= MAX_PHASE_INDUCTANCE))
        {
            state.error = ERROR_PHASE_INDUCTANCE_OUT_OF_RANGE;
            calState.current_cal_index = 0;
            calState.I_high = 0.0f;
            calState.I_low = 0.0f;
            calState.V_setpoint = 0.0f;
            Controller_SetState(STATE_IDLE);
        }
        else
        {
            Motor_SetPhaseInductance(L);
            Controller_UpdateCurrentGains();
        }
#else
        Motor_SetPhaseInductance(0.005);
#endif
        calState.V_setpoint = 0.0f;
    }

    // Offset
    else if (calState.current_cal_index < CAL_OFFSET_END_INDEX)
    {
        float pwm_setpoint = (config.I_cal_offset_setpoint * Motor_GetPhaseResistance()) / ADC_GetVBus();
        clamp(&pwm_setpoint, -PWM_LIMIT, PWM_LIMIT);
        SVM(pwm_setpoint, 0.0f, &state.modulation_values.A,
                    &state.modulation_values.B, &state.modulation_values.C);
    }
    else if (calState.current_cal_index == CAL_OFFSET_END_INDEX)
    {
        Observer_Reset();
        Observer_CalibrateOffset();
        calState.dir_initial_pos = Observer_GetPosEstimate();
    }

    // Pole Pairs & Direction
    else if (calState.current_cal_index < CAL_DIR_END_INDEX)
    {
        // Ensure rotor stays at 2*Pi eangle a bit
        const float numerator = calState.current_cal_index - CAL_OFFSET_END_INDEX;
        const float denominator = CAL_DIR_END_INDEX - CAL_OFFSET_END_INDEX;
        static const float end_angle = CAL_PHASE_TURNS * twopi;
        float cur_angle = 1.2f * end_angle * (numerator/denominator);
        clamp(&cur_angle, 0.0f, end_angle);
        float pwm_setpoint = (config.I_cal_offset_setpoint * Motor_GetPhaseResistance()) / ADC_GetVBus();
        clamp(&pwm_setpoint, -PWM_LIMIT, PWM_LIMIT);
        SVM(pwm_setpoint * fast_cos(cur_angle),
            pwm_setpoint * fast_sin(cur_angle),
            &state.modulation_values.A,
            &state.modulation_values.B,
            &state.modulation_values.C);
    }
    else if (calState.current_cal_index >= CAL_DIR_END_INDEX)
    {
        // Set neutral pwm immediately
        GateDriver_SetDutyCycle(&zeroDC);
        static const float end_angle = CAL_PHASE_TURNS * twopi;
        if (!Motor_FindPolePairs(ENCODER_CPR, calState.dir_initial_pos, Observer_GetPosEstimate(), end_angle))
        {
            state.error = ERROR_INVALID_POLE_PAIRS;
        }
        // Direction needs to be calibrated LAST
        Observer_CalibrateDirection(calState.dir_initial_pos);
        calState.current_cal_index = 0;
        calState.I_high = 0.0f;
        calState.I_low = 0.0f;
        Controller_SetState(STATE_IDLE);
    }
    else
    {
        // No action
    }

    if (calState.current_cal_index < CAL_DIR_END_INDEX)
    {
        GateDriver_SetDutyCycle(&state.modulation_values);
    }
    calState.current_cal_index++;
}

