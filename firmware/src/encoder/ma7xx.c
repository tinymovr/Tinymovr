
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

#include <src/system/system.h>
#include <src/ssp/ssp_func.h>
#include <src/utils/utils.h>
#include <src/encoder/ma7xx.h>

static MA7xxState state = { 0 };

#define MAX_ALLOWED_DELTA     (ENCODER_TICKS / 6)
#define MAX_ALLOWED_DELTA_ADD (MAX_ALLOWED_DELTA + ENCODER_TICKS)
#define MAX_ALLOWED_DELTA_SUB (MAX_ALLOWED_DELTA - ENCODER_TICKS)
#define MIN_ALLOWED_DELTA_ADD (-MAX_ALLOWED_DELTA + ENCODER_TICKS)
#define MIN_ALLOWED_DELTA_SUB (-MAX_ALLOWED_DELTA - ENCODER_TICKS)

#if defined(BOARD_REV_R3)
#define PRIMARY_ENCODER_SSP_PORT SSPD
#define PRIMARY_ENCODER_SSP_STRUCT PAC55XX_SSPD
#elif defined(BOARD_REV_M1)
#define PRIMARY_ENCODER_SSP_PORT SSPC
#define PRIMARY_ENCODER_SSP_STRUCT PAC55XX_SSPC
#endif

void ma7xx_init(void)
{
    ssp_init(PRIMARY_ENCODER_SSP_PORT, SSP_MS_MASTER, 0, 0); // Mode 0
    delay_us(16000); // ensure 16ms sensor startup time as per the datasheet
    ma7xx_send_angle_cmd();
    ma7xx_update_angle(false);
}

PAC5XXX_RAMFUNC void ma7xx_send_angle_cmd(void)
{
	ssp_write_one(PRIMARY_ENCODER_SSP_STRUCT, MA_CMD_ANGLE);
}

PAC5XXX_RAMFUNC int16_t ma7xx_get_angle(void)
{
    return state.angle;
}

PAC5XXX_RAMFUNC void ma7xx_update_angle(bool check_error)
{
    while (!PRIMARY_ENCODER_SSP_STRUCT->STAT.RNE) {}
    const int16_t angle = (PRIMARY_ENCODER_SSP_STRUCT->DAT.DATA) >> 3;

    if (check_error)
    {
    	const int16_t delta = state.angle - angle;
		if ( ((delta > MAX_ALLOWED_DELTA) || (delta < -MAX_ALLOWED_DELTA)) &&
		     ((delta > MAX_ALLOWED_DELTA_ADD) || (delta < MIN_ALLOWED_DELTA_ADD)) &&
		     ((delta > MAX_ALLOWED_DELTA_SUB) || (delta < MIN_ALLOWED_DELTA_SUB)) )
		{
			add_error_flag(ERROR_ENCODER_READING_UNSTABLE);
		}
    }
    state.angle = angle;
}
