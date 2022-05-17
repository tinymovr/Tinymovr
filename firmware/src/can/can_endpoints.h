/*
* This file was automatically generated using Avlos.
* https://github.com/tinymovr/avlos
*
* Any changes to this file will be overwritten when
* content is regenerated.
*/

#pragma once
#include <src/common.h>

typedef enum
{
    AVLOS_RET_NOACTION,
    AVLOS_RET_READ = 1,
    AVLOS_RET_WRITE = 2,
    AVLOS_RET_CALL = 3
} Avlos_Return;

typedef enum
{
    AVLOS_CMD_WRITE,
    AVLOS_CMD_READ = 1
} Avlos_Command;

extern uint32_t avlos_proto_hash;
extern uint8_t (*avlos_endpoints[33])(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* uid_0
*
* The unique device ID, unique to each PAC55xx chip produced.
*
* @param buffer
* @param buffer_len
*/
uint8_t uid_0(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* Vbus_1
*
* The bus voltage.
*
* @param buffer
* @param buffer_len
*/
uint8_t Vbus_1(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* temp_2
*
* The internal temperature of the PAC55xx MCU.
*
* @param buffer
* @param buffer_len
*/
uint8_t temp_2(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* reset_3
*
* Reset the device.
*
* @param buffer
* @param buffer_len
*/
uint8_t reset_3(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* total_4
*
* Total processor cycles in a single PWM cycle.
*
* @param buffer
* @param buffer_len
*/
uint8_t total_4(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* busy_5
*
* Busy processor cycles in a single PWM cycle.
*
* @param buffer
* @param buffer_len
*/
uint8_t busy_5(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* state_6
*
* The state of the controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t state_6(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* mode_7
*
* The control mode of the controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t mode_7(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* pos_setpoint_8
*
* The position setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t pos_setpoint_8(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* vel_setpoint_9
*
* The velocity setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t vel_setpoint_9(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* Iq_setpoint_10
*
* The Iq setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t Iq_setpoint_10(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* Iq_estimate_11
*
* The Iq estimate.
*
* @param buffer
* @param buffer_len
*/
uint8_t Iq_estimate_11(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* pos_p_gain_12
*
* The proportional gain of the position controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t pos_p_gain_12(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* vel_p_gain_13
*
* The proportional gain of the velocity controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t vel_p_gain_13(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* vel_i_gain_14
*
* The integral gain of the velocity controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t vel_i_gain_14(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* Iq_p_gain_15
*
* The current controller proportional gain.
*
* @param buffer
* @param buffer_len
*/
uint8_t Iq_p_gain_15(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* gain_16
*
* The gain of the velocity integrator.
*
* @param buffer
* @param buffer_len
*/
uint8_t gain_16(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* deadband_17
*
* The deadband of the velocity integrator. A region around the position setpoint where the velocity integrator is not updated.
*
* @param buffer
* @param buffer_len
*/
uint8_t deadband_17(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* rate_18
*
* The baud rate of the CAN interface.
*
* @param buffer
* @param buffer_len
*/
uint8_t rate_18(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* id_19
*
* The ID of the CAN interface.
*
* @param buffer
* @param buffer_len
*/
uint8_t id_19(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* R_20
*
* The motor Resistance value.
*
* @param buffer
* @param buffer_len
*/
uint8_t R_20(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* L_21
*
* The motor Inductance value.
*
* @param buffer
* @param buffer_len
*/
uint8_t L_21(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* pole_pairs_22
*
* The motor pole pair count.
*
* @param buffer
* @param buffer_len
*/
uint8_t pole_pairs_22(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* type_23
*
* The type of the motor. Either high current or gimbal.
*
* @param buffer
* @param buffer_len
*/
uint8_t type_23(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* offset_24
*
* User-defined offset of the motor.
*
* @param buffer
* @param buffer_len
*/
uint8_t offset_24(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* direction_25
*
* User-defined direction of the motor.
*
* @param buffer
* @param buffer_len
*/
uint8_t direction_25(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* position_estimate_26
*
* The filtered encoder position estimate.
*
* @param buffer
* @param buffer_len
*/
uint8_t position_estimate_26(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* type_27
*
* The encoder type. Either INTERNAL or HALL.
*
* @param buffer
* @param buffer_len
*/
uint8_t type_27(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* bandwidth_28
*
* The encoder observer bandwidth.
*
* @param buffer
* @param buffer_len
*/
uint8_t bandwidth_28(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* max_accel_29
*
* The trajectory planner max acceleration.
*
* @param buffer
* @param buffer_len
*/
uint8_t max_accel_29(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* max_decel_30
*
* The trajectory planner max deceleration.
*
* @param buffer
* @param buffer_len
*/
uint8_t max_decel_30(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* max_vel_31
*
* The trajectory planner max cruise velocity.
*
* @param buffer
* @param buffer_len
*/
uint8_t max_vel_31(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* pos_setpoint_32
*
* The position setpoint of the trajectory planner.
*
* @param buffer
* @param buffer_len
*/
uint8_t pos_setpoint_32(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);
