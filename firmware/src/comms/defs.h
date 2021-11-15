
#pragma once

size_t noop(uint8_t *buffer_in, uint8_t *buffer_out);
size_t pos_est_user_frame_getter(uint8_t *buffer_in, uint8_t *buffer_out);
size_t vel_est_user_frame_getter(uint8_t *buffer_in, uint8_t *buffer_out);
size_t save_config_caller(uint8_t *buffer_in, uint8_t *buffer_out);
size_t reset_caller(uint8_t *buffer_in, uint8_t *buffer_out);

void init_server(void);
size_t handle_message(uint8_t *buffer_in, uint8_t *buffer_out);
