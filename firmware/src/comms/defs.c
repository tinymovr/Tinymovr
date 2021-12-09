
#include <avlos/avlos.h>
#include <src/observer/observer.h>
#include <src/controller/controller.h>
#include <src/nvm/nvm.h>
#include <src/scheduler/scheduler.h>
#include <src/system/system.h>
#include <src/comms/defs.h>

RemoteObject *store[50];
Server s;

size_t noop(uint8_t *buffer_in, uint8_t *buffer_out) {return 0;}

// --- Encoder
MAKE_GETTER(observer_get_pos_estimate_user_frame)
MAKE_GETTER(observer_get_vel_estimate_user_frame)
MAKE_GETTER(observer_get_filter_bandwidth)
MAKE_SETTER(observer_set_filter_bandwidth)
size_t ticks_getter(uint8_t *buffer_in, uint8_t *buffer_out) {return write_le((uint16_t)ENCODER_TICKS, buffer_out);}

// --- Motor
MAKE_GETTER(motor_get_pole_pairs)
MAKE_SETTER(motor_set_pole_pairs)
MAKE_GETTER(motor_get_phase_resistance)
MAKE_SETTER(motor_set_phase_resistance)
MAKE_GETTER(motor_get_phase_inductance)
MAKE_SETTER(motor_set_phase_inductance)
MAKE_GETTER(motor_is_calibrated)
MAKE_GETTER(motor_is_gimbal)
MAKE_SETTER(motor_set_is_gimbal)

// PAC5XXX_RAMFUNC float motor_get_user_offset(void);
// PAC5XXX_RAMFUNC void motor_set_user_offset(float offset);

// PAC5XXX_RAMFUNC int8_t motor_get_user_direction(void);
// PAC5XXX_RAMFUNC void motor_set_user_direction(int8_t dir);

// --- Controller
MAKE_GETTER(controller_get_state)
MAKE_SETTER(controller_set_state)
MAKE_GETTER(controller_get_mode)
MAKE_SETTER(controller_set_mode)
MAKE_GETTER(controller_get_Iq_estimate_user_frame)
MAKE_GETTER(controller_get_pos_setpoint_user_frame)
MAKE_SETTER(controller_set_pos_setpoint_user_frame)
MAKE_GETTER(controller_get_vel_setpoint_user_frame)
MAKE_SETTER(controller_set_vel_setpoint_user_frame)
MAKE_GETTER(controller_get_Iq_setpoint_user_frame)
MAKE_SETTER(controller_set_Iq_setpoint_user_frame)
MAKE_GETTER(controller_get_pos_gain)
MAKE_SETTER(controller_set_pos_gain)
MAKE_GETTER(controller_get_vel_gain)
MAKE_SETTER(controller_set_vel_gain)
MAKE_GETTER(controller_get_vel_integrator_gain)
MAKE_SETTER(controller_set_vel_integrator_gain)
MAKE_GETTER(controller_get_Iq_bandwidth)
MAKE_SETTER(controller_set_Iq_bandwidth)
MAKE_GETTER(Controller_GetVelLimit)
MAKE_SETTER(Controller_SetVelLimit)
MAKE_GETTER(Controller_GetIqLimit)
MAKE_SETTER(Controller_SetIqLimit)

// --- CAN
MAKE_GETTER(CAN_get_ID)
MAKE_SETTER(CAN_set_ID)
MAKE_GETTER(CAN_get_kbit_rate)
MAKE_SETTER(CAN_set_kbit_rate)

// --- System
MAKE_GETTER(Scheduler_GetBusyCycles)

size_t save_config_caller(uint8_t *buffer_in, uint8_t *buffer_out) {NVM_SaveConfig();return 0;}
size_t erase_config_caller(uint8_t *buffer_in, uint8_t *buffer_out) {NVM_Erase();return 0;}
size_t reset_caller(uint8_t *buffer_in, uint8_t *buffer_out) {system_reset();return 0;}

    // VOID = 0
    // BOOL = 1
    // UINT8 = 2
    // INT8 = 3
    // UINT16 = 4
    // INT16 = 5
    // UINT32 = 6
    // INT32 = 7
    // UINT64 = 8
    // INT64 = 9
    // FLOAT = 10
    // DOUBLE = 11

RemoteObject *make_system(void)
{
    // --- Motor
    MAKE_ATTR(pole_pairs, 4, motor_get_pole_pairs_getter, motor_set_pole_pairs_setter)
    MAKE_ATTR(R, 10, motor_get_phase_resistance_getter, motor_set_phase_resistance_setter)
    MAKE_ATTR(L, 10, motor_get_phase_inductance_getter, motor_set_phase_inductance_setter)
    MAKE_ATTR(calibrated, 1, motor_is_calibrated_getter, noop)
    MAKE_ATTR(gimbal, 1, motor_is_gimbal_getter, motor_set_is_gimbal_setter)
    MAKE_OBJECT(motor, &pole_pairs, &R, &L, &calibrated, &gimbal)

    // --- Encoder
    MAKE_ATTR(pos_est, 10, observer_get_pos_estimate_user_frame_getter, noop)
    MAKE_ATTR(vel_est, 10, observer_get_vel_estimate_user_frame_getter, noop)
    MAKE_ATTR(bw, 10, observer_get_filter_bandwidth_getter, observer_set_filter_bandwidth_setter)
    MAKE_ATTR(ticks, 4, ticks_getter, noop)
    MAKE_OBJECT(encoder, &pos_est, &vel_est, &bw, &ticks)

    // --- Controller
    MAKE_ATTR(state, 2, controller_get_state_getter, controller_set_state_setter)
    MAKE_ATTR(mode, 2, controller_get_mode_getter, controller_set_mode_setter)
    MAKE_ATTR(pos_set, 10, controller_get_pos_setpoint_user_frame_getter, controller_set_pos_setpoint_user_frame_setter)
    MAKE_ATTR(vel_set, 10, controller_get_vel_setpoint_user_frame_getter, controller_set_vel_setpoint_user_frame_setter)
    MAKE_ATTR(Iq_est, 10, controller_get_Iq_estimate_user_frame_getter, noop)
    MAKE_ATTR(Iq_set, 10, controller_get_Iq_setpoint_user_frame_getter, controller_set_Iq_setpoint_user_frame_setter)
    MAKE_ATTR(pos_gain, 10, controller_get_pos_gain_getter, controller_set_pos_gain_setter)
    MAKE_ATTR(vel_gain, 10, controller_get_vel_gain_getter, controller_set_vel_gain_setter)
    MAKE_ATTR(vel_I_gain, 10, controller_get_vel_integrator_gain_getter, controller_set_vel_integrator_gain_setter)
    MAKE_ATTR(I_bw, 10, controller_get_Iq_bandwidth_getter, controller_set_Iq_bandwidth_setter)
    MAKE_ATTR(vel_lim, 10, Controller_GetVelLimit_getter, Controller_SetVelLimit_setter)
    MAKE_ATTR(Iq_lim, 10, Controller_GetIqLimit_getter, Controller_SetIqLimit_setter)
    MAKE_OBJECT(controller, &state, &mode, &pos_set, &vel_set, &Iq_est, &Iq_set, &pos_gain, &vel_gain, &vel_I_gain, &I_bw, &vel_lim, &Iq_lim)

    // --- CAN
    MAKE_ATTR(can_id, 2, CAN_get_ID_getter, CAN_set_ID_setter)
    MAKE_ATTR(rate, 2, CAN_get_kbit_rate_getter, CAN_set_kbit_rate_setter)
    MAKE_OBJECT(can, &can_id, &rate)

    // --- System
    MAKE_FUNC(save, 0, save_config_caller)
    MAKE_FUNC(erase, 0, erase_config_caller)
    MAKE_FUNC(reset, 0, reset_caller)
    MAKE_ATTR(busy_cycles, 6, Scheduler_GetBusyCycles_getter, noop)
    MAKE_OBJECT(system, &motor, &encoder, &controller, &can, &busy_cycles, &save, &erase, &reset)
    return &system;
}

void init_server(void)
{
    make_server(&s, make_system(), store);
}

size_t handle_message(uint8_t *buffer_in, uint8_t *buffer_out)
{
    return avlos_handle(&s, buffer_in, buffer_out);
}
