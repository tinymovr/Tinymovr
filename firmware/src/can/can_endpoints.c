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
#include <src/encoder/encoder.h>
#include <src/observer/observer.h>
#include <src/motor/motor.h>
#include <src/scheduler/scheduler.h>
#include <src/controller/controller.h>
#include <src/nvm/nvm.h>
#include <src/can/can_endpoints.h>


uint8_t (*avlos_endpoints[41])(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd) = {&uid_0, &Vbus_1, &temp_2, &save_config_3, &erase_config_4, &reset_5, &total_6, &busy_7, &state_8, &mode_9, &pos_setpoint_10, &vel_setpoint_11, &Iq_setpoint_12, &Iq_estimate_13, &pos_p_gain_14, &vel_p_gain_15, &vel_i_gain_16, &Iq_p_gain_17, &gain_18, &deadband_19, &calibrate_20, &idle_21, &position_mode_22, &velocity_mode_23, &current_mode_24, &rate_25, &id_26, &R_27, &L_28, &pole_pairs_29, &type_30, &offset_31, &direction_32, &position_estimate_33, &type_34, &bandwidth_35, &max_accel_36, &max_decel_37, &max_vel_38, &move_to_39, &move_to_tlimit_40 };
uint32_t avlos_proto_hash = 1322105449;

uint8_t uid(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t Vbus(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (AVLOS_CMD_READ == cmd) {
        float v;
        v = adc_get_Vbus();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t temp(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (AVLOS_CMD_READ == cmd) {
        float v;
        v = adc_get_mcu_temp();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t save_config(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    nvm_save_config();

    return AVLOS_RET_CALL;
}

uint8_t erase_config(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    nvm_erase();

    return AVLOS_RET_CALL;
}

uint8_t reset(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    system_reset();

    return AVLOS_RET_CALL;
}

uint8_t cycles_total(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (AVLOS_CMD_READ == cmd) {
        uint32_t v;
        v = Scheduler_GetTotalCycles();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t cycles_busy(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (AVLOS_CMD_READ == cmd) {
        uint32_t v;
        v = Scheduler_GetBusyCycles();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t controller_state(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t controller_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t controller_pos_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t controller_vel_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t controller_Iq_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t controller_Iq_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t controller_pos_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t controller_vel_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t controller_vel_i_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_vel_integrator_gain();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_integrator_gain(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t controller_Iq_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t controller_vel_integrator_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_vel_integrator_gain();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_integrator_gain(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t controller_vel_integrator_deadband(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (AVLOS_CMD_READ == cmd) {
        float v;
        v = controller_get_vel_integrator_deadband();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_integrator_deadband(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t controller_calibrate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    controller_calibrate();

    return AVLOS_RET_CALL;
}

uint8_t controller_idle(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    controller_idle();

    return AVLOS_RET_CALL;
}

uint8_t controller_position_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    controller_position_mode();

    return AVLOS_RET_CALL;
}

uint8_t controller_velocity_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    controller_velocity_mode();

    return AVLOS_RET_CALL;
}

uint8_t controller_current_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    controller_current_mode();

    return AVLOS_RET_CALL;
}

uint8_t comms_can_rate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t comms_can_id(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t motor_R(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t motor_L(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t motor_pole_pairs(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t motor_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t motor_offset(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (AVLOS_CMD_READ == cmd) {
        float v;
        v = motor_get_user_offset();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        motor_set_user_offset(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t motor_direction(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = motor_get_user_direction();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        uint8_t v;
        memcpy(&v, buffer, sizeof(v));
        motor_set_user_direction(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t encoder_position_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (AVLOS_CMD_READ == cmd) {
        float v;
        v = observer_get_pos_estimate();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t encoder_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (AVLOS_CMD_READ == cmd) {
        uint8_t v;
        v = encoder_get_type();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        uint8_t v;
        memcpy(&v, buffer, sizeof(v));
        encoder_set_type(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t encoder_bandwidth(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (AVLOS_CMD_READ == cmd) {
        float v;
        v = observer_get_bw();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        observer_set_bw(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t traj_planner_max_accel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t traj_planner_max_decel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t traj_planner_max_vel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t traj_planner_move_to(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint8_t offset = 0;
    float pos_setpoint;
    memcpy(&pos_setpoint, buffer+offset, sizeof(pos_setpoint));
    offset += sizeof(pos_setpoint);
    planner_move_to_vlimit(pos_setpoint);

    return AVLOS_RET_CALL;
}

uint8_t traj_planner_move_to_tlimit(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint8_t offset = 0;
    float pos_setpoint;
    memcpy(&pos_setpoint, buffer+offset, sizeof(pos_setpoint));
    offset += sizeof(pos_setpoint);
    planner_move_to_tlimit(pos_setpoint);

    return AVLOS_RET_CALL;
}
