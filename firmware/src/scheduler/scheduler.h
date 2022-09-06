
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

typedef struct 
{
	bool adc_interrupt;
	bool can_interrupt;
	bool uart_message_interrupt;
    bool wwdt_interrupt;
	bool busy;

    uint8_t errors;
    uint32_t busy_cycles;
    uint32_t total_cycles;
    uint32_t busy_loop_start;
    uint32_t total_loop_start;
} SchedulerState;

void WaitForControlLoopInterrupt(void);

uint32_t Scheduler_GetTotalCycles(void);
uint32_t Scheduler_GetBusyCycles(void);

PAC5XXX_RAMFUNC uint8_t scheduler_get_errors(void);
