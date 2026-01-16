# Tinymovr Code Conventions

**Purpose**: Code style guidelines and common patterns for Tinymovr development.

**Target Audience**: Developers and AI agents contributing to firmware or Python client.

## C/C++ Style (Firmware)

### General Rules

- **Style**: K&R-inspired
- **Indentation**: 4 spaces (no tabs)
- **Naming**: Descriptive variable names (`position_setpoint`, not `ps`)
- **Comments**: Explain non-obvious logic, not obvious code
- **Precision**: Use `float` (not `double`) - Cortex-M4F has single-precision FPU only

### Function Naming

```c
// Module prefix + action + object
float motor_get_temperature(void)       // Getter
void controller_set_pos_setpoint(float) // Setter
bool ADC_calibrate_offset(void)         // Action
```

### Variable Naming

```c
// Good: Descriptive, clear purpose
float R_current = motor_measure_resistance();
float delta_R = R_current - motor_config.R_calibrated;
float delta_T = delta_R / (motor_config.R_calibrated * COPPER_TEMP_COEFF);
return CALIBRATION_TEMP + delta_T;

// Bad: Cryptic, unclear
float r=mr();
float d=r-c.r;
return 25+d/(c.r*0.00393);
```

### RAM Functions

Functions must be placed in RAM using `TM_RAMFUNC` or `PAC5XXX_RAMFUNC` for two distinct reasons:

#### 1. Flash Write Operations (Hardware Constraint)

**CRITICAL**: Code that writes or erases flash **cannot execute from flash** - it must run from RAM. This is a hardware limitation: the flash controller cannot simultaneously execute code and modify flash memory.

```c
PAC5XXX_RAMFUNC void flash_write(uint8_t *dest, uint8_t *src, uint32_t size) {
    // Must run from RAM because flash is being modified
    // Executing from flash during write would cause undefined behavior
}

PAC5XXX_RAMFUNC void flash_erase_page(uint32_t page_num) {
    // Must run from RAM - cannot execute from flash during erase
}
```

**Rule**: Any function that writes/erases flash **must** be `PAC5XXX_RAMFUNC`. See [flash_func.c](firmware/src/nvm/flash_func.c).

#### 2. Control Loop Performance (Zero Wait State)

Mark control loop functions with `TM_RAMFUNC` for deterministic zero-wait-state execution:

```c
TM_RAMFUNC void CLControlStep(void) {
    // Runs at 20 kHz from RAM for predictable timing
}
```

**Rule**: If called every 50 μs (20 kHz), it **must** be `TM_RAMFUNC` for performance.

#### Important: When NOT to Use RAMFUNC

**Do NOT add RAMFUNC unless required by one of the two reasons above.** Unnecessary RAMFUNC markers:
- Waste limited RAM space (only 64 KB total)
- Trigger GCC debug symbol bugs on some compiler versions
- Provide no benefit for non-critical code

Example: NVM wear leveling functions (`nvm_wl_scan_slots`, `calculate_checksum`) do NOT need RAMFUNC:
- Not called from control loop (performance reason doesn't apply)
- Don't write to flash themselves (hardware constraint doesn't apply)
- Called by `nvm_save_config` which calls `flash_write`, but that's fine - only `flash_write` needs RAM placement

### Performance Guidelines

**Float Precision**:
```c
float value = 3.14f;  // ✓ Hardware FPU
double value = 3.14;  // ❌ Software emulation (10-100× slower)
```

**Division Optimization**:
```c
// Bad: Integer division (no hardware divider)
int result = value / 123;

// Good: Pre-compute reciprocal
float result = value * (1.0f / 123.0f);
```

**Blocking Operations** (Never in control loop):
- ❌ `printf()`, `SEGGER_RTT_printf()`
- ❌ `delay()`, busy-wait loops
- ❌ `malloc()`, `free()`
- ❌ Large loops (>100 iterations)

**Allowed Operations**:
- ✓ Simple arithmetic (`+`, `-`, `*`)
- ✓ Fast approximations (`fast_sin()`, `fast_cos()`)
- ✓ Lookup tables
- ✓ Conditional branches

### Example: Well-Formatted Function

```c
float motor_get_temperature(void)
{
    // Estimate temperature from resistance change using copper temp coefficient
    // Formula: T = T_cal + ΔR / (R_cal × α)
    // where α = 0.00393/°C for copper

    MotorConfig *config = motor_get_config();
    float R_cal = config->phase_resistance;  // Resistance at calibration
    float T_cal = 25.0f;  // Assumed calibration temperature (°C)
    float alpha = 0.00393f;  // Copper temperature coefficient

    // Measure current resistance
    float R_current = motor_measure_resistance();

    // Calculate temperature change
    float delta_R = R_current - R_cal;
    float delta_T = delta_R / (R_cal * alpha);

    return T_cal + delta_T;
}
```

## Python Style (Studio)

### General Rules

- **Standard**: PEP 8
- **Formatter**: Black (line length: 127 characters)
- **Type Hints**: Encouraged for public APIs
- **Docstrings**: Required for public APIs (Google style)

### Function Example

```python
def calibrate_motor(current: float, timeout: float = 10.0) -> bool:
    """
    Calibrate motor with specified current.

    Args:
        current: Calibration current in amperes
        timeout: Maximum time to wait in seconds

    Returns:
        True if calibration succeeded, False otherwise

    Raises:
        ValueError: If current is negative or exceeds safe limits
    """
    if current < 0 or current > MAX_CALIBRATION_CURRENT:
        raise ValueError(f"Invalid calibration current: {current}")

    # Implementation...
    return True
```

### Test Naming

```python
class TestMyFeature(TMTestCase):
    @pytest.mark.hitl_default
    def test_feature_basic(self):
        """Test basic functionality of feature"""
        pass

    @pytest.mark.hitl_default
    def test_feature_edge_case(self):
        """Test edge case handling"""
        pass
```

### Unit Handling

```python
from pint import UnitRegistry
ureg = UnitRegistry()

# Reading returns unit-aware quantity
voltage = tm.Vbus  # Returns: 24.0 <Unit('volt')>

# Writing accepts units
tm.controller.current.Iq_setpoint = 2.5 * ureg.ampere

# Or plain floats (assumes base unit)
tm.controller.current.Iq_setpoint = 2.5  # Interpreted as amperes
```

## YAML Style (Avlos Specs)

### General Rules

- **Indentation**: 2 spaces (consistent with existing specs)
- **Structure**: Follow existing patterns in [tinymovr_2_3_x.yaml](studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml)
- **Documentation**: Clear `summary` for all endpoints
- **Ordering**: Keep logical grouping (related attributes together)

### Endpoint Pattern

```yaml
- name: parameter_name
  dtype: float                    # Data type
  unit: ampere                    # Physical unit (if applicable)
  meta: {dynamic: True}           # Metadata (optional)
  getter_name: module_get_parameter  # C getter function
  setter_name: module_set_parameter  # C setter function (optional)
  summary: Brief description of what this parameter represents.
```

### Nested Attributes

```yaml
- name: controller
  remote_attributes:
    - name: position
      remote_attributes:
        - name: setpoint
          dtype: float
          unit: tick
          getter_name: controller_get_pos_setpoint
          setter_name: controller_set_pos_setpoint
          summary: Position setpoint in user frame.
```

### Enums

```yaml
- name: state
  options: [IDLE, CALIBRATE, CL_CONTROL]
  getter_name: controller_get_state
  setter_name: controller_set_state
  summary: Current controller state.
```

### Flags (Bitmasks)

```yaml
- name: errors
  flags: [UNDERVOLTAGE, OVERCURRENT]
  meta: {dynamic: True}
  getter_name: system_get_errors
  summary: System errors as bitmask.
```

## Common Code Patterns

### Pattern: Adding State Variable to Firmware Module

**Header** ([firmware/src/module/module.h](firmware/src/module/module.h)):
```c
typedef struct {
    float my_value;  // New state variable
    // ... existing fields ...
} ModuleState;
```

**Implementation** ([firmware/src/module/module.c](firmware/src/module/module.c)):
```c
static ModuleState state = {
    .my_value = 0.0f,  // Default initialization
};

float module_get_my_value(void) {
    return state.my_value;
}

void module_set_my_value(float value) {
    state.my_value = value;
}
```

### Pattern: Persistent Configuration (Saved to Flash)

**Header** ([firmware/src/module/module.h](firmware/src/module/module.h)):
```c
typedef struct {
    float my_parameter;  // Will be saved to NVM
} ModuleConfig;

ModuleConfig *module_get_config(void);
void module_restore_config(ModuleConfig *config_);
```

**Implementation** ([firmware/src/module/module.c](firmware/src/module/module.c)):
```c
static ModuleConfig config = {
    .my_parameter = 10.0f,  // Default value
};

ModuleConfig *module_get_config(void) {
    return &config;
}

void module_restore_config(ModuleConfig *config_) {
    memcpy(&config, config_, sizeof(ModuleConfig));
}
```

**NVM Integration** ([firmware/src/nvm/nvm.c](firmware/src/nvm/nvm.c)):
```c
// In NVMStruct:
typedef struct {
    // ... existing configs ...
    ModuleConfig module_config;  // Add new config
} NVMStruct;

// In nvm_save_config():
nvm_struct.module_config = *module_get_config();

// In nvm_load_config():
module_restore_config(&nvm_struct.module_config);
```

### Pattern: Reading Firmware State (Python)

```python
# Simple attribute read
voltage = tm.Vbus
current = tm.Ibus
position = tm.encoder.position_estimate

# Nested attributes
setpoint = tm.controller.position.setpoint

# Enums/options
state = tm.controller.state
if state == 0:  # IDLE
    print("Motor is idle")
elif state == 2:  # CL_CONTROL
    print("Closed-loop control active")

# Flags/bitmasks
errors = tm.motor.errors
if errors & (1 << 4):  # Check specific bit
    print("Abnormal calibration voltage")
```

### Pattern: Writing Firmware State (Python)

```python
# Simple write
tm.controller.current.Iq_setpoint = 2.5  # amperes

# With units
from pint import UnitRegistry
ureg = UnitRegistry()
tm.controller.current.Iq_setpoint = 2.5 * ureg.ampere

# Set limits
tm.controller.velocity.limit = 100000  # ticks/s
tm.controller.current.Iq_limit = 10.0  # amperes

# Change mode
tm.controller.position_mode()  # Enter position control
tm.controller.velocity_mode()  # Enter velocity control
tm.controller.current_mode()   # Enter current control
tm.controller.idle()           # Enter idle (safe stop)
```

### Pattern: Control Loop Modification

**File**: [firmware/src/controller/controller.c](firmware/src/controller/controller.c)

```c
// Mark function for RAM execution
TM_RAMFUNC void CLControlStep(void) {
    // Read sensors
    observer_update();

    // Example: Add low-pass filter
    static float Iq_filtered = 0.0f;
    const float alpha = 0.1f;  // Filter coefficient
    float Iq_raw = ADC_get_Iq();
    Iq_filtered = alpha * Iq_raw + (1.0f - alpha) * Iq_filtered;

    // Use filtered value in control loop
    // ... rest of control algorithm ...
}
```

**Verify timing**:
```python
load = tm.scheduler.load
assert load < 3000, f"Control loop overrun: {load} cycles"
```

## Naming Conventions

### Firmware Constants

```c
// All caps with underscores
#define PWM_FREQ_HZ (20000)
#define MAX_CALIBRATION_VOLTAGE (0.5f)
#define CAL_R_LEN (2 * PWM_FREQ_HZ)

// Board-specific prefixes
#if defined BOARD_REV_R5
#define MIN_TRIP_CURRENT (5.0f)
#elif defined BOARD_REV_M5
#define MIN_TRIP_CURRENT (1.0f)
#endif
```

### Firmware Types

```c
// PascalCase for structs and enums
typedef struct {
    float phase_resistance;
    float phase_inductance;
} MotorConfig;

typedef enum {
    CONTROLLER_STATE_IDLE = 0,
    CONTROLLER_STATE_CALIBRATE = 1,
    CONTROLLER_STATE_CL_CONTROL = 2
} controller_state_options;
```

### Firmware Functions

```c
// module_action_object (lowercase with underscores)
float motor_get_temperature(void);
void controller_set_state(uint8_t state);
bool ADC_calibrate_offset(void);
```

### Python Variables

```python
# snake_case for variables and functions
current_setpoint = 2.5
position_estimate = tm.encoder.position_estimate

def calibrate_motor(current: float) -> bool:
    pass

# PascalCase for classes
class TMTestCase(unittest.TestCase):
    pass
```

## File Organization

### Firmware Module Structure

```
firmware/src/module/
├── module.h           # Public interface (typedefs, function declarations)
├── module.c           # Implementation (static state, function definitions)
└── README.md          # Module documentation (optional)
```

**Header Pattern**:
```c
#pragma once

// Forward declarations
typedef struct ModuleState ModuleState;
typedef struct ModuleConfig ModuleConfig;

// Public API
void module_init(void);
float module_get_value(void);
void module_set_value(float value);
```

**Implementation Pattern**:
```c
#include "module.h"

// Static state (module-private)
static ModuleState state = {0};
static ModuleConfig config = {0};

// Public functions
void module_init(void) {
    // Initialize module
}

float module_get_value(void) {
    return state.value;
}
```

### Python Test Structure

```python
import pytest
from tests.tm_test_case import TMTestCase

class TestFeatureName(TMTestCase):
    """Test suite for specific feature"""

    def setUp(self):
        """Setup before each test (optional)"""
        super().setUp()

    @pytest.mark.hitl_default
    def test_basic_functionality(self):
        """Test basic operation"""
        self.try_calibrate()
        # Test implementation

    @pytest.mark.hitl_default
    def test_edge_cases(self):
        """Test boundary conditions"""
        self.try_calibrate()
        # Test implementation
```

## Documentation Comments

### Firmware

```c
/**
 * @brief Estimate motor temperature from phase resistance change
 *
 * Uses the temperature coefficient of copper (0.00393/°C) to estimate
 * winding temperature based on measured resistance during operation.
 *
 * @return Estimated temperature in degrees Celsius
 *
 * @note Assumes calibration was performed at 25°C
 * @warning Accuracy degrades if motor is rapidly accelerating/decelerating
 */
float motor_get_temperature(void);
```

### Python

```python
def calibrate_motor(current: float, timeout: float = 10.0) -> bool:
    """
    Perform motor calibration sequence.

    Calibrates phase resistance, inductance, pole pairs, and encoder offset.
    Motor must be free to rotate during calibration.

    Args:
        current: Calibration current in amperes (typically 1-5A)
        timeout: Maximum time to wait for calibration completion (seconds)

    Returns:
        True if calibration succeeded, False otherwise

    Raises:
        ValueError: If current is outside safe range
        TimeoutError: If calibration exceeds timeout

    Example:
        >>> tm = create_device(node_id=1)
        >>> success = calibrate_motor(current=2.5, timeout=15.0)
        >>> if success:
        ...     print("Calibration complete")
    """
    pass
```

## References

- [ARCHITECTURE.md](ARCHITECTURE.md) - System design and module organization
- [AVLOS_GUIDE.md](AVLOS_GUIDE.md) - YAML specification details
- [SAFETY.md](SAFETY.md) - Performance and safety constraints
- [TESTING.md](TESTING.md) - Testing practices and patterns
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution workflow

---

**Document Status**: Living document, updated as coding conventions evolve.
