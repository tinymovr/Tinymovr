
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
#define VERSION_MINOR (8u)
#define VERSION_PATCH (1u)

/// TINYMOVR CONFIGURATION OPTIONS ///

// Uncomment to disable live gate driver control
// #define DRY_RUN

// WARNING! PLEASE DO NOT ADJUST CLOCK DIVIDER YET!
// LEAVE IT AT 1
// ACLK Clock Divider
// The ACLK clock is equal to SCLK / ACLK_PRESCALER
// Higher ACLK_DIVIDER values mean less PWM resolution, but also
// slightly lower power consumption.
// Valid values are {1, 2, 3, 4, 5, 6, 7, 8}
#define ACLK_DIVIDER				(1)
#define ACLK_FREQ_HZ                (300000000/ACLK_DIVIDER)
#define HCLK_FREQ_HZ                (150000000)

// PWM Timer Frequency (Hz).
// Determines the PWM Timer Frequency by varying the
// PWM timer base.
#define PWM_TIMER_FREQ              (20000)

// Limits
#define PWM_LIMIT                   (0.8f)
#define I_INTEGRATOR_DECAY_FACTOR   (0.995f)
#define I_TRIP_MARGIN               (1.35f)
#define VBUS_LOW_THRESHOLD          (11.0f)

// Calibration
#define CAL_R_LEN             (2 * PWM_TIMER_FREQ)
#define CAL_L_LEN             (1 * PWM_TIMER_FREQ)
#define CAL_OFFSET_LEN        (1 * PWM_TIMER_FREQ)
#define CAL_DIR_LEN           (4 * PWM_TIMER_FREQ)

#define CAL_PHASE_TURNS             8

#define CAL_V_GAIN (0.0005f)
#define CAL_I_SETPOINT (5.0f)
#define CAL_V_INDUCTANCE (2.0f)

// UART
#define UART_ENUM UARTB
#define UART_REF PAC55XX_UARTB
#define UART_BAUD_RATE 115200

#define UART_I_SCALING_FACTOR ( 1000.0f )
#define UART_R_SCALING_FACTOR ( 1000.0f )
#define UART_L_SCALING_FACTOR ( 1000.0f )
#define ONE_OVER_UART_I_SCALING_FACTOR ( 0.001f )
#define UART_V_SCALING_FACTOR ( 1000.0f )
