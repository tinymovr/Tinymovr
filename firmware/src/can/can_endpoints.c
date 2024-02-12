/*
* This file was automatically generated using Avlos.
* https://github.com/tinymovr/avlos
*
* Any changes to this file will be overwritten when
* content is regenerated.
*/
#include <string.h>
#include <src/adc/adc.h>
#include <src/system/system.h>
#include <src/sensor/sensors.h>
#include <src/observer/observer.h>
#include <src/motor/motor.h>
#include <src/scheduler/scheduler.h>
#include <src/controller/controller.h>
#include <src/nvm/nvm.h>
#include <src/watchdog/watchdog.h>
#include <src/can/can_endpoints.h>


uint8_t (*avlos_endpoints[94])(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd) = {&avlos_protocol_hash, &avlos_uid, &avlos_fw_version, &avlos_hw_revision, &avlos_Vbus, &avlos_Ibus, &avlos_power, &avlos_temp, &avlos_calibrated, &avlos_errors, &avlos_warnings, &avlos_save_config, &avlos_erase_config, &avlos_reset, &avlos_enter_dfu, &avlos_config_size, &avlos_scheduler_errors, &avlos_controller_state, &avlos_controller_mode, &avlos_controller_warnings, &avlos_controller_errors, &avlos_controller_position_setpoint, &avlos_controller_position_p_gain, &avlos_controller_velocity_setpoint, &avlos_controller_velocity_limit, &avlos_controller_velocity_p_gain, &avlos_controller_velocity_i_gain, &avlos_controller_velocity_deadband, &avlos_controller_velocity_increment, &avlos_controller_current_Iq_setpoint, &avlos_controller_current_Id_setpoint, &avlos_controller_current_Iq_limit, &avlos_controller_current_Iq_estimate, &avlos_controller_current_bandwidth, &avlos_controller_current_Iq_p_gain, &avlos_controller_current_max_Ibus_regen, &avlos_controller_current_max_Ibrake, &avlos_controller_voltage_Vq_setpoint, &avlos_controller_calibrate, &avlos_controller_idle, &avlos_controller_position_mode, &avlos_controller_velocity_mode, &avlos_controller_current_mode, &avlos_controller_set_pos_vel_setpoints, &avlos_comms_can_rate, &avlos_comms_can_id, &avlos_motor_R, &avlos_motor_L, &avlos_motor_pole_pairs, &avlos_motor_type, &avlos_motor_calibrated, &avlos_motor_I_cal, &avlos_motor_errors, &avlos_sensors_user_frame_position_estimate, &avlos_sensors_user_frame_velocity_estimate, &avlos_sensors_user_frame_offset, &avlos_sensors_user_frame_multiplier, &avlos_sensors_setup_onboard_calibrated, &avlos_sensors_setup_onboard_errors, &avlos_sensors_setup_external_spi_type, &avlos_sensors_setup_external_spi_calibrated, &avlos_sensors_setup_external_spi_errors, &avlos_sensors_setup_hall_calibrated, &avlos_sensors_setup_hall_errors, &avlos_sensors_select_position_sensor_connection, &avlos_sensors_select_position_sensor_bandwidth, &avlos_sensors_select_position_sensor_raw_angle, &avlos_sensors_select_position_sensor_position_estimate, &avlos_sensors_select_position_sensor_velocity_estimate, &avlos_sensors_select_commutation_sensor_connection, &avlos_sensors_select_commutation_sensor_bandwidth, &avlos_sensors_select_commutation_sensor_raw_angle, &avlos_sensors_select_commutation_sensor_position_estimate, &avlos_sensors_select_commutation_sensor_velocity_estimate, &avlos_traj_planner_max_accel, &avlos_traj_planner_max_decel, &avlos_traj_planner_max_vel, &avlos_traj_planner_t_accel, &avlos_traj_planner_t_decel, &avlos_traj_planner_t_total, &avlos_traj_planner_move_to, &avlos_traj_planner_move_to_tlimit, &avlos_traj_planner_errors, &avlos_homing_velocity, &avlos_homing_max_homing_t, &avlos_homing_retract_dist, &avlos_homing_warnings, &avlos_homing_stall_detect_velocity, &avlos_homing_stall_detect_delta_pos, &avlos_homing_stall_detect_t, &avlos_homing_home, &avlos_watchdog_enabled, &avlos_watchdog_triggered, &avlos_watchdog_timeout };
uint32_t avlos_proto_hash = 1958736319;

uint32_t _avlos_get_proto_hash(void)
{
    return avlos_proto_hash;
}

uint8_t avlos_protocol_hash(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint32_t v;
        v = _avlos_get_proto_hash();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_uid(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint32_t v;
        v = system_get_uid();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_fw_version(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        *buffer_len = system_get_fw_version_string((char *)buffer); 
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_hw_revision(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint32_t v;
        v = system_get_hw_revision();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_Vbus(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = system_get_Vbus();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_Ibus(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_Ibus_est();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_power(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_power_est();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_temp(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = ADC_get_mcu_temp();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_calibrated(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        bool v;
        v = system_get_calibrated();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = system_get_errors();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_warnings(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = system_get_warnings();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_save_config(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    nvm_save_config();

    return AVLOS_RET_CALL;
}

uint8_t avlos_erase_config(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    nvm_erase();

    return AVLOS_RET_CALL;
}

uint8_t avlos_reset(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    system_reset();

    return AVLOS_RET_CALL;
}

uint8_t avlos_enter_dfu(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    system_enter_dfu();

    return AVLOS_RET_CALL;
}

uint8_t avlos_config_size(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint32_t v;
        v = system_get_config_size();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_scheduler_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = scheduler_get_errors();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_state(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = controller_get_state();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        uint8_t v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_state(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = controller_get_mode();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        uint8_t v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_mode(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_warnings(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = controller_get_warnings();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = controller_get_errors();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_position_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_pos_setpoint_user_frame();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_pos_setpoint_user_frame(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_position_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_pos_gain();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_pos_gain(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_velocity_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_vel_setpoint_user_frame();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_setpoint_user_frame(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_velocity_limit(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_vel_limit();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_limit(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_velocity_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_vel_gain();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_gain(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_velocity_i_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_vel_integral_gain();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_integral_gain(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_velocity_deadband(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_vel_integral_deadband();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_integral_deadband(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_velocity_increment(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_vel_increment();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_increment(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_current_Iq_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_Iq_setpoint_user_frame();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_Iq_setpoint_user_frame(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_current_Id_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_Id_setpoint_user_frame();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_current_Iq_limit(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_Iq_limit();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_Iq_limit(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_current_Iq_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_Iq_estimate_user_frame();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_current_bandwidth(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_I_bw();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_I_bw(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_current_Iq_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_Iq_gain();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_current_max_Ibus_regen(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_max_Ibus_regen();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_max_Ibus_regen(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_current_max_Ibrake(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_max_Ibrake();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_max_Ibrake(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_voltage_Vq_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_Vq_setpoint_user_frame();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_controller_calibrate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    controller_calibrate();

    return AVLOS_RET_CALL;
}

uint8_t avlos_controller_idle(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    controller_idle();

    return AVLOS_RET_CALL;
}

uint8_t avlos_controller_position_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    controller_position_mode();

    return AVLOS_RET_CALL;
}

uint8_t avlos_controller_velocity_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    controller_velocity_mode();

    return AVLOS_RET_CALL;
}

uint8_t avlos_controller_current_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    controller_current_mode();

    return AVLOS_RET_CALL;
}

uint8_t avlos_controller_set_pos_vel_setpoints(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint8_t _offset = 0;
    float pos_setpoint;
    memcpy(&pos_setpoint, buffer+_offset, sizeof(pos_setpoint));
    _offset += sizeof(pos_setpoint);
    float vel_setpoint;
    memcpy(&vel_setpoint, buffer+_offset, sizeof(vel_setpoint));
    _offset += sizeof(vel_setpoint);
    float ret_val = controller_set_pos_vel_setpoints(pos_setpoint, vel_setpoint);
    memcpy(buffer, &ret_val, sizeof(ret_val));
    *buffer_len = sizeof(ret_val);

    return AVLOS_RET_CALL;
}

uint8_t avlos_comms_can_rate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint32_t v;
        v = CAN_get_kbit_rate();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        uint32_t v;
        memcpy(&v, buffer, sizeof(v));
        CAN_set_kbit_rate(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_comms_can_id(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint32_t v;
        v = CAN_get_ID();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        uint32_t v;
        memcpy(&v, buffer, sizeof(v));
        CAN_set_ID(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_motor_R(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = motor_get_phase_resistance();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        motor_set_phase_resistance(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_motor_L(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = motor_get_phase_inductance();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        motor_set_phase_inductance(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_motor_pole_pairs(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = motor_get_pole_pairs();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        uint8_t v;
        memcpy(&v, buffer, sizeof(v));
        motor_set_pole_pairs(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_motor_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = motor_get_is_gimbal();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        uint8_t v;
        memcpy(&v, buffer, sizeof(v));
        motor_set_is_gimbal(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_motor_calibrated(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        bool v;
        v = motor_get_calibrated();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_motor_I_cal(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = motor_get_I_cal();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        motor_set_I_cal(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_motor_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = motor_get_errors();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_user_frame_position_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = user_frame_get_pos_estimate();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_user_frame_velocity_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = user_frame_get_vel_estimate();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_user_frame_offset(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = get_position_observer_to_user_offset();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        set_position_observer_to_user_offset(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_user_frame_multiplier(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = get_position_observer_to_user_multiplier();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        set_position_observer_to_user_multiplier(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_setup_onboard_calibrated(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        bool v;
        v = sensor_onboard_get_is_calibrated();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_setup_onboard_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = sensor_onboard_get_errors();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_setup_external_spi_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = sensor_external_spi_get_type_avlos();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        uint8_t v;
        memcpy(&v, buffer, sizeof(v));
        sensor_external_spi_set_type_avlos(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_setup_external_spi_calibrated(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        bool v;
        v = sensor_external_spi_get_is_calibrated();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_setup_external_spi_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = sensor_external_spi_get_errors();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_setup_hall_calibrated(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        bool v;
        v = sensor_hall_get_is_calibrated();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_setup_hall_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = sensor_hall_get_errors();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_select_position_sensor_connection(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = position_sensor_get_connection();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        uint8_t v;
        memcpy(&v, buffer, sizeof(v));
        position_sensor_set_connection(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_select_position_sensor_bandwidth(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = position_observer_get_bandwidth();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        position_observer_set_bandwidth(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_select_position_sensor_raw_angle(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        int32_t v;
        v = sensor_position_get_raw_angle();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_select_position_sensor_position_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = position_observer_get_pos_estimate();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_select_position_sensor_velocity_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = position_observer_get_vel_estimate();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_select_commutation_sensor_connection(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = commutation_sensor_get_connection();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        uint8_t v;
        memcpy(&v, buffer, sizeof(v));
        commutation_sensor_set_connection(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_select_commutation_sensor_bandwidth(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = commutation_observer_get_bandwidth();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        commutation_observer_set_bandwidth(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_select_commutation_sensor_raw_angle(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        int32_t v;
        v = sensor_commutation_get_raw_angle();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_select_commutation_sensor_position_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = commutation_observer_get_pos_estimate();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_sensors_select_commutation_sensor_velocity_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = commutation_observer_get_vel_estimate();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_traj_planner_max_accel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = planner_get_max_accel();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        planner_set_max_accel(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_traj_planner_max_decel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = planner_get_max_decel();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        planner_set_max_decel(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_traj_planner_max_vel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = planner_get_max_vel();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        planner_set_max_vel(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_traj_planner_t_accel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = planner_get_deltat_accel();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        planner_set_deltat_accel(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_traj_planner_t_decel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = planner_get_deltat_decel();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        planner_set_deltat_decel(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_traj_planner_t_total(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = planner_get_deltat_total();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        planner_set_deltat_total(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_traj_planner_move_to(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint8_t _offset = 0;
    float pos_setpoint;
    memcpy(&pos_setpoint, buffer+_offset, sizeof(pos_setpoint));
    _offset += sizeof(pos_setpoint);
    planner_move_to_vlimit(pos_setpoint);

    return AVLOS_RET_CALL;
}

uint8_t avlos_traj_planner_move_to_tlimit(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint8_t _offset = 0;
    float pos_setpoint;
    memcpy(&pos_setpoint, buffer+_offset, sizeof(pos_setpoint));
    _offset += sizeof(pos_setpoint);
    planner_move_to_tlimit(pos_setpoint);

    return AVLOS_RET_CALL;
}

uint8_t avlos_traj_planner_errors(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = planner_get_errors();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_homing_velocity(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = homing_planner_get_homing_velocity();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        homing_planner_set_homing_velocity(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_homing_max_homing_t(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = homing_planner_get_max_homing_t();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        homing_planner_set_max_homing_t(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_homing_retract_dist(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = homing_planner_get_retract_distance();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        homing_planner_set_retract_distance(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_homing_warnings(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = homing_planner_get_warnings();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_homing_stall_detect_velocity(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = homing_planner_get_max_stall_vel();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        homing_planner_set_max_stall_vel(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_homing_stall_detect_delta_pos(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = homing_planner_get_max_stall_delta_pos();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        homing_planner_set_max_stall_delta_pos(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_homing_stall_detect_t(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = homing_planner_get_max_stall_t();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        homing_planner_set_max_stall_t(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_homing_home(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    homing_planner_home();

    return AVLOS_RET_CALL;
}

uint8_t avlos_watchdog_enabled(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        bool v;
        v = Watchdog_get_enabled();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        bool v;
        memcpy(&v, buffer, sizeof(v));
        Watchdog_set_enabled(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_watchdog_triggered(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        bool v;
        v = Watchdog_triggered();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_watchdog_timeout(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
if (AVLOS_CMD_READ == cmd) {
        float v;
        v = Watchdog_get_timeout_seconds();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        Watchdog_set_timeout_seconds(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}
