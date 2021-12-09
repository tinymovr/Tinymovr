
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

#include <string.h>
#include "src/common.h"

#include "src/system/system.h"
#include "src/nvm/flash_func.h"
#include "src/nvm/nvm.h"

static struct NVMStruct s;

void NVM_Init(void) {}

bool NVM_SaveConfig(void)
{
	bool commited = false;
	uint8_t data[sizeof(struct NVMStruct)];
	s.motor_config = *Motor_GetConfig();
	s.observer_config = *Observer_GetConfig();
	s.controller_config = *Controller_GetConfig();
	s.can_config = *CAN_get_config();
	s.version = (VERSION_MAJOR << 16) + (VERSION_MINOR << 8) + VERSION_PATCH;
	memcpy(data, &s, sizeof(struct NVMStruct));
	if (controller_get_state() == STATE_IDLE)
	{
		uint8_t* dataBuffer = data;
		__disable_irq();
		flash_erase_page(SETTINGS_PAGE);
		flash_write((uint8_t *)SETTINGS_PAGE_HEX, dataBuffer, sizeof(struct NVMStruct));
		__enable_irq();
		commited = true;
	}
	return commited;
}

bool NVM_LoadConfig(void)
{
	memcpy(&s, (uint8_t *)SETTINGS_PAGE_HEX, sizeof(struct NVMStruct));
	// TODO: Also validate checksum
	bool loaded = false;
	if (s.version == ((VERSION_MAJOR << 16) + (VERSION_MINOR << 8) + VERSION_PATCH))
	{
		Motor_RestoreConfig(&s.motor_config);
		Observer_RestoreConfig(&s.observer_config);
		Controller_RestoreConfig(&s.controller_config);
		CAN_restore_config(&s.can_config);
		loaded = true;
	}
	return loaded;
}

void NVM_Erase(void)
{
	if (controller_get_state() == STATE_IDLE)
	{
		flash_erase_page(SETTINGS_PAGE);
		system_reset();
	}
}
