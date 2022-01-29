
#include <avlos/avlos.h>
#include <src/adc/adc.h>
#include <src/encoder/encoder.h>
#include <src/observer/observer.h>
#include <src/controller/controller.h>
#include <src/gatedriver/gatedriver.h>
#include <src/nvm/nvm.h>
#include <src/scheduler/scheduler.h>
#include <src/system/system.h>
#include <src/comms/defs.h>

RemoteObject *store[55];
Server s;

static inline size_t noop(const uint8_t *buffer_in, uint8_t *buffer_out) {return 0;}

// --- Encoder
MAKE_GETTER(observer_get_pos_estimate_user_frame)
MAKE_GETTER(observer_get_vel_estimate_user_frame)
MAKE_GETTER(observer_get_filter_bandwidth)
MAKE_SETTER(observer_set_filter_bandwidth)
MAKE_GETTER(encoder_get_faults)
size_t ticks_getter(const uint8_t *buffer_in, uint8_t *buffer_out) {return write_le((uint16_t)ENCODER_TICKS, buffer_out);}

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
MAKE_GETTER(motor_get_faults)
MAKE_GETTER(motor_get_user_offset)
MAKE_SETTER(motor_set_user_offset)
MAKE_GETTER(motor_get_user_direction)
MAKE_SETTER(motor_set_user_direction)

// --- Driver
MAKE_GETTER(gate_driver_get_faults)

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
MAKE_GETTER(controller_get_vel_integrator_deadband)
MAKE_SETTER(controller_set_vel_integrator_deadband)
MAKE_GETTER(controller_get_Iq_bandwidth)
MAKE_SETTER(controller_set_Iq_bandwidth)
MAKE_GETTER(Controller_GetVelLimit)
MAKE_SETTER(Controller_SetVelLimit)
MAKE_GETTER(Controller_GetIqLimit)
MAKE_SETTER(Controller_SetIqLimit)
MAKE_GETTER(controller_get_faults)

// --- Planner
MAKE_GETTER(planner_get_max_accel)
MAKE_SETTER(planner_set_max_accel)
MAKE_GETTER(planner_get_max_decel)
MAKE_SETTER(planner_set_max_decel)
MAKE_GETTER(planner_get_max_vel)
MAKE_SETTER(planner_set_max_vel)
MAKE_SETTER(planner_move_to_vlimit)
MAKE_GETTER(planner_get_faults)

// --- CAN
MAKE_GETTER(CAN_get_ID)
MAKE_SETTER(CAN_set_ID)
MAKE_GETTER(CAN_get_kbit_rate)
MAKE_SETTER(CAN_set_kbit_rate)

// --- System
MAKE_GETTER(ADC_GetVBus)
MAKE_GETTER(Scheduler_GetBusyCycles)
MAKE_GETTER(get_unique_id)
MAKE_GETTER(system_get_faults)

size_t save_config_caller(const uint8_t *buffer_in, uint8_t *buffer_out) {NVM_SaveConfig();return 0;}
size_t erase_config_caller(const uint8_t *buffer_in, uint8_t *buffer_out) {NVM_Erase();return 0;}
size_t reset_caller(const uint8_t *buffer_in, uint8_t *buffer_out) {system_reset();return 0;}

RemoteObject *make_system(void)
{
    // --- Motor
    MAKE_ATTR(pps, &motor_get_pole_pairs, motor_get_pole_pairs_getter, motor_set_pole_pairs_setter)
    MAKE_ATTR(R, &motor_get_phase_resistance, motor_get_phase_resistance_getter, motor_set_phase_resistance_setter)
    MAKE_ATTR(L, &motor_get_phase_inductance, motor_get_phase_inductance_getter, motor_set_phase_inductance_setter)
    MAKE_ATTR(calb, &motor_is_calibrated, motor_is_calibrated_getter, noop)
    MAKE_ATTR(gimbal, &motor_is_gimbal, motor_is_gimbal_getter, motor_set_is_gimbal_setter)
    MAKE_ATTR(offset, &motor_get_user_offset, motor_get_user_offset_getter, motor_set_user_offset_setter)
    MAKE_ATTR(dir, &motor_get_user_direction, motor_get_user_direction_getter, motor_set_user_direction_setter)
    MAKE_ATTR(mtr_flt, &motor_get_faults, motor_get_faults_getter, noop)
    MAKE_OBJECT(motor, &pps, &R, &L, &calb, &gimbal, &offset, &dir, &mtr_flt)

    // --- Driver
    MAKE_ATTR(drv_flt, &gate_driver_get_faults, gate_driver_get_faults_getter, noop)
    MAKE_OBJECT(driver, &drv_flt)

    // --- Encoder
    MAKE_ATTR(pos_est, &observer_get_pos_estimate_user_frame, observer_get_pos_estimate_user_frame_getter, noop)
    MAKE_ATTR(vel_est, &observer_get_vel_estimate_user_frame, observer_get_vel_estimate_user_frame_getter, noop)
    MAKE_ATTR(bw, &observer_get_filter_bandwidth, observer_get_filter_bandwidth_getter, observer_set_filter_bandwidth_setter)
    MAKE_ATTR(ticks, 4, ticks_getter, noop)
    MAKE_ATTR(enc_flt, &encoder_get_faults, encoder_get_faults_getter, noop)
    MAKE_OBJECT(encoder, &pos_est, &vel_est, &bw, &ticks, &enc_flt)

    // --- Controller
    MAKE_ATTR(state, &controller_get_state, controller_get_state_getter, controller_set_state_setter)
    MAKE_ATTR(mode, &controller_get_mode, controller_get_mode_getter, controller_set_mode_setter)
    MAKE_ATTR(pos_set, &controller_get_pos_setpoint_user_frame, controller_get_pos_setpoint_user_frame_getter, controller_set_pos_setpoint_user_frame_setter)
    MAKE_ATTR(vel_set, &controller_get_vel_setpoint_user_frame, controller_get_vel_setpoint_user_frame_getter, controller_set_vel_setpoint_user_frame_setter)
    MAKE_ATTR(Iq_est, &controller_get_Iq_estimate_user_frame, controller_get_Iq_estimate_user_frame_getter, noop)
    MAKE_ATTR(Iq_set, &controller_get_Iq_setpoint_user_frame, controller_get_Iq_setpoint_user_frame_getter, controller_set_Iq_setpoint_user_frame_setter)
    MAKE_ATTR(pos_gain, &controller_get_pos_gain, controller_get_pos_gain_getter, controller_set_pos_gain_setter)
    MAKE_ATTR(vel_gain, &controller_get_vel_gain, controller_get_vel_gain_getter, controller_set_vel_gain_setter)
    MAKE_ATTR(vel_I_gain, &controller_get_vel_integrator_gain, controller_get_vel_integrator_gain_getter, controller_set_vel_integrator_gain_setter)
    MAKE_ATTR(vel_I_band, &controller_get_vel_integrator_deadband, controller_get_vel_integrator_deadband_getter, controller_set_vel_integrator_deadband_setter)
    MAKE_ATTR(I_bw, &controller_get_Iq_bandwidth, controller_get_Iq_bandwidth_getter, controller_set_Iq_bandwidth_setter)
    MAKE_ATTR(vel_lim, &Controller_GetVelLimit, Controller_GetVelLimit_getter, Controller_SetVelLimit_setter)
    MAKE_ATTR(Iq_lim, &Controller_GetIqLimit, Controller_GetIqLimit_getter, Controller_SetIqLimit_setter)
    MAKE_ATTR(ctrl_flt, &controller_get_faults, controller_get_faults_getter, noop)
    MAKE_OBJECT(ctrlr, &state, &mode, &pos_set, &vel_set, &Iq_est, &Iq_set, &pos_gain, &vel_gain, &vel_I_gain, &vel_I_band, &I_bw, &vel_lim, &Iq_lim, &ctrl_flt)

    // --- Trajectory Planner
    MAKE_ATTR(max_accel, &planner_get_max_accel, planner_get_max_accel_getter, planner_set_max_accel_setter)
    MAKE_ATTR(max_decel, &planner_get_max_decel, planner_get_max_decel_getter, planner_set_max_decel_setter)
    MAKE_ATTR(max_vel, &planner_get_max_vel, planner_get_max_vel_getter, planner_set_max_vel_setter)
    MAKE_ATTR(move_to, 10, controller_get_pos_setpoint_user_frame_getter, planner_move_to_vlimit_setter)
    MAKE_ATTR(pln_flt, &planner_get_faults, planner_get_faults_getter, noop)
    MAKE_OBJECT(planner, &max_accel, &max_decel, &max_vel, &move_to)

    // --- CAN
    MAKE_ATTR(can_id, &CAN_get_ID, CAN_get_ID_getter, CAN_set_ID_setter)
    MAKE_ATTR(rate, &CAN_get_kbit_rate, CAN_get_kbit_rate_getter, CAN_set_kbit_rate_setter)
    MAKE_OBJECT(can, &can_id, &rate)

    // --- Comms
    MAKE_OBJECT(comms, &can)

    // --- System
    MAKE_FUNC(save, &NVM_SaveConfig, save_config_caller)
    MAKE_FUNC(erase, &NVM_Erase, erase_config_caller)
    MAKE_FUNC(reset, &system_reset, reset_caller)
    MAKE_ATTR(busy, 6, Scheduler_GetBusyCycles_getter, noop)
    MAKE_ATTR(vbus, &ADC_GetVBus, ADC_GetVBus_getter, noop)
    MAKE_ATTR(uid, &get_unique_id, get_unique_id_getter, noop)
    MAKE_ATTR(sys_flt, &system_get_faults, system_get_faults_getter, noop)
    MAKE_OBJECT(system, &motor, &driver, &encoder, &ctrlr, &planner, &comms, &vbus, &busy, &uid, &save, &erase, &reset, &sys_flt)
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
