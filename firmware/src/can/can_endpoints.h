/*
* This file was automatically generated using Avlos.
* https://github.com/tinymovr/avlos
*
* Any changes to this file will be overwritten when
* content is regenerated.
*/

#pragma once
#include <src/common.h>
#include <src/tm_enums.h>

static const uint32_t avlos_proto_hash = 1031937702;
extern uint8_t (*avlos_endpoints[97])(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);
extern uint32_t _avlos_get_proto_hash(void);

/*
* avlos_protocol_hash
*
* The Avlos protocol hash.
*
* Endpoint ID: 0
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
* Endpoint ID: 1
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_uid(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_fw_version
*
* The firmware version.
*
* Endpoint ID: 2
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_fw_version(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_hw_revision
*
* The hardware revision.
*
* Endpoint ID: 3
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_hw_revision(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_Vbus
*
* The measured bus voltage.
*
* Endpoint ID: 4
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_Vbus(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_Ibus
*
* The estimated bus current. Only estimates current drawn by motor.
*
* Endpoint ID: 5
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_Ibus(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_power
*
* The estimated power. Only estimates power drawn by motor.
*
* Endpoint ID: 6
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_power(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_temp
*
* The internal temperature of the PAC55xx MCU.
*
* Endpoint ID: 7
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
* Endpoint ID: 8
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
* Endpoint ID: 9
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_warnings
*
* Any system warnings, as a bitmask
*
* Endpoint ID: 10
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_warnings(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_save_config
*
* Save configuration to non-volatile memory.
*
* Endpoint ID: 11
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
* Endpoint ID: 12
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
* Endpoint ID: 13
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_reset(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_enter_dfu
*
* Enter DFU mode.
*
* Endpoint ID: 14
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_enter_dfu(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_config_size
*
* Size (in bytes) of the configuration object.
*
* Endpoint ID: 15
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_config_size(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_scheduler_load
*
* Processor load in ticks per PWM cycle.
*
* Endpoint ID: 16
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_scheduler_load(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_scheduler_warnings
*
* Any scheduler warnings, as a bitmask
*
* Endpoint ID: 17
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_scheduler_warnings(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_state
*
* The state of the controller.
*
* Endpoint ID: 18
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
* Endpoint ID: 19
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
* Endpoint ID: 20
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
* Endpoint ID: 21
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_position_setpoint
*
* The position setpoint in the user reference frame.
*
* Endpoint ID: 22
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
* Endpoint ID: 23
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_position_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_velocity_setpoint
*
* The velocity setpoint in the user reference frame.
*
* Endpoint ID: 24
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
* Endpoint ID: 25
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
* Endpoint ID: 26
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
* Endpoint ID: 27
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
* Endpoint ID: 28
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
* Endpoint ID: 29
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_velocity_increment(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_Iq_setpoint
*
* The Iq setpoint in the user reference frame.
*
* Endpoint ID: 30
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_Iq_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_Id_setpoint
*
* The Id setpoint in the user reference frame.
*
* Endpoint ID: 31
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_Id_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_Iq_limit
*
* The Iq limit.
*
* Endpoint ID: 32
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_Iq_limit(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_Iq_estimate
*
* The Iq estimate in the user reference frame.
*
* Endpoint ID: 33
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
* Endpoint ID: 34
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
* Endpoint ID: 35
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_Iq_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_max_Ibus_regen
*
* The max current allowed to be fed back to the power source before flux braking activates.
*
* Endpoint ID: 36
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_max_Ibus_regen(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_current_max_Ibrake
*
* The max current allowed to be dumped to the motor windings during flux braking. Set to zero to deactivate flux braking.
*
* Endpoint ID: 37
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_max_Ibrake(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_voltage_Vq_setpoint
*
* The Vq setpoint.
*
* Endpoint ID: 38
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
* Endpoint ID: 39
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
* Endpoint ID: 40
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
* Endpoint ID: 41
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
* Endpoint ID: 42
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
* Endpoint ID: 43
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_controller_current_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_controller_set_pos_vel_setpoints
*
* Set the position and velocity setpoints in the user reference frame in one go, and retrieve the position estimate
*
* Endpoint ID: 44
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
* Endpoint ID: 45
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
* Endpoint ID: 46
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_comms_can_id(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_comms_can_heartbeat
*
* Toggle sending of heartbeat messages.
*
* Endpoint ID: 47
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_comms_can_heartbeat(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_motor_R
*
* The motor Resistance value.
*
* Endpoint ID: 48
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
* Endpoint ID: 49
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
* Endpoint ID: 50
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
* Endpoint ID: 51
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_motor_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_motor_calibrated
*
* Whether the motor has been calibrated.
*
* Endpoint ID: 52
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
* Endpoint ID: 53
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
* Endpoint ID: 54
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_motor_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_user_frame_position_estimate
*
* The filtered position estimate in the user reference frame.
*
* Endpoint ID: 55
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_user_frame_position_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_user_frame_velocity_estimate
*
* The filtered velocity estimate in the user reference frame.
*
* Endpoint ID: 56
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_user_frame_velocity_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_user_frame_offset
*
* The user defined offset.
*
* Endpoint ID: 57
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_user_frame_offset(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_user_frame_multiplier
*
* The user defined multipler.
*
* Endpoint ID: 58
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_user_frame_multiplier(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_setup_onboard_calibrated
*
* Whether the sensor has been calibrated.
*
* Endpoint ID: 59
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_setup_onboard_calibrated(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_setup_onboard_errors
*
* Any sensor errors, as a bitmask
*
* Endpoint ID: 60
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_setup_onboard_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_setup_external_spi_type
*
* The type of the external sensor.
*
* Endpoint ID: 61
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_setup_external_spi_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_setup_external_spi_rate
*
* The rate of the external sensor.
*
* Endpoint ID: 62
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_setup_external_spi_rate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_setup_external_spi_calibrated
*
* Whether the sensor has been calibrated.
*
* Endpoint ID: 63
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_setup_external_spi_calibrated(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_setup_external_spi_errors
*
* Any sensor errors, as a bitmask
*
* Endpoint ID: 64
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_setup_external_spi_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_setup_hall_calibrated
*
* Whether the sensor has been calibrated.
*
* Endpoint ID: 65
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_setup_hall_calibrated(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_setup_hall_errors
*
* Any sensor errors, as a bitmask
*
* Endpoint ID: 66
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_setup_hall_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_select_position_sensor_connection
*
* The position sensor connection. Either ONBOARD, EXTERNAL_SPI or HALL.
*
* Endpoint ID: 67
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_select_position_sensor_connection(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_select_position_sensor_bandwidth
*
* The position sensor observer bandwidth.
*
* Endpoint ID: 68
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_select_position_sensor_bandwidth(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_select_position_sensor_raw_angle
*
* The raw position sensor angle.
*
* Endpoint ID: 69
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_select_position_sensor_raw_angle(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_select_position_sensor_position_estimate
*
* The filtered position estimate in the position sensor reference frame.
*
* Endpoint ID: 70
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_select_position_sensor_position_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_select_position_sensor_velocity_estimate
*
* The filtered velocity estimate in the position sensor reference frame.
*
* Endpoint ID: 71
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_select_position_sensor_velocity_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_select_commutation_sensor_connection
*
* The commutation sensor connection. Either ONBOARD, EXTERNAL_SPI or HALL.
*
* Endpoint ID: 72
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_select_commutation_sensor_connection(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_select_commutation_sensor_bandwidth
*
* The commutation sensor observer bandwidth.
*
* Endpoint ID: 73
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_select_commutation_sensor_bandwidth(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_select_commutation_sensor_raw_angle
*
* The raw commutation sensor angle.
*
* Endpoint ID: 74
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_select_commutation_sensor_raw_angle(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_select_commutation_sensor_position_estimate
*
* The filtered position estimate in the commutation sensor reference frame.
*
* Endpoint ID: 75
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_select_commutation_sensor_position_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_sensors_select_commutation_sensor_velocity_estimate
*
* The filtered velocity estimate in the commutation sensor reference frame.
*
* Endpoint ID: 76
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_sensors_select_commutation_sensor_velocity_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_max_accel
*
* The max allowed acceleration of the generated trajectory.
*
* Endpoint ID: 77
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_max_accel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_max_decel
*
* The max allowed deceleration of the generated trajectory.
*
* Endpoint ID: 78
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_max_decel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_max_vel
*
* The max allowed cruise velocity of the generated trajectory.
*
* Endpoint ID: 79
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_max_vel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_t_accel
*
* In time mode, the acceleration time of the generated trajectory.
*
* Endpoint ID: 80
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_t_accel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_t_decel
*
* In time mode, the deceleration time of the generated trajectory.
*
* Endpoint ID: 81
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_t_decel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_t_total
*
* In time mode, the total time of the generated trajectory.
*
* Endpoint ID: 82
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_t_total(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_move_to
*
* Move to target position in the user reference frame respecting velocity and acceleration limits.
*
* Endpoint ID: 83
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_move_to(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_traj_planner_move_to_tlimit
*
* Move to target position in the user reference frame respecting time limits for each sector.
*
* Endpoint ID: 84
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
* Endpoint ID: 85
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_traj_planner_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_homing_velocity
*
* The velocity at which the motor performs homing.
*
* Endpoint ID: 86
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_homing_velocity(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_homing_max_homing_t
*
* The maximum time the motor is allowed to travel before homing times out and aborts.
*
* Endpoint ID: 87
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_homing_max_homing_t(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_homing_retract_dist
*
* The retraction distance the motor travels after the endstop has been found.
*
* Endpoint ID: 88
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_homing_retract_dist(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_homing_warnings
*
* Any homing warnings, as a bitmask
*
* Endpoint ID: 89
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_homing_warnings(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_homing_stall_detect_velocity
*
* The velocity below which (and together with `stall_detect.delta_pos`) stall detection mode is triggered.
*
* Endpoint ID: 90
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_homing_stall_detect_velocity(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_homing_stall_detect_delta_pos
*
* The velocity below which (and together with `stall_detect.delta_pos`) stall detection mode is triggered.
*
* Endpoint ID: 91
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_homing_stall_detect_delta_pos(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_homing_stall_detect_t
*
* The time to remain in stall detection mode before the motor is considered stalled.
*
* Endpoint ID: 92
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_homing_stall_detect_t(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_homing_home
*
* Perform the homing operation.
*
* Endpoint ID: 93
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_homing_home(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_watchdog_enabled
*
* Whether the watchdog is enabled or not.
*
* Endpoint ID: 94
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
* Endpoint ID: 95
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
* Endpoint ID: 96
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_watchdog_timeout(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);
