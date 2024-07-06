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

const uint32_t config_size = sizeof(struct NVMStruct);

uint32_t calculate_checksum(const uint8_t *data, size_t len)
{
    uint32_t checksum = 0;
    for (size_t i = 0; i < len; ++i)
    {
        checksum += data[i];
    }
    return ~checksum + 1;
}

bool nvm_save_config(void)
{
    uint8_t data[sizeof(struct NVMStruct)];
    uint8_t readback_data[sizeof(struct NVMStruct)];

    s.node_id_1 = CAN_get_ID();
    s.node_id_2 = CAN_get_ID();
    frames_get_config(&(s.frames_config));
    s.adc_config = *ADC_get_config();
    s.motor_config = *motor_get_config();
    sensors_get_config(&(s.sensors_config));
    observers_get_config(&(s.observers_config));
    s.controller_config = *controller_get_config();
    s.can_config = *CAN_get_config();
    s.traj_planner_config = *traj_planner_get_config();
    strncpy(s.version, GIT_VERSION, sizeof(s.version) - 1);
    s.version[sizeof(s.version) - 1] = '\0'; // Ensure null-termination
    
    memcpy(data, &s, sizeof(struct NVMStruct) - sizeof(s.checksum));
    s.checksum = calculate_checksum(data, sizeof(struct NVMStruct) - sizeof(s.checksum));
    memcpy(data + sizeof(struct NVMStruct) - sizeof(s.checksum), &s.checksum, sizeof(s.checksum));

    if (CONTROLLER_STATE_IDLE == controller_get_state())
    {
        uint8_t *dataBuffer = data;
        __disable_irq();
        flash_erase_page(SETTINGS_PAGE);
        flash_write((uint8_t *)SETTINGS_PAGE_HEX, dataBuffer, sizeof(struct NVMStruct));
        __enable_irq();

        memcpy(readback_data, (uint8_t *)SETTINGS_PAGE_HEX, sizeof(struct NVMStruct));
        if (memcmp(data, readback_data, sizeof(struct NVMStruct)) == 0)
        {
            return true;
        }
    }
    return false;
}

bool nvm_load_config(void)
{
    memcpy(&s, (uint8_t *)SETTINGS_PAGE_HEX, sizeof(struct NVMStruct));
    uint32_t calculated_checksum = calculate_checksum((uint8_t *)&s, sizeof(struct NVMStruct) - sizeof(s.checksum));
    if (calculated_checksum != s.checksum)
    {
        return false; // Checksum mismatch, data is corrupt
    }

    if (strncmp(s.version, GIT_VERSION, sizeof(s.version)) == 0)
    {
        frames_restore_config(&s.frames_config);
        ADC_restore_config(&s.adc_config);
        motor_restore_config(&s.motor_config);
        sensors_restore_config(&s.sensors_config);
        observers_restore_config(&s.observers_config);
        controller_restore_config(&s.controller_config);
        CAN_restore_config(&s.can_config);
        traj_planner_restore_config(&s.traj_planner_config);
        return true;
    }
    return false;
}

void nvm_erase(void)
{
    if (CONTROLLER_STATE_IDLE == controller_get_state())
    {
        for (uint8_t i = 0; i < SETTINGS_PAGE_COUNT; i++)
        {
            flash_erase_page(SETTINGS_PAGE + i);
        }
        system_reset();
    }
}
