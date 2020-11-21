
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

#include "src/encoders/MA702.h"
#include "src/observer/observer.h"
#include "src/adc/adc.h"
#include "src/motor/motor.h"
#include "src/gatedriver/gatedriver.h"
#include "src/watchdog/watchdog.h"
#include "src/utils/utils.h"
#include <src/scheduler/scheduler.h>
#include <src/controller/calibration.h>
#include "controller.h"

PAC5XXX_RAMFUNC ControlError HealthCheck(void);
PAC5XXX_RAMFUNC void CLControlStep(void);
PAC5XXX_RAMFUNC void IdleStep(void);
PAC5XXX_RAMFUNC static inline bool Controller_LimitVelocity(float min_limit, float max_limit, float vel_estimate,
    float vel_gain, float *I);

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
    .I_k = 0.3f
};

void Controller_ControlLoop(void)
{
	while (true)
	{
		ControlError e = HealthCheck();
		if ((e != ERROR_NO_ERROR) && (state.state != STATE_IDLE))
		{
			state.error = e;
			Controller_SetState(STATE_IDLE);
		}

		if (state.state == STATE_CALIBRATE)
		{
			CalibrateResistance();
			CalibrateInductance();
			CalibrateOffsetDirectionAndPolePairs();
			Controller_SetState(STATE_IDLE);
		}
		else if (state.state == STATE_CL_CONTROL)
		{
			CLControlStep();
		}
		else
		{
			IdleStep();
		}
		WaitForControlLoopInterrupt();
	}
}

PAC5XXX_RAMFUNC ControlError HealthCheck(void)
{
	const float VBus = ADC_GetVBus();
	const float Iq = Controller_GetIqEstimate();
	ControlError e;
	if ((VBus < VBUS_LOW_THRESHOLD) && (state.state != STATE_IDLE))
	{
		e = ERROR_VBUS_UNDERVOLTAGE;
	}
	else if ( (Iq > (config.I_limit * I_TRIP_MARGIN)) ||
			  (Iq < -(config.I_limit * I_TRIP_MARGIN)) )
	{
		e = ERROR_OVERCURRENT;
	}
	else
	{
		e = ERROR_NO_ERROR;
	}
	return e;
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

    ADC_GetPhaseCurrents(&(state.I_phase_meas));

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

    float mod_q = Vq / VBus;
    float mod_d = Vd / VBus;

    // dq modulation limiter
    const float dq_mod_scale_factor = PWM_LIMIT * fast_inv_sqrt((mod_q * mod_q) + (mod_d * mod_d));

    if (dq_mod_scale_factor < 1.0f)
    {
        mod_q *= dq_mod_scale_factor;
        mod_d *= dq_mod_scale_factor;
        state.Id_integrator_Vd *= I_INTEGRATOR_DECAY_FACTOR;
        state.Iq_integrator_Vq *= I_INTEGRATOR_DECAY_FACTOR;
    }

    // Inverse Park transform
    float mod_a = (c_I * mod_d) - (s_I * mod_q);
    float mod_b = (c_I * mod_q) + (s_I * mod_d);

    SVM(mod_a, mod_b, &state.modulation_values.A,
        &state.modulation_values.B, &state.modulation_values.C);
    GateDriver_SetDutyCycle(&state.modulation_values);
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
    if (state.state == new_state)
    {
        // No action
    }
    else if (new_state == STATE_IDLE)
    {
        GateDriver_SetDutyCycle(&zeroDC);
        GateDriver_Disable();
        state.state = STATE_IDLE;
    }
    else if ((state.state == STATE_IDLE) && (state.error == ERROR_NO_ERROR))
    {
        if ((new_state == STATE_CL_CONTROL) && Controller_Calibrated())
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
        else
        {
            // No action
        }
    }
    else
    {
        // No action
    }
}

ControlMode Controller_GetMode(void)
{
	return state.mode;
}

void Controller_SetMode(ControlMode new_mode)
{
    if (new_mode == state.mode)
    {
        // No action
    }
	else if (new_mode == CTRL_POSITION)
	{
		state.mode = CTRL_POSITION;
	}
	else if (new_mode == CTRL_VELOCITY)
	{
		state.mode = CTRL_VELOCITY;
	}
	else if (new_mode == CTRL_CURRENT)
	{
		state.mode = CTRL_CURRENT;
	}
    else
    {
        // No action
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

PAC5XXX_RAMFUNC float Controller_GetIqEstimate(void)
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

PAC5XXX_RAMFUNC void Controller_UpdateCurrentGains(void)
{
    config.I_gain = config.I_bw * Motor_GetPhaseInductance();
    float plant_pole = Motor_GetPhaseResistance() / Motor_GetPhaseInductance();
    config.Iq_integrator_gain = plant_pole * config.I_gain;
    config.Id_integrator_gain = config.Iq_integrator_gain;
}
