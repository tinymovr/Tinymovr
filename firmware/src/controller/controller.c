
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

#include "src/system/system.h"
#include <src/sensor/sensor.h>
#include <src/observer/observer.h>
#include "src/adc/adc.h"
#include "src/motor/motor.h"
#include <src/gatedriver/gatedriver.h>
#include <src/utils/utils.h>
#include <src/scheduler/scheduler.h>
#include <src/can/can_endpoints.h>
#include <src/controller/controller.h>
#include "src/watchdog/watchdog.h"

void CLControlStep(void);
static inline bool Controller_LimitVelocity(float min_limit, float max_limit, float vel_estimate,
                                                            float vel_gain, float *I);

static MotionPlan motion_plan;
static ControllerState state = {

    .state = CONTROLLER_STATE_IDLE,
    .mode = CONTROLLER_MODE_CURRENT,
    .errors = CONTROLLER_ERRORS_NONE,
    .is_calibrating = false,

    .I_phase_meas = {0.0f, 0.0f, 0.0f},
    .modulation_values = {0.0f, 0.0f, 0.0f},

    .Iq_estimate = 0.0f,
    .Id_estimate = 0.0f,

    .Ibus_est = 0.0f,
    .power_est = 0.0f,

    .pos_setpoint = 0.0f,
    .vel_setpoint = 0.0f,
    .vel_ramp_setpoint  = 0.0f,
    .Iq_setpoint = 0.0f,
    .Id_setpoint = 0.0f,

    .Vq_setpoint = 0.0f,

    .vel_integrator = 0.0f,

    .Iq_integrator = 0.0f,
    .Id_integrator = 0.0f,

    .t_plan = 0.0f};

#if defined BOARD_REV_R32 || BOARD_REV_R33 || defined BOARD_REV_R5

static ControllerConfig config = {
    .vel_limit = 300000.0f,
    .I_limit = 10.0f,
    .pos_gain = 20.0f,
    .vel_gain = 8.0e-5f,
    .vel_integral_gain = 0.00020f,
    .vel_integral_deadband = 200.0f,
    .I_bw = 2000.0,
    .I_gain = 0.0f,
    .Iq_integral_gain = 0.0f,
    .Id_integral_gain = 0.0f,
    .I_k = 0.3f,
    .vel_increment = 100.0f, // ticks/cycle
    .max_Ibus_regen = 0.0f,
    .max_Ibrake = 0.0f}; 

#elif defined BOARD_REV_M5

static ControllerConfig config = {
    .vel_limit = 100000.0f,
    .I_limit = 4.0f,
    .pos_gain = 8.0f,
    .vel_gain = 5.0e-5f,
    .vel_integral_gain = 0.00020f,
    .vel_integral_deadband = 200.0f,
    .I_bw = 2000.0,
    .I_gain = 0.0f,
    .Iq_integral_gain = 0.0f,
    .Id_integral_gain = 0.0f,
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
        if (errors_exist() && (state.state != CONTROLLER_STATE_IDLE))
        {
            controller_set_state(CONTROLLER_STATE_IDLE);
        }

        if (state.state == CONTROLLER_STATE_CALIBRATE)
        {
            state.is_calibrating = true;
            system_reset_calibration();
            (void)(ADC_calibrate_offset() && motor_calibrate_resistance() && motor_calibrate_inductance());
            (void)(sensors_calibrate());
            state.is_calibrating = false;
            controller_set_state(CONTROLLER_STATE_IDLE); 
        }
        else if (state.state == CONTROLLER_STATE_CL_CONTROL)
        {
            // Check the watchdog and revert to idle if it has timed out
            if (Watchdog_triggered())
            {
                controller_set_state(CONTROLLER_STATE_IDLE);
                Watchdog_reset();
            }
            else
            {
                CLControlStep();
            }
        }
        wait_for_control_loop_interrupt();
    }
}

TM_RAMFUNC void CLControlStep(void)
{
    switch (state.mode)
    {
        case CONTROLLER_MODE_TRAJECTORY:
        state.t_plan += PWM_PERIOD_S;
        // This will set state.pos_setpoint state.vel_setpoint (in user frame)
        if (!traj_planner_evaluate(state.t_plan, &motion_plan))
        {
            // Drop to position mode on error or completion
            controller_set_mode(CONTROLLER_MODE_POSITION);
            state.t_plan = 0;
        }
        break;
        case CONTROLLER_MODE_HOMING:
        // This will set state.pos_setpoint state.vel_setpoint (in user frame)
        if (!homing_planner_evaluate())
        {
            // Drop to position mode on error or completion
            controller_set_mode(CONTROLLER_MODE_POSITION);
        }
        break;
        default: break;
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
    float vel_setpoint_integral = state.vel_ramp_setpoint ;

    if (state.mode >= CONTROLLER_MODE_POSITION)
    {
        const float delta_pos = get_diff_position_sensor_frame(state.pos_setpoint);
        const float delta_pos_integral = sgnf(delta_pos) * our_fmaxf(0, fabsf(delta_pos) - config.vel_integral_deadband);
        vel_setpoint += delta_pos * config.pos_gain;
        vel_setpoint_integral += delta_pos_integral * config.pos_gain;
    }

    const float vel_estimate = observer_get_vel_estimate(&position_observer);
    float Iq_setpoint = state.Iq_setpoint;

    if (state.mode >= CONTROLLER_MODE_VELOCITY) 
    {
        const float delta_vel = vel_setpoint - vel_estimate;
        // Velocity limiting will be done later on based on the estimate
        Iq_setpoint += apply_velocity_transform(delta_vel * config.vel_gain + state.vel_integrator, &position_sensor_to_motor);
        state.vel_integrator += (vel_setpoint_integral - vel_estimate) * PWM_PERIOD_S * config.vel_integral_gain;
    }
    else
    {
        state.vel_integrator = 0.0f;
    }

    // Velocity-dependent current limiting
    if (Controller_LimitVelocity(-config.vel_limit, config.vel_limit, vel_estimate, config.vel_gain, &Iq_setpoint) == true)
    {
        state.vel_integrator *= 0.995f;
        state.warnings |= CONTROLLER_WARNINGS_VELOCITY_LIMITED;
    }

    // Absolute current & velocity integrator limiting
    if (our_clampc(&Iq_setpoint, -config.I_limit, config.I_limit) == true)
    {
        state.vel_integrator *= 0.995f;
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

    const float e_phase = observer_get_epos_motor_frame();
    const float c_I = fast_cos(e_phase);
    const float s_I = fast_sin(e_phase);

    float Vd;
    float Vq;
    if (motor_get_is_gimbal() == true)
    {
        const float e_phase_vel = observer_get_evel_motor_frame();
        Vd = -e_phase_vel * motor_get_phase_inductance() * Iq_setpoint;
        Vq = motor_get_phase_resistance() * Iq_setpoint;
    }
    else
    {
        ADC_get_phase_currents(&(state.I_phase_meas));

        // Clarke transform
        const float Ialpha = state.I_phase_meas.A;
        const float Ibeta = one_by_sqrt3 * (state.I_phase_meas.B - state.I_phase_meas.C);

        // Park transform
        const float Id = (c_I * Ialpha) + (s_I * Ibeta);
        const float Iq = (c_I * Ibeta) - (s_I * Ialpha);

        state.Id_estimate += config.I_k * (Id - state.Id_estimate);
        state.Iq_estimate += config.I_k * (Iq - state.Iq_estimate);

        const float delta_Id = state.Id_setpoint - state.Id_estimate;
        const float delta_Iq = Iq_setpoint - state.Iq_estimate;

        state.Id_integrator += delta_Id * PWM_PERIOD_S * config.Id_integral_gain;
        state.Iq_integrator += delta_Iq * PWM_PERIOD_S * config.Iq_integral_gain;

        Vd = (delta_Id * config.I_gain) + state.Id_integrator;
        Vq = (delta_Iq * config.I_gain) + state.Iq_integrator;
    }
    state.Vq_setpoint = Vq;
    
    float mod_q = Vq / Vbus_voltage;
    float mod_d = Vd / Vbus_voltage;
    state.Ibus_est = state.Iq_estimate * mod_q + state.Id_estimate * mod_d;
    state.power_est = state.Ibus_est * Vbus_voltage;

    // dq modulation limiter
    const float dq_mod_scale_factor = PWM_LIMIT * fast_inv_sqrt((mod_q * mod_q) + (mod_d * mod_d));

    if (dq_mod_scale_factor < 1.0f)
    {
        mod_q *= dq_mod_scale_factor;
        mod_d *= dq_mod_scale_factor;
        state.Id_integrator *= I_INTEGRATOR_DECAY_FACTOR;
        state.Iq_integrator *= I_INTEGRATOR_DECAY_FACTOR;
        state.warnings |= CONTROLLER_WARNINGS_MODULATION_LIMITED;
    }

    // Inverse Park transform
    float mod_a = (c_I * mod_d) - (s_I * mod_q);
    float mod_b = (c_I * mod_q) + (s_I * mod_d);

    SVM(mod_a, mod_b, &state.modulation_values.A,
        &state.modulation_values.B, &state.modulation_values.C);
    gate_driver_set_duty_cycle(&state.modulation_values);
}


TM_RAMFUNC controller_state_options controller_get_state(void)
{
    return state.state;
}

TM_RAMFUNC void controller_set_state(controller_state_options new_state)
{
    if ((new_state != state.state) && (false == state.is_calibrating))
    {
        if ((new_state == CONTROLLER_STATE_CL_CONTROL) && (state.state == CONTROLLER_STATE_IDLE) && (!errors_exist()) && motor_get_calibrated())
        {
            state.pos_setpoint = observer_get_pos_estimate(&position_observer);
            gate_driver_enable();
            state.state = CONTROLLER_STATE_CL_CONTROL;
        }
        else if ((new_state == CONTROLLER_STATE_CALIBRATE) && (state.state == CONTROLLER_STATE_IDLE) && (!errors_exist()))
        {
            gate_driver_enable();
            state.state = CONTROLLER_STATE_CALIBRATE;
        }
        else // state != CONTROLLER_STATE_IDLE --> Got to idle state anyway
        {
            gate_driver_set_duty_cycle(&three_phase_zero);
            gate_driver_disable();
            state.state = CONTROLLER_STATE_IDLE;
        }
    }
}

TM_RAMFUNC controller_mode_options controller_get_mode(void)
{
    return state.mode;
}

TM_RAMFUNC void controller_set_mode(controller_mode_options new_mode)
{
    if (new_mode != state.mode)
    {
        switch (new_mode)
        {
            case CONTROLLER_MODE_HOMING:
            state.mode = CONTROLLER_MODE_HOMING;
            break;

            case CONTROLLER_MODE_TRAJECTORY:
            state.mode = CONTROLLER_MODE_TRAJECTORY;
            break;

            case CONTROLLER_MODE_POSITION:
            state.mode = CONTROLLER_MODE_POSITION;
            break;

            case CONTROLLER_MODE_VELOCITY:
            state.mode = CONTROLLER_MODE_VELOCITY;
            break;

            case CONTROLLER_MODE_CURRENT:
            state.mode = CONTROLLER_MODE_CURRENT;
            break;

            default:
            break;
        }
    }
}

TM_RAMFUNC float controller_get_Iq_estimate_user_frame(void)
{
    return apply_velocity_transform(state.Iq_estimate, &motor_to_user);
}

TM_RAMFUNC float controller_get_pos_setpoint_user_frame(void)
{
    return apply_transform(state.pos_setpoint, &position_sensor_to_user);
}

TM_RAMFUNC float controller_get_vel_setpoint_user_frame(void)
{
    return apply_velocity_transform(state.vel_setpoint, &position_sensor_to_user);
}

TM_RAMFUNC float controller_get_Iq_estimate(void)
{
    return state.Iq_estimate;
}
TM_RAMFUNC float controller_get_Iq_setpoint_user_frame(void)
{
    return apply_velocity_transform(state.Iq_setpoint, &motor_to_user);
}

TM_RAMFUNC float controller_get_Id_setpoint_user_frame(void)
{
    return apply_velocity_transform(state.Id_setpoint, &motor_to_user);
}

TM_RAMFUNC void controller_set_pos_setpoint_user_frame(float value)
{
    state.pos_setpoint = apply_transform(value, &user_to_position_sensor);
}

TM_RAMFUNC void controller_set_vel_setpoint_user_frame(float value)
{
    state.vel_setpoint = apply_velocity_transform(value, &user_to_position_sensor);
}

TM_RAMFUNC void controller_set_Iq_setpoint_user_frame(float value)
{
    state.Iq_setpoint = apply_velocity_transform(value, &user_to_motor);
}

float controller_set_pos_vel_setpoints_user_frame(float pos_setpoint, float vel_setpoint)
{
    controller_set_pos_setpoint_user_frame(pos_setpoint);
    controller_set_vel_setpoint_user_frame(vel_setpoint);
    return user_frame_get_pos_estimate();
}

TM_RAMFUNC float controller_get_Vq_setpoint_user_frame(void)
{
    return apply_velocity_transform(state.Vq_setpoint, &motor_to_user);
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

float controller_get_vel_integral_gain(void)
{
    return config.vel_integral_gain;
}

void controller_set_vel_integral_gain(float gain)
{
    if (gain >= 0.0f)
    {
        config.vel_integral_gain = gain;
    }
}

float controller_get_vel_integral_deadband(void)
{
    return config.vel_integral_deadband;
}

void controller_set_vel_integral_deadband(float value)
{
    if (value >= 0.0f)
    {
        config.vel_integral_deadband = value;
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

ControllerConfig *controller_get_config(void)
{
    return &config;
}

void controller_restore_config(ControllerConfig *config_)
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

TM_RAMFUNC void controller_update_I_gains(void)
{
    config.I_gain = config.I_bw * motor_get_phase_inductance();
    float plant_pole = motor_get_phase_resistance() / motor_get_phase_inductance();
    config.Iq_integral_gain = plant_pole * config.I_gain;
    config.Id_integral_gain = config.Iq_integral_gain;
}

TM_RAMFUNC uint8_t controller_get_warnings(void)
{
    return state.warnings;
}

TM_RAMFUNC uint8_t controller_get_errors(void)
{
    return state.errors;
}
