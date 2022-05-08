
#pragma once
#include <src/common.h>

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

extern uint32_t avlos_proto_hash;
extern uint8_t (*avlos_endpoints[32])(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

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
* total_3
*
* Total processor cycles in a single PWM cycle.
*
* @param buffer
* @param buffer_len
*/
uint8_t total_3(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* busy_4
*
* Busy processor cycles in a single PWM cycle.
*
* @param buffer
* @param buffer_len
*/
uint8_t busy_4(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* state_5
*
* The state of the controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t state_5(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* mode_6
*
* The control mode of the controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t mode_6(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* pos_setpoint_7
*
* The position setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t pos_setpoint_7(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* vel_setpoint_8
*
* The velocity setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t vel_setpoint_8(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* Iq_setpoint_9
*
* The Iq setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t Iq_setpoint_9(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* Iq_estimate_10
*
* The Iq estimate.
*
* @param buffer
* @param buffer_len
*/
uint8_t Iq_estimate_10(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* pos_p_gain_11
*
* The proportional gain of the position controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t pos_p_gain_11(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* vel_p_gain_12
*
* The proportional gain of the velocity controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t vel_p_gain_12(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* vel_i_gain_13
*
* The integral gain of the velocity controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t vel_i_gain_13(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* Iq_p_gain_14
*
* The current controller proportional gain.
*
* @param buffer
* @param buffer_len
*/
uint8_t Iq_p_gain_14(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* gain_15
*
* The gain of the velocity integrator.
*
* @param buffer
* @param buffer_len
*/
uint8_t gain_15(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* deadband_16
*
* The deadband of the velocity integrator. A region around the position setpoint where the velocity integrator is not updated.
*
* @param buffer
* @param buffer_len
*/
uint8_t deadband_16(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* rate_17
*
* The baud rate of the CAN interface.
*
* @param buffer
* @param buffer_len
*/
uint8_t rate_17(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* id_18
*
* The ID of the CAN interface.
*
* @param buffer
* @param buffer_len
*/
uint8_t id_18(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* R_19
*
* The motor Resistance value.
*
* @param buffer
* @param buffer_len
*/
uint8_t R_19(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* L_20
*
* The motor Inductance value.
*
* @param buffer
* @param buffer_len
*/
uint8_t L_20(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* pole_pairs_21
*
* The motor pole pair count.
*
* @param buffer
* @param buffer_len
*/
uint8_t pole_pairs_21(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* type_22
*
* The type of the motor. Either high current or gimbal.
*
* @param buffer
* @param buffer_len
*/
uint8_t type_22(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* offset_23
*
* User-defined offset of the motor.
*
* @param buffer
* @param buffer_len
*/
uint8_t offset_23(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* direction_24
*
* User-defined direction of the motor.
*
* @param buffer
* @param buffer_len
*/
uint8_t direction_24(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* position_estimate_25
*
* The filtered encoder position estimate.
*
* @param buffer
* @param buffer_len
*/
uint8_t position_estimate_25(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* type_26
*
* The encoder type. Either INTERNAL or HALL.
*
* @param buffer
* @param buffer_len
*/
uint8_t type_26(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* bandwidth_27
*
* The encoder observer bandwidth.
*
* @param buffer
* @param buffer_len
*/
uint8_t bandwidth_27(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* max_accel_28
*
* The trajectory planner max acceleration.
*
* @param buffer
* @param buffer_len
*/
uint8_t max_accel_28(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* max_decel_29
*
* The trajectory planner max deceleration.
*
* @param buffer
* @param buffer_len
*/
uint8_t max_decel_29(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* max_vel_30
*
* The trajectory planner max cruise velocity.
*
* @param buffer
* @param buffer_len
*/
uint8_t max_vel_30(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* pos_setpoint_31
*
* The position setpoint of the trajectory planner.
*
* @param buffer
* @param buffer_len
*/
uint8_t pos_setpoint_31(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);
