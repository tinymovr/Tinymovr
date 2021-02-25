
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

bool planner_move_to(float p_target, float deltat_tot, float deltat_acc, float deltat_dec);
bool planner_prepare_plan(float p_target, float deltat_tot, float deltat_acc, float deltat_dec, MotionPlan *plan);
bool planner_evaluate(float t, MotionPlan *plan, float *pos, float *vel);

#endif /* CONTROLLER_TRAJECTORY_PLANNER_H_ */
