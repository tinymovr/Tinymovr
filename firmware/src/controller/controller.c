
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
#include <src/encoder/encoder.h>
#include <src/observer/observer.h>
#include "src/adc/adc.h"
#include "src/motor/motor.h"
#include "src/gatedriver/gatedriver.h"
#include "src/utils/utils.h"
#include <src/scheduler/scheduler.h>
#include <src/motor/calibration.h>
#include <src/can/can_endpoints.h>
#include <src/controller/controller.h>
#include "src/watchdog/watchdog.h"

void CLControlStep(void);
static inline bool Controller_LimitVelocity(float min_limit, float max_limit, float vel_estimate,
                                                            float vel_gain, float *I);

static struct FloatTriplet zeroDC = {0.5f, 0.5f, 0.5f};
static MotionPlan motion_plan;
static ControllerState state = {

    .state = STATE_IDLE,
    .mode = CTRL_CURRENT,
    .errors = CONTROLLER_ERRORS_NONE,
    .is_calibrating = false,

    .I_phase_meas = {0.0f, 0.0f, 0.0f},
    .modulation_values = {0.0f, 0.0f, 0.0f},

    .Iq_est = 0.0f,
    .Id_est = 0.0f,

    .Ibus_est = 0.0f,
    .power_est = 0.0f,

    .pos_setpoint = 0.0f,
    .vel_setpoint = 0.0f,
    .vel_ramp_setpoint  = 0.0f,
    .Iq_setpoint = 0.0f,
    .Id_setpoint = 0.0f,

    .Vq_setpoint = 0.0f,

    .vel_integrator_Iq = 0.0f,

    .Iq_integrator_Vq = 0.0f,
    .Id_integrator_Vd = 0.0f,

    .t_plan = 0.0f};

#if defined BOARD_REV_R32 || BOARD_REV_R33 || defined BOARD_REV_R5

static ControllerConfig config = {
    .vel_limit = 300000.0f,
    .I_limit = 10.0f,
    .pos_gain = 20.0f,
    .vel_gain = 8.0e-5f,
    .vel_integrator_gain = 0.00020f,
    .vel_integrator_deadband = 200.0f,
    .I_bw = 1500.0,
    .I_gain = 0.0f,
    .Iq_integrator_gain = 0.0f,
    .Id_integrator_gain = 0.0f,
    .I_k = 0.3f,
    .vel_increment = 100.0f, // ticks/cycle
    .max_Ibus_regen = 0.0f,
    .max_Ibrake = 0.0f}; 

#elif defined BOARD_REV_M5

static ControllerConfig config = {
    .vel_limit = 100000.0f,
    .I_limit = 5.0f,
    .pos_gain = 20.0f,
    .vel_gain = 8.0e-5f,
    .vel_integrator_gain = 0.00020f,
    .vel_integrator_deadband = 200.0f,
    .I_bw = 1500.0,
    .I_gain = 0.0f,
    .Iq_integrator_gain = 0.0f,
    .Id_integrator_gain = 0.0f,
    .I_k = 0.3f,
    .vel_increment = 100.0f, // ticks/cycle
    .max_Ibus_regen = 0.0f,
    .max_Ibrake = 0.0f}; 

#endif

void Controller_ControlLoop(void)
{
    while (true)
    {
        state.warnings = 0;
        const float Iq = controller_get_Iq_estimate();
        if ((Iq > (config.I_limit * I_TRIP_MARGIN)) ||
            (Iq < -(config.I_limit * I_TRIP_MARGIN)))
        {
            state.errors |= CONTROLLER_ERRORS_CURRENT_LIMIT_EXCEEDED;
        }
        if (errors_exist() && (state.state != STATE_IDLE))
        {
            controller_set_state(STATE_IDLE);
        }

        if (state.state == STATE_CALIBRATE)
        {
            state.is_calibrating = true;
            reset_calibration();
            if (ENCODER_MA7XX == encoder_get_type())
            {
                (void)((CalibrateResistance() && CalibrateInductance()) && CalibrateDirectionAndPolePairs() && calibrate_offset_and_rectification());
            }
            else if (ENCODER_HALL == encoder_get_type())
            {
                (void)((CalibrateResistance() && CalibrateInductance()) && calibrate_hall_sequence());
            }
            state.is_calibrating = false;
            controller_set_state(STATE_IDLE);
        }
        else if (state.state == STATE_CL_CONTROL)
        {
            // Check the watchdog and revert to idle if it has timed out
            if (Watchdog_triggered())
            {
                controller_set_state(STATE_IDLE);
                Watchdog_reset();
            }
            else
            {
                CLControlStep();
            }
        }
        else
        {
            // pass
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
            controller_set_mode(CTRL_POSITION);
            state.t_plan = 0;
        }
    }

    // Sudden changes in velocity setpoints would lead to sudden
    // jerks and current spikes, so a ramping function makes transitions
    // a bit smoother.
    if (config.vel_increment > 0)
    {
        state.vel_ramp_setpoint  += our_clamp(state.vel_setpoint - state.vel_ramp_setpoint, -config.vel_increment, config.vel_increment);
    }
    else
    {
        state.vel_ramp_setpoint  = state.vel_setpoint ;
    }

    // The actual velocity setpoint and the one used by the velocity integrator are
    // separate because the latter takes into account a user-configurable deadband
    // around the position setpoint, where the integrator "sees" no error
    float vel_setpoint = state.vel_ramp_setpoint ;
    float vel_setpoint_integrator = state.vel_ramp_setpoint ;

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
    if (Controller_LimitVelocity(-config.vel_limit, config.vel_limit, vel_estimate, config.vel_gain, &Iq_setpoint) == true)
    {
        state.vel_integrator_Iq *= 0.995f;
        state.warnings |= CONTROLLER_WARNINGS_VELOCITY_LIMITED;
    }

    // Absolute current & velocity integrator limiting
    if (our_clampc(&Iq_setpoint, -config.I_limit, config.I_limit) == true)
    {
        state.vel_integrator_Iq *= 0.995f;
        state.warnings |= CONTROLLER_WARNINGS_CURRENT_LIMITED;
    }

    // Flux braking
    const float Vbus_voltage = system_get_Vbus();
    if (config.max_Ibrake > 0)
    {
        state.Id_setpoint = our_clamp(-state.Ibus_est*Vbus_voltage, 0, config.max_Ibrake);
    }
    else
    {
        state.Id_setpoint = 0.0f;
    }

    const float e_phase = observer_get_epos();
    const float c_I = fast_cos(e_phase);
    const float s_I = fast_sin(e_phase);

    float Vd;
    float Vq;
    if (motor_get_is_gimbal() == true)
    {
        const float e_phase_vel = observer_get_evel();
        Vd = -e_phase_vel * motor_get_phase_inductance() * Iq_setpoint;
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

        state.Id_est += config.I_k * (Id - state.Id_est);
        state.Iq_est += config.I_k * (Iq - state.Iq_est);

        const float delta_Id = state.Id_setpoint - state.Id_est;
        const float delta_Iq = Iq_setpoint - state.Iq_est;

        state.Id_integrator_Vd += delta_Id * PWM_PERIOD_S * config.Id_integrator_gain;
        state.Iq_integrator_Vq += delta_Iq * PWM_PERIOD_S * config.Iq_integrator_gain;

        Vd = (delta_Id * config.I_gain) + state.Id_integrator_Vd;
        Vq = (delta_Iq * config.I_gain) + state.Iq_integrator_Vq;
    }
    state.Vq_setpoint = Vq;
    
    float mod_q = Vq / Vbus_voltage;
    float mod_d = Vd / Vbus_voltage;
    state.Ibus_est = state.Iq_est * mod_q + state.Id_est * mod_d;
    state.power_est = state.Ibus_est * Vbus_voltage;

    // dq modulation limiter
    const float dq_mod_scale_factor = PWM_LIMIT * fast_inv_sqrt((mod_q * mod_q) + (mod_d * mod_d));

    if (dq_mod_scale_factor < 1.0f)
    {
        mod_q *= dq_mod_scale_factor;
        mod_d *= dq_mod_scale_factor;
        state.Id_integrator_Vd *= I_INTEGRATOR_DECAY_FACTOR;
        state.Iq_integrator_Vq *= I_INTEGRATOR_DECAY_FACTOR;
        state.warnings |= CONTROLLER_WARNINGS_MODULATION_LIMITED;
    }

    // Inverse Park transform
    float mod_a = (c_I * mod_d) - (s_I * mod_q);
    float mod_b = (c_I * mod_q) + (s_I * mod_d);

    SVM(mod_a, mod_b, &state.modulation_values.A,
        &state.modulation_values.B, &state.modulation_values.C);
    gate_driver_set_duty_cycle(&state.modulation_values);
}


PAC5XXX_RAMFUNC ControlState controller_get_state(void)
{
    return state.state;
}

PAC5XXX_RAMFUNC void controller_set_state(ControlState new_state)
{
    if ((new_state != state.state) && (false == state.is_calibrating))
    {
        if ((new_state == STATE_CL_CONTROL) && (state.state == STATE_IDLE) && (!errors_exist()) && motor_get_calibrated())
        {
            state.pos_setpoint = observer_get_pos_estimate();
            gate_driver_enable();
            state.state = STATE_CL_CONTROL;
        }
        else if ((new_state == STATE_CALIBRATE) && (state.state == STATE_IDLE) && (!errors_exist()))
        {
            gate_driver_enable();
            state.state = STATE_CALIBRATE;
        }
        else // state != STATE_IDLE --> Got to idle state anyway
        {
            gate_driver_set_duty_cycle(&zeroDC);
            gate_driver_disable();
            state.state = STATE_IDLE;
        }
    }
}

PAC5XXX_RAMFUNC ControlMode controller_get_mode(void)
{
    return state.mode;
}

PAC5XXX_RAMFUNC void controller_set_mode(ControlMode new_mode)
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
    return state.Iq_est;
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
    return state.Iq_est * motor_get_user_direction();
}

PAC5XXX_RAMFUNC float controller_get_Iq_setpoint_user_frame(void)
{
    return state.Iq_setpoint * motor_get_user_direction();
}

PAC5XXX_RAMFUNC void controller_set_Iq_setpoint_user_frame(float value)
{
    state.Iq_setpoint = value * motor_get_user_direction();
}

PAC5XXX_RAMFUNC float controller_get_Id_setpoint_user_frame(void)
{
    return state.Id_setpoint;
}

PAC5XXX_RAMFUNC float controller_get_Vq_setpoint_user_frame(void)
{
    return state.Vq_setpoint * motor_get_user_direction();
}

PAC5XXX_RAMFUNC float controller_set_pos_vel_setpoints(float pos_setpoint, float vel_setpoint)
{
    controller_set_pos_setpoint_user_frame(pos_setpoint);
    controller_set_vel_setpoint_user_frame(vel_setpoint);
    return observer_get_pos_estimate_user_frame();
}

void controller_get_modulation_values(struct FloatTriplet *dc)
{
    dc->A = state.modulation_values.A;
    dc->B = state.modulation_values.B;
    dc->C = state.modulation_values.C;
}

float controller_get_pos_gain(void)
{
    return config.pos_gain;
}

void controller_set_pos_gain(float gain)
{
    if (gain >= 0.0f)
    {
        config.pos_gain = gain;
    }
}

float controller_get_vel_gain(void)
{
    return config.vel_gain;
}

void controller_set_vel_gain(float gain)
{
    if (gain >= 0.0f)
    {
        config.vel_gain = gain;
    }
}

float controller_get_vel_integrator_gain(void)
{
    return config.vel_integrator_gain;
}

void controller_set_vel_integrator_gain(float gain)
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

float controller_get_Iq_gain(void)
{
    return config.I_gain;
}

float controller_get_I_bw(void)
{
    return config.I_bw;
}

void controller_set_I_bw(float bw)
{
    if (bw > 0.0f)
    {
        config.I_bw = bw;
        controller_update_I_gains();
    }
}

float controller_get_Ibus_est(void)
{
    return state.Ibus_est;
}

float controller_get_power_est(void)
{
    return state.power_est;
}

float controller_get_vel_limit(void)
{
    return config.vel_limit;
}

void controller_set_vel_limit(float limit)
{
    if ((limit > 0.0f) && (config.vel_limit < VEL_HARD_LIMIT))
    {
        config.vel_limit = limit;
    }
}

float controller_get_Iq_limit(void)
{
    return config.I_limit;
}

void controller_set_Iq_limit(float limit)
{
    if ((limit > 0.0f) && (limit < I_HARD_LIMIT))
    {
        config.I_limit = limit;
    }
}

float controller_get_vel_increment(void)
{
    return config.vel_increment;
}

void controller_set_vel_increment(float increment)
{
    if (increment >= 0.0f)
    {
        config.vel_increment = increment;
    }
}

float controller_get_max_Ibus_regen(void)
{
    return config.max_Ibus_regen;
}

void controller_set_max_Ibus_regen(float value)
{
    if (value >= 0.0f)
    {
        config.max_Ibus_regen = value;
    }
}

float controller_get_max_Ibrake(void)
{
    return config.max_Ibrake;
}

void controller_set_max_Ibrake(float value)
{
    if (value >= 0.0f)
    {
        config.max_Ibrake = value;
    }
}

void controller_set_motion_plan(MotionPlan mp)
{
    motion_plan = mp;
    state.t_plan = 0.0f;
}

ControllerConfig *Controller_GetConfig(void)
{
    return &config;
}

void Controller_RestoreConfig(ControllerConfig *config_)
{
    config = *config_;
}

static inline bool Controller_LimitVelocity(float min_limit, float max_limit, float vel_estimate,
                                            float vel_gain, float *I)
{
    float Imax = (max_limit - vel_estimate) * vel_gain;
    float Imin = (min_limit - vel_estimate) * vel_gain;
    return our_clampc(I, Imin, Imax);
}

PAC5XXX_RAMFUNC void controller_update_I_gains(void)
{
    config.I_gain = config.I_bw * motor_get_phase_inductance();
    float plant_pole = motor_get_phase_resistance() / motor_get_phase_inductance();
    config.Iq_integrator_gain = plant_pole * config.I_gain;
    config.Id_integrator_gain = config.Iq_integrator_gain;
}

PAC5XXX_RAMFUNC uint8_t controller_get_warnings(void)
{
    return state.warnings;
}

PAC5XXX_RAMFUNC uint8_t controller_get_errors(void)
{
    return state.errors;
}
