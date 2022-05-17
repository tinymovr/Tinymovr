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
extern uint8_t (*avlos_endpoints[35])(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

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
* save_config_3
*
* Save configuration to non-volatile memory.
*
* @param buffer
* @param buffer_len
*/
uint8_t save_config_3(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* erase_config_4
*
* Erase the config stored in non-volatile memory and reset the device.
*
* @param buffer
* @param buffer_len
*/
uint8_t erase_config_4(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* reset_5
*
* Reset the device.
*
* @param buffer
* @param buffer_len
*/
uint8_t reset_5(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* total_6
*
* Total processor cycles in a single PWM cycle.
*
* @param buffer
* @param buffer_len
*/
uint8_t total_6(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* busy_7
*
* Busy processor cycles in a single PWM cycle.
*
* @param buffer
* @param buffer_len
*/
uint8_t busy_7(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* state_8
*
* The state of the controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t state_8(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* mode_9
*
* The control mode of the controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t mode_9(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* pos_setpoint_10
*
* The position setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t pos_setpoint_10(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* vel_setpoint_11
*
* The velocity setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t vel_setpoint_11(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* Iq_setpoint_12
*
* The Iq setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t Iq_setpoint_12(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* Iq_estimate_13
*
* The Iq estimate.
*
* @param buffer
* @param buffer_len
*/
uint8_t Iq_estimate_13(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* pos_p_gain_14
*
* The proportional gain of the position controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t pos_p_gain_14(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* vel_p_gain_15
*
* The proportional gain of the velocity controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t vel_p_gain_15(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* vel_i_gain_16
*
* The integral gain of the velocity controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t vel_i_gain_16(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* Iq_p_gain_17
*
* The current controller proportional gain.
*
* @param buffer
* @param buffer_len
*/
uint8_t Iq_p_gain_17(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* gain_18
*
* The gain of the velocity integrator.
*
* @param buffer
* @param buffer_len
*/
uint8_t gain_18(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* deadband_19
*
* The deadband of the velocity integrator. A region around the position setpoint where the velocity integrator is not updated.
*
* @param buffer
* @param buffer_len
*/
uint8_t deadband_19(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* rate_20
*
* The baud rate of the CAN interface.
*
* @param buffer
* @param buffer_len
*/
uint8_t rate_20(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* id_21
*
* The ID of the CAN interface.
*
* @param buffer
* @param buffer_len
*/
uint8_t id_21(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* R_22
*
* The motor Resistance value.
*
* @param buffer
* @param buffer_len
*/
uint8_t R_22(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* L_23
*
* The motor Inductance value.
*
* @param buffer
* @param buffer_len
*/
uint8_t L_23(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* pole_pairs_24
*
* The motor pole pair count.
*
* @param buffer
* @param buffer_len
*/
uint8_t pole_pairs_24(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* type_25
*
* The type of the motor. Either high current or gimbal.
*
* @param buffer
* @param buffer_len
*/
uint8_t type_25(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* offset_26
*
* User-defined offset of the motor.
*
* @param buffer
* @param buffer_len
*/
uint8_t offset_26(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* direction_27
*
* User-defined direction of the motor.
*
* @param buffer
* @param buffer_len
*/
uint8_t direction_27(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* position_estimate_28
*
* The filtered encoder position estimate.
*
* @param buffer
* @param buffer_len
*/
uint8_t position_estimate_28(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* type_29
*
* The encoder type. Either INTERNAL or HALL.
*
* @param buffer
* @param buffer_len
*/
uint8_t type_29(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* bandwidth_30
*
* The encoder observer bandwidth.
*
* @param buffer
* @param buffer_len
*/
uint8_t bandwidth_30(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* max_accel_31
*
* The trajectory planner max acceleration.
*
* @param buffer
* @param buffer_len
*/
uint8_t max_accel_31(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* max_decel_32
*
* The trajectory planner max deceleration.
*
* @param buffer
* @param buffer_len
*/
uint8_t max_decel_32(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* max_vel_33
*
* The trajectory planner max cruise velocity.
*
* @param buffer
* @param buffer_len
*/
uint8_t max_vel_33(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* pos_setpoint_34
*
* The position setpoint of the trajectory planner.
*
* @param buffer
* @param buffer_len
*/
uint8_t pos_setpoint_34(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);
