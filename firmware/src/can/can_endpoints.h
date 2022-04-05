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

#pragma once

#include "src/common.h"

typedef enum
{
    AVLOS_RET_NOACTION,
    AVLOS_RET_READ = 1,
    AVLOS_RET_WRITE = 2
} Avlos_Return;

typedef enum
{
    AVLOS_CMD_WRITE,
    AVLOS_CMD_READ = 1
} Avlos_Command;

uint8_t avlos_get_hash(uint8_t * buffer, uint8_t * buffer_len);

/*
* avlos_tm_uid
*
* Retrieves the unique device ID.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_uid(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_Vbus
*
* Retrieves the bus voltage.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_Vbus(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_motor_R
*
* Accesses the motor Resistance value.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_motor_R(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_motor_L
*
* Accesses the motor Inductance value.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_motor_L(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_motor_pole_pairs
*
* Accesses the motor pole pair count.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_motor_pole_pairs(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_motor_type
*
* Accesses the motor type.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_motor_type(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_encoder_position_estimate
*
* Retrieves the encoder position estimate.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_encoder_position_estimate(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

/*
* avlos_tm_encoder_bandwidth
*
* Accesses the encoder observer bandwidth.
*
* @param buffer
* @param buffer_len
*/
uint8_t avlos_tm_encoder_bandwidth(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);

uint8_t (*avlos_endpoints[9])(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd) = {&avlos_get_hash, &avlos_system_get_uid, &avlos_system_get_vbus, &avlos_motor_get_R, &avlos_motor_get_L, &avlos_motor_get_pole_pairs, &avlos_motor_get_type, &avlos_encoder_get_pos_estimate, &avlos_encoder_get_bandwidth};
