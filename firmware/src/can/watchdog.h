
/*


*/

#pragma once

#include "src/common.h"

typedef struct
{
    uint16_t timeout_cycles;
    uint16_t counter;
    bool enabled;
} TimeoutWatchdog;

void Watchdog_init(void);
void Watchdog_reset(void);
bool Watchdog_increment_and_check(void);
void Watchdog_set_timeout_cycles(uint16_t cycles);
void Watchdog_set_timeout_s(float s);
void Watchdog_enable(void);
void Watchdog_disable(void);