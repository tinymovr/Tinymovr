/*
* This file was automatically generated using csnake v0.3.4.
*
* This file should not be edited directly, any changes will be
* overwritten next time the script is run.
*
* Source code for csnake is available at:
* https://gitlab.com/andrejr/csnake
*
* csnake is also available on PyPI, at :
* https://pypi.org/project/csnake
*/

#pragma once

#include "src/common.h"

typedef enum
{
    AVLOS_RET_NOACTION,
    AVLOS_RET_READ = 1,
    AVLOS_RET_WRITE = 2
} Avlos_Return;

typedef enum
{
    AVLOS_CMD_WRITE,
    AVLOS_CMD_READ = 1
} Avlos_Command;

static uint32_t avlos_proto_hash = 3354184238;

/*
* avlos_tm_uid
*
* The unique device ID, unique to each PAC55xx chip produced.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_uid(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_Vbus
*
* The bus voltage.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_Vbus(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_temp
*
* The internal temperature of the PAC55xx MCU.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_temp(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_cycles_total
*
* Total processor cycles in a single PWM cycle.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_cycles_total(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_cycles_busy
*
* Busy processor cycles in a single PWM cycle.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_cycles_busy(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_controller_state
*
* The state of the controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_controller_state(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_controller_mode
*
* The control mode of the controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_controller_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_controller_pos_setpoint
*
* The position setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_controller_pos_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_controller_vel_setpoint
*
* The velocity setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_controller_vel_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_controller_Iq_setpoint
*
* The Iq setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_controller_Iq_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_controller_Iq_estimate
*
* The Iq estimate.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_controller_Iq_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_controller_pos_p_gain
*
* The proportional gain of the position controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_controller_pos_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_controller_vel_p_gain
*
* The proportional gain of the velocity controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_controller_vel_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_controller_vel_i_gain
*
* The integral gain of the velocity controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_controller_vel_i_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_controller_Iq_p_gain
*
* The current controller proportional gain.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_controller_Iq_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_controller_vel_integrator_gain
*
* The gain of the velocity integrator.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_controller_vel_integrator_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_controller_vel_integrator_deadband
*
* The deadband of the velocity integrator. A region around the position setpoint where the velocity integrator is not updated.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_controller_vel_integrator_deadband(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_comms_can_rate
*
* The baud rate of the CAN interface.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_comms_can_rate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_comms_can_id
*
* The ID of the CAN interface.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_comms_can_id(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_motor_R
*
* The motor Resistance value.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_motor_R(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_motor_L
*
* The motor Inductance value.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_motor_L(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_motor_pole_pairs
*
* The motor pole pair count.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_motor_pole_pairs(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_motor_type
*
* The type of the motor. Either high current or gimbal.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_motor_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_motor_offset
*
* User-defined offset of the motor.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_motor_offset(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_motor_direction
*
* User-defined direction of the motor.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_motor_direction(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_encoder_position_estimate
*
* The filtered encoder position estimate.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_encoder_position_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_encoder_type
*
* The encoder type. Either INTERNAL or HALL.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_encoder_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_encoder_bandwidth
*
* The encoder observer bandwidth.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_encoder_bandwidth(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_traj_planner_max_accel
*
* The trajectory planner max acceleration.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_traj_planner_max_accel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_traj_planner_max_decel
*
* The trajectory planner max deceleration.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_traj_planner_max_decel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_traj_planner_max_vel
*
* The trajectory planner max cruise velocity.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_traj_planner_max_vel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_traj_planner_pos_setpoint
*
* The position setpoint of the trajectory planner.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_traj_planner_pos_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

static uint8_t (*avlos_endpoints[32])(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd) = {&avlos_tm_uid, &avlos_tm_Vbus, &avlos_tm_temp, &avlos_tm_cycles_total, &avlos_tm_cycles_busy, &avlos_tm_controller_state, &avlos_tm_controller_mode, &avlos_tm_controller_pos_setpoint, &avlos_tm_controller_vel_setpoint, &avlos_tm_controller_Iq_setpoint, &avlos_tm_controller_Iq_estimate, &avlos_tm_controller_pos_p_gain, &avlos_tm_controller_vel_p_gain, &avlos_tm_controller_vel_i_gain, &avlos_tm_controller_Iq_p_gain, &avlos_tm_controller_vel_integrator_gain, &avlos_tm_controller_vel_integrator_deadband, &avlos_tm_comms_can_rate, &avlos_tm_comms_can_id, &avlos_tm_motor_R, &avlos_tm_motor_L, &avlos_tm_motor_pole_pairs, &avlos_tm_motor_type, &avlos_tm_motor_offset, &avlos_tm_motor_direction, &avlos_tm_encoder_position_estimate, &avlos_tm_encoder_type, &avlos_tm_encoder_bandwidth, &avlos_tm_traj_planner_max_accel, &avlos_tm_traj_planner_max_decel, &avlos_tm_traj_planner_max_vel, &avlos_tm_traj_planner_pos_setpoint};
