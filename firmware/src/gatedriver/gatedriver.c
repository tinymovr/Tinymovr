
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

#include <src/common.h>
#include <src/motor/motor.h>
#include <src/gatedriver/gatedriver.h>

struct GateDriver_ gateDriver = 
{
    .state = GATEDRIVER_DISABLED
};

void GateDriver_Init(void) {}

TM_RAMFUNC void gate_driver_enable(void)
{
    // Select PWMA peripheral for Port B
    PAC55XX_SCC->PBMUXSEL.w =  0x01110111;

    // Select GPIO mode for Port B
    PAC55XX_GPIOB->MODE.w = 0x1515;

    // Turn on output enables
    PAC55XX_GPIOB->OUTMASK.w = 0x00;

    // Set slew rate
    // 01000101: push: 750mA pull: 750mA
    pac5xxx_tile_register_write(ADDR_DRVILIMLS, 0x45);
    pac5xxx_tile_register_write(ADDR_DRVILIMHS, 0x45);

    // Enable driver manager and verify active - need to enable even in PAC5210 to get ENHS pin to work
    pac5xxx_tile_register_write(ADDR_ENDRV, 1);

    pac5xxx_tile_register_write(ADDR_CFGDRV4,
            pac5xxx_tile_register_read(ADDR_CFGDRV4) | 0x1); // BBM is bit 0
    gateDriver.state = GATEDRIVER_ENABLED;
}

TM_RAMFUNC void gate_driver_disable(void)
{
    // Disable driver manager and verify active - need to enable even in PAC5210 to get ENHS pin to work
    pac5xxx_tile_register_write(ADDR_ENDRV, 0);

    // Set IO state of all pins to 0
    PAC55XX_GPIOB->OUT.w = 0x00;

    // Select GPIO peripheral for Port B
    PAC55XX_SCC->PBMUXSEL.w =  0x00000000;

    // Select GPIO mode for Port B
    PAC55XX_GPIOB->MODE.w = 0x1515;

    // Turn on output enables
    PAC55XX_GPIOB->OUTMASK.w = 0x00;
    gateDriver.state = GATEDRIVER_DISABLED;
}

TM_RAMFUNC bool gate_driver_is_enabled(void)
{
    return (GATEDRIVER_ENABLED == gateDriver.state);
}

TM_RAMFUNC void gate_driver_set_duty_cycle(const FloatTriplet *dutycycles)
{
	m1_u_set_duty(dutycycles->A);
	if (motor_phases_swapped())
	{
		m1_v_set_duty(dutycycles->C);
		m1_w_set_duty(dutycycles->B);
	}
	else
	{
		m1_v_set_duty(dutycycles->B);
		m1_w_set_duty(dutycycles->C);
	}
}
