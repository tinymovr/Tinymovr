
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

typedef struct 
{
	bool adc_interrupt;
	bool can_interrupt;
	bool uart_message_interrupt;
    bool wwdt_interrupt;
	bool busy;
	uint32_t load;

    uint8_t warnings;
} SchedulerState;

extern volatile SchedulerState scheduler_state;

void wait_for_control_loop_interrupt(void);

static inline uint8_t scheduler_get_warnings(void)
{
	return scheduler_state.warnings;
}

static inline uint32_t scheduler_get_load(void)
{
	return scheduler_state.load;
}