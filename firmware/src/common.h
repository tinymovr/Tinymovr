
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

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pac5xxx.h"
#include "pac5xxx_adc.h"
#include "pac5xxx_can.h"
#include "pac5xxx_crc.h"
#include "pac5xxx_gp_timers.h"
#include "pac5xxx_gpio.h"
#include "pac5xxx_i2c.h"
#include "pac5xxx_memory.h"
#include "pac5xxx_rtc.h"
#include "pac5xxx_scc.h"
#include "pac5xxx_ssp.h"
#include "pac5xxx_timers.h"
#include "pac5xxx_uart.h"
#include "pac5xxx_wwdt.h"
#include "pac5xxx_driver_adc.h"
#include "pac5xxx_driver_can.h"
#include "pac5xxx_driver_config.h"
#include "pac5xxx_driver_gpio.h"
#include "pac5xxx_driver_memory.h"
#include "pac5xxx_driver_socbridge.h"
#include "pac5xxx_driver_system.h"
#include "pac5xxx_driver_tile.h"
#include "pac5xxx_driver_timer.h"
#include "pac5xxx_driver_uart.h"
#include "pac5527.h"
#include "pac5xxx_tile_power_manager.h"
#include "pac5xxx_tile_signal_manager.h"
#include "config.h"

// ARM DWT
#define  ARM_CM_DEMCR      (*(uint32_t *)0xE000EDFC)
#define  ARM_CM_DWT_CTRL   (*(uint32_t *)0xE0001000)
#define  ARM_CM_DWT_CYCCNT (*(uint32_t *)0xE0001004)

#ifndef PAC5XXX_OK
#define PAC5XXX_OK      0
#endif

#ifndef PAC5XXX_ERROR
#define PAC5XXX_ERROR   1
#endif

#define PWM_TIMER_PERIOD (1.0f / PWM_TIMER_FREQ)

#define READ_UINT16(address)                      (*((uint16_t *) address))
#define READ_UINT32(address)                      (*((uint32_t *) address))

static const float one_by_sqrt3 = 0.57735026919f;
static const float two_by_sqrt3 = 1.15470053838f;

#define PI (3.141592f)

static const float invtwopi=0.1591549f;
static const float twopi=6.283185f;
static const float threehalfpi=4.7123889f;
static const float pi=PI;
static const float halfpi=PI*0.5f;
static const float quarterpi=PI*0.25f;

struct FloatTriplet
{
	float A;
	float B;
	float C;
};

typedef enum {
	MODULE_SYSTEM
	MODULE_ENCODER
	MODULE_CONTROLLER
	MODULE_MOTOR
	MODULE_WATCHDOG
	_MODULE_COUNT
} SystemModule;

#endif // #ifndef COMMON_H
