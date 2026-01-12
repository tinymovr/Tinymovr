# PAC5527 MCU Architecture Notes

## Overview

The **Qorvo PAC5527** is a specialized motor control System-on-Chip (SoC) featuring an ARM Cortex-M4F core with integrated power stage and motor control peripherals. This document extracts key architectural information from the PAC5527 datasheets and describes how Tinymovr leverages the MCU's capabilities.

**Note**: The full PAC5527 datasheets are not included in version control due to copyright restrictions. When available, they can be found at:
- `hardware/Datasheets/PAC5527 Data Sheet.pdf`
- `hardware/Datasheets/UG_PAC5527.pdf`

## Core Processor

### ARM Cortex-M4F Specifications
- **Architecture**: ARMv7E-M with DSP extensions
- **Core Frequency**: 150 MHz (HCLK_FREQ_HZ)
- **FPU**: Single-precision floating-point unit (FPv4-SP-D16)
- **Instruction Set**: Thumb-2 with hardware multiply and divide
- **NVIC**: Nested Vector Interrupt Controller with 16 priority levels
- **MPU**: Memory Protection Unit (optional, not used in Tinymovr)

**Reference**: [firmware/src/config.h](firmware/src/config.h#L22-L25)
```c
#define ACLK_FREQ_HZ    (300000000)  // Peripheral clock
#define HCLK_FREQ_HZ    (150000000)  // CPU clock
#define CLKREF_FREQ_HZ  (4000000)    // Reference clock
```

### Clock Architecture
- **ACLK (Peripheral Clock)**: 300 MHz - Drives PWM timers and ADC
- **HCLK (CPU Clock)**: 150 MHz - ARM Cortex-M4F core
- **FRCLK (Free-Running Clock)**: 4 MHz - Reference for timing
- **PLL**: On-chip PLL generates high-speed clocks from external crystal/oscillator

## Memory Architecture

### Flash Memory
- **Total Size**: 128 KB (0x00020000 bytes)
- **Bootloader Region**: 4 KB at 0x00000000 - 0x00001000
- **Application Region**: 124 KB at 0x00001000 - 0x00020000
- **Flash Wait States**: Automatically inserted for high-frequency access
- **Endurance**: Typical 10,000 erase/write cycles
- **Data Retention**: 20+ years at 85°C

**Reference**: [firmware/pac55xx.ld](firmware/pac55xx.ld#L3-L9)
```ld
MEMORY
{
    rom0 (rx) : ORIGIN = 0x00000000, LENGTH = 0x00001000  /* Bootloader */
    rom (rx)  : ORIGIN = 0x00001000, LENGTH = 0x0001F000  /* Application */
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00007F00  /* Main RAM */
    SHARED (rwx) : ORIGIN = 0x20007F00, LENGTH = 0x00000100  /* Bootloader shared */
}
```

### RAM Memory
- **Total Size**: 32 KB (0x00008000 bytes)
- **Main RAM**: 31.75 KB (0x20000000 - 0x20007F00) - Application use
- **Shared Region**: 256 bytes (0x20007F00 - 0x20008000) - Bootloader communication
- **Zero Wait State**: All RAM accesses complete in single cycle
- **Retention Mode**: Low-power RAM retention available

### RAM Execution Optimization
Critical real-time functions execute from RAM to avoid flash wait states:

**Reference**: [firmware/src/common.h](firmware/src/common.h#L67-L74)
```c
#if defined(DEBUG)
#define TM_RAMFUNC
#elif defined (NDEBUG)
#define TM_RAMFUNC PAC5XXX_RAMFUNC  // Copy function to RAM at startup
#endif
```

Functions marked `TM_RAMFUNC` are copied to RAM during initialization and execute with zero wait states, critical for maintaining 20 kHz control loop timing.

### Bootloader Integration
The bootloader resides in the first 4 KB of flash and provides DFU (Device Firmware Update) capability:

**Trigger Patterns** ([firmware/src/common.h](firmware/src/common.h#L76-L77)):
```c
#define BTL_TRIGGER_PATTERN      0x39          // Flash-based trigger
#define BTL_TRIGGER_PATTERN_RAM  0x5048434D    // RAM-based trigger ("HPCM")
```

The shared RAM region at 0x20007F00 allows the application to pass parameters to the bootloader before reset.

## Motor Control Peripherals

### 3-Phase PWM Generation
The PAC5527 integrates specialized timer peripherals for 3-phase motor control:

- **PWM Frequency**: 20 kHz (50 μs period) - Configured in [firmware/src/config.h](firmware/src/config.h#L31)
- **Resolution**: 16-bit counters provide sub-microsecond duty cycle resolution
- **Dead-Time Insertion**: Hardware dead-time generation prevents shoot-through
- **Center-Aligned Mode**: Reduces current ripple and EMI
- **Complementary Outputs**: Automatic generation of high/low-side gate signals

### Integrated Gate Drivers
The PAC5527 includes on-chip gate drivers for 6 external MOSFETs (3 half-bridges):

- **Driver Supply**: Integrated charge pump for high-side gate drive
- **Current Capability**: Sufficient for typical power MOSFETs
- **Protection**: Overcurrent, overvoltage, and thermal shutdown
- **Status Monitoring**: Fault detection with interrupt capability

### ADC (Analog-to-Digital Converter)
The PAC5527 features a high-speed ADC optimized for motor control:

- **Resolution**: 12-bit successive approximation (SAR)
- **Sampling Rate**: Up to 1 MSPS
- **Channels**: Multiple input channels for current, voltage, and temperature sensing
- **Trigger Modes**: Hardware-triggered from PWM timer for synchronous sampling
- **DMA Support**: Direct memory access for zero-CPU-overhead sampling

**Tinymovr Usage**:
- **Phase Current Sensing**: Samples A, B, C phase currents at PWM frequency (20 kHz)
- **Bus Voltage Monitoring**: Continuous Vbus measurement for FOC calculations
- **Temperature Sensing**: Internal die temperature sensor

The ADC interrupt at 20 kHz drives the main control loop. See [firmware/src/controller/controller.c](firmware/src/controller/controller.c) for the control loop implementation.

### Current Sensing Architecture
The PAC5527 integrates current sense amplifiers for shunt resistor measurement:

- **Amplifier Gain**: Programmable gain for different shunt resistor values
- **Offset Calibration**: Automatic offset calibration during startup
- **Noise Performance**: Low-noise design for accurate current measurement

**Board-Specific Shunt Resistors**:
- **R3.x/R5.x Boards**: Lower resistance shunts for high-current motors
- **M5.x Boards**: Higher resistance shunts for gimbal motors (lower current)

Trip current thresholds vary by board revision ([firmware/src/common.h](firmware/src/common.h#L129-L133)):
```c
#if defined BOARD_REV_R3 || defined BOARD_REV_R5
#define MIN_TRIP_CURRENT (5.0f)  // A
#elif defined BOARD_REV_M5
#define MIN_TRIP_CURRENT (1.0f)  // A
#endif
```

## Communication Peripherals

### CAN 2.0B Controller
The PAC5527 includes a full CAN 2.0B controller with the following features:

- **Standard**: ISO 11898-1 CAN 2.0B
- **Bit Rates**: Up to 1 Mbps (Tinymovr default: 1 Mbps)
- **ID Support**: 11-bit standard and 29-bit extended identifiers
- **Message Objects**: Multiple hardware message buffers (typically 32)
- **Acceptance Filtering**: Hardware ID filtering to reduce CPU load
- **Error Handling**: Automatic error detection, error counters, and bus-off recovery

**Tinymovr CAN Configuration**:
- **Extended ID**: 29-bit arbitration ID structure
- **Node ID**: 8-bit device address (configurable 1-255)
- **Protocol Hash**: 9-bit protocol version identifier
- **Endpoint ID**: 12-bit endpoint address (97 total endpoints)

**Pin Assignments by Board Revision** ([firmware/src/common.h](firmware/src/common.h#L103-L109)):
```c
#if defined BOARD_REV_R3
#define CAN_PE23        // Pins PE2 (TX), PE3 (RX)
#elif defined BOARD_REV_R5
#define CAN_PF67        // Pins PF6 (TX), PF7 (RX)
#elif defined BOARD_REV_M5
#define CAN_PD56        // Pins PD5 (TX), PD6 (RX)
#endif
```

### UART (Universal Asynchronous Receiver/Transmitter)
- **Baud Rates**: Up to 1 Mbps (Tinymovr uses 115200 baud)
- **Data Format**: 8N1 (8 data bits, no parity, 1 stop bit)
- **FIFO**: Hardware TX/RX FIFOs reduce interrupt overhead
- **Interrupt Support**: TX empty, RX available, error detection

**Reference**: [firmware/src/config.h](firmware/src/config.h#L49-L51)
```c
#define UART_ENUM UARTB           // UART peripheral selection
#define UART_REF PAC55XX_UARTB    // Hardware register base
#define UART_BAUD_RATE (115200)   // Default baud rate
```

**Note**: UART is primarily used for firmware updates and debugging. CAN bus is the primary communication interface during normal operation.

### SSP/SPI (Synchronous Serial Port)
The PAC5527 provides SSP peripherals for SPI communication with external sensors:

- **Modes**: SPI Master and Slave modes
- **Clock Rates**: Up to 12 Mbps (HCLK/4)
- **Data Width**: Configurable 4-16 bits per transfer
- **DMA Support**: Reduces CPU overhead for bulk transfers

**Tinymovr Usage**:
Communicates with external encoders:
- **MA7xx**: MPS magnetic encoders (1.5-12 Mbps)
- **AS5047**: AMS magnetic encoder (SPI)
- **AMT22**: CUI absolute encoder (SPI)

See [firmware/src/sensor/](firmware/src/sensor/) for encoder driver implementations.

## Unique Device Identification

The PAC5527 provides a 96-bit unique device ID burned during manufacturing:

**Reference**: [firmware/src/system/system.h](firmware/src/system/system.h#L51-L54)
```c
static inline uint32_t system_get_uid(void)
{
    // XOR three 32-bit words to create unique 32-bit ID
    return PAC55XX_INFO1->UNIQUEID[0] ^
           PAC55XX_INFO1->UNIQUEID[1] ^
           PAC55XX_INFO1->UNIQUEID[2];
}
```

This UID is:
- **Factory Programmed**: Unique to each PAC55xx chip produced
- **Read-Only**: Cannot be modified by application code
- **Use Cases**: Device identification, serial number generation, encryption seeds

## Board Revision Support

Tinymovr firmware supports multiple board revisions through compile-time configuration:

**Reference**: [firmware/src/common.h](firmware/src/common.h#L93-L127)
```c
// Top-level revision families
#if defined BOARD_REV_R32 || defined BOARD_REV_R33
#define BOARD_REV_R3
#elif defined BOARD_REV_R50 || BOARD_REV_R51 || defined BOARD_REV_R52 || defined BOARD_REV_R53
#define BOARD_REV_R5
#elif defined BOARD_REV_M51
#define BOARD_REV_M5
#endif

// Specific revision indices
#define BOARD_REV_IDX   // R32=2, R33=3, R50=10, R51=11, R52=12, R53=13, M51=21
```

**Board Characteristics**:
- **R3.x**: Original high-current design (CAN on PE2/PE3)
- **R5.x**: Updated design with improved layout (CAN on PF6/PF7)
- **M5.x**: Gimbal motor variant with lower current (CAN on PD5/PD6)

**Build Command Example**:
```bash
make release REV=R52   # Builds for R5.2 board
make release REV=M51   # Builds for M5.1 board
```

## Power Management Features

### Supply Monitoring
- **Vbus Monitoring**: Continuous bus voltage measurement via ADC
- **Undervoltage Lockout (UVLO)**: Protection against low supply voltage
- **Overvoltage Protection**: Shuts down drivers if Vbus exceeds safe limits

**Threshold** ([firmware/src/config.h](firmware/src/config.h#L38)):
```c
#define VBUS_LOW_THRESHOLD  (10.4f)  // V - Minimum operating voltage
```

### Charge Pump
The integrated charge pump provides gate drive voltage for high-side MOSFETs:

- **Output Voltage**: Vbus + 10V (typical)
- **Fault Detection**: Charge pump fault signals monitored by firmware
- **Warnings**: Charge pump faults trigger system warnings

### Temperature Monitoring
- **Die Temperature Sensor**: Internal ADC channel for MCU temperature
- **Thermal Shutdown**: Hardware protection at extreme temperatures
- **Application Monitoring**: Firmware can read die temperature via `tm.temp`

**Reference**: [studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml](studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml) (lines 38-42)

## Watchdog Timer (WWDT)

The PAC5527 includes a windowed watchdog timer for fault detection:

- **Window Mode**: Must be refreshed within specific time window
- **Reset on Timeout**: Generates system reset if not serviced
- **Configuration**: Programmable timeout period

**Tinymovr Usage**:
Detects communication loss and resets to safe IDLE state. See [firmware/src/watchdog/](firmware/src/watchdog/) for implementation.

## Debug and Programming Interface

### JTAG/SWD
The PAC5527 supports ARM Serial Wire Debug (SWD) for programming and debugging:

- **Pins**: SWDIO, SWCLK (standard ARM Cortex-M debug interface)
- **Debugger Support**: J-Link, ST-Link, CMSIS-DAP compatible
- **Breakpoints**: 6 hardware breakpoints, unlimited software breakpoints
- **Watchpoints**: 4 hardware data watchpoints

**J-Link Configuration**: [firmware/JLinkDevices.xml](firmware/JLinkDevices.xml)

### SEGGER RTT (Real-Time Transfer)
The firmware includes optional RTT support for printf-style debugging:

- **Zero Overhead**: Does not halt CPU for output
- **High Speed**: Much faster than UART
- **Bidirectional**: Supports input and output
- **Implementation**: [firmware/src/rtt/](firmware/src/rtt/)

## PAC55xx SDK Structure

The Qorvo PAC55xx SDK provides hardware abstraction layers:

**Location**: [firmware/pac55xx_sdk/](firmware/pac55xx_sdk/) (not in repo, download from Qorvo)

**SDK Components**:
- `device/`: Chip-specific headers (pac5527.h, system_pac5xxx.h)
- `peripheral/`: Peripheral register definitions (pac5xxx_*.h)
- `driver/`: Driver functions (pac5xxx_driver_*.h)

**Important Notes**:
1. The SDK is **not included** in the Tinymovr repository due to Qorvo licensing restrictions
2. Developers must **download** from Qorvo website (requires registration)
3. A **patch** must be applied: [firmware/sdk_patch.patch](firmware/sdk_patch.patch)
4. Apply patch: `python3 -m patch sdk_patch.patch`

**SDK Includes** ([firmware/src/common.h](firmware/src/common.h#L28-L54)):
```c
#include "pac5xxx.h"              // Core chip definitions
#include "pac5xxx_adc.h"          // ADC peripheral
#include "pac5xxx_can.h"          // CAN peripheral
#include "pac5xxx_timers.h"       // PWM timers
#include "pac5xxx_uart.h"         // UART peripheral
#include "pac5xxx_ssp.h"          // SPI/SSP peripheral
// ... additional peripheral headers
```

## Performance Characteristics

### Timing Budget (20 kHz Control Loop)
- **Loop Period**: 50 μs (1 / 20 kHz)
- **CPU Cycles Available**: 7,500 cycles @ 150 MHz
- **Target Usage**: <3,000 cycles (40%) for control algorithm
- **Overhead**: Interrupts, context switches, ADC sampling

**Monitoring**: `tm.scheduler.load` reports actual cycle count per iteration

### Interrupt Priorities
The NVIC priority scheme (higher value = lower priority):
1. **ADC IRQ** (Highest): 20 kHz control loop trigger
2. **CAN IRQ**: Asynchronous message reception
3. **UART IRQ**: Asynchronous message reception
4. **SysTick**: 1 kHz system timing
5. **Watchdog IRQ**: Safety timeout handling

## Key Design Constraints

### Real-Time Requirements
- Control loop **must complete** in <50 μs (leaves 10 μs margin)
- Functions in control path **must be** RAM-resident (TM_RAMFUNC)
- Avoid dynamic memory allocation (malloc/free) in real-time code
- Disable interrupts only for critical sections (<1 μs)

### Float Precision
- ARM Cortex-M4F has **single-precision** FPU only
- All floating-point variables are `float` (32-bit), not `double` (64-bit)
- Double-precision math falls back to software emulation (very slow)

### Hardware Limitations
- **No Hardware Division**: Integer division uses software routine (expensive)
- **No RTOS**: Bare-metal firmware with cooperative scheduling
- **Limited RAM**: 32 KB total, must be carefully managed
- **Flash Endurance**: Limit configuration writes to extend flash lifetime

## References

### Firmware Files
- [firmware/src/common.h](firmware/src/common.h) - Core definitions and PAC55xx includes
- [firmware/src/config.h](firmware/src/config.h) - System configuration constants
- [firmware/src/system/system.h](firmware/src/system/system.h) - System initialization
- [firmware/pac55xx.ld](firmware/pac55xx.ld) - Linker script with memory layout
- [firmware/Makefile](firmware/Makefile) - Build system with board revision selection

### Hardware
- `hardware/Datasheets/PAC5527 Data Sheet.pdf` - Full datasheet (when available)
- `hardware/Datasheets/UG_PAC5527.pdf` - User guide (when available)
- [hardware/R5/](hardware/R5/) - R5.x board designs
- [hardware/M5/](hardware/M5/) - M5.x board designs

### External Resources
- **Qorvo Website**: https://www.qorvo.com (SDK download, datasheets)
- **ARM Cortex-M4**: https://developer.arm.com/ip-products/processors/cortex-m/cortex-m4
- **CMSIS**: [firmware/cmsis_core/core_cm4.h](firmware/cmsis_core/core_cm4.h)

---

**Document Status**: Living document, updated as firmware evolves.
**Last Major Update**: Based on firmware v2.3.x with calibration voltage debouncing fix.
