
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
#include <src/motor/motor.h>

#define CAL_R_LEN             (2 * PWM_FREQ_HZ)
#define CAL_L_LEN             (1 * PWM_FREQ_HZ)
#define CAL_OFFSET_LEN        (1 * PWM_FREQ_HZ)
#define CAL_STAY_LEN          (PWM_FREQ_HZ / 2)
#define CAL_DIR_LEN           (3 * PWM_FREQ_HZ)
#define CAL_PHASE_TURNS       (8)
#if defined BOARD_REV_R32 || BOARD_REV_R33 || defined BOARD_REV_R5
#define CAL_V_GAIN            (0.0005f)
#define CAL_V_INDUCTANCE      (2.0f)
#elif defined BOARD_REV_M5
#define CAL_V_GAIN            (0.001f)
#define CAL_V_INDUCTANCE      (5.0f)
#endif

bool CalibrateResistance(void);
bool CalibrateInductance(void);
bool CalibrateDirectionAndPolePairs(void);
bool calibrate_hall_sequence(void);
bool calibrate_offset_and_rectification(void);
void reset_calibration(void);
