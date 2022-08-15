#include "src/watchdog/watchdog.h"
#include "src/common.h"

static TimeoutWatchdog watchdog = {
    .enabled = false,
    .triggered = false
};


void Watchdog_init(void)
{
    // Allow WWDT config editing
    PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_WRITE_AVALABLE;

    // Set the clock divider
    PAC55XX_WWDT->WWDTCTL.CLKDIV = WWDT_CLKDIV_DIV32768;
    // Select FRCLK
    PAC55XX_WWDT->WWDTCTL.CLKSEL = WWDTCTL_CLKSEL_FRCLK;

    // Enable interrupt but not reset ("interval mode")
    PAC55XX_WWDT->WWDTCTL.INTEN = 1;
    PAC55XX_WWDT->WWDTCDCTL.WINDOW = 0;

    // Set WWDT interrupt priority
    NVIC_SetPriority(Wdt_IRQn, 4);

    // Set initial 1s timeout
    Watchdog_set_timeout_cycles(0xF000);
}

PAC5XXX_RAMFUNC bool Watchdog_triggered(void)
{
    return watchdog.triggered;
}

PAC5XXX_RAMFUNC void Watchdog_reset(void)
{
    watchdog.triggered = false;
    PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_WRITE_AVALABLE;
    // Clear the counter
    PAC55XX_WWDT->WWDTCLEAR = 1;

}

void Watchdog_set_timeout_cycles(uint32_t cycles)
{
    PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_WRITE_AVALABLE;
    // Set the counter start value
    PAC55XX_WWDT->WWDTCDCTL.CDV = cycles;
}

void Watchdog_set_timeout_seconds(float seconds)
{
    uint32_t cycles = (uint32_t)(seconds * wwdt_freq);
    Watchdog_set_timeout_cycles(cycles);
}

void Watchdog_enable(void)
{
    watchdog.triggered = false;
    watchdog.enabled = true;

    PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_WRITE_AVALABLE;
    // The WWDTCTR is automatically reset on enabling
    PAC55XX_WWDT->WWDTCTL.EN = 1;
    // Enable WWDT interrupt in the NVIC
    NVIC_EnableIRQ(Wdt_IRQn); //NN
}

void Watchdog_disable(void)
{
    PAC55XX_WWDT->WWDTLOCK = WWDTLOCK_REGS_WRITE_AVALABLE;
    PAC55XX_WWDT->WWDTCTL.EN = 0;
    // Disable WWDT interrupt in the NVIC - crashes without this
    NVIC_DisableIRQ(Wdt_IRQn);
    watchdog.enabled = false;
    
}

void WWDT_process_interrupt(void)
{
    watchdog.triggered = true;
}