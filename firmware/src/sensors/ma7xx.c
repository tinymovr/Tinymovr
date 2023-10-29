
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

// ONBOARD_SENSOR_SSP_PORT, SSP_MS_MASTER, Mode 0)

Sensor ma7xx_init_with_ssp_port(SSP_TYPE ssp)
{
    MA7xxConfig c = {0};
    c.ssp_port = ssp;
    return ma7xx_init_with_config(*c);
}

Sensor ma7xx_init_with_config(SensorSpecificConfig *c)
{
    Sensor s;
    s->read_func = ma7xx_get_angle_rectified;
    s->update_func = ma7xx_update;
    s->reset_func = ma7xx_clear_rec_table;
    s->get_error_func = ma7xx_get_errors;
    s->is_calibrated_func = ma7xx_rec_is_calibrated;
    s->config = *c;
    MA7xxConfig *c = s->config.ss_config.ma7xx_config;
    ssp_init(c->ssp_port, SSP_MS_MASTER, 0, 0); // ONBOARD_SENSOR_SSP_PORT, SSP_MS_MASTER, Mode 0
    delay_us(16000); // ensure 16ms sensor startup time as per the datasheet
    ma7xx_send_angle_cmd(&s);
    ma7xx_update(&s, false);
    return s;
}

void ma7xx_deinit(Sensor *s)
{

}

void ma7xx_clear_rec_table(Sensor *s)
{
    MA7xxConfig *c = s->config.ss_config.ma7xx_config;
    (void)memset(c->rec_table, 0, sizeof(c->rec_table));
	c->rec_calibrated = false;
}

void ma7xx_set_rec_calibrated(Sensor *s)
{
    s->config.ss_config.ma7xx_config.rec_calibrated = true;
}

bool ma7xx_rec_is_calibrated(Sensor *s)
{
    return s->config.ss_config.ma7xx_config.rec_calibrated;
}

int16_t *ma7xx_get_rec_table_ptr(Sensor *s)
{
    return s->config.ss_config.ma7xx_config.rec_table;
}

/**
 * @brief Write to a register of the sensor
 * 
 * @param reg The 5-bit register address
 * @param value The value to write
 * @return uint16_t 
 */
uint16_t ma7xx_write_reg(Sensor *s, uint8_t reg, uint8_t value)
{
    uint16_t cmd = MA_CMD_WRITE | reg << 8 | value;
    uint32_t result = ssp_write_one(s->config.ss_config.ma7xx_config.ssp_struct, cmd);

    // Delay at least 20ms to let the encoder write to memory
    delay_us(20100);
    result |= ssp_write_one(s->config.ss_config.ma7xx_config.ssp_struct, 0);

    // The encoder returns the value written to memory, so check that it is the same as what we wrote
    uint8_t retval =  ssp_read_one(s->config.ss_config.ma7xx_config.ssp_struct) >> 8;
    if ((retval != value) || (result != 0))
    {
        return false;
    }
    return true;
}

/**
 * @brief Read from a register of the sensor
 * 
 * @param register The 5-bit register address
 * @return uint8_t 
 */
uint8_t ma7xx_read_reg(Sensor *s, uint8_t reg)
{
    uint16_t cmd[2] = {MA_CMD_READ  | (reg << 8), 0};
    uint16_t result = ssp_write_multi(s->params.ma7xx_params.ssp_struct, cmd, 2u);
    if (result != 0)
    {
        return false;
    }
    return ssp_read_one(s->params.ma7xx_params.ssp_struct) >> 8;
}