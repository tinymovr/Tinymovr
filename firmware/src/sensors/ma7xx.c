
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
#include <src/system/system.h>
#include <src/ssp/ssp_func.h>
#include <src/utils/utils.h>
#include <src/can/can_endpoints.h>
#include <src/sensors/ma7xx.h>

static MA7xxConfig config = {0};
static MA7xxState state = {0};


void ma7xx_init(void)
{
    ssp_init(PRIMARY_ENCODER_SSP_PORT, SSP_MS_MASTER, 0, 0); // Mode 0
    delay_us(16000); // ensure 16ms sensor startup time as per the datasheet
    ma7xx_send_angle_cmd();
    ma7xx_update(false);
}

TM_RAMFUNC uint8_t ma7xx_get_errors(void)
{
    return state.errors;
}

TM_RAMFUNC void ma7xx_send_angle_cmd(void)
{
	ssp_write_one(PRIMARY_ENCODER_SSP_STRUCT, MA_CMD_ANGLE);
}

TM_RAMFUNC int16_t ma7xx_get_angle_raw(void)
{
    return state.angle;
}

TM_RAMFUNC int16_t ma7xx_get_angle_rectified(void)
{
    const uint8_t offset_bits = (ENCODER_BITS - ECN_BITS);
    const int16_t angle = state.angle;
    const int16_t off_1 = config.rec_table[angle>>offset_bits];
	const int16_t off_2 = config.rec_table[((angle>>offset_bits) + 1) % ECN_SIZE];
	const int16_t off_interp = off_1 + ((off_2 - off_1)* (angle - ((angle>>offset_bits)<<offset_bits))>>offset_bits);
	return angle + off_interp;
}

TM_RAMFUNC void ma7xx_update(bool check_error)
{
    const int16_t angle = ssp_read_one(PRIMARY_ENCODER_SSP_STRUCT) >> 3;

    if (check_error)
    {
    	const int16_t delta = state.angle - angle;
		if ( ((delta > MAX_ALLOWED_DELTA) || (delta < -MAX_ALLOWED_DELTA)) &&
		     ((delta > MAX_ALLOWED_DELTA_ADD) || (delta < MIN_ALLOWED_DELTA_ADD)) &&
		     ((delta > MAX_ALLOWED_DELTA_SUB) || (delta < MIN_ALLOWED_DELTA_SUB)) )
		{
            state.errors |= ENCODER_ERRORS_READING_UNSTABLE;
		}
    }
    state.angle = angle;
}

void ma7xx_clear_rec_table(void)
{
    (void)memset(config.rec_table, 0, sizeof(config.rec_table));
	config.rec_calibrated = false;
}

void ma7xx_set_rec_calibrated(void)
{
    config.rec_calibrated = true;
}

bool ma7xx_rec_is_calibrated(void)
{
    return config.rec_calibrated;
}

int16_t *ma7xx_get_rec_table_ptr(void)
{
    return config.rec_table;
}

MA7xxConfig* ma7xx_get_config(void)
{
    return &config;
}

void ma7xx_restore_config(MA7xxConfig* config_)
{
    config = *config_;
}

/**
 * @brief Write to a register of the encoder
 * 
 * @param reg The 5-bit register address
 * @param value The value to write
 * @return uint16_t 
 */
uint16_t ma7xx_write_reg(uint8_t reg, uint8_t value)
{
    uint16_t cmd = MA_CMD_WRITE | reg << 8 | value;
    uint32_t result = ssp_write_one(PRIMARY_ENCODER_SSP_STRUCT, cmd);

    // Delay at least 20ms to let the encoder write to memory
    delay_us(20100);
    result |= ssp_write_one(PRIMARY_ENCODER_SSP_STRUCT, 0);

    // The encoder returns the value written to memory, so check that it is the same as what we wrote
    uint8_t retval =  ssp_read_one(PRIMARY_ENCODER_SSP_STRUCT) >> 8;
    if ((retval != value) || (result != 0))
    {
        return false;
    }
    return true;
}

/**
 * @brief Read from a register of the encoder
 * 
 * @param register The 5-bit register address
 * @return uint8_t 
 */
uint8_t ma7xx_read_reg(uint8_t reg)
{
    uint16_t cmd[2] = {MA_CMD_READ  | (reg << 8), 0};
    uint16_t result = ssp_write_multi(PRIMARY_ENCODER_SSP_STRUCT, cmd, 2u);
    if (result != 0)
    {
        return false;
    }
    return ssp_read_one(PRIMARY_ENCODER_SSP_STRUCT) >> 8;
}