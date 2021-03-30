
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

#include "string.h"
#include <src/encoder/encoder.h>
#include "src/system/system.h"
#include "src/motor/motor.h"
#include "src/observer/observer.h"
#include "src/controller/controller.h"
#include "src/adc/adc.h"
#include "src/nvm/nvm.h"
#include "src/can/can.h"
#include "src/utils/utils.h"
#include "src/uart/uart_lowlevel.h"
#include "src/uart/uart_interface.h"


void UART_WriteAddr(uint8_t addr, int32_t data)
{
    switch (addr)
    {
        case 'P': // pos setpoint
            Controller_SetIqSetpoint(0);
            Controller_SetVelSetpoint(0);
            Controller_SetPosSetpoint(data);
            Controller_SetMode(CTRL_POSITION);
        break;

        case 'V': // vel setpoint
            Controller_SetIqSetpoint(0);
            Controller_SetVelSetpoint(data);
            Controller_SetMode(CTRL_VELOCITY);
            Controller_SetVelSetpoint((float)data);
        break;

        case 'I': // current setpoint
            Controller_SetMode(CTRL_CURRENT);
            Controller_SetIqSetpoint((float)data * ONE_OVER_UART_I_SCALING_FACTOR);
        break;

        case 'G': // velocity integrator gain
            Controller_SetVelIntegratorGain((float)data * ONE_OVER_UART_VEL_INT_SCALING_FACTOR);
        break;

        case 'H': // phase resistance
            Motor_SetPhaseResistance((float)data * ONE_OVER_UART_R_SCALING_FACTOR);
        break;

        case 'L': // phase inductance
            Motor_SetPhaseInductance((float)data * ONE_OVER_UART_L_SCALING_FACTOR);
        break;

        case 'M': // Set is motor gimbal?
            motor_set_is_gimbal((bool)data);
        break;
        
        case 'U': // CAN Baud Rate
            CAN_set_kbit_rate((uint16_t)data);
        break;

        case 'C': // CAN ID
            CAN_set_ID((uint8_t)data);
        break;

        default:
            // No action
        break;
    }
}

int32_t UART_ReadAddr(uint8_t addr)
{
    int32_t ret_val = 0;
    switch (addr)
    {
        case 'b': // vbus value
            ret_val = (int32_t)(ADC_GetVBus() * UART_V_SCALING_FACTOR);
        break;

        case 'e': // controller error
            {uint8_t *error_flags = get_error_flags();
            memcpy(&ret_val, error_flags, sizeof(uint32_t));}
        break;

        case 'o': // encoder pos
            ret_val =  MA_GetAngle();
        break;

        case 'p': // pos estimate
            ret_val = Observer_GetPosEstimate();
        break;

        case 'P': // pos setpoint
            ret_val = Controller_GetPosSetpoint();
        break;

        case 'v': // vel estimate
            ret_val = (int32_t)Observer_GetVelEstimate();
        break;

        case 'V': // vel setpoint
            ret_val = (int32_t)Controller_GetVelSetpoint();
        break;

        case 'i': // current estimate
            ret_val = (int32_t)(Controller_GetIqEstimate() * UART_I_SCALING_FACTOR);
        break;

        case 'I': // current setpoint
            ret_val = (int32_t)(Controller_GetIqSetpoint() * UART_I_SCALING_FACTOR);
        break;

        case 'G': // velocity integrator setpoint
            ret_val = (int32_t)(Controller_GetVelIntegratorGain() * UART_VEL_INT_SCALING_FACTOR);
        break;

        case 'h': // phase resistance
            ret_val = Motor_GetPhaseResistance() * UART_R_SCALING_FACTOR;
        break;

        case 'l': // phase inductance
            ret_val = Motor_GetPhaseInductance() * UART_L_SCALING_FACTOR;
        break;

        case 'U': // CAN Baud Rate
            ret_val = CAN_get_kbit_rate();
        break;

        case 'C': // CAN ID
            ret_val = CAN_get_ID();
        break;

        case 'm': // Is motor gimbal?
            ret_val = motor_is_gimbal();
        break;

        case 'Q': // calibrate
            Controller_SetState(STATE_CALIBRATE);
        break;

        case 'A': // closed loop
            Controller_SetState(STATE_CL_CONTROL);
        break;

        case 'Z': // idle
            Controller_SetState(STATE_IDLE);
        break;

        case 'R': // reset mcu
            system_reset();
        break;

        case 'S': // save config
            NVM_SaveConfig();
        break;

        case 'X': // erase config
            NVM_Erase();
        break;

        default:
            // No action
        break;

    }
    return ret_val;
}

void UART_ProcessMessage(void)
{
    int8_t addr = uart_rx_msg[1];
    int8_t len = ((int8_t)uart_rx_msg_len) - 3;

    // Ensure buffer is null-terminated
    uart_rx_msg[uart_rx_msg_len] = '\0';

    if (len > 0)
    {
        // Write operation
        int32_t n = atol(&(uart_rx_msg)[2]);
        UART_WriteAddr(addr, n);
    }
    else if (len == 0)
    {
        // Read operation
        int32_t val = UART_ReadAddr(uart_rx_msg[1]);
        UART_SendInt32(val);
    }
    else
    {
        // Error
    }
}

void UART_SendInt32(int32_t val)
{
    (void)itoa(val, uart_tx_msg, 10);
    for (uint8_t i=0; i<UART_BYTE_LIMIT; i++)
    {
        if (uart_tx_msg[i] == '\0')
        {
            uart_tx_msg[i] = UART_NEWLINE;
            break;
        }
    }
    // Enable transmit interrupt to send reponse to host
    pac5xxx_uart_int_enable_THREI2(UART_REF, 1);
}
