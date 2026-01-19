# Avlos Framework Guide

## What is Avlos?

**Avlos** is a schema-driven code generation framework that automatically creates communication protocol implementations for embedded systems. It generates both firmware (C code) and client library (Python) code from a single YAML specification, ensuring perfect synchronization between device and host.

**Key Benefits**:
- **Single Source of Truth**: One YAML file defines the entire API surface
- **Zero Boilerplate**: No manual protocol wrapper coding required
- **Type Safety**: Automatic serialization/deserialization with type checking
- **Version Control**: Protocol hash ensures firmware-client compatibility
- **Self-Documenting**: API documentation generated from YAML summaries

**Avlos Repository**: https://github.com/tinymovr/avlos

## How Tinymovr Uses Avlos

Tinymovr's entire CAN bus protocol (97 endpoints) is defined in YAML and automatically generated into:
1. **Firmware**: C functions for handling CAN messages ([firmware/src/can/can_endpoints.c](firmware/src/can/can_endpoints.c))
2. **Python Client**: Dynamic Python objects with attribute access ([studio/Python/tinymovr/](studio/Python/tinymovr/))
3. **Documentation**: RST reference documentation ([docs/protocol/reference.rst](docs/protocol/reference.rst))

**Current Protocol Version**: Hash `641680925` (v2.3.x)

## YAML Specification Structure

### Basic Anatomy

Avlos specs are hierarchical trees of `remote_attributes`. Each attribute can represent:
- **Readable value** (with `getter_name`)
- **Writable value** (with `setter_name`)
- **Callable function** (with `caller_name`)
- **Nested namespace** (with child `remote_attributes`)

**Reference**: [studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml](studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml)

### Example: Simple Read-Only Attribute

```yaml
name: tm
remote_attributes:
  - name: Vbus
    dtype: float
    unit: volt
    meta: {dynamic: True}
    getter_name: system_get_Vbus
    summary: The measured bus voltage.
```

**Generated Firmware** ([firmware/src/can/can_endpoints.h](firmware/src/can/can_endpoints.h)):
```c
// Endpoint ID: 4
uint8_t avlos_Vbus(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd);
```

**Generated Python**:
```python
voltage = tm.Vbus  # Automatically calls avlos_Vbus endpoint
```

### Example: Read-Write Attribute

```yaml
- name: position
  remote_attributes:
    - name: setpoint
      dtype: float
      unit: tick
      getter_name: controller_get_pos_setpoint_user_frame
      setter_name: controller_set_pos_setpoint_user_frame
      summary: The position setpoint in user frame.
```

**Generated Python**:
```python
tm.controller.position.setpoint = 5000  # Write via setter
current_setpoint = tm.controller.position.setpoint  # Read via getter
```

### Example: Callable Function

```yaml
- name: calibrate
  summary: Initiate calibration sequence.
  caller_name: controller_calibrate
  dtype: void
  arguments: []
```

**Generated Python**:
```python
tm.controller.calibrate()  # Calls controller_calibrate() on device
```

### Example: Enum/Options

```yaml
- name: state
  options: [IDLE, CALIBRATE, CL_CONTROL]
  getter_name: controller_get_state
  setter_name: controller_set_state
  summary: The current controller state.
```

**Generated Firmware** ([firmware/src/tm_enums.h](firmware/src/tm_enums.h)):
```c
typedef enum
{
    CONTROLLER_STATE_IDLE = 0,
    CONTROLLER_STATE_CALIBRATE = 1,
    CONTROLLER_STATE_CL_CONTROL = 2
} controller_state_options;
```

**Generated Python**:
```python
tm.controller.state = 2  # Set to CL_CONTROL
if tm.controller.state == 0:  # Check if IDLE
    print("Device is idle")
```

### Example: Flags (Bitmasks)

```yaml
- name: errors
  flags: [UNDERVOLTAGE]
  meta: {dynamic: True}
  getter_name: system_get_errors
  summary: Any system errors, as a bitmask
```

**Generated Firmware**:
```c
typedef enum
{
    ERRORS_NONE = 0,
    ERRORS_UNDERVOLTAGE = (1 << 0)
} errors_flags;
```

**Usage**:
```python
if tm.errors & (1 << 0):  # Check UNDERVOLTAGE bit
    print("Undervoltage detected!")
```

## Attribute Properties

### Required Properties

- **`name`**: Identifier for the attribute (used in Python API and C function names)
- **`summary`**: Brief description (appears in documentation and generated comments)

### Data Type Properties

- **`dtype`**: Data type - `float`, `uint32`, `int32`, `uint16`, `int16`, `uint8`, `int8`, `bool`, `string`, `void`
- **`unit`**: Physical unit - `tick`, `ampere`, `volt`, `degC`, `ohm`, `henry`, `watt`, etc.
- **`options`**: List of enum values (mutually exclusive with `flags`)
- **`flags`**: List of bitmask flag names (mutually exclusive with `options`)

### Access Properties

- **`getter_name`**: C function name for reading the value
- **`setter_name`**: C function name for writing the value
- **`caller_name`**: C function name for calling (void functions)
- **`arguments`**: List of argument definitions for callable functions

### Metadata

- **`meta`**: Dictionary of optional metadata
  - `dynamic: True` - Value changes frequently (read every time)
  - `reload_data: True` - Causes device reset/reload after call

### Nested Attributes

```yaml
- name: controller
  remote_attributes:
    - name: position
      remote_attributes:
        - name: setpoint
          dtype: float
          ...
```

**Generates Nested Access**:
```python
tm.controller.position.setpoint = 1000
```

## Code Generation Configuration

The code generation process is configured in [avlos_config.yaml](avlos_config.yaml):

```yaml
generators:
  generator_c:
    enabled: true
    paths:
      output_enums: ./firmware/src/tm_enums.h
      output_header: ./firmware/src/can/can_endpoints.h
      output_impl: ./firmware/src/can/can_endpoints.c
    header_includes:
    - src/common.h
    - src/tm_enums.h
    impl_includes:
    - <string.h>
    - src/adc/adc.h
    - src/system/system.h
    - src/sensor/sensors.h
    - src/observer/observer.h
    - src/motor/motor.h
    - src/scheduler/scheduler.h
    - src/controller/controller.h
    - src/nvm/nvm.h
    - src/watchdog/watchdog.h
    - src/can/can_endpoints.h
  generator_rst:
    enabled: true
    paths:
      output_file: ./docs/protocol/reference.rst
```

## Complete Workflow: Adding a New Endpoint

### Step 1: Edit YAML Specification

Let's add a motor temperature estimation endpoint:

**File**: [studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml](studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml)

Find the `motor` section and add:

```yaml
- name: motor
  remote_attributes:
    # ... existing attributes ...
    - name: temperature
      dtype: float
      unit: degC
      meta: {dynamic: True}
      getter_name: motor_get_temperature
      summary: Estimated motor temperature from resistance change.
```

### Step 2: Run Code Generator

```bash
cd studio/Python/tinymovr/specs
avlos from file <spec_file>.yaml
```

Replace `<spec_file>` with the appropriate protocol version spec file (e.g., `tinymovr_2_3_x`, `tinymovr_2_4_x`, etc.). Use the spec file that matches the firmware version you're working with.

**Output**:
```
Generating C code...
  Written: firmware/src/tm_enums.h
  Written: firmware/src/can/can_endpoints.h
  Written: firmware/src/can/can_endpoints.c
Generating RST documentation...
  Written: docs/protocol/reference.rst
Protocol hash: 641680925 -> 789456123 (CHANGED!)
```

**Important**: The protocol hash **will change** with any YAML modification. This breaks compatibility with existing firmware.

### Step 3: Implement C Function

Create the getter function in [firmware/src/motor/motor.c](firmware/src/motor/motor.c):

```c
float motor_get_temperature(void)
{
    // Estimate temperature from resistance change
    // T = T_cal + (R - R_cal) / (R_cal * alpha)
    // where alpha = temperature coefficient of copper (0.00393 / °C)

    MotorConfig *config = motor_get_config();
    float R_cal = config->phase_resistance;  // Resistance at calibration
    float T_cal = 25.0f;  // Assumed calibration temperature (°C)
    float alpha = 0.00393f;  // Copper temp coefficient

    // Measure current resistance (simplified)
    float R_current = R_cal;  // TODO: Implement actual measurement

    float delta_T = (R_current - R_cal) / (R_cal * alpha);
    return T_cal + delta_T;
}
```

Add declaration to [firmware/src/motor/motor.h](firmware/src/motor/motor.h):

```c
float motor_get_temperature(void);
```

### Step 4: Build Firmware

```bash
cd firmware
make release REV=R52
```

**Verify**:
- No compilation errors
- Binary size hasn't exceeded flash capacity
- New endpoint included in endpoint array (97 -> 98 endpoints)

### Step 5: Flash Firmware

```bash
# Using J-Link (example)
JLinkExe -device PAC5527 -if SWD -speed 4000
> loadbin build/tinymovr_fw.bin 0x00000000
> r
> g
> exit
```

### Step 6: Test Python Client

The Python client automatically generates the new attribute from the YAML spec:

```python
from tinymovr import init_router, create_device
from tinymovr.config import get_bus_config
import can

# Initialize CAN bus
params = get_bus_config(["canine", "slcan_disco"], bitrate=1000000)
init_router(can.Bus, params)

# Create device
tm = create_device(node_id=1)

# Check protocol hash matches
print(f"Protocol hash: {tm.protocol_hash}")  # Should be 789456123

# Read new endpoint
temp = tm.motor.temperature
print(f"Motor temperature: {temp:.1f} °C")
```

### Step 7: Write Tests

Add test to [studio/Python/tests/test_board.py](studio/Python/tests/test_board.py):

```python
@pytest.mark.hitl_default
def test_motor_temperature(self):
    """Test motor temperature estimation"""
    self.try_calibrate()

    # Read temperature
    temp = self.tm.motor.temperature

    # Sanity check: temperature should be reasonable
    self.assertGreater(temp, 0.0)   # Above absolute zero
    self.assertLess(temp, 150.0)    # Below typical motor max
```

Run test:

```bash
cd studio/Python
pytest -m hitl_default tests/test_board.py::TestTinymovr::test_motor_temperature
```

## Protocol Hash System

### What is the Protocol Hash?

The protocol hash is a **32-bit checksum** computed from the entire YAML spec structure. It ensures firmware and client are using compatible protocol definitions.

**Reference**: [firmware/src/can/can_endpoints.h](firmware/src/can/can_endpoints.h#L13)
```c
static const uint32_t avlos_proto_hash = 641680925;
```

### Hash Verification on Connection

When the Python client connects to a device:

1. Read device's `protocol_hash` endpoint (endpoint ID 0)
2. Compare to hash computed from local YAML spec
3. If mismatch → Raise `IncompatibleSpecVersionError`

**Reference**: [studio/Python/tinymovr/config/config.py](studio/Python/tinymovr/config/config.py#L46-L64)

```python
def get_device_spec(hash, logger=None):
    try:
        return specs["hash_uint32"][hash]
    except KeyError:
        # Try hash aliases for backward compatibility
        for hash_alias in hash_aliases.get(hash, []):
            try:
                return specs["hash_uint32"][hash_alias]
            except KeyError:
                pass
    return None  # No compatible spec found
```

### When Does the Hash Change?

The hash changes when **any** of the following are modified in the YAML:
- Add/remove/rename an attribute
- Change data type (`dtype`)
- Change unit
- Modify options/flags
- Change nesting structure
- Change order of attributes (endpoint IDs shift)

### Breaking vs. Non-Breaking Changes

**Breaking Changes** (new hash required):
- Adding new endpoints → Endpoint IDs shift → Hash changes
- Changing data types → Serialization incompatible → Hash changes
- Removing endpoints → Missing functionality → Hash changes

**Non-Breaking Changes** (can use hash aliases):
- Fixing typos in `summary` → Hash changes but protocol compatible
- Changing metadata (`meta`) → Hash changes but protocol compatible

**Hash Aliases** ([studio/Python/tinymovr/config/config.py](studio/Python/tinymovr/config/config.py#L43)):
```python
# Allow old clients to work with new firmware if only cosmetic changes
hash_aliases = {3526126264: [4118115615]}
```

## Multi-Version Support

### Version-Specific YAML Files

Tinymovr maintains multiple YAML specs for different firmware versions:

```
studio/Python/tinymovr/specs/
├── tinymovr_1_3_x.yaml  # Legacy v1.3.x
├── tinymovr_1_4_x.yaml  # Legacy v1.4.x
├── tinymovr_2_0_x.yaml  # v2.0.x
├── tinymovr_2_1_x.yaml  # v2.1.x
├── tinymovr_2_2_x.yaml  # v2.2.x
└── tinymovr_2_3_x.yaml  # Current v2.3.x (hash: 641680925)
```

### Automatic Spec Selection

The Python client automatically selects the correct spec based on device hash:

```python
# Device discovery reads protocol_hash from device
device_hash = 641680925  # Read from device

# Load matching spec
spec = get_device_spec(device_hash)
if spec is None:
    raise IncompatibleSpecVersionError(device_hash)

# Deserialize spec into Python object tree
tm = deserialize(spec)
```

### Maintaining Backward Compatibility

**Best Practices**:
1. **Never remove endpoints** - Mark as deprecated instead
2. **Append new endpoints** - Add to end to avoid shifting IDs
3. **Create new version** - Copy YAML, modify, update hash
4. **Test with old clients** - Ensure graceful degradation

**Example: Deprecation Pattern**:
```yaml
- name: old_parameter
  dtype: float
  getter_name: get_old_parameter
  summary: "[DEPRECATED] Use new_parameter instead."
```

## Breaking Changes and Protocol Versioning

### When to Create a New Protocol Version

**Always create a new protocol version** (e.g., 2.3.x → 2.4.x) when making breaking changes:

1. **API Structure Changes**
   - Moving endpoints to new namespaces
   - Renaming endpoints
   - Changing endpoint hierarchy

2. **Data Type Changes**
   - Changing `dtype` of existing endpoint
   - Changing units
   - Changing enum values or flags

3. **Removing Endpoints**
   - Deleting functionality (even if deprecated)

**Never** create new versions for:
- Adding new endpoints (append to end of spec)
- Fixing typos in documentation (`summary` fields)
- Adding metadata (`meta` fields)
- Internal implementation changes

### How to Create a New Protocol Version

1. **Copy the previous version file**:
   ```bash
   cp studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml \
      studio/Python/tinymovr/specs/tinymovr_2_4_x.yaml
   ```

2. **Make breaking changes** in the new file

3. **Generate code** for both versions:
   ```bash
   avlos from file <spec_file>.yaml
   ```

4. **Update firmware** to use new generated code

5. **Test with Python client** - it will automatically detect version via protocol hash

6. **Keep old version file** for backward compatibility with older firmware

### Example: Adding Monitoring Endpoints Under New Namespace

When adding new monitoring/debug endpoints, group them under a descriptive namespace while keeping user-facing operations at the top level for backward compatibility.

**Before** (v2.3.x):
```yaml
name: tm
remote_attributes:
  - name: save_config
    caller_name: nvm_save_config
    dtype: void
  - name: erase_config
    caller_name: nvm_erase_and_reset
    dtype: void
```

**After** (v2.4.x) - Adding wear leveling monitoring:
```yaml
name: tm
remote_attributes:
  # User operations stay at top level (no breaking changes)
  - name: save_config
    caller_name: nvm_save_config
    dtype: void
  - name: erase_config
    caller_name: nvm_erase_and_reset
    dtype: void
  # New monitoring endpoints under nvm namespace
  - name: nvm
    remote_attributes:
      - name: num_slots
        getter_name: nvm_wl_get_num_slots
        dtype: uint8
      - name: current_slot
        getter_name: nvm_wl_get_current_slot
        dtype: uint8
      - name: write_count
        getter_name: nvm_wl_get_write_count
        dtype: uint32
```

**Impact**:
- User operations unchanged: `tm.save_config()`, `tm.erase_config()` (backward compatible)
- New monitoring API: `tm.nvm.num_slots`, `tm.nvm.write_count`, etc.
- Protocol hash changes (2.3.x clients won't see new endpoints, but existing code works)
- Tests can detect protocol version via `hasattr(tm, 'nvm')` for optional features

### Version Numbering Convention

- **Major version** (e.g., 1.x → 2.x): Major firmware rewrite, architecture change
- **Minor version** (e.g., 2.3.x → 2.4.x): Breaking protocol changes, API reorganization
- **Patch version** (within x.y.x): Usually not reflected in spec filename, used for firmware-only changes

## Generated Code Structure

### Firmware (C)

#### Enumerations ([firmware/src/tm_enums.h](firmware/src/tm_enums.h))

```c
// Auto-generated from YAML
typedef enum
{
    CONTROLLER_STATE_IDLE = 0,
    CONTROLLER_STATE_CALIBRATE = 1,
    CONTROLLER_STATE_CL_CONTROL = 2
} controller_state_options;

typedef enum
{
    MOTOR_ERRORS_NONE = 0,
    MOTOR_ERRORS_PHASE_RESISTANCE_OUT_OF_RANGE = (1 << 0),
    MOTOR_ERRORS_PHASE_INDUCTANCE_OUT_OF_RANGE = (1 << 1),
    // ... more error flags
} motor_errors_flags;
```

#### Endpoint Array ([firmware/src/can/can_endpoints.h](firmware/src/can/can_endpoints.h#L14))

```c
// Array of 97 function pointers, indexed by endpoint ID
extern uint8_t (*avlos_endpoints[97])(uint8_t * buffer,
                                       uint8_t * buffer_len,
                                       Avlos_Command cmd);
```

#### Endpoint Functions ([firmware/src/can/can_endpoints.c](firmware/src/can/can_endpoints.c))

```c
uint8_t avlos_Vbus(uint8_t * buffer, uint8_t * buffer_len, Avlos_Command cmd)
{
    if (cmd == AVLOS_CMD_READ)
    {
        float value = system_get_Vbus();  // Call implementation
        serialize_float(value, buffer);   // Pack into CAN frame
        *buffer_len = sizeof(float);
        return AVLOS_RET_READ;
    }
    return AVLOS_RET_NOACTION;
}
```

### Python Client

#### Dynamic Object Tree

The Python client creates a nested object structure at runtime:

```python
from avlos.deserializer import deserialize

# Load YAML spec
spec = yaml.safe_load(open("tinymovr_2_3_x.yaml"))

# Deserialize into Python objects
tm = deserialize(spec)

# Object hierarchy mirrors YAML structure
tm.Vbus                              # Float attribute
tm.controller.state                   # Enum attribute
tm.controller.position.setpoint       # Nested float attribute
tm.controller.calibrate()            # Callable method
```

#### Unit-Aware Quantities (Pint Integration)

Attributes with `unit` in YAML return Pint quantities:

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

## Testing Protocol Changes

### Hardware-in-the-Loop (HITL) Tests

All protocol changes **must** be validated with HITL tests:

```bash
cd studio/Python

# Run full test suite (requires hardware)
pytest -m hitl_default

# Run specific test
pytest -m hitl_default tests/test_board.py::TestTinymovr::test_position_control

# Run end-of-line comprehensive tests
pytest -m eol
```

### Test Structure

HITL tests use [studio/Python/tests/tm_test_case.py](studio/Python/tests/tm_test_case.py) base class:

```python
import pytest
from tests.tm_test_case import TMTestCase

class TestNewEndpoint(TMTestCase):
    @pytest.mark.hitl_default
    def test_new_endpoint_read(self):
        """Test reading new endpoint"""
        self.try_calibrate()  # Calibrate if needed

        value = self.tm.new_endpoint
        self.assertIsNotNone(value)

    @pytest.mark.hitl_default
    def test_new_endpoint_write(self):
        """Test writing new endpoint"""
        self.try_calibrate()

        self.tm.new_endpoint = 123.45
        readback = self.tm.new_endpoint

        self.assertAlmostEqual(readback, 123.45, delta=0.01)
```

### Simulation Tests (No Hardware)

For logic that doesn't require hardware:

```python
# tests/test_simulation.py
import unittest

class TestProtocolHash(unittest.TestCase):
    def test_hash_consistency(self):
        """Verify protocol hash computation is deterministic"""
        from tinymovr.config import specs

        # Load spec twice
        spec1 = specs["hash_uint32"][641680925]
        spec2 = specs["hash_uint32"][641680925]

        # Hashes should match
        self.assertEqual(spec1, spec2)
```

## Common Pitfalls

### 1. Forgetting to Regenerate Code

**Problem**: Modified YAML but didn't run `avlos.generate`

**Symptom**: Python client has new attribute, firmware doesn't recognize it

**Solution**: Always run code generation after YAML changes:
```bash
avlos from file <spec_file>.yaml
```

### 2. Editing Generated Files Directly

**Problem**: Modified `can_endpoints.c` or `tm_enums.h` manually

**Symptom**: Changes lost on next code generation

**Solution**: Never edit generated files. They have warnings at the top:
```c
/*
* This file was automatically generated using Avlos.
* Any changes to this file will be overwritten when
* content is regenerated.
*/
```

### 3. C Function Doesn't Exist

**Problem**: Added endpoint with `getter_name` but function not implemented

**Symptom**: Linker error: `undefined reference to 'motor_get_temperature'`

**Solution**: Implement the C function before building firmware

### 4. Type Mismatch

**Problem**: YAML says `dtype: uint32` but C function returns `float`

**Symptom**: Garbage data, incorrect values, or crashes

**Solution**: Ensure C function signature matches YAML dtype:
```yaml
dtype: uint32   → uint32_t function_name(void)
dtype: float    → float function_name(void)
dtype: bool     → bool function_name(void)
```

### 5. Protocol Hash Mismatch

**Problem**: Firmware and Python client have different protocol hashes

**Symptom**: `IncompatibleSpecVersionError` on device connection

**Solution**:
- Flash updated firmware with new hash
- Or revert YAML changes to match firmware
- Or add hash alias for backward compatibility

## Advanced Topics

### Custom Data Types

Avlos supports structures via nested attributes:

```yaml
- name: pid_gains
  remote_attributes:
    - name: kp
      dtype: float
      getter_name: controller_get_kp
      setter_name: controller_set_kp
    - name: ki
      dtype: float
      getter_name: controller_get_ki
      setter_name: controller_set_ki
```

**Access**:
```python
tm.controller.pid_gains.kp = 0.5
tm.controller.pid_gains.ki = 0.01
```

### Function Arguments

Callables can accept arguments:

```yaml
- name: set_limits
  caller_name: controller_set_limits
  dtype: void
  arguments:
    - name: vel_limit
      dtype: float
      unit: tick/s
    - name: current_limit
      dtype: float
      unit: ampere
```

**Generated C**:
```c
void controller_set_limits(float vel_limit, float current_limit);
```

**Python Call**:
```python
tm.controller.set_limits(100000, 5.0)
```

### Dynamic vs. Static Attributes

Attributes marked `meta: {dynamic: True}` are **read every time**:

```yaml
- name: Vbus
  dtype: float
  meta: {dynamic: True}  # Read from device on every access
  getter_name: system_get_Vbus
```

Static attributes (no `dynamic` flag) are **cached**:

```yaml
- name: hw_revision
  dtype: uint32
  getter_name: system_get_hw_revision  # Read once, cached
```

## References

### Avlos Framework
- **Repository**: https://github.com/tinymovr/avlos
- **Documentation**: https://github.com/tinymovr/avlos/blob/main/README.md

### Tinymovr Files
- [avlos_config.yaml](avlos_config.yaml) - Code generation configuration
- [studio/Python/tinymovr/specs/](studio/Python/tinymovr/specs/) - YAML specifications
- [firmware/src/can/can_endpoints.h](firmware/src/can/can_endpoints.h) - Generated C header
- [firmware/src/can/can_endpoints.c](firmware/src/can/can_endpoints.c) - Generated C implementation
- [firmware/src/tm_enums.h](firmware/src/tm_enums.h) - Generated enumerations
- [studio/Python/tinymovr/config/config.py](studio/Python/tinymovr/config/config.py) - Spec loading
- [docs/protocol/reference.rst](docs/protocol/reference.rst) - Generated documentation

### Related Documentation
- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture overview
- [CLAUDE.md](CLAUDE.md) - Development guidelines for AI agents
- [SAFETY.md](SAFETY.md) - Safety-critical constraints

---

**Document Status**: Living document, updated as Avlos framework evolves.
**Current Version**: Based on Avlos v0.6.6+ and Tinymovr firmware v2.3.x.
