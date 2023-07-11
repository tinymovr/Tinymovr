
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
	s.node_id = CAN_get_ID();
	s.adc_config = *ADC_get_config();
	s.motor_config = *motor_get_config();
	s.hall_config = *hall_get_config();
	s.ma7xx_config = *ma7xx_get_config();
	s.encoder_config = *encoder_get_config();
	s.observer_config = *observer_get_config();
	s.controller_config = *controller_get_config();
	s.can_config = *CAN_get_config();
	s.traj_planner_config = *traj_planner_get_config();
	strlcpy(s.version, GIT_VERSION, sizeof(s.version));
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
	char static_version[16];
	strlcpy(static_version, GIT_VERSION, sizeof(static_version));
	if (strcmp(s.version, static_version) == 0)
	{
		ADC_restore_config(&s.adc_config);
		motor_restore_config(&s.motor_config);
		hall_restore_config(&s.hall_config);
		ma7xx_restore_config(&s.ma7xx_config);
		encoder_restore_config(&s.encoder_config);
		observer_restore_config(&s.observer_config);
		controller_restore_config(&s.controller_config);
		CAN_restore_config(&s.can_config);
		traj_planner_restore_config(&s.traj_planner_config);
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
