
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

#include "src/system/system.h"
#include "src/observer/observer.h"
#include "src/adc/adc.h"
#include "src/motor/motor.h"
#include "src/gatedriver/gatedriver.h"
#include "src/utils/utils.h"
#include <src/scheduler/scheduler.h>
#include <src/motor/calibration.h>
#include <src/controller/controller.h>

PAC5XXX_RAMFUNC void CLControlStep(void);
PAC5XXX_RAMFUNC void IdleStep(void);
PAC5XXX_RAMFUNC static inline bool Controller_LimitVelocity(float min_limit, float max_limit, float vel_estimate,
    float vel_gain, float *I);

static struct FloatTriplet zeroDC = {0.5f, 0.5f, 0.5f};
static MotionPlan motion_plan;
static ControllerState state = {

    .state = STATE_IDLE,
    .mode = CTRL_CURRENT,
    .is_calibrating = false,

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

    .t_plan = 0.0f
};

static ControllerConfig config ={

    .vel_limit = 300000.0f,
    .I_limit = 10.0f,

    .pos_gain = 20.0f,
    .vel_gain = 8.0e-5f,
    .vel_integrator_gain = 0.00020f,
    .vel_integrator_deadband = 200.0f,
    .I_bw = 1000.0,
    .I_gain = 0.0f,
    .Iq_integrator_gain = 0.0f,
    .Id_integrator_gain = 0.0f,
    .I_k = 0.3f
};

void Controller_ControlLoop(void)
{
	while (true)
	{
		health_check();
		const float Iq = controller_get_Iq_estimate();
		if ( (Iq > (config.I_limit * I_TRIP_MARGIN)) ||
					  (Iq < -(config.I_limit * I_TRIP_MARGIN)) )
		{
			add_error_flag(ERROR_OVERCURRENT);
		}
		if (error_flags_exist() && (state.state != STATE_IDLE))
		{
			Controller_SetState(STATE_IDLE);
		}

		if (state.state == STATE_CALIBRATE)
		{
			state.is_calibrating = true;
            if (ENCODER_MA7XX == system_get_encoder_type())
            {
                (void) ((CalibrateResistance() && CalibrateInductance()) && CalibrateDirectionAndPolePairs() && calibrate_offset_and_rectification());
            }
            else if (ENCODER_HALL == system_get_encoder_type())
            {
                (void) ((CalibrateResistance() && CalibrateInductance()) && calibrate_hall_sequence());
            }
			state.is_calibrating = false;
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

PAC5XXX_RAMFUNC void CLControlStep(void)
{
    if (state.mode >= CTRL_TRAJECTORY)
    {
        state.t_plan += PWM_PERIOD_S;
        // This will set state.pos_setpoint state.vel_setpoint (in user frame)
        if (!planner_evaluate(state.t_plan, &motion_plan))
        {
        	// Drop to position mode on error or completion
            Controller_SetMode(CTRL_POSITION);
            state.t_plan = 0;
        }
    }

    // The actual velocity setpoint and the one used by the velocity integrator are
    // separate because the latter takes into account a user-confiugurable deadband
    // around the position setpoint, where the integrator "sees" no error
    float vel_setpoint = state.vel_setpoint;
    float vel_setpoint_integrator = state.vel_setpoint;
    
    if (state.mode >= CTRL_POSITION)
    {
        const float delta_pos = observer_get_diff(state.pos_setpoint);
        const float delta_pos_integrator = sgnf(delta_pos) * our_fmaxf(0, fabsf(delta_pos) - config.vel_integrator_deadband);
        vel_setpoint += delta_pos * config.pos_gain;
        vel_setpoint_integrator += delta_pos_integrator * config.pos_gain;
    }

    const float vel_estimate = observer_get_vel_estimate();
    float Iq_setpoint = state.Iq_setpoint;

    if (state.mode >= CTRL_VELOCITY)
    {
        const float delta_vel = vel_setpoint - vel_estimate;
        const float delta_vel_integrator = vel_setpoint_integrator - vel_estimate;
        // Velocity limiting will be done later on based on the estimate
        Iq_setpoint += delta_vel * config.vel_gain;
        Iq_setpoint += state.vel_integrator_Iq;
        state.vel_integrator_Iq += delta_vel_integrator * PWM_PERIOD_S * config.vel_integrator_gain;
    }
    else
    {
        state.vel_integrator_Iq = 0.0f;
    }
    
    // Velocity-dependent current limiting
    const float vel_limit = our_fminf(config.vel_limit, VEL_HARD_LIMIT);
    if (Controller_LimitVelocity(-vel_limit, vel_limit, vel_estimate, config.vel_gain, &Iq_setpoint) == true)
    {
        state.vel_integrator_Iq *= 0.995f;
    }

    // Absolute current & velocity integrator limiting
    const float I_limit = our_fminf(config.I_limit, I_HARD_LIMIT);
    if (our_clamp(&Iq_setpoint, -I_limit, I_limit) == true)
    {
        state.vel_integrator_Iq *= 0.995f;
    }

    const float e_phase = observer_get_epos();
    const float c_I = fast_cos(e_phase);
    const float s_I = fast_sin(e_phase);
    const float VBus = ADC_GetVBus();

    float Vd; float Vq;
    if (motor_is_gimbal() == true)
    {
    	const float e_phase_vel = observer_get_evel();
        Vd = - e_phase_vel * motor_get_phase_inductance() * Iq_setpoint;
        Vq = motor_get_phase_resistance() * Iq_setpoint;
    }
    else
    {
        ADC_GetPhaseCurrents(&(state.I_phase_meas));

        // Clarke transform
        const float Ialpha = state.I_phase_meas.A;
        const float Ibeta = one_by_sqrt3 * (state.I_phase_meas.B - state.I_phase_meas.C);

        // Park transform
        const float Id = (c_I * Ialpha) + (s_I * Ibeta);
        const float Iq = (c_I * Ibeta) - (s_I * Ialpha);

        state.Id_meas += config.I_k * (Id - state.Id_meas);
        state.Iq_meas += config.I_k * (Iq - state.Iq_meas);

        const float delta_Id = 0 - state.Id_meas;
        const float delta_Iq = Iq_setpoint - state.Iq_meas;

        state.Id_integrator_Vd += delta_Id * PWM_PERIOD_S * config.Id_integrator_gain;
        state.Iq_integrator_Vq += delta_Iq * PWM_PERIOD_S * config.Iq_integrator_gain;

        Vd = (delta_Id * config.I_gain) + state.Id_integrator_Vd;
        Vq = (delta_Iq * config.I_gain) + state.Iq_integrator_Vq;
    }
    
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
    //pass
}

PAC5XXX_RAMFUNC ControlState Controller_GetState(void)
{
    return state.state;
}

PAC5XXX_RAMFUNC void Controller_SetState(ControlState new_state)
{
	if ((new_state != state.state) && (state.is_calibrating == false))
	{
		if ((new_state == STATE_CL_CONTROL) && (state.state == STATE_IDLE)
				&& (!error_flags_exist()) && motor_is_calibrated())
		{
			state.pos_setpoint = Observer_GetPosEstimate();
			GateDriver_Enable();
			state.state = STATE_CL_CONTROL;
		}
		else if ((new_state == STATE_CALIBRATE) && (state.state == STATE_IDLE)
				&& (!error_flags_exist()))
		{
			GateDriver_Enable();
			state.state = STATE_CALIBRATE;
		}
		else // state != STATE_IDLE --> Got to idle state anyway
		{
			GateDriver_SetDutyCycle(&zeroDC);
			GateDriver_Disable();
			state.state = STATE_IDLE;
		}
	}
}

PAC5XXX_RAMFUNC ControlMode Controller_GetMode(void)
{
	return state.mode;
}

PAC5XXX_RAMFUNC void Controller_SetMode(ControlMode new_mode)
{
    if (new_mode != state.mode)
    {
		if (new_mode == CTRL_TRAJECTORY)
		{
			state.mode = CTRL_TRAJECTORY;
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
    }
}

PAC5XXX_RAMFUNC float controller_get_pos_setpoint_user_frame(void)
{
    return (state.pos_setpoint - motor_get_user_offset()) * motor_get_user_direction();
}

PAC5XXX_RAMFUNC void controller_set_pos_setpoint_user_frame(float value)
{
	// direction is either 1 or -1 so we can multiply instead of divide
    state.pos_setpoint = value * motor_get_user_direction() + motor_get_user_offset();
}

PAC5XXX_RAMFUNC float controller_get_vel_setpoint_user_frame(void)
{
    return state.vel_setpoint * motor_get_user_direction();
}

PAC5XXX_RAMFUNC void controller_set_vel_setpoint_user_frame(float value)
{
	// direction is either 1 or -1 so we can multiply instead of divide
    state.vel_setpoint = value * motor_get_user_direction();
}

PAC5XXX_RAMFUNC float controller_get_Iq_estimate(void)
{
    return state.Iq_meas;
}

PAC5XXX_RAMFUNC float controller_get_Iq_setpoint(void)
{
    return state.Iq_setpoint;
}

PAC5XXX_RAMFUNC void controller_set_Iq_setpoint(float value)
{
    state.Iq_setpoint = value;
}

PAC5XXX_RAMFUNC float controller_get_Iq_estimate_user_frame(void)
{
	return state.Iq_meas * motor_get_user_direction();
}

PAC5XXX_RAMFUNC float controller_get_Iq_setpoint_user_frame(void)
{
	return state.Iq_setpoint * motor_get_user_direction();
}

PAC5XXX_RAMFUNC void controller_set_Iq_setpoint_user_frame(float value)
{
	state.Iq_setpoint = value * motor_get_user_direction();
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

float controller_get_vel_integrator_deadband(void)
{
    return config.vel_integrator_deadband;
}

void controller_set_vel_integrator_deadband(float value)
{
    if (value >= 0.0f)
    {
        config.vel_integrator_deadband = value;
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
    if (limit > 0.0f)
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
    if (limit > 0.0f)
    {
        config.I_limit = limit;
    }
}

void controller_set_motion_plan(MotionPlan mp)
{
    motion_plan = mp;
    state.t_plan = 0.0f;
}

ControllerConfig* Controller_GetConfig(void)
{
    return &config;
}

void Controller_RestoreConfig(ControllerConfig* config_)
{
    config = *config_;
}

static inline bool Controller_LimitVelocity(float min_limit, float max_limit, float vel_estimate,
    float vel_gain, float *I)
{
    float Imax = (max_limit - vel_estimate) * vel_gain;
    float Imin = (min_limit - vel_estimate) * vel_gain;
    return our_clamp(I, Imin, Imax);
}

PAC5XXX_RAMFUNC void Controller_UpdateCurrentGains(void)
{
    config.I_gain = config.I_bw * motor_get_phase_inductance();
    float plant_pole = motor_get_phase_resistance() / motor_get_phase_inductance();
    config.Iq_integrator_gain = plant_pole * config.I_gain;
    config.Id_integrator_gain = config.Iq_integrator_gain;
}
