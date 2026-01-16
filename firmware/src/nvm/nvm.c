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

// Wear leveling state (RAM only, reconstructed on boot)
typedef struct {
    uint8_t current_slot;      // Index of most recent valid config
    uint32_t latest_sequence;  // Sequence number of current_slot
    uint8_t next_write_slot;   // Where next write will go
    bool initialized;          // Has scan completed?
} NVMWearLevelingState;

static NVMWearLevelingState wl_state = {0};

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
    // Ensure wear leveling is initialized
    if (!wl_state.initialized)
    {
        nvm_wl_scan_slots();
    }

    // Prepare combined buffer: [Metadata 16B][NVMStruct]
    uint8_t data[NVM_METADATA_SIZE + sizeof(struct NVMStruct)];
    uint8_t readback_data[NVM_METADATA_SIZE + sizeof(struct NVMStruct)];

    // Prepare metadata
    NVMMetadata *metadata = (NVMMetadata *)data;
    nvm_wl_prepare_metadata(metadata, sizeof(struct NVMStruct));

    // Prepare config data (existing logic)
    struct NVMStruct *config = (struct NVMStruct *)(data + NVM_METADATA_SIZE);
    config->node_id_1 = CAN_get_ID();
    config->node_id_2 = CAN_get_ID();
    frames_get_config(&(config->frames_config));
    config->adc_config = *ADC_get_config();
    config->motor_config = *motor_get_config();
    sensors_get_config(&(config->sensors_config));
    observers_get_config(&(config->observers_config));
    config->controller_config = *controller_get_config();
    config->can_config = *CAN_get_config();
    config->traj_planner_config = *traj_planner_get_config();
    strncpy(config->version, GIT_VERSION, sizeof(config->version));

    // Calculate config checksum
    uint8_t *config_bytes = (uint8_t *)config;
    config->checksum = calculate_checksum(config_bytes,
        sizeof(struct NVMStruct) - sizeof(config->checksum));

    if (CONTROLLER_STATE_IDLE == controller_get_state())
    {
        // Calculate target slot address
        uint32_t slot_addr = SETTINGS_PAGE_HEX +
            (wl_state.next_write_slot * NVM_SLOT_BYTES);

        __disable_irq();

        // Erase only the target slot pages (generic: adapts to slot size)
        for (uint8_t i = 0; i < NVM_SLOT_SIZE; i++)
        {
            flash_erase_page(SETTINGS_PAGE + wl_state.next_write_slot * NVM_SLOT_SIZE + i);
        }

        // Write metadata + config
        flash_write((uint8_t *)slot_addr, data,
            NVM_METADATA_SIZE + sizeof(struct NVMStruct));

        __enable_irq();

        // Verify write
        memcpy(readback_data, (uint8_t *)slot_addr,
            NVM_METADATA_SIZE + sizeof(struct NVMStruct));
        if (memcmp(data, readback_data,
            NVM_METADATA_SIZE + sizeof(struct NVMStruct)) == 0)
        {
            // Update wear leveling state
            wl_state.current_slot = wl_state.next_write_slot;
            wl_state.latest_sequence = metadata->sequence_number;
            wl_state.next_write_slot = (wl_state.next_write_slot + 1) % NVM_NUM_SLOTS;

            // Also update RAM copy for consistency
            s = *config;

            return true;
        }
    }
    return false;
}

bool nvm_load_config(void)
{
    // Initialize wear leveling if not already initialized
    if (!wl_state.initialized)
    {
        nvm_wl_scan_slots();

        // Check for legacy config (no metadata, directly starts with NVMStruct)
        if (wl_state.latest_sequence == 0 && nvm_wl_detect_legacy_config())
        {
            // Migrate legacy config to new format
            if (nvm_wl_migrate_legacy_config())
            {
                // Re-scan after migration to update state
                nvm_wl_scan_slots();
            }
            else
            {
                return false;  // Migration failed
            }
        }
    }

    // Check if any valid config exists
    if (wl_state.latest_sequence == 0)
    {
        return false;  // No valid config found
    }

    // Calculate address of most recent config
    uint32_t slot_addr = SETTINGS_PAGE_HEX +
        (wl_state.current_slot * NVM_SLOT_BYTES);

    // Read metadata
    const NVMMetadata *metadata = (const NVMMetadata *)slot_addr;
    if (!nvm_wl_validate_metadata(metadata))
    {
        return false;
    }

    // Read config (skip metadata header)
    const struct NVMStruct *stored_config =
        (const struct NVMStruct *)(slot_addr + NVM_METADATA_SIZE);
    memcpy(&s, stored_config, sizeof(struct NVMStruct));

    // Validate config checksum
    uint32_t calculated_checksum = calculate_checksum(
        (const uint8_t *)&s,
        sizeof(struct NVMStruct) - sizeof(s.checksum)
    );
    if (calculated_checksum != s.checksum)
    {
        return false;
    }

    // Validate version
    if (strncmp(s.version, GIT_VERSION, sizeof(s.version)) == 0)
    {
        // Restore config (existing logic)
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
    // Erase all slots (generic: adapts to total page count)
    for (uint8_t i = 0; i < NVM_TOTAL_PAGES; i++)
	{
		flash_erase_page(SETTINGS_PAGE + i);
	}

    // Reset wear leveling state
    wl_state.current_slot = 0;
    wl_state.latest_sequence = 0;
    wl_state.next_write_slot = 0;
    wl_state.initialized = true;
}

// This separate function is needed to interface with the protocol
void nvm_erase_and_reset(void)
{
	if (CONTROLLER_STATE_IDLE == controller_get_state())
    {
		nvm_erase();
		system_reset();
	}
}

//==============================================================================
// Wear Leveling Functions
//==============================================================================

/**
 * @brief Validate metadata of a config slot
 * @param metadata Pointer to metadata to validate
 * @return true if metadata is valid and uncorrupted, false otherwise
 */
bool nvm_wl_validate_metadata(const NVMMetadata *metadata)
{
    // Check magic marker
    if (metadata->magic_marker != NVM_MAGIC_MARKER)
    {
        return false;
    }

    // Check metadata version
    if (metadata->metadata_version != NVM_METADATA_VERSION)
    {
        return false;
    }

    // Check data size makes sense
    if (metadata->data_size == 0 ||
        metadata->data_size > (NVM_SLOT_BYTES - NVM_METADATA_SIZE))
    {
        return false;
    }

    // Verify metadata checksum
    uint32_t calc_checksum = calculate_checksum(
        (const uint8_t *)metadata,
        sizeof(NVMMetadata) - sizeof(metadata->metadata_checksum)
    );
    if (calc_checksum != metadata->metadata_checksum)
    {
        return false;
    }

    return true;
}

/**
 * @brief Prepare metadata header before write
 * @param metadata Pointer to metadata to fill
 * @param data_size Size of NVMStruct that follows
 */
void nvm_wl_prepare_metadata(NVMMetadata *metadata, uint16_t data_size)
{
    metadata->sequence_number = wl_state.latest_sequence + 1;
    metadata->magic_marker = NVM_MAGIC_MARKER;
    metadata->data_size = data_size;
    metadata->metadata_version = NVM_METADATA_VERSION;

    // Calculate metadata checksum (excluding the checksum field itself)
    metadata->metadata_checksum = calculate_checksum(
        (const uint8_t *)metadata,
        sizeof(NVMMetadata) - sizeof(metadata->metadata_checksum)
    );
}

/**
 * @brief Scan all slots to find most recent valid config
 *
 * This function scans all NVM_NUM_SLOTS slots to find the one with the
 * highest sequence number. It handles sequence number wraparound correctly.
 */
void nvm_wl_scan_slots(void)
{
    // Initialize state
    wl_state.current_slot = 0;
    wl_state.latest_sequence = 0;
    wl_state.next_write_slot = 0;
    wl_state.initialized = false;

    bool found_valid_slot = false;

    // Scan all slots
    for (uint8_t slot = 0; slot < NVM_NUM_SLOTS; slot++)
    {
        uint32_t slot_addr = SETTINGS_PAGE_HEX + (slot * NVM_SLOT_BYTES);
        const NVMMetadata *metadata = (const NVMMetadata *)slot_addr;

        // Check if slot is valid
        if (nvm_wl_validate_metadata(metadata))
        {
            // Handle sequence number wraparound using distance-based comparison
            // If the difference is more than UINT32_MAX/2, assume wraparound occurred
            if (!found_valid_slot)
            {
                // First valid slot found
                wl_state.latest_sequence = metadata->sequence_number;
                wl_state.current_slot = slot;
                found_valid_slot = true;
            }
            else
            {
                // Compare with current latest, handling wraparound
                uint32_t diff = metadata->sequence_number - wl_state.latest_sequence;

                // If diff is small (< UINT32_MAX/2), it's newer
                // If diff is large (> UINT32_MAX/2), wraparound occurred and it's older
                if (diff < (UINT32_MAX / 2))
                {
                    wl_state.latest_sequence = metadata->sequence_number;
                    wl_state.current_slot = slot;
                }
            }
        }
    }

    // Next write goes to slot after current, or slot 0 if no valid config found
    if (found_valid_slot)
    {
        wl_state.next_write_slot = (wl_state.current_slot + 1) % NVM_NUM_SLOTS;
    }
    else
    {
        wl_state.next_write_slot = 0;  // Start from slot 0 if no valid config found
    }
    wl_state.initialized = true;
}

/**
 * @brief Detect if a legacy config (without metadata) exists at page 120
 * @return true if valid legacy config found, false otherwise
 */
bool nvm_wl_detect_legacy_config(void)
{
    // Check if page 120 has new-format config (with metadata header)
    const NVMMetadata *potential_metadata = (const NVMMetadata *)SETTINGS_PAGE_HEX;
    if (potential_metadata->magic_marker == NVM_MAGIC_MARKER)
    {
        return false;  // Not legacy, already has metadata
    }

    // Try to validate as legacy config (no metadata, starts directly with NVMStruct)
    struct NVMStruct *legacy_config = (struct NVMStruct *)SETTINGS_PAGE_HEX;

    uint32_t calc_checksum = calculate_checksum(
        (const uint8_t *)legacy_config,
        sizeof(struct NVMStruct) - sizeof(legacy_config->checksum)
    );

    if (calc_checksum == legacy_config->checksum &&
        strncmp(legacy_config->version, GIT_VERSION, sizeof(legacy_config->version)) == 0)
    {
        return true;  // Valid legacy config found
    }

    return false;
}

/**
 * @brief Migrate legacy config to new format with metadata
 * @return true if migration successful, false otherwise
 */
bool nvm_wl_migrate_legacy_config(void)
{
    // Read legacy config from page 120
    struct NVMStruct legacy_config;
    memcpy(&legacy_config, (uint8_t *)SETTINGS_PAGE_HEX, sizeof(struct NVMStruct));

    // Temporarily store in RAM
    s = legacy_config;

    // Save using new format (automatically adds metadata and uses wear leveling)
    // This will write to slot 0 with sequence number 1
    return nvm_save_config();
}

//==============================================================================
// Wear Leveling Debug/Status Functions
//==============================================================================

/**
 * @brief Get number of wear leveling slots
 * @return Number of slots (calculated from config size)
 */
uint8_t nvm_wl_get_num_slots(void)
{
    return NVM_NUM_SLOTS;
}

/**
 * @brief Get current active slot
 * @return Current slot index (0 to num_slots-1)
 */
uint8_t nvm_wl_get_current_slot(void)
{
    if (!wl_state.initialized)
    {
        nvm_wl_scan_slots();
    }
    return wl_state.current_slot;
}

/**
 * @brief Get total write count (sequence number)
 * @return Total writes since first use
 */
uint32_t nvm_wl_get_write_count(void)
{
    if (!wl_state.initialized)
    {
        nvm_wl_scan_slots();
    }
    return wl_state.latest_sequence;
}

/**
 * @brief Get estimated remaining writes
 * @return Estimated remaining writes before wear out
 */
uint32_t nvm_wl_get_remaining_writes_estimate(void)
{
    if (!wl_state.initialized)
    {
        nvm_wl_scan_slots();
    }

    // Conservative estimate: 10,000 cycles per page × number of slots
    const uint32_t total_lifetime = (uint32_t)NVM_NUM_SLOTS * 10000;

    if (wl_state.latest_sequence >= total_lifetime)
    {
        return 0;  // Already exceeded conservative estimate
    }

    return total_lifetime - wl_state.latest_sequence;
}
