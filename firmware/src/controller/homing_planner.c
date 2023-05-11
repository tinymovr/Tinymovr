
#include <src/system/system.h>
#include <src/motor/motor.h>
#include <src/observer/observer.h>
#include <src/controller/controller.h>
#include <src/utils/utils.h>
#include <homing_planner.h>
#include <src/can/can_endpoints.h>

static HomingPlannerConfig config = {
    .homing_velocity = -8192.0f, // ticks/s
    .max_homing_t = 20.0f, // s
    .max_stay_vel = 4000.0f, // ticks/s
    .max_stay_dpos = 1000.0f, // ticks
    .max_stay_t = 1.0f, // s
    .retract_distance = 1000.0f // ticks
};

static HomingPlannerState state = {0};

bool homing_planner_home(void)
{
    if (!errors_exist() && controller_get_mode() != CTRL_HOMING)
    {
        state.stay_t_current = 0;
        controller_set_mode(CTRL_HOMING);
        state.warnings = HOMING_WARNINGS_NONE;
        return true;
    }
    return false;
}

TM_RAMFUNC bool homing_planner_evaluate()
{
    const float current_pos_setpoint = controller_get_pos_setpoint_user_frame();
    if (state.stay_t_current >= config.max_stay_t)
    {
        if (fabsf(current_pos_setpoint) > fabsf(config.retract_distance))
        {
            return false;
        }
        const float next_pos_setpoint = current_pos_setpoint - config.homing_velocity * PWM_PERIOD_S;
        controller_set_pos_setpoint_user_frame(next_pos_setpoint);
        controller_set_vel_setpoint_user_frame(-config.homing_velocity);
    }
    else if (state.home_t_current < config.max_homing_t)
    {
        const float next_pos_setpoint = current_pos_setpoint + config.homing_velocity * PWM_PERIOD_S;
        controller_set_pos_setpoint_user_frame(next_pos_setpoint);
        controller_set_vel_setpoint_user_frame(config.homing_velocity);

        if (fabsf(observer_get_vel_estimate_user_frame()) < config.max_stay_vel && fabsf(current_pos_setpoint - observer_get_pos_estimate_user_frame()) > config.max_stay_dpos)
        {
            state.stay_t_current += PWM_PERIOD_S;
            if (state.stay_t_current >= config.max_stay_t)
            {
                // First time the endstop is considered found, reset origins and setpoints
                motor_set_user_offset(0);
                motor_set_user_offset(observer_get_pos_estimate_user_frame());
                controller_set_pos_setpoint_user_frame(0);
                controller_set_vel_setpoint_user_frame(0);
            }
        }
        else
        {
            state.stay_t_current = 0;
            state.home_t_current += PWM_PERIOD_S;
        }
    }
    else
    {
        state.warnings |= HOMING_WARNINGS_HOMING_TIMEOUT;
        return false;
    }
    return true;
}

uint8_t homing_planner_get_warnings(void)
{
    return state.warnings;
}

float homing_planner_get_homing_velocity(void)
{
    return config.homing_velocity;
}

float homing_planner_get_max_homing_t(void)
{
    return config.max_homing_t;
}

float homing_planner_get_max_stall_vel(void)
{
    return config.max_stay_vel;
}

float homing_planner_get_max_stall_delta_pos(void)
{
    return config.max_stay_dpos;
}

float homing_planner_get_max_stall_t(void)
{
    return config.max_stay_t;
}

float homing_planner_get_retract_distance(void)
{
    return config.retract_distance;
}

void homing_planner_set_homing_velocity(float vel)
{
    if (vel != 0)
    {
        config.homing_velocity = vel;
    }
}

void homing_planner_set_max_homing_t(float t)
{
    if (t > 0)
    {
        config.max_homing_t = t;
    }
}

void homing_planner_set_max_stall_vel(float vel)
{
    if (vel >= 0)
    {
        config.max_stay_vel = vel;
    }
}

void homing_planner_set_max_stall_delta_pos(float dpos)
{
    if (dpos >= 0)
    {
        config.max_stay_dpos = dpos;
    }
}

void homing_planner_set_max_stall_t(float t)
{
    if (t > 0)
    {
        config.max_stay_t = t;
    }
}

void homing_planner_set_retract_distance(float dist)
{
    if (dist > 0)
    {
        config.retract_distance = dist;
    }
}