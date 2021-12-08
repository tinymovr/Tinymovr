
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

// --- Controller
MAKE_GETTER(controller_get_Iq_estimate_user_frame)
MAKE_GETTER(controller_get_pos_setpoint_user_frame)
MAKE_SETTER(controller_set_pos_setpoint_user_frame)
MAKE_GETTER(controller_get_vel_setpoint_user_frame)
MAKE_SETTER(controller_set_vel_setpoint_user_frame)
MAKE_GETTER(controller_get_Iq_setpoint_user_frame)
MAKE_SETTER(controller_set_Iq_setpoint_user_frame)

// --- CAN
MAKE_GETTER(CAN_get_ID)
MAKE_SETTER(CAN_set_ID)

// --- System
MAKE_GETTER(Scheduler_GetBusyCycles)

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
    MAKE_ATTR(pos_est, 10, observer_get_pos_estimate_user_frame_getter, noop)
    MAKE_ATTR(vel_est, 10, observer_get_vel_estimate_user_frame_getter, noop)
    //MAKE_ATTR(D, pos_est_getter, pos_est_setter)
    MAKE_OBJECT(encoder, &pos_est, &vel_est)

    MAKE_ATTR(pos_set, 10, controller_get_pos_setpoint_user_frame_getter, controller_set_pos_setpoint_user_frame_setter)
    MAKE_ATTR(vel_set, 10, controller_get_vel_setpoint_user_frame_getter, controller_set_vel_setpoint_user_frame_setter)
    MAKE_ATTR(Iq_est, 10, controller_get_Iq_estimate_user_frame_getter, noop)
    MAKE_ATTR(Iq_set, 10, controller_get_Iq_setpoint_user_frame_getter, controller_set_Iq_setpoint_user_frame_setter)
    MAKE_OBJECT(controller, &pos_set, &vel_set, &Iq_est, &Iq_set)

    MAKE_ATTR(can_id, 2, CAN_get_ID_getter, CAN_set_ID_setter)
    MAKE_OBJECT(can, &can_id)

    //MAKE_ATTR(counter, counter_getter, counter_setter)
    //MAKE_ATTR(version, version_getter, version_setter)
    MAKE_FUNC(save, 0, save_config_caller)
    MAKE_FUNC(reset, 0, reset_caller)
    MAKE_ATTR(busy_cycles, 6, Scheduler_GetBusyCycles_getter, noop)
    MAKE_OBJECT(system, &encoder, &controller, &can, &busy_cycles, &save, &reset)
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
