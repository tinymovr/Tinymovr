
/*
The watchdog is used as a timer for inactivity on the CAN bus.
When it times out, it sets the control state back to idle as a failsafe if the ESC becomes disconnected.
It can be enabled/disabled via the set_watchdog can endpoint.
Timing works via the PAC55 Watchdog timer (WWDT)
*/

#pragma once

#include "src/system/system.h"

// 4MHz CLKREF >>> FRCLK (set in system.h) >>> /32768 WWDT = 122.07 Hz
// 2^16 / 122.07 = 536.87s maximum
#define WWDT_CLKDIV 32768

static const float wwdt_freq = FRCLK_FREQ_HZ / WWDT_CLKDIV;

typedef struct
{
    bool enabled;
    bool triggered;
} TimeoutWatchdog;

void Watchdog_init(void);
void Watchdog_reset(void);
bool Watchdog_triggered(void);
void Watchdog_set_timeout_cycles(uint16_t cycles);
void Watchdog_set_timeout_seconds(float s);
void Watchdog_enable(void);
void Watchdog_disable(void);
void WWDT_process_interrupt(void);