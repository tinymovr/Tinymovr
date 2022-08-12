
/*
The watchdog is used as a timer for inactivity on the CAN bus.
When it times out, it sets the control state back to idle as a failsafe if the ESC becomes disconnected.
It can be enabled/disabled via the set_watchdog can endpoint.
*/

#pragma once

#include "src/common.h"

typedef struct
{
    uint32_t timeout_cycles;
    uint32_t counter;
    bool enabled;
} TimeoutWatchdog;

void Watchdog_init(void);
void Watchdog_reset(void);
bool Watchdog_increment_and_check(void);
void Watchdog_set_timeout_cycles(uint32_t cycles);
void Watchdog_set_timeout_s(float s);
void Watchdog_enable(void);
void Watchdog_disable(void);