
#include <src/controller/controller.h>
#include <src/controller/trajectory_planner.h>

MotionPlan plan;

bool planner_move_to(float p_target, float deltat_tot, float deltat_acc, float deltat_dec)
{
    if (!error_flags_exist() && planner_prepare_plan(p_target, deltat_tot, deltat_acc, deltat_dec &plan))
    {
        controller_set_motion_plan(&plan);
        Controller_SetMode(CTRL_TRAJECTORY);
    }
}

bool planner_prepare_plan(float p_target, float deltat_tot, float deltat_acc, float deltat_dec, MotionPlan *plan)
{
    bool response = false;
    // TODO: Check inputs for validity!
    float S = p_target - Controller_GetPosSetpoint();
    float v_0 = Controller_GetVelSetpoint();
    if (S != 0.0f)
    {
        float deltat_cruise = deltat_tot - deltat_acc - deltat_dec;
        if (deltat_cruise >= 0.0f)
        {
            float v_cruise = (S - 0.5f * t_acc * v_0) / (deltat_acc + deltat_cruise + 0.5f * deltat_dec);
            float acc = (v_cruise - v_0) / deltat_acc;
            float dec = v_cruise / deltat_dec;
            // Assign everything
            plan->p_target = p_target;
            plan->deltat_acc = deltat_acc;
            plan->t_acc_cruise = deltat_acc;
            plan->deltat_cruise = deltat_cruise;
            plan->t_cruise_dec = deltat_acc + deltat_cruise;
            plan->deltat_dec = deltat_dec;
            plan->t_end = deltat_acc + deltat_cruise + deltat_dec;
            plan->v_0 = v_0;
            plan->v_cruise = v_cruise;
            plan->v_target = 0;
            plan->acc = acc;
            plan->dec = dec;
            plan->p_acc_cruise = v_0 * deltat_acc + 0.5f * acc * deltat_acc * deltat_acc;
            plan->p_cruise_dec = plan->p_acc_cruise + v_cruise * deltat_cruise;
            response = true;
        }
    }
    return response;
}

bool planner_evaluate(float t, MotionPlan *plan, float *pos, float *vel)
{
    // We assume that t is zero at the start of trajectory
    bool response = true;
    if (t <= plan->t_acc_cruise)
    {
        *pos = (plan->v_0 * tr) + (0.5 * plan->acc * tr * tr);
        *vel = v_0 + (plan->acc * tr);
    }
    else if (t <= plan->t_cruise_dec)
    {
        const float tr = (t - t_acc_cruise);
        *pos = plan->p_acc_cruise + (plan->v_cruise * tr);
        *vel = plan->v_cruise;
    }
    else if (t <= plan->t_end)
    {
        const float tr = (t - t_cruise_dec);
        *pos = plan->p_cruise_dec + (plan->v_cruise * tr) + (0.5 * plan->dec * tr * tr);
        *vel = plan->v_cruise + (tr * plan->dec);
    }
    else
    {
        response = false;
    }
    return response;
}