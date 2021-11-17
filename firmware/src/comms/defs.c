
#include <avlos/avlos.h>
#include <src/observer/observer.h>
#include <src/nvm/nvm.h>
#include <src/system/system.h>
#include <src/comms/defs.h>

RemoteObject *store[50];

Server s;

size_t noop(uint8_t *buffer_in, uint8_t *buffer_out) {return 0;}

size_t pos_est_user_frame_getter(uint8_t *buffer_in, uint8_t *buffer_out)
{
    size_t pos = 0;
    write_le(observer_get_pos_estimate_user_frame(), buffer_out, &pos);
    return pos;
}

size_t vel_est_user_frame_getter(uint8_t *buffer_in, uint8_t *buffer_out)
{
    size_t pos = 0;
    write_le(observer_get_vel_estimate_user_frame(), buffer_out, &pos);
    return pos;
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

    //MAKE_ATTR(counter, counter_getter, counter_setter)
    //MAKE_ATTR(version, version_getter, version_setter)
    MAKE_FUNC(save, 0, save_config_caller)
    MAKE_FUNC(reset, 0, reset_caller)
    MAKE_OBJECT(system, &encoder, &save, &reset)
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
