
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
#include <src/common.h>
#include <src/system/system.h>
#include <src/nvm/flash_func.h>
#include <src/nvm/nvm.h>

static struct NVMStruct s;

bool nvm_save_config(void)
{
	bool commited = false;
	uint8_t data[sizeof(struct NVMStruct)];
	s.motor_config = *motor_get_config();
	s.hall_config = *hall_get_config();
	s.encoder_config = *encoder_get_config();
	s.observer_config = *Observer_GetConfig();
	s.controller_config = *Controller_GetConfig();
	s.can_config = *CAN_get_config();
	s.version = (VERSION_MAJOR << 16) + (VERSION_MINOR << 8) + VERSION_PATCH;
	memcpy(data, &s, sizeof(struct NVMStruct));
	if (STATE_IDLE == controller_get_state())
	{
		uint8_t *dataBuffer = data;
		__disable_irq();
		flash_erase_page(SETTINGS_PAGE);
		flash_write((uint8_t *)SETTINGS_PAGE_HEX, dataBuffer, sizeof(struct NVMStruct));
		__enable_irq();
		commited = true;
	}
	return commited;
}

bool nvm_load_config(void)
{
	memcpy(&s, (uint8_t *)SETTINGS_PAGE_HEX, sizeof(struct NVMStruct));
	// TODO: Also validate checksum
	bool loaded = false;
	if (s.version == ((VERSION_MAJOR << 16) + (VERSION_MINOR << 8) + VERSION_PATCH))
	{
		motor_restore_config(&s.motor_config);
		hall_restore_config(&s.hall_config);
		encoder_restore_config(&s.encoder_config);
		Observer_RestoreConfig(&s.observer_config);
		Controller_RestoreConfig(&s.controller_config);
		CAN_restore_config(&s.can_config);
		loaded = true;
	}
	return loaded;
}

void nvm_erase(void)
{
	if (STATE_IDLE == controller_get_state())
	{
		flash_erase_page(SETTINGS_PAGE);
		system_reset();
	}
}
