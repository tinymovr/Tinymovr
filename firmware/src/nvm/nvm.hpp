
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

#ifndef NVM_NVM_H_
#define NVM_NVM_H_

#include <src/comms/can/can.hpp>
#include <src/controller/controller.hpp>
#include <src/motor/motor.hpp>
#include <src/observer/observer.hpp>

#define SETTINGS_PAGE (120)
#define SETTINGS_PAGE_HEX (0x0001E000)


struct NVMStruct {
    struct MotorConfig motor_config;
    struct ObserverConfig observer_config;
    struct ControllerConfig controller_config;
    struct CANConfig can_config;
    uint32_t version;
};

void NVM_Init(void);
bool NVM_SaveConfig(void);
bool NVM_LoadConfig(void);
void NVM_Erase(void);

#endif /* NVM_NVM_H_ */
