
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

#include <src/comms/can/can.hpp>
#include <src/comms/uart/uart.hpp>
#include <system.hpp>

typedef enum
{
    BEFORE_ACLK_DIVIDER                 = 0,        // The DTGCLK is the clock before the TACTL.CLKDIV clock divider
    AFTER_ACLK_DIVIDER                  = 1,        // The DTGCLK is the clock after the TACTL.CLKDIV clock divider
}TXCTL_DTCLK_Type;

typedef enum
{
    TIMER_SLAVE_SYNC_DISABLE            = 0,        // The timer auto reload
    TIMER_SLAVE_SYNC_ENABLE             = 1,        // The timer single shot
}TXCTL_SSYNC_Type;

typedef enum
{
    AUTO_RELOAD                         = 0,        // The timer auto reload
    SINGLE_SHOT                         = 1,        // The timer single shot
}TXCTL_SINGLE_Type;

System::System(void)
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
    PAC55XX_SCC->CCSCTL.ACLKDIV = (ACLK_DIVIDER - 1);
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

	if (ARM_CM_DWT_CTRL != 0) {        // See if DWT is available
		ARM_CM_DEMCR      |= 1u << 24;  // Set bit 24
		ARM_CM_DWT_CYCCNT  = 0;
		ARM_CM_DWT_CTRL   |= 1u << 0;   // Set bit 0
	}

	encoder = Encoder();
	observer = Observer();
	motor = Motor();
	driver = Driver();
	adc = ADC();
	can = CAN();
	uart = UART();
	controller = Controller();
	watchdog = Watchdog();
}

void System::Reset(void)
{
	pac5xxx_tile_register_write(ADDR_WATCHDOG,
		pac5xxx_tile_register_read(ADDR_WATCHDOG) | 0x80);
}

void System::DelayUS(uint32_t us)
{
    pac_delay_asm(us * 16u);
}

void System::InitTimer(void)
{
    // Configure Timer A Controls
    pac5xxx_timer_clock_config(TimerA, TXCTL_CS_ACLK, TXCTL_PS_DIV1);                   // Configure timer clock input for ACLK, /1 divider
    pac5xxx_timer_base_config(TimerA, (ACLK_FREQ_HZ/2/PWM_TIMER_FREQ), AUTO_RELOAD,
            TxCTL_MODE_UPDOWN, TIMER_SLAVE_SYNC_DISABLE);                               // Configure timer frequency and count mode

    // Configure Dead time generators
    PAC55XX_TIMERA->CTL.DTGCLK = BEFORE_ACLK_DIVIDER;                                   // 0--> The DTGCLK is the clock before the TACTL.CLKDIV clock divider.
                                                                                        // 1--> The DTGCLK is the clock after the TACTL.CLKDIV clock divider.

    pac5xxx_dtg_config2(&(PAC55XX_TIMERA->DTGCTL0), RED_DEATH_TIMET, FED_DEATH_TIMET);  // Configure DTGA0 for phase U
    pac5xxx_dtg_config2(&(PAC55XX_TIMERA->DTGCTL1), RED_DEATH_TIMET, FED_DEATH_TIMET);  // Configure DTGA1 for phase V
    pac5xxx_dtg_config2(&(PAC55XX_TIMERA->DTGCTL2), RED_DEATH_TIMET, FED_DEATH_TIMET);  // Configure DTGA2 for phase W

    PAC55XX_TIMERA->CCTR4.CTR = 0;
    PAC55XX_TIMERA->CCTR5.CTR = 0;
    PAC55XX_TIMERA->CCTR6.CTR = 0;
}

void System::HandleADCInterrupt(void)
{
	adc_interrupt = true;
}

void System::HandleCANInterrupt(void)
{
	can_interrupt = true;
}

void System::HandleUARTInterrupt(void)
{
	uart_interrupt = true;
}

void System::WaitForControlLoopInterrupt(void)
{
	// Control loop is synced to ADC measurements
	while (!adc_interrupt)
	{
		// Received an interrupt but it's not ADC.
		// If there are any tasks pending from other interrupts, do them now.
		if (can_interrupt)
		{
			can.ProcessInterrupt();
			can_interrupt = false;
		}
		if (uart_interrupt)
		{
			uart.ProcessInterrupt();
			uart_interrupt = false;
		}
		// Go back to sleep
		__WFI();
	}
	adc_interrupt = false;
}
