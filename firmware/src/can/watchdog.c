#include "src/can/watchdog.h"
#include "src/common.h"

static TimeoutWatchdog watchdog = {
    .timeout_cycles = 10000,
    .counter = 0,
    .enabled = false
};

void Watchdog_reset(void)
{
    watchdog.counter = 0;
}

PAC5XXX_RAMFUNC bool Watchdog_increment_and_check(void)
{
    if (!watchdog.enabled)
    {
        return false;
    }

    ++watchdog.counter;
    if (watchdog.counter >= watchdog.timeout_cycles)
    {
        return true;
    }
    return false;
}

void Watchdog_set_timeout_cycles(uint16_t cycles)
{
    watchdog.timeout_cycles = cycles;
}

void Watchdog_set_timeout_s(float s)
{
    uint16_t cycles = (uint16_t)(s / PWM_PERIOD_S);
    Watchdog_set_timeout_cycles(cycles);
}

void Watchdog_enable(void)
{
    Watchdog_reset();
    watchdog.enabled = true;
}
void Watchdog_disable(void)
{
    watchdog.enabled = false;
}