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

typedef enum
{
    ERRORS_NONE = 0,
    ERRORS_UNDERVOLTAGE = (1 << 0), 
    ERRORS_DRIVER_FAULT = (1 << 1)
} errors_flags;

typedef enum
{
    SCHEDULER_ERRORS_NONE = 0,
    SCHEDULER_ERRORS_CONTROL_BLOCK_REENTERED = (1 << 0)
} scheduler_errors_flags;

typedef enum
{
    CONTROLLER_WARNINGS_NONE = 0,
    CONTROLLER_WARNINGS_VELOCITY_LIMITED = (1 << 0), 
    CONTROLLER_WARNINGS_CURRENT_LIMITED = (1 << 1), 
    CONTROLLER_WARNINGS_MODULATION_LIMITED = (1 << 2)
} controller_warnings_flags;

typedef enum
{
    CONTROLLER_ERRORS_NONE = 0,
    CONTROLLER_ERRORS_CURRENT_LIMIT_EXCEEDED = (1 << 0)
} controller_errors_flags;

typedef enum
{
    MOTOR_ERRORS_NONE = 0,
    MOTOR_ERRORS_PHASE_RESISTANCE_OUT_OF_RANGE = (1 << 0), 
    MOTOR_ERRORS_PHASE_INDUCTANCE_OUT_OF_RANGE = (1 << 1), 
    MOTOR_ERRORS_INVALID_POLE_PAIRS = (1 << 2)
} motor_errors_flags;

typedef enum
{
    ENCODER_ERRORS_NONE = 0,
    ENCODER_ERRORS_CALIBRATION_FAILED = (1 << 0), 
    ENCODER_ERRORS_READING_UNSTABLE = (1 << 1)
} encoder_errors_flags;

typedef enum
{
    TRAJ_PLANNER_ERRORS_NONE = 0,
    TRAJ_PLANNER_ERRORS_INVALID_INPUT = (1 << 0), 
    TRAJ_PLANNER_ERRORS_VCRUISE_OVER_LIMIT = (1 << 1)
} traj_planner_errors_flags;

extern uint32_t avlos_proto_hash;
extern uint8_t (*avlos_endpoints[64])(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);
extern uint32_t _avlos_get_proto_hash(void);

/*
* avlos_protocol_hash
*
* The Avlos protocol hash.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_protocol_hash(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_uid
*
* The unique device ID, unique to each PAC55xx chip produced.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_uid(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_Vbus
*
* The bus voltage.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_Vbus(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_temp
*
* The internal temperature of the PAC55xx MCU.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_temp(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_calibrated
*
* Whether the system has been calibrated.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_calibrated(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_errors
*
* Any system errors, as a bitmask
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_save_config
*
* Save configuration to non-volatile memory.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_save_config(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_erase_config
*
* Erase the config stored in non-volatile memory and reset the device.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_erase_config(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_reset
*
* Reset the device.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_reset(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_scheduler_total
*
* Total processor cycles in a single PWM cycle.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_scheduler_total(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_scheduler_busy
*
* Busy processor cycles in a single PWM cycle.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_scheduler_busy(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_scheduler_errors
*
* Any scheduler errors, as a bitmask
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_scheduler_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_state
*
* The state of the controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_state(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_mode
*
* The control mode of the controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_warnings
*
* Any controller warnings, as a bitmask
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_warnings(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_errors
*
* Any controller errors, as a bitmask
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_position_setpoint
*
* The position setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_position_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_position_p_gain
*
* The proportional gain of the position controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_position_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_velocity_setpoint
*
* The velocity setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_velocity_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_velocity_limit
*
* The velocity limit.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_velocity_limit(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_velocity_p_gain
*
* The proportional gain of the velocity controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_velocity_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_velocity_i_gain
*
* The integral gain of the velocity controller.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_velocity_i_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_velocity_deadband
*
* The deadband of the velocity integrator. A region around the position setpoint where the velocity integrator is not updated.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_velocity_deadband(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_velocity_increment
*
* Max velocity setpoint increment (ramping) rate. Set to 0 to disable.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_velocity_increment(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_Iq_setpoint
*
* The Iq setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_Iq_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_Iq_limit
*
* The Iq limit.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_Iq_limit(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_Iq_estimate
*
* The Iq estimate.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_Iq_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_bandwidth
*
* The current controller bandwidth.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_bandwidth(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_Iq_p_gain
*
* The current controller proportional gain.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_Iq_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_max_Iq_feedback
*
* The max current allowed to be fed back to the power source before flux braking activates.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_max_Iq_feedback(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_max_Id_dump
*
* The max current allowed to be dumped to the motor windings during flux braking. Set to zero to deactivate flux braking.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_max_Id_dump(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_voltage_Vq_setpoint
*
* The Vq setpoint.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_voltage_Vq_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_calibrate
*
* Calibrate the device.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_calibrate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_idle
*
* Set idle mode, disabling the driver.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_idle(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_position_mode
*
* Set position control mode.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_position_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_velocity_mode
*
* Set velocity control mode.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_velocity_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_mode
*
* Set current control mode.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_set_pos_vel_setpoints
*
* Set the position and velocity setpoints in one go, and retrieve the position estimate
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_set_pos_vel_setpoints(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_comms_can_rate
*
* The baud rate of the CAN interface.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_comms_can_rate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_comms_can_id
*
* The ID of the CAN interface.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_comms_can_id(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_motor_R
*
* The motor Resistance value.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_motor_R(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_motor_L
*
* The motor Inductance value.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_motor_L(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_motor_pole_pairs
*
* The motor pole pair count.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_motor_pole_pairs(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_motor_type
*
* The type of the motor. Either high current or gimbal.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_motor_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_motor_offset
*
* User-defined offset of the motor.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_motor_offset(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_motor_direction
*
* User-defined direction of the motor.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_motor_direction(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_motor_calibrated
*
* Whether the motor has been calibrated.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_motor_calibrated(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_motor_I_cal
*
* The calibration current.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_motor_I_cal(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_motor_errors
*
* Any motor/calibration errors, as a bitmask
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_motor_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_encoder_position_estimate
*
* The filtered encoder position estimate.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_encoder_position_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_encoder_velocity_estimate
*
* The filtered encoder velocity estimate.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_encoder_velocity_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_encoder_type
*
* The encoder type. Either INTERNAL or HALL.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_encoder_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_encoder_bandwidth
*
* The encoder observer bandwidth.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_encoder_bandwidth(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_encoder_calibrated
*
* Whether the encoder has been calibrated.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_encoder_calibrated(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_encoder_errors
*
* Any encoder errors, as a bitmask
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_encoder_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_max_accel
*
* The trajectory planner max acceleration.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_max_accel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_max_decel
*
* The trajectory planner max deceleration.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_max_decel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_max_vel
*
* The trajectory planner max cruise velocity.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_max_vel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_move_to
*
* Move to target position respecting velocity and acceleration limits.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_move_to(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_move_to_tlimit
*
* Move to target position respecting time limits for each sector.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_move_to_tlimit(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_errors
*
* Any errors in the trajectory planner, as a bitmask
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_watchdog_enabled
*
* Whether the watchdog is enabled or not.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_watchdog_enabled(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_watchdog_triggered
*
* Whether the watchdog has been triggered or not.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_watchdog_triggered(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_watchdog_timeout
*
* The watchdog timeout period.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_watchdog_timeout(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);
