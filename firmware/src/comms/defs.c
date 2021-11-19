
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

size_t pos_est_user_frame_getter(uint8_t *buffer_in, uint8_t *buffer_out) {
    size_t v = 0;
    write_le(observer_get_pos_estimate_user_frame(), buffer_out, &v);
    return v;
}

size_t vel_est_user_frame_getter(uint8_t *buffer_in, uint8_t *buffer_out) {
    size_t v = 0;
    write_le(observer_get_vel_estimate_user_frame(), buffer_out, &v);
    return v;
}

size_t Iq_est_user_frame_getter(uint8_t *buffer_in, uint8_t *buffer_out) {
    size_t v = 0;
    write_le(controller_get_Iq_estimate_user_frame(), buffer_out, &v);
    return v;
}

size_t pos_set_user_frame_getter(uint8_t *buffer_in, uint8_t *buffer_out) {
    size_t v = 0;
    write_le(controller_get_pos_setpoint_user_frame(), buffer_out, &v);
    return v;
}
size_t pos_set_user_frame_setter(uint8_t *buffer_in, uint8_t *buffer_out) {
    
}

size_t vel_set_user_frame_getter(uint8_t *buffer_in, uint8_t *buffer_out) {
    size_t v = 0;
    write_le(controller_get_vel_setpoint_user_frame(), buffer_out, &v);
    return v;
}
size_t vel_set_user_frame_setter(uint8_t *buffer_in, uint8_t *buffer_out) {
    
}

size_t Iq_set_user_frame_getter(uint8_t *buffer_in, uint8_t *buffer_out) {
    size_t v = 0;
    write_le(controller_get_Iq_estimate_user_frame(), buffer_out, &v);
    return v;
}
size_t Iq_set_user_frame_setter(uint8_t *buffer_in, uint8_t *buffer_out) {
    
}

size_t busy_cycles_getter(uint8_t *buffer_in, uint8_t *buffer_out) {
    size_t v = 0;
    write_le(Scheduler_GetBusyCycles(), buffer_out, &v);
    return v;
}

size_t save_config_caller(uint8_t *buffer_in, uint8_t *buffer_out) {NVM_SaveConfig();return 0;}
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
    MAKE_ATTR(pos_est, 10, pos_est_user_frame_getter, noop)
    MAKE_ATTR(vel_est, 10, vel_est_user_frame_getter, noop)
    //MAKE_ATTR(D, pos_est_getter, pos_est_setter)
    MAKE_OBJECT(encoder, &pos_est, &vel_est)

    MAKE_ATTR(pos_set, 10, pos_set_user_frame_getter, pos_set_user_frame_setter)
    MAKE_ATTR(vel_set, 10, vel_set_user_frame_getter, vel_set_user_frame_setter)
    MAKE_ATTR(Iq_est, 10, Iq_est_user_frame_getter, noop)
    MAKE_ATTR(Iq_set, 10, Iq_set_user_frame_getter, Iq_set_user_frame_setter)
    MAKE_OBJECT(controller, &pos_set, &vel_set, &Iq_est, &Iq_set)

    //MAKE_ATTR(counter, counter_getter, counter_setter)
    //MAKE_ATTR(version, version_getter, version_setter)
    MAKE_FUNC(save, 0, save_config_caller)
    MAKE_FUNC(reset, 0, reset_caller)
    MAKE_ATTR(busy_cycles, 6, busy_cycles_getter, noop)
    MAKE_OBJECT(system, &encoder, &busy_cycles, &save, &reset)
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
