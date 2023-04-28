
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

#pragma once

#include <src/common.h>

typedef struct {
    float homing_velocity;
    float max_homing_t;
    float max_stay_vel;
    float max_stay_dpos;
    float max_stay_t;
    float retract_distance;

} HomingPlannerConfig;

typedef struct {
    float stay_t_meas;
} HomingPlannerState;

bool homing_planner_start_homing(void);
bool homing_planner_evaluate(void);

float homing_planner_get_homing_velocity(void);
float homing_planner_get_max_homing_time(void);
float homing_planner_get_max_stay_vel(void);
float homing_planner_get_max_stay_dpos(void);
float homing_planner_get_max_stay_t(void);
float homing_planner_get_retract_distance(void);

void homing_planner_set_homing_velocity(float vel);
void homing_planner_set_max_homing_time(float t);
void homing_planner_set_max_stay_vel(float vel);
void homing_planner_set_max_stay_dpos(float dpos);
void homing_planner_set_max_stay_t(float t);
void homing_planner_set_retract_distance(float dist);