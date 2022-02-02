
#pragma once

void init_server(void);
size_t handle_message(uint8_t *buffer_in, size_t buffer_in_size, uint8_t *buffer_out);
