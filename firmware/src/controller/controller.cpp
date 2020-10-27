
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

#include <src/adc/adc.hpp>
#include <src/controller/controller.hpp>
#include <src/encoder/Encoder.hpp>
#include <src/gatedriver/gatedriver.hpp>
#include <src/motor/motor.hpp>
#include <src/observer/observer.hpp>
#include <src/watchdog/watchdog.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include "src/utils/utils.h"

#ifdef __cplusplus
}
#endif

static struct FloatTriplet zeroDC = {0.5f, 0.5f, 0.5f};

struct CalibrateState
{
    float V_setpoint;
    float I_low;
    float I_high;

    float dir_initial_pos;

    uint32_t current_cal_index;
};

PAC5XXX_RAMFUNC void Controller::HealthCheck(void)
{
	const float VBus = systm.adc.GetVBus();
	const float Iq = GetIqEstimate();

	if (VBus < VBUS_LOW_THRESHOLD)
	{
		throw ERROR_VBUS_UNDERVOLTAGE;
	}
	else if ( (Iq > (config.I_limit * I_TRIP_MARGIN)) ||
			  (Iq < -(config.I_limit * I_TRIP_MARGIN)) )
	{
		throw ERROR_OVERCURRENT;
	}
}

PAC5XXX_RAMFUNC void Controller::ControlLoop(void)
{
	while (true)
	{
		const uint32_t current_timestamp = ARM_CM_DWT_CYCCNT;
		total_cycles = current_timestamp - last_timestamp;
		try
		{
			systm.HealthCheck();
			systm.adc.UpdateCurrentMeas();
			systm.adc.GetPhaseCurrents(&(I_phase_meas));
			systm.observer.UpdateEstimates();
			if (state == STATE_CALIBRATE)
			{
				CalibrateResistance(systm);
				CalibrateInductance(systm);
				CalibrateOffset(systm);
				CalibrateDirection(systm);
			}
			else if (state == STATE_CLOSED_LOOP_CONTROL)
			{
				ClosedLoopControlStep();
			}
			else
			{
				IdleStep();
			}
		}
		catch (ControlError e)
		{
			error = e;
			SetState(STATE_IDLE);
		}
		busy_cycles = ARM_CM_DWT_CYCCNT - current_timestamp;
		last_timestamp = current_timestamp;
		systm.WaitForControlLoopInterrupt();
	}
}

PAC5XXX_RAMFUNC void Controller::ClosedLoopControlStep(void)
{
    float _vel_setpoint = vel_setpoint;
    if (mode >= CTRL_POSITION)
    {
        const float delta_pos = systm.observer.GetPosDiff(pos_setpoint);
        _vel_setpoint += delta_pos * config.pos_gain;
    }

    const float vel_estimate = systm.observer.GetVelEstimate();
    float _Iq_setpoint = Iq_setpoint;

    if (mode >= CTRL_VELOCITY)
    {
        float delta_vel = _vel_setpoint - vel_estimate;
        // Velocity limiting will be done later on based on the estimate
        _Iq_setpoint += (delta_vel * config.vel_gain) + vel_integrator_Iq;
        vel_integrator_Iq += delta_vel * PWM_TIMER_PERIOD * config.vel_integrator_gain;
    }
    else
    {
        vel_integrator_Iq = 0.0f;
    }

    // Velocity-dependent current limiting
    if ((config.vel_limit > 0.0f) && LimitVelocity(-config.vel_limit, config.vel_limit,
            vel_estimate, config.vel_gain, &_Iq_setpoint))
    {
        vel_integrator_Iq *= 0.995f;
    }

    // Absolute current & velocity integrator limiting
    if ((config.I_limit > 0.0f) && clamp(&_Iq_setpoint, -config.I_limit, config.I_limit))
    {
        vel_integrator_Iq *= 0.995f;
    }

    const float angle = systm.observer.GetPosEstimateWrappedRadians();
    const float I_phase = angle * systm.motor.GetPolePairs();
    const float VBus = systm.adc.GetVBus();

    // Clarke transform
    const float Ialpha = I_phase_meas.A;
    const float Ibeta = one_by_sqrt3 * (I_phase_meas.B - I_phase_meas.C);

    // Park transform
    const float c_I = fast_cos(I_phase);
    const float s_I = fast_sin(I_phase);
    const float Id = (c_I * Ialpha) + (s_I * Ibeta);
    const float Iq = (c_I * Ibeta) - (s_I * Ialpha);

    Id_meas += config.I_k * (Id - Id_meas);
    Iq_meas += config.I_k * (Iq - Iq_meas);

    const float delta_Id = 0 - Id_meas;
    const float delta_Iq = _Iq_setpoint - Iq_meas;

    Id_integrator_Vd += delta_Id * PWM_TIMER_PERIOD * config.Id_integrator_gain;
    Iq_integrator_Vq += delta_Iq * PWM_TIMER_PERIOD * config.Iq_integrator_gain;

    const float Vd = (delta_Id * config.I_gain) + Id_integrator_Vd;
    const float Vq = (delta_Iq * config.I_gain) + Iq_integrator_Vq;

    float mod_q = Vq / VBus;
    float mod_d = Vd / VBus;

    // dq modulation limiter
    const float dq_mod_scale_factor = PWM_LIMIT * fast_inv_sqrt((mod_q * mod_q) + (mod_d * mod_d));

    if (dq_mod_scale_factor < 1.0f)
    {
        mod_q *= dq_mod_scale_factor;
        mod_d *= dq_mod_scale_factor;
        Id_integrator_Vd *= I_INTEGRATOR_DECAY_FACTOR;
        Iq_integrator_Vq *= I_INTEGRATOR_DECAY_FACTOR;
    }

    // Inverse Park transform
    float mod_a = (c_I * mod_d) - (s_I * mod_q);
    float mod_b = (c_I * mod_q) + (s_I * mod_d);

    SVM(mod_a, mod_b, &modulation_values.A,
        &modulation_values.B, &modulation_values.C);
    systm.driver.SetDutyCycle(&modulation_values);
}

PAC5XXX_RAMFUNC void Controller::IdleStep(void)
{
    systm.watchdog.Feed();
}

PAC5XXX_RAMFUNC ControlState Controller::GetState(void)
{
    return state;
}

PAC5XXX_RAMFUNC void Controller::SetState(ControlState new_state)
{
    if (state == new_state)
    {
        // No action
    }
    else if (new_state == STATE_IDLE)
    {
    	systm.driver.SetDutyCycle(&zeroDC);
        systm.driver.Disable();
        state = STATE_IDLE;
    }
    else if ((state == STATE_IDLE) && (error == ERROR_NO_ERROR))
    {
        if ((new_state == STATE_CLOSED_LOOP_CONTROL) && Calibrated())
        {
            pos_setpoint = systm.observer.GetPosEstimate();
            systm.driver.Enable();
            state = STATE_CLOSED_LOOP_CONTROL;
        }
        else if (new_state == STATE_CALIBRATE)
        {
        	systm.driver.Enable();
            state = STATE_CALIBRATE;
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

ControlMode Controller::GetMode(void)
{
	return mode;
}

void Controller::SetMode(ControlMode new_mode)
{
    if (new_mode == mode)
    {
        // No action
    }
	else if (new_mode == CTRL_POSITION)
	{
		mode = CTRL_POSITION;
	}
	else if (new_mode == CTRL_VELOCITY)
	{
		mode = CTRL_VELOCITY;
	}
	else if (new_mode == CTRL_CURRENT)
	{
		mode = CTRL_CURRENT;
	}
    else
    {
        // No action
    }
}

float Controller::GetPosSetpoint(void)
{
    return pos_setpoint;
}

void Controller::SetPosSetpoint(float value)
{
    pos_setpoint = value;
}

float Controller::GetVelSetpoint(void)
{
    return vel_setpoint;
}

void Controller::SetVelSetpoint(float value)
{
    vel_setpoint = value;
}

float Controller::GetIqEstimate(void)
{
    return Iq_meas;
}

float Controller::GetIqSetpoint(void)
{
    return Iq_setpoint;
}

void Controller::SetIqSetpoint(float value)
{
    Iq_setpoint = value;
}

void Controller::GetModulationValues(struct FloatTriplet *dc)
{
    dc->A = modulation_values.A;
    dc->B = modulation_values.B;
    dc->C = modulation_values.C;
}

float Controller::GetPosGain(void)
{
    return config.pos_gain;
}

void Controller::SetPosGain(float gain)
{
    if (gain >= 0.0f)
    {
        config.pos_gain = gain;
    }
}

float Controller::GetVelGain(void)
{
    return config.vel_gain;
}

void Controller::SetVelGain(float gain)
{
    if (gain >= 0.0f)
    {
        config.vel_gain = gain;
    }
}

float Controller::GetVelIntegratorGain(void)
{
    return config.vel_integrator_gain;
}

void Controller::SetVelIntegratorGain(float gain)
{
    if (gain >= 0.0f)
    {
        config.vel_integrator_gain = gain;
    }
}

float Controller::GetIqGain(void)
{
    return config.I_gain;
}

float Controller::GetIqBandwidth(void)
{
    return config.I_bw;
}

void Controller::SetIqBandwidth(float bw)
{
    if (bw > 0.0f)
    {
        config.I_bw = bw;
        UpdateCurrentGains();
    }
}

float Controller::GetVelLimit(void)
{
    return config.vel_limit;
}

void Controller::SetVelLimit(float limit)
{
    if (limit >= 0.0f)
    {
        config.vel_limit = limit;
    }
}

float Controller::GetIqLimit(void)
{
    return config.I_limit;
}

void Controller::SetIqLimit(float limit)
{
    if (limit >= 0.0f)
    {
        config.I_limit = limit;
    }
}

PAC5XXX_RAMFUNC bool Controller::Calibrated(void)
{
    return systm.motor.Calibrated() & systm.observer.Calibrated();
}

uint8_t Controller::GetError(void)
{
    return (uint8_t)(error);
}

uint32_t Controller::GetTotalCycles(void)
{
    return total_cycles;
}

uint32_t Controller::GetBusyCycles(void)
{
    return busy_cycles;
}

PAC5XXX_RAMFUNC bool Controller::LimitVelocity(float min_limit, float max_limit, float vel_estimate,
    float vel_gain, float *I)
{
    float Imax = (max_limit - vel_estimate) * vel_gain;
    float Imin = (min_limit - vel_estimate) * vel_gain;
    return clamp(I, Imin, Imax);
}

PAC5XXX_RAMFUNC void Controller::UpdateCurrentGains(void)
{
    config.I_gain = config.I_bw * systm.motor.GetPhaseInductance();
    float plant_pole = systm.motor.GetPhaseResistance() / systm.motor.GetPhaseInductance();
    config.Iq_integrator_gain = plant_pole * config.I_gain;
    config.Id_integrator_gain = config.Iq_integrator_gain;
}
