
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

#include <src/adc/adc.h>
#include <src/motor/motor.h>
#include <src/encoder/hall.h>
#include <src/encoder/ma7xx.h>
#include <src/encoder/encoder.h>
#include <src/observer/observer.h>
#include <src/controller/controller.h>
#include <src/controller/trajectory_planner.h>
#include <src/can/can.h>

#define SETTINGS_PAGE (120)
#define SETTINGS_PAGE_HEX (0x0001E000)

struct NVMStruct {
    uint8_t node_id_1;
    uint8_t node_id_2;
    ADCConfig adc_config;
    MotorConfig motor_config;
    HallConfig hall_config;
    MA7xxConfig ma7xx_config;
    EncoderConfig encoder_config;
    ObserverConfig observer_config;
    ControllerConfig controller_config;
    CANConfig can_config;
    TrajPlannerConfig traj_planner_config;
    char version[16];
};

bool nvm_save_config(void);
bool nvm_load_config(void);
void nvm_erase(void);

