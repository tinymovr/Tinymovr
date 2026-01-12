# Tinymovr Safety-Critical Constraints

## Overview

Tinymovr controls high-power electric motors capable of causing physical harm if misused. This document outlines **safety-critical constraints** that must be understood and respected when modifying firmware, especially the control loop and motor calibration systems.

**Violating these constraints can result in**:
- Motor runaway or uncontrolled motion
- Overheating and fire hazard
- Mechanical damage to equipment
- Electrical damage to the controller
- Personal injury

## ⚠️ Real-Time Constraints

### Control Loop Timing

**Hard Requirement**: The control loop **must complete** in less than 50 μs (microseconds).

**Reference**: [firmware/src/config.h](firmware/src/config.h#L31-L32)
```c
#define PWM_FREQ_HZ (20000)  // 20 kHz = 50 μs period
```

**Timing Budget**:
- **Period**: 50 μs (1 / 20 kHz)
- **CPU Cycles**: 7,500 cycles @ 150 MHz
- **Target Usage**: <3,000 cycles (40%) for control algorithm
- **Maximum Allowed**: <5,000 cycles (67%) including overhead
- **Overhead**: Interrupts, ADC sampling, context switches

**Monitoring**: Check actual timing via `tm.scheduler.load`:
```python
load = tm.scheduler.load  # CPU cycles per control loop iteration
print(f"Control loop load: {load} cycles ({load/7500*100:.1f}%)")
```

**Reference**: [firmware/src/scheduler/scheduler.h](firmware/src/scheduler/scheduler.h#L27)

### Critical Functions Must Be RAM-Resident

Functions called from the control loop **must** be marked `TM_RAMFUNC` to avoid flash wait states.

**Reference**: [firmware/src/common.h](firmware/src/common.h#L67-L74)
```c
#if defined(DEBUG)
#define TM_RAMFUNC
#elif defined (NDEBUG)
#define TM_RAMFUNC PAC5XXX_RAMFUNC  // Copy function to RAM at startup
#endif
```

**Why**: Flash memory has wait states at 150 MHz. RAM access is zero-wait-state, ensuring deterministic timing.

**Examples of Critical Functions**:
- `CLControlStep()` - Main control loop ([firmware/src/controller/controller.c](firmware/src/controller/controller.c))
- `observer_update()` - State estimation
- `ADC_update()` - Current measurement
- `sensor_update()` - Encoder reading

**Rule**: If a function is called every 50 μs (20 kHz), it **must** be `TM_RAMFUNC`.

### Blocking Operations Forbidden

**Never** perform blocking operations in the control loop or ISRs:

❌ **Forbidden**:
- `delay()` or busy-wait loops
- `printf()` or UART output
- Dynamic memory allocation (`malloc`, `free`)
- Floating-point division (slow on Cortex-M4F)
- Large loops (>100 iterations)

✓ **Allowed**:
- Simple arithmetic (`+`, `-`, `*`)
- Fast approximations (`fast_sin()`, `fast_cos()`)
- Lookup tables
- Conditional branches (if/else)

### Interrupt Priorities

**Reference**: NVIC priority configuration (lower number = higher priority)

1. **ADC IRQ** (Highest) - Triggers control loop at 20 kHz
2. **CAN IRQ** - Asynchronous message handling
3. **UART IRQ** - Asynchronous message handling
4. **SysTick** - 1 kHz system timing
5. **Watchdog IRQ** - Safety timeout

**Rule**: Do not reconfigure interrupt priorities without understanding the impact on control loop timing.

## 🔥 Current Limits

### Board-Specific Trip Thresholds

Current limits vary by board revision due to different shunt resistors and power stages:

**Reference**: [firmware/src/common.h](firmware/src/common.h#L129-L133)
```c
#if defined BOARD_REV_R3 || defined BOARD_REV_R5
#define MIN_TRIP_CURRENT (5.0f)  // A
#elif defined BOARD_REV_M5
#define MIN_TRIP_CURRENT (1.0f)  // A
#endif
```

**Board Characteristics**:
| Board | Min Trip Current | Typical Max Current | Target Application |
|-------|-----------------|---------------------|-------------------|
| R3.x  | 5.0 A          | 30 A peak          | High-current motors |
| R5.x  | 5.0 A          | 30 A peak          | High-current motors |
| M5.x  | 1.0 A          | 10 A peak          | Gimbal motors |

### Trip Margin

The controller automatically trips to IDLE state if current exceeds 1.5× the configured limit:

**Reference**: [firmware/src/config.h](firmware/src/config.h#L37) and [firmware/src/controller/controller.c](firmware/src/controller/controller.c#L115)
```c
#define I_TRIP_MARGIN (1.5f)

// In control loop:
const float Iq_trip = our_fmaxf(config.I_limit * I_TRIP_MARGIN, MIN_TRIP_CURRENT);
if (our_fabsf(Iq) > Iq_trip)
{
    state.errors |= CONTROLLER_ERRORS_CURRENT_LIMIT_EXCEEDED;
}
```

**Example**:
- User sets `tm.controller.current.Iq_limit = 10.0` (amperes)
- Trip threshold = 10.0 × 1.5 = 15.0 A
- If Iq exceeds 15.0 A, controller enters IDLE state with error flag

### Hard Current Limit

An absolute maximum current limit protects against misconfiguration:

**Reference**: [firmware/src/config.h](firmware/src/config.h#L40)
```c
#define I_HARD_LIMIT (60.0f)  // A - Absolute maximum
```

**Rule**: Never set `I_limit` above this value. The hardware may be damaged.

### Default Current Limits by Board

**Reference**: [firmware/src/controller/controller.c](firmware/src/controller/controller.c#L73-L105)

**R3.x / R5.x Boards**:
```c
static ControllerConfig config = {
    .I_limit = 10.0f,  // Default: 10 A continuous
    // ...
};
```

**M5.x Boards**:
```c
static ControllerConfig config = {
    .I_limit = 4.0f,  // Default: 4 A continuous
    // ...
};
```

## ⚡ Voltage Monitoring

### Undervoltage Protection

The controller monitors bus voltage and enters IDLE if voltage drops below threshold:

**Reference**: [firmware/src/config.h](firmware/src/config.h#L38)
```c
#define VBUS_LOW_THRESHOLD (10.4f)  // V
```

**Why**: Low voltage can cause:
- Erratic motor behavior
- Controller brownout and reset
- Incomplete PWM cycles

**Recommendation**: Operate above 12V for stability.

### Calibration Voltage Limits

During resistance and inductance calibration, applied voltage is limited to safe values:

**Reference**: [firmware/src/motor/motor.h](firmware/src/motor/motor.h#L25-L38)

**R3.x / R5.x Boards**:
```c
#define MAX_CALIBRATION_VOLTAGE (0.5f)  // V
#define MIN_CALIBRATION_CURRENT (0.2f)  // A
```

**M5.x Boards**:
```c
#define MAX_CALIBRATION_VOLTAGE (5.0f)  // V
#define MIN_CALIBRATION_CURRENT (0.05f)  // A
```

**Why Different Values?**:
- **High-current motors** (R3/R5): Low resistance requires low voltage
- **Gimbal motors** (M5): High resistance requires higher voltage

### Calibration Voltage Debouncing (Recent Fix)

Power line noise can cause false `ABNORMAL_CALIBRATION_VOLTAGE` errors. The firmware now includes warm-up and debouncing:

**Reference**: [firmware/src/motor/motor.h](firmware/src/motor/motor.h#L56-L57)
```c
#define CAL_R_WARMUP_ITERATIONS (1500u)   // ~75ms warm-up at 20kHz
#define CAL_R_ABNORMAL_DEBOUNCE (300u)    // ~15ms debounce at 20kHz
```

**Behavior**:
1. **Warm-up Phase**: First 75ms (1500 iterations) ignored to allow voltage stabilization
2. **Debounce Window**: Voltage must exceed threshold for 15ms (300 iterations) to trigger fault
3. **Genuine Faults**: Shorts, open circuits, or wrong motor still detected

**Commit**: Added in commit `426ab7e` - "fix: Add debouncing and warm-up to resistance calibration voltage check"

## 🛡️ State Machine Safety

### State Transitions

The controller operates in three states:

**Reference**: [firmware/src/tm_enums.h](firmware/src/tm_enums.h) (generated from YAML)
```c
typedef enum
{
    CONTROLLER_STATE_IDLE = 0,       // Safe state, motors off
    CONTROLLER_STATE_CALIBRATE = 1,  // Calibration sequence
    CONTROLLER_STATE_CL_CONTROL = 2  // Closed-loop control active
} controller_state_options;
```

### Automatic Error Recovery

**Critical Safety Feature**: Errors automatically force transition to IDLE state:

**Reference**: [firmware/src/controller/controller.c](firmware/src/controller/controller.c#L120-L123)
```c
if (errors_exist() && (state.state != CONTROLLER_STATE_IDLE))
{
    controller_set_state(CONTROLLER_STATE_IDLE);
}
```

**Error Sources**:
- `CONTROLLER_ERRORS_CURRENT_LIMIT_EXCEEDED` - Overcurrent
- `MOTOR_ERRORS_ABNORMAL_CALIBRATION_VOLTAGE` - Calibration fault
- `ERRORS_UNDERVOLTAGE` - Low bus voltage
- Watchdog timeout
- Pre-control current standard deviation too high

**Rule**: Never disable automatic error transitions. This is a critical safety mechanism.

### Pre-Control Warmup

Before entering closed-loop control, the controller validates current sensor noise:

**Reference**: [firmware/src/controller/controller.c](firmware/src/controller/controller.c#L145-L150) and [firmware/src/config.h](firmware/src/config.h#L41-L42)
```c
#define MAX_CL_INIT_STEPS (200)    // Warmup iterations
#define PRE_CL_I_SD_MAX (0.4f)     // Max current standard deviation (A)

// In control loop:
if ((motor_get_is_gimbal() == false) && pre_cl_stats.size < MAX_CL_INIT_STEPS)
{
    CLPreStep();  // Collect statistics
}
```

**Why**: High current sensor noise indicates:
- Loose connections
- Electromagnetic interference
- Faulty hardware

**Behavior**: If standard deviation exceeds `PRE_CL_I_SD_MAX`, error `CONTROLLER_ERRORS_PRE_CL_I_SD_EXCEEDED` is raised and control is aborted.

## 🎯 Calibration Safety

### Multi-Stage Calibration Sequence

Calibration proceeds in strict order. Failure at any stage aborts the sequence:

**Reference**: [firmware/src/controller/controller.c](firmware/src/controller/controller.c#L125-L136)
```c
if (state.state == CONTROLLER_STATE_CALIBRATE)
{
    state.is_calibrating = true;
    system_reset_calibration();
    if (ADC_calibrate_offset() &&          // Stage 1: ADC offset
        motor_calibrate_resistance() &&     // Stage 2: Resistance (R)
        motor_calibrate_inductance())       // Stage 3: Inductance (L)
    {
        (void)(sensors_calibrate());       // Stage 4: Encoder
    }
    state.is_calibrating = false;
    controller_set_state(CONTROLLER_STATE_IDLE);
}
```

### Resistance Calibration Timing

**Reference**: [firmware/src/motor/motor.h](firmware/src/motor/motor.h#L41-L44)
```c
#define CAL_R_LEN (2 * PWM_FREQ_HZ)  // 2 seconds @ 20kHz = 40,000 iterations
#define CAL_L_LEN (1 * PWM_FREQ_HZ)  // 1 second @ 20kHz = 20,000 iterations
```

**Why Long Duration?**:
- Averages out noise
- Ensures thermal stability
- Validates measurement consistency

**Rule**: Do not reduce calibration lengths without extensive testing. Shorter calibration = less accurate R/L measurements = unstable FOC control.

### Resistance and Inductance Range Checks

Calibrated values must fall within expected ranges:

**Reference**: [firmware/src/motor/motor.h](firmware/src/motor/motor.h#L25-L39)

**R3.x / R5.x Boards**:
```c
#define MIN_PHASE_RESISTANCE (0.01f)   // 10 mΩ
#define MAX_PHASE_RESISTANCE (1.0f)    // 1 Ω
#define MIN_PHASE_INDUCTANCE (5e-6f)   // 5 μH
#define MAX_PHASE_INDUCTANCE (1e-3f)   // 1 mH
```

**M5.x Boards**:
```c
#define MIN_PHASE_RESISTANCE (0.5f)    // 500 mΩ
#define MAX_PHASE_RESISTANCE (20.0f)   // 20 Ω
#define MIN_PHASE_INDUCTANCE (1e-5f)   // 10 μH
#define MAX_PHASE_INDUCTANCE (1e-2f)   // 10 mH
```

**Why Different Ranges?**:
- **High-current motors**: Low resistance, low inductance
- **Gimbal motors**: High resistance, high inductance

**Errors Raised**:
- `MOTOR_ERRORS_PHASE_RESISTANCE_OUT_OF_RANGE` - R outside valid range
- `MOTOR_ERRORS_PHASE_INDUCTANCE_OUT_OF_RANGE` - L outside valid range

**Action**: If these errors occur, check:
1. Motor is correctly connected (all 3 phases)
2. Motor type matches board (high-current vs gimbal)
3. No shorts or opens in motor windings

### Pole Pair Detection

The firmware automatically detects motor pole pairs by rotating the motor and counting electrical cycles:

**Reference**: [firmware/src/common.h](firmware/src/common.h#L142)
```c
#define MOTOR_MAX_POLE_PAIRS 24
```

**Error Raised**:
- `MOTOR_ERRORS_POLE_PAIRS_OUT_OF_RANGE` - Detected pole pairs <1 or >24
- `MOTOR_ERRORS_POLE_PAIRS_CALCULATION_DID_NOT_CONVERGE` - Inconsistent results

**Action**: Ensure motor is free to rotate and encoder is properly mounted during calibration.

## 🏎️ Velocity Limits

### Hard Velocity Limit

An absolute maximum velocity protects against encoder errors and runaway:

**Reference**: [firmware/src/config.h](firmware/src/config.h#L39)
```c
#define VEL_HARD_LIMIT (600000.0f)  // ticks/s
```

**Calculation**:
- Encoder resolution: 8192 ticks/revolution (common)
- Hard limit: 600,000 ticks/s
- Maximum speed: 600,000 / 8192 ≈ **73 rev/s** ≈ **4,380 RPM**

**Board-Specific Default Limits**:

**R3.x / R5.x Boards** ([firmware/src/controller/controller.c](firmware/src/controller/controller.c#L74)):
```c
.vel_limit = 300000.0f,  // 36.6 rev/s ≈ 2,196 RPM
```

**M5.x Boards** ([firmware/src/controller/controller.c](firmware/src/controller/controller.c#L92)):
```c
.vel_limit = 100000.0f,  // 12.2 rev/s ≈ 732 RPM (gimbal motors)
```

**Warning Flag**: If velocity exceeds limit, `CONTROLLER_WARNINGS_VELOCITY_LIMITED` is set and velocity is clamped.

### Velocity Integrator Deadband

To prevent steady-state error accumulation, the velocity integrator has a deadband:

**Reference**: [firmware/src/controller/controller.c](firmware/src/controller/controller.c#L79)
```c
.vel_integral_deadband = 200.0f,  // ticks/s
```

**Why**: Small steady-state errors (<200 ticks/s ≈ 0.024 rev/s) do not integrate, preventing wind-up.

## ⚙️ Watchdog Protection

The firmware includes a watchdog timer to detect communication loss:

**Reference**: [firmware/src/controller/controller.c](firmware/src/controller/controller.c#L139-L144)
```c
// Check the watchdog and revert to idle if it has timed out
if (Watchdog_triggered())
{
    controller_set_state(CONTROLLER_STATE_IDLE);
    Watchdog_reset();
}
```

**Behavior**:
1. Watchdog must be "fed" regularly via CAN/UART communication
2. If no communication for timeout period → Watchdog triggers
3. Controller automatically enters IDLE state
4. Motor safely stops

**Configuration**: Watchdog timeout is configurable (default: typically 1-2 seconds)

**Purpose**: Prevents runaway if host computer crashes or communication is lost.

## 💾 Non-Volatile Memory (NVM) Safety

### Configuration Persistence

User settings (current limits, gains, etc.) can be saved to flash:

```python
tm.save_config()  # Saves to NVM
tm.erase_config()  # Erases NVM and resets to defaults
```

**Flash Write Limits**:
- **Endurance**: ~10,000 erase/write cycles
- **Retention**: 20+ years at 85°C

**Rule**: Minimize `save_config()` calls to extend flash lifetime. Do not call in loops or high-frequency events.

**Bad Example**:
```python
while True:
    tm.controller.current.Iq_limit += 0.1
    tm.save_config()  # ❌ Will wear out flash quickly!
```

**Good Example**:
```python
tm.controller.current.Iq_limit = 15.0
tm.save_config()  # ✓ Save once after configuration complete
```

## 🧪 Testing Requirements

### Mandatory Testing for Safety-Critical Changes

If you modify any of the following, **comprehensive testing is required**:

**Control Loop Changes**:
- ✓ Test `tm.scheduler.load` on multiple board revisions (R5, M5 minimum)
- ✓ Verify load stays below 3000 cycles under all conditions
- ✓ Test with different motors (high-current and gimbal)
- ✓ Monitor for oscillations, instability, overheating
- ✓ Verify error handling (force errors and confirm IDLE transition)

**Calibration Changes**:
- ✓ Test on 5+ different motors (varying R, L, pole pairs)
- ✓ Verify calibration succeeds consistently (10+ attempts per motor)
- ✓ Check measured R/L values against known specifications
- ✓ Test with noisy power supply (introduce line noise)
- ✓ Verify abnormal voltage detection (short phases, disconnect phase)

**Current Limit Changes**:
- ✓ Gradually increase current and verify trip at 1.5× limit
- ✓ Monitor for hardware damage (smoke, overheating, burnt MOSFETs)
- ✓ Test at different bus voltages (12V, 24V, 48V)
- ✓ Verify warning flags set correctly

**Timing Changes**:
- ✓ Measure actual execution time with oscilloscope (ADC IRQ to PWM update)
- ✓ Test under worst-case conditions (max CPU load)
- ✓ Verify no scheduler warnings (`SCHEDULER_WARNINGS_CONTROL_BLOCK_REENTERED`)

### Test Environments

**Simulation Tests**: No hardware, logic validation only
```bash
cd studio/Python
python -m unittest tests/test_simulation.py
```

**HITL (Hardware-in-the-Loop) Tests**: Requires Tinymovr board and motor
```bash
cd studio/Python
pytest -m hitl_default  # Basic tests
pytest -m eol           # End-of-line comprehensive tests
```

**Stress Tests**: Long-duration reliability testing
- Run motor continuously for 1+ hours
- Monitor temperature, current, voltage
- Check for accumulated errors or warnings

## 🚨 Common Pitfalls

### 1. Blocking in Control Loop

**Problem**: Called `printf()` or `delay()` in control loop

**Symptom**: Control loop overruns, scheduler warning raised, erratic motor behavior

**Solution**: Never block in control loop or ISRs. Use asynchronous logging (RTT) if needed.

### 2. Missing TM_RAMFUNC Annotation

**Problem**: Critical function not marked `TM_RAMFUNC`

**Symptom**: Intermittent timing issues, jitter in motor control, high CPU load

**Solution**: Mark all functions called from control loop with `TM_RAMFUNC`:
```c
TM_RAMFUNC void my_critical_function(void) {
    // This runs from RAM
}
```

### 3. Double-Precision Float

**Problem**: Used `double` instead of `float`

**Symptom**: Extremely high CPU load (>5000 cycles), control loop overruns

**Solution**: ARM Cortex-M4F has single-precision FPU only. Always use `float`:
```c
float value = 3.14f;  // ✓ Correct
double value = 3.14;  // ❌ Will use software emulation (slow!)
```

### 4. Integer Division

**Problem**: Used `/` operator on integers in critical path

**Symptom**: High CPU load, no hardware divider on Cortex-M4

**Solution**: Pre-compute reciprocals or use lookup tables:
```c
// Bad:
int result = value / 123;  // ❌ Software division

// Good:
float result = value * (1.0f / 123.0f);  // ✓ Hardware multiply
```

### 5. Disabling Error Handling

**Problem**: Commented out error checks or IDLE transitions

**Symptom**: Motor runaway, hardware damage, fire hazard

**Solution**: **Never disable error handling**. It's there for safety.

### 6. Reducing Calibration Time

**Problem**: Changed `CAL_R_LEN` to speed up calibration

**Symptom**: Inaccurate R/L measurements, unstable FOC, motor vibration

**Solution**: Keep calibration lengths at default values (2s for R, 1s for L)

### 7. Exceeding Current Limits

**Problem**: Set `I_limit` above hardware capability

**Symptom**: Burnt MOSFETs, damaged PCB traces, destroyed motor

**Solution**: Respect board-specific current ratings:
- R3/R5: Max 30 A peak
- M5: Max 10 A peak

## 📚 Related Documentation

- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture overview
- [PAC5527_NOTES.md](PAC5527_NOTES.md) - MCU architecture and timing
- [AVLOS_GUIDE.md](AVLOS_GUIDE.md) - Protocol modification guide
- [CLAUDE.md](CLAUDE.md) - Development guidelines for AI agents
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution workflow

## 🔐 Safety Checklist for Pull Requests

Before submitting a PR that modifies control code:

- [ ] Read this entire SAFETY.md document
- [ ] Verified no blocking operations in control loop
- [ ] Marked critical functions with TM_RAMFUNC
- [ ] Tested on multiple board revisions (R5, M5)
- [ ] Tested with multiple motors (high-current, gimbal)
- [ ] Verified `tm.scheduler.load` < 3000 cycles
- [ ] Checked error handling and IDLE transitions
- [ ] Ran HITL tests (`pytest -m hitl_default`)
- [ ] Monitored for oscillations, overheating, instability
- [ ] Documented changes in PR description
- [ ] Reviewed changes with maintainer before merging

---

**Document Status**: Living document, updated as firmware safety mechanisms evolve.
**Last Major Update**: Based on firmware v2.3.x with calibration voltage debouncing fix (commit 426ab7e).
