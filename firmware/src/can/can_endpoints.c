/*
* This file was automatically generated using csnake v0.3.4.
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

#include "src/system/system.h"
#include "src/encoder/encoder.h"
#include "src/observer/observer.h"
#include "src/motor/motor.h"
#include "src/controller/controller.h"
#include "src/nvm/nvm.h"
#include "/home/ihatz/Tinymovr/firmware/src/can/can_endpoints.h"

uint8_t avlos_get_hash(uint8_t * buffer, uint8_t * buffer_len) { const uint32_t v = 0x9db7dc73; memcpy(buffer, &v, sizeof(v)); return AVLOS_RET_READ; }

uint8_t avlos_tm_uid(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint32_t v;
    if (AVLOS_DIR_READ == cmd) {
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
    if (AVLOS_DIR_READ == cmd) {
        v = system_get_vbus();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_motor_R(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_DIR_READ == cmd) {
        v = motor_get_R();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_DIR_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        motor_set_R(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_motor_L(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_DIR_READ == cmd) {
        v = motor_get_L();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_DIR_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        motor_set_L(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_motor_pole_pairs(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint8_t v;
    if (AVLOS_DIR_READ == cmd) {
        v = motor_get_pole_pairs();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_DIR_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        motor_set_pole_pairs(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_motor_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    uint8_t v;
    if (AVLOS_DIR_READ == cmd) {
        v = motor_get_type();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_DIR_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        motor_set_type(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_encoder_position_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_DIR_READ == cmd) {
        v = encoder_get_pos_estimate();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}

uint8_t avlos_tm_encoder_bandwidth(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    float v;
    if (AVLOS_DIR_READ == cmd) {
        v = encoder_get_bandwidth();
        *buffer_len = sizeof(v);
        memcpy(buffer, &v, sizeof(v));
        return AVLOS_RET_READ;
    }
    else if (AVLOS_DIR_WRITE == cmd) {
        memcpy(&v, buffer, sizeof(v));
        encoder_set_bandwidth(v);
        return AVLOS_RET_WRITE;
    }
    return AVLOS_RET_NOACTION;
}

