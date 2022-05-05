/*
* This file was automatically generated using csnake v0.3.5.
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

#include <string.h>
#include "src/adc/adc.h"
#include "src/system/system.h"
#include "src/encoder/encoder.h"
#include "src/observer/observer.h"
#include "src/motor/motor.h"
#include "src/controller/controller.h"
#include "src/nvm/nvm.h"
#include "can_endpoints.h"

uint8_t avlos_get_hash(uint8_t * buffer, uint8_t * buffer_len, uint8_t cmd) { const uint32_t v = 0x503e4a86; memcpy(buffer, &v, sizeof(v)); return AVLOS_RET_READ; }

uint8_t avlos_tm_uid(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint32_t v;
    if (AVLOS_CMD_READ == cmd) {
        v = system_get_uid();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_Vbus(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = adc_get_Vbus();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_temp(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = adc_get_mcu_temp();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_cycles_total(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint32_t v;
    if (AVLOS_CMD_READ == cmd) {
        v = Scheduler_GetTotalCycles();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_cycles_busy(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint32_t v;
    if (AVLOS_CMD_READ == cmd) {
        v = Scheduler_GetBusyCycles();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_controller_state(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint8_t v;
    if (AVLOS_CMD_READ == cmd) {
        v = controller_get_state();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        controller_set_state(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_controller_mode(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint8_t v;
    if (AVLOS_CMD_READ == cmd) {
        v = controller_get_mode();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        controller_set_mode(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_controller_pos_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = controller_get_pos_setpoint_user_frame();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        controller_set_pos_setpoint_user_frame(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_controller_vel_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = controller_get_vel_setpoint_user_frame();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_setpoint_user_frame(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_controller_Iq_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = controller_get_Iq_setpoint_user_frame();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        controller_set_Iq_setpoint_user_frame(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_controller_Iq_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = controller_get_Iq_estimate_user_frame();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_controller_pos_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = controller_get_pos_gain();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        controller_set_pos_gain(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_controller_vel_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = controller_get_vel_gain();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_gain(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_controller_vel_i_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = controller_get_vel_integrator_gain();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_integrator_gain(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_controller_Iq_p_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = controller_get_Iq_gain();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_controller_vel_integrator_gain(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = controller_get_vel_integrator_gain();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_integrator_gain(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_controller_vel_integrator_deadband(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = controller_get_vel_integrator_deadband();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        controller_set_vel_integrator_deadband(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_comms_can_rate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint32_t v;
    if (AVLOS_CMD_READ == cmd) {
        v = CAN_get_kbit_rate();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        CAN_set_kbit_rate(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_comms_can_id(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint32_t v;
    if (AVLOS_CMD_READ == cmd) {
        v = CAN_get_ID();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        CAN_set_ID(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_motor_R(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = motor_get_phase_resistance();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        motor_set_phase_resistance(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_motor_L(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = motor_get_phase_inductance();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        motor_set_phase_inductance(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_motor_pole_pairs(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint8_t v;
    if (AVLOS_CMD_READ == cmd) {
        v = motor_get_pole_pairs();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        motor_set_pole_pairs(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_motor_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint8_t v;
    if (AVLOS_CMD_READ == cmd) {
        v = motor_get_is_gimbal();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        motor_set_is_gimbal(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_motor_offset(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = motor_get_user_offset();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        motor_set_user_offset(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_motor_direction(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint8_t v;
    if (AVLOS_CMD_READ == cmd) {
        v = motor_get_user_direction();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        motor_set_user_direction(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_motor_phase_currents_U(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = motor_get_IU();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_motor_phase_currents_V(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = motor_get_IV();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_motor_phase_currents_W(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = motor_get_IW();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_encoder_position_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = observer_get_pos_estimate();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_encoder_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint8_t v;
    if (AVLOS_CMD_READ == cmd) {
        v = encoder_get_type();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        encoder_set_type(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_encoder_bandwidth(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = observer_get_bw();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        observer_set_bw(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_traj_planner_max_accel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = planner_get_max_accel();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        planner_set_max_accel(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_traj_planner_max_decel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = planner_get_max_decel();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        planner_set_max_decel(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_traj_planner_max_vel(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = planner_get_max_vel();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        planner_set_max_vel(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_traj_planner_pos_setpoint(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_CMD_READ == cmd) {
        v = None();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_CMD_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        planner_move_to_vlimit(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

