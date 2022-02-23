
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

// Firmware Version
#define VERSION_MAJOR (0u)
#define VERSION_MINOR (8u)
#define VERSION_PATCH (12u)

// Required Studio version
#define STUDIO_MIN_VERSION_MAJOR (0u)
#define STUDIO_MIN_VERSION_MINOR (3u)
#define STUDIO_MIN_VERSION_PATCH (12u)

/// TINYMOVR CONFIGURATION OPTIONS ///

// --- Board Version
// #define BOARD_REV_R32
#define BOARD_REV_R33
// #define BOARD_REV_M1

#define ACLK_FREQ_HZ                (300000000)
#define HCLK_FREQ_HZ                (150000000)

// Timer clock divider
#define TXCTL_PS_DIV                TXCTL_PS_DIV2

// Desired PWM Frequency (Hz).
#define PWM_FREQ_HZ              (20000)

// Control parameters
#define PWM_LIMIT                   (0.8f)
#define I_INTEGRATOR_DECAY_FACTOR   (0.995f)
#define I_TRIP_MARGIN               (1.5f)
#define VBUS_LOW_THRESHOLD          (10.4f)   // V
#define VEL_HARD_LIMIT              (600000.0f)  // ticks/s
#define I_HARD_LIMIT                (60.0f)    // A
#define VEL_INTEGRATOR_THRESHOLD    (2000.0f) // ticks/s

// Calibration
#define CAL_R_LEN             (2 * PWM_FREQ_HZ)
#define CAL_L_LEN             (1 * PWM_FREQ_HZ)
#define CAL_OFFSET_LEN        (1 * PWM_FREQ_HZ)
#define CAL_STAY_LEN          (PWM_FREQ_HZ / 2)
#define CAL_DIR_LEN           (3 * PWM_FREQ_HZ)

#define CAL_PHASE_TURNS             8

#define CAL_V_GAIN (0.0005f)
#define CAL_V_INDUCTANCE (2.0f)

// Eccentricity correction lookup table size
#define ECN_BITS (6)
#define ECN_SIZE (2 << ECN_BITS)

// UART
#define UART_ENUM UARTB
#define UART_REF PAC55XX_UARTB
#define UART_BAUD_RATE 115200

#define UART_I_SCALING_FACTOR ( 1000.0f )
#define ONE_OVER_UART_I_SCALING_FACTOR ( 0.001f )

#define UART_R_SCALING_FACTOR ( 1000.0f )
#define ONE_OVER_UART_R_SCALING_FACTOR ( 0.001f )

#define UART_L_SCALING_FACTOR ( 1000.0f )
#define ONE_OVER_UART_L_SCALING_FACTOR ( 0.001f )

#define UART_VEL_GAIN_SCALING_FACTOR ( 1000000f )
#define ONE_OVER_UART_VEL_GAIN_SCALING_FACTOR ( 0.000001f )

#define UART_VEL_INT_SCALING_FACTOR ( 1000.0f )
#define ONE_OVER_UART_VEL_INT_SCALING_FACTOR ( 0.001f )

#define UART_V_SCALING_FACTOR ( 1000.0f )
