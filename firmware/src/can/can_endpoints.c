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


uint8_t (*avlos_endpoints[33])(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd) = {&uid_0, &Vbus_1, &temp_2, &reset_3, &total_4, &busy_5, &state_6, &mode_7, &pos_setpoint_8, &vel_setpoint_9, &Iq_setpoint_10, &Iq_estimate_11, &pos_p_gain_12, &vel_p_gain_13, &vel_i_gain_14, &Iq_p_gain_15, &gain_16, &deadband_17, &rate_18, &id_19, &R_20, &L_21, &pole_pairs_22, &type_23, &offset_24, &direction_25, &position_estimate_26, &type_27, &bandwidth_28, &max_accel_29, &max_decel_30, &max_vel_31, &pos_setpoint_32 };
uint32_t avlos_proto_hash = 3907629374;

uint8_t uid_0(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t Vbus_1(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t temp_2(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t reset_3(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    system_reset();

    return AVLOS_RET_CALL;
}

uint8_t total_4(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t busy_5(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t state_6(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t mode_7(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t pos_setpoint_8(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t vel_setpoint_9(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t Iq_setpoint_10(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t Iq_estimate_11(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t pos_p_gain_12(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t vel_p_gain_13(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t vel_i_gain_14(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t Iq_p_gain_15(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t gain_16(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t deadband_17(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t rate_18(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t id_19(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t R_20(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t L_21(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t pole_pairs_22(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t type_23(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t offset_24(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t direction_25(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t position_estimate_26(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t type_27(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t bandwidth_28(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t max_accel_29(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t max_decel_30(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t max_vel_31(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
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

uint8_t pos_setpoint_32(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (AVLOS_CMD_WRITE == cmd) {
        float v;
        memcpy(&v, buffer, sizeof(v));
        planner_move_to_vlimit(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}
