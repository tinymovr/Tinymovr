
#include <src/system/system.h>
#include <src/controller/controller.h>
#include <src/utils/utils.h>
#include <src/controller/trajectory_planner.h>

static MotionPlan motion_plan = {0};

bool planner_move_to(float p_target, float deltat_tot, float deltat_acc, float deltat_dec)
{
	bool response = false;
    if (!error_flags_exist() && planner_prepare_plan(p_target, deltat_tot, deltat_acc, deltat_dec, &motion_plan))
    {
        controller_set_motion_plan(&motion_plan);
        Controller_SetMode(CTRL_TRAJECTORY);
        response = true;
    }
    return response;
}

bool planner_prepare_plan(float p_target, float deltat_tot, float deltat_acc, float deltat_dec, MotionPlan *plan)
{
    bool response = false;
    float p_0 = Controller_GetPosSetpoint();
    float S = p_target - p_0;
    float v_0 = Controller_GetVelSetpoint();
    float deltat_cruise = deltat_tot - deltat_acc - deltat_dec;
    float v_cruise = (S - 0.5f * deltat_acc * v_0) / (0.5f * deltat_acc + deltat_cruise + 0.5f * deltat_dec);
    if (deltat_tot < 0 || deltat_acc < 0 || deltat_dec < 0 || deltat_cruise < 0.0f)
    {
    	add_error_flag(ERROR_PLANNER_INVALID_INPUT);
    }
    else if (fabsf(v_cruise) > Controller_GetVelLimit())
    {
    	add_error_flag(ERROR_PLANNER_VCRUISE_OVER_LIMIT);
    }
    else if (S != 0.0f)
	{
		response = true;
	}
    if (response == true)
    {
		float acc = (v_cruise - v_0) / deltat_acc;
		float dec = - v_cruise / deltat_dec;
		// Assign everything
		plan->p_0 = p_0;
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
		plan->p_acc_cruise = plan->p_0 + v_0 * deltat_acc + 0.5f * acc * deltat_acc * deltat_acc;
		plan->p_cruise_dec = plan->p_acc_cruise + v_cruise * deltat_cruise;
    }
    return response;
}

PAC5XXX_RAMFUNC bool planner_evaluate(float t, MotionPlan *plan, float *pos, float *vel)
{
    // We assume that t is zero at the start of trajectory
    bool response = true;
    if (t <= plan->t_acc_cruise)
    {
        *pos = plan->p_0 + (plan->v_0 * t) + (0.5f * plan->acc * t * t);
        *vel = plan->v_0 + (plan->acc * t);
    }
    else if (t <= plan->t_cruise_dec)
    {
        const float tr = (t - plan->t_acc_cruise);
        *pos = plan->p_acc_cruise + (plan->v_cruise * tr);
        *vel = plan->v_cruise;
    }
    else if (t <= plan->t_end)
    {
        const float tr = (t - plan->t_cruise_dec);
        *pos = plan->p_cruise_dec + (plan->v_cruise * tr) + (0.5f * plan->dec * tr * tr);
        *vel = plan->v_cruise + (tr * plan->dec);
    }
    else
    {
        response = false;
    }
    return response;
}
