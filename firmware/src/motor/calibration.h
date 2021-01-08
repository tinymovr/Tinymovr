
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

#ifndef SRC_MOTOR_CALIBRATION_H_
#define SRC_MOTOR_CALIBRATION_H_

#include <src/common.h>
#include <src/motor/motor.h>

bool CalibrateResistance(void);
bool CalibrateInductance(void);
bool CalibrateDirectionAndPolePairs(void);
bool CalibrateEccentricity(void);

#endif /* SRC_MOTOR_CALIBRATION_H_ */
