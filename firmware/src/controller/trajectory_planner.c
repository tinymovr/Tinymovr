
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

#include <src/common.h>
#include <src/system/system.h>
#include <src/controller/controller.h>
#include <src/can/can_endpoints.h>
#include <src/utils/utils.h>
#include <src/controller/trajectory_planner.h>

static PlannerConfig config = {
	.max_accel = ENCODER_TICKS_FLOAT,
	.max_decel = ENCODER_TICKS_FLOAT,
	.max_vel = 50000.0f
};

static PlannerState state = {0};

bool planner_move_to_tlimit(float p_target)
{
	bool response = false;
	MotionPlan motion_plan = {0};
	if (!errors_exist() && planner_prepare_plan_tlimit(p_target, config.deltat_tot, config.deltat_acc, config.deltat_dec, &motion_plan))
	{
		controller_set_motion_plan(motion_plan);
		controller_set_mode(CTRL_TRAJECTORY);
		response = true;
	}
	return response;
}

bool planner_move_to_vlimit(float p_target)
{
	bool response = false;
	MotionPlan motion_plan = {0};
	if (!errors_exist() && planner_prepare_plan_vlimit(p_target, config.max_vel, config.max_accel, config.max_decel, &motion_plan))
	{
		controller_set_motion_plan(motion_plan);
		controller_set_mode(CTRL_TRAJECTORY);
		response = true;
	}
	return response;
}

bool planner_prepare_plan_tlimit(float p_target, float deltat_tot, float deltat_acc, float deltat_dec, MotionPlan *plan)
{
	float p_0 = controller_get_pos_setpoint_user_frame();
	float S = p_target - p_0;
	float v_0 = controller_get_vel_setpoint_user_frame();
	float deltat_cruise = deltat_tot - deltat_acc - deltat_dec;
	float v_cruise = (S - 0.5f * deltat_acc * v_0) / (0.5f * deltat_acc + deltat_cruise + 0.5f * deltat_dec);
	if (deltat_tot < 0 || deltat_acc < 0 || deltat_dec < 0 || deltat_cruise < 0.0f)
	{
		state.errors |= TRAJ_PLANNER_ERRORS_INVALID_INPUT;
		return false;
	}
	else if (our_fabsf(v_cruise) > controller_get_vel_limit())
	{
		state.errors |= TRAJ_PLANNER_ERRORS_VCRUISE_OVER_LIMIT;
		return false;
	}
	else if (S == 0.0f)
	{
		return false;
	}
	float acc = (v_cruise - v_0) / deltat_acc;
	float dec = v_cruise / deltat_dec;
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
	// plan->v_target = 0;
	plan->acc = acc;
	plan->dec = dec;
	plan->p_acc_cruise = plan->p_0 + v_0 * deltat_acc + 0.5f * acc * deltat_acc * deltat_acc;
	plan->p_cruise_dec = plan->p_acc_cruise + v_cruise * deltat_cruise;
	return true;
}

bool planner_prepare_plan_vlimit(float p_target, float v_max, float a_max, float d_max, MotionPlan *plan)
{
	const float p_0 = controller_get_pos_setpoint_user_frame();
	const float S = p_target - p_0;
	const float v_0 = controller_get_vel_setpoint_user_frame();
	const float sign = S >= 0 ? 1.0f : -1.0f;
	if (S == 0.0f)
	{
		return false;
	}
	// Case 1. Distance to v=0 > desired distance. Full stop trajectory.
	if (v_0 * v_0 > our_fabsf(2 * d_max * S))
	{
		const float sign_fs = v_0 >= 0 ? 1.0f : -1.0f;
		const float deltat_dec = sign_fs * v_0 / d_max;
		const float dec = sign_fs * d_max;
		const float p_target_fullstop = p_0 + v_0 * deltat_dec - 0.5f * dec * deltat_dec * deltat_dec;

		plan->p_0 = p_0;
		plan->deltat_dec = deltat_dec;
		plan->t_end = deltat_dec;
		plan->v_0 = v_0;
		plan->v_cruise = v_0;
		// plan->v_target = 0;
		plan->dec = dec;
		plan->p_target = p_target_fullstop;
		plan->p_acc_cruise = p_0;
		plan->p_cruise_dec = p_0;
		return true;
	}
	//	// Case 2. |v_0| > |v_max|. Drop to v_max immediately
	//	// TODO: Here wa actually drop to v_0. We should normally
	//	// drop to v_max and replan.
	//	else if (v_0 > v_max || v_0 < -v_max)
	//	{
	//		plan->p_0 = p_0;
	//		plan->p_target = p_target;
	//		plan->deltat_dec = v_0 > 0? v_0/d_max : -v_0/d_max;
	//		plan->t_end = plan->deltat_dec;
	//		plan->v_0 = v_0;
	//		plan->v_cruise = v_0;
	//		//plan->v_target = 0;
	//		plan->dec = d_max;
	//		plan->p_acc_cruise = p_0;
	//		plan->p_cruise_dec = p_0;
	//		response = true;
	//	}
	// Case 3. Triangular profile
	else if (our_fabsf(S) < (v_max * v_max - v_0 * v_0) / (2 * a_max) + (v_max * v_max) / (2 * d_max))
	{
		const float acc = sign * a_max;
		const float dec = sign * d_max;
		const float v_reached = sign * fast_sqrt((2 * a_max * d_max * our_fabsf(S) + d_max * v_0 * v_0) / (a_max + d_max));
		const float deltat_acc = (v_reached - v_0) / acc;
		const float deltat_dec = v_reached / dec;
		const float t_end = deltat_acc + deltat_dec;
		const float p_acc_cruise = p_0 + v_0 * deltat_acc + 0.5f * acc * deltat_acc * deltat_acc;

		plan->p_0 = p_0;
		plan->p_target = p_target;
		plan->acc = acc;
		plan->dec = dec;
		plan->v_cruise = v_reached;
		plan->deltat_acc = deltat_acc;
		plan->deltat_dec = deltat_dec;
		plan->t_acc_cruise = deltat_acc;
		plan->t_cruise_dec = deltat_acc;
		plan->t_end = t_end;
		plan->v_0 = v_0;
		// plan->v_target = 0;
		plan->p_acc_cruise = p_acc_cruise;
		plan->p_cruise_dec = p_acc_cruise;
		return true;
	}
	// Case 4. Trapezoidal profile
	else
	{
		const float acc = sign * a_max;
		const float dec = sign * d_max;
		const float v_cruise = sign * v_max;
		const float deltat_acc = (v_cruise - v_0) / acc;
		const float deltat_dec = (sign * v_max) / dec;
		const float S_vmax = (v_max * v_max - v_0 * v_0) / (2 * a_max) + (v_max * v_max) / (2 * d_max);
		const float deltat_cruise = (S - sign * S_vmax) / v_cruise;
		const float t_acc_cruise = deltat_acc;
		const float t_cruise_dec = deltat_acc + deltat_cruise;
		const float t_end = t_cruise_dec + deltat_dec;
		const float p_acc_cruise = p_0 + v_0 * deltat_acc + 0.5f * acc * deltat_acc * deltat_acc;
		const float p_cruise_dec = p_acc_cruise + v_cruise * deltat_cruise;

		plan->p_0 = p_0;
		plan->p_target = p_target;
		plan->acc = acc;
		plan->dec = dec;
		plan->v_cruise = v_cruise;
		plan->deltat_acc = deltat_acc;
		plan->deltat_dec = deltat_dec;
		plan->deltat_cruise = deltat_cruise;
		plan->t_acc_cruise = t_acc_cruise;
		plan->t_cruise_dec = t_cruise_dec;
		plan->t_end = t_end;
		plan->v_0 = v_0;
		// plan->v_target = 0;
		plan->p_acc_cruise = p_acc_cruise;
		plan->p_cruise_dec = p_cruise_dec;
		return true;
	}
	return false;
}

bool planner_set_max_accel(float max_accel)
{
	if (max_accel > 0)
	{
		config.max_accel = max_accel;
		return true;
	}
	return false;
}

bool planner_set_max_decel(float max_decel)
{
	if (max_decel > 0)
	{
		config.max_decel = max_decel;
		return true;
	}
	return false;
}

float planner_get_max_accel(void)
{
	return config.max_accel;
}

float planner_get_max_decel(void)
{
	return config.max_decel;
}

bool planner_set_max_vel(float max_vel)
{
	if (max_vel > 0)
	{
		config.max_vel = max_vel;
		return true;
	}
	return false;
}

float planner_get_max_vel(void)
{
	return config.max_vel;
}

float planner_get_deltat_acc(void)
{
	return config.deltat_acc;
}

bool planner_set_deltat_acc(float deltat_acc)
{
	if (deltat_acc > 0)
	{
		config.deltat_acc = deltat_acc;
		return true;
	}
	return false;
}

float planner_get_deltat_tot(void)
{
	return config.deltat_tot;
}

bool planner_set_deltat_tot(float deltat_tot)
{
	if (deltat_tot > 0)
	{
		config.deltat_tot = deltat_tot;
		return true;
	}
	return false;
}

float planner_get_deltat_dec(void)
{
	return config.deltat_dec;
}

bool planner_set_deltat_dec(float deltat_dec)
{
	if (deltat_dec > 0)
	{
		config.deltat_dec = deltat_dec;
		return true;
	}
	return false;
}

TM_RAMFUNC uint8_t planner_get_errors(void)
{
	return state.errors;
}

TM_RAMFUNC bool planner_evaluate(float t, MotionPlan *plan)
{
	// We assume that t is zero at the start of trajectory
	bool response = true;
	if (t < plan->t_acc_cruise)
	{
		controller_set_pos_setpoint_user_frame(plan->p_0 + (plan->v_0 * t) + (0.5f * plan->acc * t * t));
		controller_set_vel_setpoint_user_frame(plan->v_0 + (plan->acc * t));
	}
	else if (t < plan->t_cruise_dec)
	{
		const float tr = (t - plan->t_acc_cruise);
		controller_set_pos_setpoint_user_frame(plan->p_acc_cruise + (plan->v_cruise * tr));
		controller_set_vel_setpoint_user_frame(plan->v_cruise);
	}
	else if (t <= plan->t_end)
	{
		const float tr = (t - plan->t_cruise_dec);
		controller_set_pos_setpoint_user_frame(plan->p_cruise_dec + (plan->v_cruise * tr) - (0.5f * plan->dec * tr * tr));
		controller_set_vel_setpoint_user_frame(plan->v_cruise - (tr * plan->dec));
	}
	else
	{
		response = false;
	}
	return response;
}
