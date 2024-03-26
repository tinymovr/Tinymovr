

#include <src/xfs.h>

FramesConfig frames = {
    .position_sensor_to_user = DEFAULT_TRANSFORM,
    .user_to_position_sensor = DEFAULT_TRANSFORM,
    .position_sensor_to_motor = DEFAULT_TRANSFORM,
    .motor_to_position_sensor = DEFAULT_TRANSFORM,
    .commutation_sensor_to_motor = DEFAULT_TRANSFORM,
    .motor_to_commutation_sensor = DEFAULT_TRANSFORM,
    .motor_to_user = DEFAULT_TRANSFORM,
    .user_to_motor = DEFAULT_TRANSFORM
};
