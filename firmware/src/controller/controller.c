
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

#include "src/common.h"
#include "src/encoders/MA702.h"
#include "src/observer/observer.h"
#include "src/adc/adc.h"
#include "src/motor/motor.h"
#include "src/gatedriver/gatedriver.h"
#include "src/watchdog/watchdog.h"
#include "src/utils/utils.h"
#include "controller.h"

PAC5XXX_RAMFUNC void CLControlStep(void);
void CalibrateStep(void);
PAC5XXX_RAMFUNC void IdleStep(void);
PAC5XXX_RAMFUNC static inline bool Controller_LimitVelocity(float min_limit, float max_limit, float vel_estimate,
    float vel_gain, float *I);
PAC5XXX_RAMFUNC static inline void Controller_UpdateCurrentGains(void);

static struct FloatTriplet zeroDC = {0.5f, 0.5f, 0.5f};

static struct ControllerState state = {

    .state = STATE_IDLE,
    .mode = CTRL_CURRENT,
    .error = ERROR_NO_ERROR,

    .I_phase_meas = {0.0f, 0.0f, 0.0f},
    .modulation_values = {0.0f, 0.0f, 0.0f},

    .Iq_meas = 0.0f,
    .Id_meas = 0.0f,

    .pos_setpoint = 0.0f,
    .vel_setpoint = 0.0f,
    .Iq_setpoint = 0.0f,

    .vel_integrator_Iq = 0.0f,

    .Iq_integrator_Vq = 0.0f,
    .Id_integrator_Vd = 0.0f,

    .busy_cycles = 0,
    .total_cycles = 0,
    .last_timestamp = 0
};

static struct ControllerConfig config ={

    .vel_limit = 300000.0f,
    .I_limit = 10.0f,

    .pos_gain = 25.0f,
    .vel_gain = 1.0e-4f,
    .vel_integrator_gain = 0.0f,
    .I_bw = 800.0,
    .I_gain = 0.0f,
    .Iq_integrator_gain = 0.0f,
    .Id_integrator_gain = 0.0f,

    .I_cal_R_setpoint = 5.0f,
    .I_cal_offset_setpoint = 8.0f,

    .I_k = 0.3f,

    .V_calib_gain = 0.0005f,
    .V_calib_inductance = 2.0f
};

struct CalibrateState
{
    float V_setpoint;
    float I_low;
    float I_high;

    float dir_initial_pos;

    uint32_t current_cal_index;
};

struct CalibrateState calState = {0};

void Controller_Init(void)
{
    ADC_SetDTSE_callback(Controller_Update);
}

void Controller_Update(void)
{
    const uint32_t current_timestamp = ARM_CM_DWT_CYCCNT;
    state.total_cycles = current_timestamp - state.last_timestamp;

    const float VBus = ADC_GetVBus();
    const float Iq = Controller_GetIqEstimate();

    Observer_UpdatePos();

    if ((VBus < VBUS_LOW_THRESHOLD) && (state.state != STATE_IDLE))
    {
        state.error = ERROR_VBUS_UNDERVOLTAGE;
        Controller_SetState(STATE_IDLE);
    }
    else if ( (Iq > (config.I_limit * I_TRIP_MARGIN)) ||
              (Iq < -(config.I_limit * I_TRIP_MARGIN)) )
    {
        state.error = ERROR_OVERCURRENT;
        Controller_SetState(STATE_IDLE);
    }
    else
    {
        ADC_GetPhaseCurrents(&(state.I_phase_meas));
        if ((state.error != ERROR_NO_ERROR) && (state.state != STATE_IDLE))
        {
            Controller_SetState(STATE_IDLE);
        }

        if (state.state == STATE_CL_CONTROL)
        {
            CLControlStep();
        }
        else if (state.state == STATE_CALIBRATE)
        {
            CalibrateStep();
        }
        else // STATE_IDLE
        {
            IdleStep();
        }
        state.busy_cycles = ARM_CM_DWT_CYCCNT - current_timestamp;
        state.last_timestamp = current_timestamp;
    }
}

PAC5XXX_RAMFUNC void CLControlStep(void)
{
    float vel_setpoint = state.vel_setpoint;
    if (state.mode >= CTRL_POSITION)
    {
        const float delta_pos = Observer_GetPosDiff(state.pos_setpoint);
        vel_setpoint += delta_pos * config.pos_gain;
    }

    const float vel_estimate = Observer_GetVelEstimate();
    float Iq_setpoint = state.Iq_setpoint;

    if (state.mode >= CTRL_VELOCITY)
    {
        float delta_vel = vel_setpoint - vel_estimate;
        // Velocity limiting will be done later on based on the estimate
        Iq_setpoint += (delta_vel * config.vel_gain) + state.vel_integrator_Iq;
        state.vel_integrator_Iq += delta_vel * PWM_TIMER_PERIOD * config.vel_integrator_gain;
    }
    else
    {
        state.vel_integrator_Iq = 0.0f;
    }
    
    // Velocity-dependent current limiting
    if ((config.vel_limit > 0.0f) && Controller_LimitVelocity(-config.vel_limit, config.vel_limit,
            vel_estimate, config.vel_gain, &Iq_setpoint))
    {
        state.vel_integrator_Iq *= 0.995f;
    }

    // Absolute current & velocity integrator limiting
    if ((config.I_limit > 0.0f) && clamp(&Iq_setpoint, -config.I_limit, config.I_limit))
    {
        state.vel_integrator_Iq *= 0.995f;
    }

    const float angle = Observer_GetPosEstimateWrappedRadians();
    const float I_phase = angle * Motor_GetPolePairs();
    const float VBus = ADC_GetVBus();

    // Clarke transform
    const float Ialpha = state.I_phase_meas.A;
    const float Ibeta = one_by_sqrt3 * (state.I_phase_meas.B - state.I_phase_meas.C);

    // Park transform
    const float c_I = fast_cos(I_phase);
    const float s_I = fast_sin(I_phase);
    const float Id = (c_I * Ialpha) + (s_I * Ibeta);
    const float Iq = (c_I * Ibeta) - (s_I * Ialpha);

    state.Id_meas += config.I_k * (Id - state.Id_meas);
    state.Iq_meas += config.I_k * (Iq - state.Iq_meas);

    const float delta_Id = 0 - state.Id_meas;
    const float delta_Iq = Iq_setpoint - state.Iq_meas;

    state.Id_integrator_Vd += delta_Id * PWM_TIMER_PERIOD * config.Id_integrator_gain;
    state.Iq_integrator_Vq += delta_Iq * PWM_TIMER_PERIOD * config.Iq_integrator_gain;

    const float Vd = (delta_Id * config.I_gain) + state.Id_integrator_Vd;
    const float Vq = (delta_Iq * config.I_gain) + state.Iq_integrator_Vq;

    // Inverse Park transform
    const float Va = (c_I * Vd) - (s_I * Vq);
    const float Vb = (c_I * Vq) + (s_I * Vd);

    float mod_a = Va / VBus;
    float mod_b = Vb / VBus;

    if ((mod_a > PWM_LIMIT) && (mod_a < -PWM_LIMIT) &&
        (mod_b > PWM_LIMIT) && (mod_b < -PWM_LIMIT))
    {
        state.error = ERROR_PWM_LIMIT_EXCEEDED;
        Controller_SetState(STATE_IDLE);
    }
    else
    {
        SVM(mod_a, mod_b, &state.modulation_values.A,
            &state.modulation_values.B, &state.modulation_values.C);
        GateDriver_SetDutyCycle(&state.modulation_values);
    }
}

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

PAC5XXX_RAMFUNC void IdleStep(void)
{
    Watchdog_Feed();
}

PAC5XXX_RAMFUNC ControlState Controller_GetState(void)
{
    return state.state;
}

PAC5XXX_RAMFUNC void Controller_SetState(ControlState new_state)
{
    switch (new_state)
    {
        case CL_CONTROL:
        case CALIBRATE:
            if ((state.state == STATE_IDLE) && (state.error == ERROR_NO_ERROR))
            {
                if (new_state == STATE_CL_CONTROL && Controller_Calibrated())
                {
                    state.pos_setpoint = Observer_GetPosEstimate();
                    GateDriver_Enable();
                    state.state = STATE_CL_CONTROL;
                }
                else if (new_state == STATE_CALIBRATE)
                {
                    GateDriver_Enable();
                    state.state = STATE_CALIBRATE;
                }
            }
            break;
        default:
            GateDriver_SetDutyCycle(&zeroDC);
            GateDriver_Disable();
            state.state = STATE_IDLE;
    }
}

ControlMode Controller_GetMode(void)
{
	return state.mode;
}

void Controller_SetMode(ControlMode new_mode)
{
    if (new_mode != state.mode)
    {
        state.mode = new_mode;
    }
}

float Controller_GetPosSetpoint(void)
{
    return state.pos_setpoint;
}

void Controller_SetPosSetpoint(float value)
{
    state.pos_setpoint = value;
}

float Controller_GetVelSetpoint(void)
{
    return state.vel_setpoint;
}

void Controller_SetVelSetpoint(float value)
{
    state.vel_setpoint = value;
}

float Controller_GetIqEstimate(void)
{
    return state.Iq_meas;
}

float Controller_GetIqSetpoint(void)
{
    return state.Iq_setpoint;
}

void Controller_SetIqSetpoint(float value)
{
    state.Iq_setpoint = value;
}

void Controller_GetModulationValues(struct FloatTriplet *dc)
{
    dc->A = state.modulation_values.A;
    dc->B = state.modulation_values.B;
    dc->C = state.modulation_values.C;
}

float Controller_GetPosGain(void)
{
    return config.pos_gain;
}

void Controller_SetPosGain(float gain)
{
    if (gain >= 0.0f)
    {
        config.pos_gain = gain;
    }
}

float Controller_GetVelGain(void)
{
    return config.vel_gain;
}

void Controller_SetVelGain(float gain)
{
    if (gain >= 0.0f)
    {
        config.vel_gain = gain;
    }
}

float Controller_GetVelIntegratorGain(void)
{
    return config.vel_integrator_gain;
}

void Controller_SetVelIntegratorGain(float gain)
{
    if (gain >= 0.0f)
    {
        config.vel_integrator_gain = gain;
    }
}

float Controller_GetIqGain(void)
{
    return config.I_gain;
}

float Controller_GetIqBandwidth(void)
{
    return config.I_bw;
}

void Controller_SetIqBandwidth(float bw)
{
    if (bw > 0.0f)
    {
        config.I_bw = bw;
        Controller_UpdateCurrentGains();
    }
}

float Controller_GetVelLimit(void)
{
    return config.vel_limit;
}

void Controller_SetVelLimit(float limit)
{
    if (limit >= 0.0f)
    {
        config.vel_limit = limit;
    }
}

float Controller_GetIqLimit(void)
{
    return config.I_limit;
}

void Controller_SetIqLimit(float limit)
{
    if (limit >= 0.0f)
    {
        config.I_limit = limit;
    }
}

PAC5XXX_RAMFUNC bool Controller_Calibrated(void)
{
    return Motor_Calibrated() & Observer_Calibrated();
}

uint8_t Controller_GetError(void)
{
    return (uint8_t)(state.error);
}

uint32_t Controller_GetTotalCycles(void)
{
    return state.total_cycles;
}

uint32_t Controller_GetBusyCycles(void)
{
    return state.busy_cycles;
}

struct ControllerConfig* Controller_GetConfig(void)
{
    return &config;
}

void Controller_RestoreConfig(struct ControllerConfig* config_)
{
    config = *config_;
}

PAC5XXX_RAMFUNC static inline bool Controller_LimitVelocity(float min_limit, float max_limit, float vel_estimate,
    float vel_gain, float *I)
{
    float Imax = (max_limit - vel_estimate) * vel_gain;
    float Imin = (min_limit - vel_estimate) * vel_gain;
    return clamp(I, Imin, Imax);
}

PAC5XXX_RAMFUNC static inline void Controller_UpdateCurrentGains(void)
{
    config.I_gain = config.I_bw * Motor_GetPhaseInductance();
    float plant_pole = Motor_GetPhaseResistance() / Motor_GetPhaseInductance();
    config.Iq_integrator_gain = plant_pole * config.I_gain;
    config.Id_integrator_gain = config.Iq_integrator_gain;
}
