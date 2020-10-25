
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
#define VERSION_PATCH (3u)

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
#define I_TRIP_MARGIN               (1.25f)
#define VBUS_LOW_THRESHOLD          (11.0f)

// Calibration timer indices
#define CAL_R_END_INDEX             (2 * PWM_TIMER_FREQ)
#define CAL_L_END_INDEX             (3 * PWM_TIMER_FREQ)
#define CAL_OFFSET_END_INDEX        (4 * PWM_TIMER_FREQ)
#define CAL_DIR_END_INDEX           (8 * PWM_TIMER_FREQ)

#define CAL_PHASE_TURNS             8

// CAN
#define CAN_EP_SIZE 6

// UART
#define UART_ENUM UARTB
#define UART_REF PAC55XX_UARTB
#define UART_BAUD_RATE 115200
#define UART_ASCII_PROT_START_BYTE 0x2E
#define UART_NEWLINE 0x0A
#define UART_BYTE_LIMIT 32
#define UART_I_SCALING_FACTOR ( 1000.0f )
#define UART_R_SCALING_FACTOR ( 1000.0f )
#define UART_L_SCALING_FACTOR ( 1000.0f )
#define ONE_OVER_UART_I_SCALING_FACTOR ( 0.001f )
#define UART_V_SCALING_FACTOR ( 1000.0f )
