
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

#ifndef CONTROLLER_TRAJECTORY_PLANNER_H_
#define CONTROLLER_TRAJECTORY_PLANNER_H_

#include "src/common.h"

struct PlannerConfig {
	float max_accel;
	float max_decel;
	float max_vel;
    float deltat_acc;
    float deltat_tot;
    float deltat_dec;
};

typedef struct
{
    // NOTE: The members of this struct are redundant,
    // i.e. not all are necessary to fully define a
    // trajectory. However this definition reduces the
    // computation during trajectory evaluation.
	float p_0;
    float p_target;
    float deltat_acc;
    float t_acc_cruise;
    float deltat_cruise;
    float t_cruise_dec;
    float deltat_dec;
    float t_end;
    float v_0;
    float v_cruise;
    float v_target;
    float acc;
    float dec;
    float p_acc_cruise;
    float p_cruise_dec;
} MotionPlan;

bool planner_move_to_tlimit(float p_target);
bool planner_move_to_vlimit(float p_targetl);
bool planner_prepare_plan_tlimit(float p_target, float deltat_tot, float deltat_acc, float deltat_dec, MotionPlan *plan);
bool planner_prepare_plan_vlimit(float p_target, float v_max, float a_max, float d_max, MotionPlan *plan);
bool planner_set_max_accel(float max_accel);
bool planner_set_max_decel(float max_decel);
float planner_get_max_accel(void);
float planner_get_max_decel(void);
bool planner_set_max_vel(float max_vel);
float planner_get_max_vel(void);
float planner_get_deltat_acc(void);
bool planner_set_deltat_acc(float deltat_acc);
float planner_get_deltat_tot(void);
bool planner_set_deltat_tot(float deltat_tot);
float planner_get_deltat_dec(void);
bool planner_set_deltat_dec(float deltat_dec);
PAC5XXX_RAMFUNC bool planner_evaluate(float t, MotionPlan *plan);

#endif /* CONTROLLER_TRAJECTORY_PLANNER_H_ */
