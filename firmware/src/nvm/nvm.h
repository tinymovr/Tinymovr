
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

#pragma once

#include <src/xfs.h>
#include <src/adc/adc.h>
#include <src/motor/motor.h>
#include <src/sensor/sensors.h>
#include <src/observer/observer.h>
#include <src/controller/controller.h>
#include <src/controller/trajectory_planner.h>
#include <src/can/can.h>

struct NVMStruct {
    uint8_t node_id_1;
    uint8_t node_id_2;
    FramesConfig frames_config;
    ADCConfig adc_config;
    MotorConfig motor_config;
    SensorsConfig sensors_config;
    ObserversConfig observers_config;
    ControllerConfig controller_config;
    CANConfig can_config;
    TrajPlannerConfig traj_planner_config;
    char version[16];
};

#define SETTINGS_PAGE (120)
#define SETTINGS_PAGE_HEX (0x0001E000)
#define NVM_PAGE_SIZE (1024)
#define SETTINGS_PAGE_COUNT (DIVIDE_AND_ROUND_UP(sizeof(struct NVMStruct), NVM_PAGE_SIZE))

extern const uint32_t config_size;

bool nvm_save_config(void);
bool nvm_load_config(void);
void nvm_erase(void);

