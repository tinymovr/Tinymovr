
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

#define VERSION_MAJOR (0u)
#define VERSION_MINOR (7u)
#define VERSION_PATCH (2u)

/// TINYMOVR CONFIGURATION OPTIONS ///

// Uncomment to disable live gate driver control
// #define DRY_RUN

#define ACLK_FREQ_HZ                (300000000)
#define HCLK_FREQ_HZ                (150000000)
#define PWM_TIMER_FREQ              (20000)

#define PWM_LIMIT                   (0.8f)
#define I_TRIP_MARGIN               (1.25f)
#define VBUS_LOW_THRESHOLD          (11.0f)

// Calibration

#define CAL_R_END_INDEX             (2 * PWM_TIMER_FREQ)
#define CAL_L_END_INDEX             (3 * PWM_TIMER_FREQ)
#define CAL_OFFSET_END_INDEX        (4 * PWM_TIMER_FREQ)
#define CAL_DIR_END_INDEX           (8 * PWM_TIMER_FREQ)

#define CAL_PHASE_TURNS             8
