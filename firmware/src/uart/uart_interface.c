
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

#include "string.h"
#include "src/system/system.h"
#include "src/motor/motor.h"
#include "src/observer/observer.h"
#include "src/controller/controller.h"
#include "src/controller/trajectory_planner.h"
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
        controller_set_Iq_setpoint_user_frame(0);
        controller_set_vel_setpoint_user_frame(0);
        controller_set_pos_setpoint_user_frame(data);
        controller_set_mode(CONTROLLER_MODE_POSITION);
        break;

    case 'V': // vel setpoint
        controller_set_Iq_setpoint_user_frame(0);
        controller_set_vel_setpoint_user_frame(data);
        controller_set_mode(CONTROLLER_MODE_VELOCITY);
        controller_set_vel_setpoint_user_frame((float)data);
        break;

    case 'I': // current setpoint
        controller_set_mode(CONTROLLER_MODE_CURRENT);
        controller_set_Iq_setpoint_user_frame((float)data * ONE_OVER_UART_I_SCALING_FACTOR);
        break;

    case 'G': // velocity integrator gain
        controller_set_vel_integral_gain((float)data * ONE_OVER_UART_VEL_INT_SCALING_FACTOR);
        break;

    case 'Y': // Position gain
        controller_set_pos_gain(data);
        break;

    case 'F': // Velocity gain
        controller_set_vel_gain(data * ONE_OVER_UART_VEL_GAIN_SCALING_FACTOR);
        break;

    case 'H': // phase resistance
        motor_set_phase_resistance((float)data * ONE_OVER_UART_R_SCALING_FACTOR);
        break;

    case 'L': // phase inductance
        motor_set_phase_inductance((float)data * ONE_OVER_UART_L_SCALING_FACTOR);
        break;

    case 'M': // Set is motor gimbal?
        motor_set_is_gimbal((bool)data);
        break;

    case 'W': // Set Iq Limit
        controller_set_Iq_limit((float)data * ONE_OVER_UART_IQ_LIMIT_SCALING_FACTOR);
        break;
        
    case 'U': // CAN Baud Rate
        CAN_set_kbit_rate((uint16_t)data);
        break;

    case 'C': // CAN ID
        CAN_set_ID((uint8_t)data);
        break;

    case '<': // Max Decel
        planner_set_max_decel((float)data);
        break;

    case '>': // Max Accel
        planner_set_max_accel((float)data);
        break;

    case '^': // Max Vel
        planner_set_max_vel((float)data);
        break;

    case 'T': // Plan trajectory
        planner_move_to_vlimit((float)data);
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
        ret_val = (int32_t)(system_get_Vbus() * UART_V_SCALING_FACTOR);
        break;

    case 'e': // controller error
    {
        // pass
    }
    break;

    case 'p': // pos estimate
        ret_val = user_frame_get_pos_estimate();
        break;

    case 'P': // pos setpoint
        ret_val = controller_get_pos_setpoint_user_frame();
        break;

    case 'v': // vel estimate
        ret_val = (int32_t)user_frame_get_vel_estimate();
        break;

    case 'V': // vel setpoint
        ret_val = (int32_t)controller_get_vel_setpoint_user_frame();
        break;

    case 'i': // current estimate
        ret_val = (int32_t)(controller_get_Iq_estimate_user_frame() * UART_I_SCALING_FACTOR);
        break;

    case 'I': // current setpoint
        ret_val = (int32_t)(controller_get_Iq_setpoint_user_frame() * UART_I_SCALING_FACTOR);
        break;

    case 'G': // velocity integrator setpoint
        ret_val = (int32_t)(controller_get_vel_integral_gain() * UART_VEL_INT_SCALING_FACTOR);
        break;

    case 'H': // phase resistance
        ret_val = (int32_t)(motor_get_phase_resistance() * UART_R_SCALING_FACTOR);
        break;

    case 'L': // phase inductance
        ret_val = (int32_t)(motor_get_phase_inductance() * UART_L_SCALING_FACTOR);
        break;

    case 'W': // Get Iq Limit
        ret_val = (int32_t)(controller_get_Iq_limit() * UART_IQ_LIMIT_SCALING_FACTOR);
        break;

    case 'C': // CAN ID
        ret_val = CAN_get_ID();
        break;

    case 'M': // Is motor gimbal?
        ret_val = motor_get_is_gimbal();
        break;

    case 'Y': //
        ret_val = controller_get_pos_gain();
        break;

    case 'F': //
        ret_val = controller_get_vel_gain() * UART_VEL_GAIN_SCALING_FACTOR;
        break;

    case 'Q': // calibrate
        controller_set_state(CONTROLLER_STATE_CALIBRATE);
        break;

    case 'A': // closed loop
        controller_set_state(CONTROLLER_STATE_CL_CONTROL);
        break;

    case 'Z': // idle
        controller_set_state(CONTROLLER_STATE_IDLE);
        break;

    case 'R': // reset mcu
        system_reset();
        break;

    case 'S': // save config
        nvm_save_config();
        break;

    case 'X': // erase config
        nvm_erase();
        break;

    default:
        // No action
        break;
    }
    return ret_val;
}

void UART_process_message(void)
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
    for (uint8_t i = 0; i < UART_BYTE_LIMIT; i++)
    {
        if (uart_tx_msg[i] == '\0')
        {
            uart_tx_msg[i] = UART_LINEFEED;
            break;
        }
    }
    // Enable transmit interrupt to send reponse to host
    pac5xxx_uart_int_enable_THREI2(UART_REF, 1);
}
