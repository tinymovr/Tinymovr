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

#include <src/xfs.h>
#include <src/adc/adc.h>
#include <src/motor/motor.h>
#include <src/sensor/sensors.h>
#include <src/observer/observer.h>
#include <src/controller/controller.h>
#include <src/controller/trajectory_planner.h>
#include <src/can/can.h>

// Wear leveling metadata prepended to each config slot
typedef struct {
    uint32_t sequence_number;   // Increments with each write, identifies most recent
    uint32_t magic_marker;      // 0x544D4E56 ("TMNV" in ASCII) - validates slot
    uint16_t data_size;         // Size of NVMStruct (for compatibility check)
    uint16_t metadata_version;  // Metadata format version (for future changes)
    uint32_t metadata_checksum; // Checksum of this metadata (corruption detection)
} NVMMetadata;

_Static_assert(sizeof(NVMMetadata) == 16, "NVMMetadata must be 16 bytes");
_Static_assert(sizeof(NVMMetadata) % 16 == 0, "NVMMetadata must be 16-byte aligned for flash");

struct NVMStruct {
    uint8_t node_id_1;
    uint8_t node_id_2;
    FramesConfig frames_config;
    ADCConfig adc_config;
    MotorConfig motor_config;
    SensorsConfig sensors_config;
    ObserversConfig observers_config;
    ControllerConfig controller_config;
    CANConfig can_config;
    TrajPlannerConfig traj_planner_config;
    char version[16];
    uint32_t checksum;
};

// NVM flash region configuration - PLATFORM DEPENDENT
#define SETTINGS_PAGE (120)                     // First NVM page (legacy name)
#define SETTINGS_PAGE_START (120)               // First NVM page
#define SETTINGS_PAGE_END (127)                 // Last NVM page
#define SETTINGS_PAGE_HEX (0x0001E000)
#define NVM_PAGE_SIZE (1024)
#define SETTINGS_PAGE_COUNT (DIVIDE_AND_ROUND_UP(sizeof(struct NVMStruct), NVM_PAGE_SIZE))

// Wear leveling configuration - GENERIC, adapts to any structure size
#define NVM_TOTAL_PAGES (SETTINGS_PAGE_END - SETTINGS_PAGE_START + 1)  // 8 pages
#define NVM_METADATA_SIZE (sizeof(NVMMetadata))                         // 16 bytes
#define NVM_SLOT_SIZE (SETTINGS_PAGE_COUNT)                             // Pages per slot
#define NVM_SLOT_BYTES (NVM_SLOT_SIZE * NVM_PAGE_SIZE)                 // Bytes per slot
#define NVM_NUM_SLOTS (NVM_TOTAL_PAGES / NVM_SLOT_SIZE)                // Number of slots

// Magic values
#define NVM_MAGIC_MARKER (0x544D4E56)           // "TMNV" in ASCII
#define NVM_METADATA_VERSION (1)                 // Current metadata format version

// Compile-time validation
_Static_assert(NVM_TOTAL_PAGES >= 2, "Need at least 2 pages for wear leveling");
_Static_assert(NVM_NUM_SLOTS >= 2, "Need at least 2 slots for wear leveling");
_Static_assert(sizeof(struct NVMStruct) + NVM_METADATA_SIZE <= NVM_SLOT_BYTES,
               "NVMStruct + metadata must fit in slot");
_Static_assert(NVM_SLOT_SIZE * NVM_NUM_SLOTS == NVM_TOTAL_PAGES,
               "Slots must evenly divide total pages");

extern const uint32_t config_size;

// Main NVM functions
bool nvm_save_config(void);
bool nvm_load_config(void);
void nvm_erase(void);
void nvm_erase_and_reset(void);
uint32_t calculate_checksum(const uint8_t *data, size_t len);

// Wear leveling functions
void nvm_wl_scan_slots(void);
bool nvm_wl_validate_metadata(const NVMMetadata *metadata);
void nvm_wl_prepare_metadata(NVMMetadata *metadata, uint16_t data_size);
bool nvm_wl_detect_legacy_config(void);
bool nvm_wl_migrate_legacy_config(void);

// Wear leveling debug/status functions
uint8_t nvm_wl_get_num_slots(void);
uint8_t nvm_wl_get_current_slot(void);
uint32_t nvm_wl_get_write_count(void);
uint32_t nvm_wl_get_remaining_writes_estimate(void);

