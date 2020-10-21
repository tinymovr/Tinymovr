
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

#include <src/adc/adc.hpp>
#include <src/can/can.hpp>
#include <src/controller/controller.hpp>
#include <src/encoders/MA702.hpp>
#include <src/gatedriver/gatedriver.hpp>
#include <src/motor/motor.hpp>
#include <src/nvm/nvm.hpp>
#include <src/observer/observer.hpp>
#include <src/uart/uart.hpp>
#include "common.hpp"
#include "system/system.hpp"
#include "timer/timer.hpp"
#include "watchdog/watchdog.hpp"

int main(void)
{
  	__disable_irq();
    System_Init();
    MA_Init();
    NVM_Init();
    NVM_LoadConfig(); // This will TRY to deserialize and import config
    UART_Init();
	Observer_Init();
	ADC_Init();
	GateDriver_Init();
	Motor_Init();
	Controller_Init();
    CAN_Init();
    Timer_Init();
    Watchdog_Init();
    __enable_irq();

    while(true)
    {
        __WFI();
        Controller_Update();
    }
}

