
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

#include <src/adc/adc.h>
#include <src/gatedriver/gatedriver.h>
#include <src/encoder/encoder.h>
#include <src/motor/motor.h>
#include <src/controller/controller.h>
#include <src/comms/defs.h>
#include <src/utils/utils.h>
#include <src/rtt/SEGGER_RTT.h>
#include <src/system/system.h>

void system_init(void)
{
    // --- Mandatory System Init from Qorvo
    // Set Flash Lock to allow write access to MEMCTL register for configuring clocks
    PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_MEMCTL;

    // Turn on Flache Cache
    PAC55XX_MEMCTL->MEMCTL.CACHEDIS = 0;                        //Enable Flash CACHE

    // Always make sure MCLK is set to use ROSC CLK when making changes to FRCLK, SCLK, or HCLK
    PAC55XX_MEMCTL->MEMCTL.MCLKSEL = MEMCTL_MCLK_ROSCCLK;       // MCLK = ROSCCLK

    // Select SCLK = FRCLK before making changes to FRCLK or PLLCLK
    PAC55XX_SCC->CCSCTL.SCLKMUXSEL = CCSCTL_SCLK_FRCLK;

    // Select 4 MHz CLKREF for Free Running Clock FRCLK
    PAC55XX_SCC->CCSCTL.FRCLKMUXSEL = CCSCTL_CLKIN_CLKREF;

    pac5xxx_sys_pll_config_enable(4, 300, 0);                   // PLLCLK = 300 MHz = (4/4 * 300) /1

    // Configure SCLK=PLLCLK=300 MHz, HCLK=150 MHz, PCLK=150 MHz, ACLK=300 MHz and WaitStates;  Use default PCLKDIV=1
    PAC55XX_SCC->CCSCTL.HCLKDIV = CCSCTL_HCLKDIV_DIV2;          // HCLK = 150 MHz = SCLK/2; when SCLK = PLLCLK
    PAC55XX_SCC->CCSCTL.ACLKDIV = CCSCTL_ACLKDIV_DIV1;
    PAC55XX_MEMCTL->MEMCTL.WSTATE = 5 + 1;                      // Flash = 150/25 = 6 clocks = 5 WS; So, need 5 + 1 Extra WS
    PAC55XX_SCC->CCSCTL.SCLKMUXSEL = CCSCTL_SCLK_PLLCLK;        // SCLK = PLLCLK

    // Set MCLK for Flash write & erase in addition to read
    PAC55XX_MEMCTL->MEMCTL.MCLKDIV = MEMCTL_MCLK_DIV5;          // MCLK will = HCLK/5 when MCLKSEL = MEMCTL_MCLK_HCLKDIV
    PAC55XX_MEMCTL->MEMCTL.MCLKSEL = MEMCTL_MCLK_HCLKDIV;       // MCLK = HCLK/5 = 30 MHz; allows reading and writing of Flash

    PAC55XX_MEMCTL->FLASHLOCK = 0;                              // Disallow write access to MEMCTL

    // Enable GPIO Input clock synchronization; should always be enabled, especially when using GPIO edge based interrupts
    PAC55XX_GPIOA->CLKSYNC.w = 0xFF;
    PAC55XX_GPIOB->CLKSYNC.w = 0xFF;
    PAC55XX_GPIOC->CLKSYNC.w = 0xFF;
    PAC55XX_GPIOD->CLKSYNC.w = 0xFF;
    PAC55XX_GPIOE->CLKSYNC.w = 0xFF;
    PAC55XX_GPIOF->CLKSYNC.w = 0xFF;
    PAC55XX_GPIOG->CLKSYNC.w = 0xFF;

    // --- End Mandatory System Init from Qorvo

    // Configure SOC Bridge for talking to CAFE
    pac5xxx_tile_socbridge_config(1, 0);

    // System Configuration Power Options
    // Vp = 10V , 440mA-540mA, Charge Pump Enable
    pac5xxx_tile_register_write(ADDR_SYSCONF, 0x01);
    
    // Configure reporting of mcu cycles
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    // Configure error handling
    SCB->CCR |= 0x10;

    /* Initialize Systick per 1ms */
    SysTick_Config(150000); // TODO: Use var

    /* Initialize */
    init_server();
}

void system_reset(void)
{
    pac5xxx_tile_register_write(ADDR_WATCHDOG,
        pac5xxx_tile_register_read(ADDR_WATCHDOG) | 0x80);
}

PAC5XXX_RAMFUNC uint8_t system_get_faults(void)
{
    if (ADC_GetVBus() < VBUS_LOW_THRESHOLD)
    {
        return SYSTEM_FLT_VBUS_UNDERVOLTAGE;
    }
    return SYSTEM_FLT_NONE;
}

PAC5XXX_RAMFUNC bool system_has_faults(void)
{
    if (encoder_get_faults() |
        controller_get_faults() |
        motor_get_faults() |
        gate_driver_get_faults() |
        system_get_faults() |
        planner_get_faults())
    {
        return true;
    }
    return false;
}

void printUsageErrorMsg(uint32_t CFSRValue)
{
	SEGGER_RTT_WriteString(0, "Usage fault: ");
	CFSRValue >>= 16;                  // right shift to lsb
	if((CFSRValue & (1 << 9)) != 0) {
		SEGGER_RTT_WriteString(0, "Divide by zero\r\n");
	}
}

void HardFault_Handler(void)
{
	SEGGER_RTT_WriteString(0, "In Hard Fault Handler\r\n");
	SEGGER_RTT_printf(0, "SCB->HFSR = 0x%08x\r\n", SCB->HFSR);
	if ((SCB->HFSR & (1 << 30)) != 0) {
		SEGGER_RTT_WriteString(0, "Forced Hard Fault\r\n");
		SEGGER_RTT_printf(0, "SCB->CFSR = 0x%08x\r\n", SCB->CFSR );
		if((SCB->CFSR & 0xFFFF0000) != 0) {
			printUsageErrorMsg(SCB->CFSR);
		}
	}
	__ASM volatile("BKPT #01");
	while(1)
	{};
}
