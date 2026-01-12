# Tinymovr Testing Guide

**Purpose**: Testing infrastructure, practices, and requirements for Tinymovr development.

**Target Audience**: Developers and AI agents writing tests for firmware or Python client.

## Test Types

### 1. Simulation Tests (No Hardware Required)

**Purpose**: Logic validation, protocol testing, unit tests that don't require physical hardware.

**Location**: [studio/Python/tests/test_simulation.py](studio/Python/tests/test_simulation.py)

**Running**:
```bash
cd studio/Python
python -m unittest tests/test_simulation.py
```

**When to Use**:
- Testing protocol hash computation
- Validating spec parsing
- Testing serialization/deserialization logic
- Unit testing helper functions

### 2. Hardware-in-the-Loop (HITL) Tests

**Purpose**: End-to-end testing with real Tinymovr hardware.

**Requirements**:
- Tinymovr board (R3.x, R5.x, or M5.x)
- Motor connected (type depends on board)
- CAN interface (CANine, slcan, socketcan, etc.)
- Motor must be free to rotate

**Location**: [studio/Python/tests/](studio/Python/tests/)

**Running**:
```bash
cd studio/Python

# Basic hardware tests
pytest -m hitl_default

# Verbose output
pytest tests/test_board.py -v

# Specific test
pytest tests/test_board.py::TestTinymovr::test_position_control -v
```

### 3. Sensor-Specific Tests

**Purpose**: Testing specific encoder types.

**Markers**:
- `@pytest.mark.sensor_amt22` - Requires AMT22 encoder
- `@pytest.mark.sensor_as5047` - Requires AS5047 encoder
- `@pytest.mark.sensor_hall` - Requires Hall effect sensors

**Running**:
```bash
pytest -m sensor_as5047   # Only AS5047 tests
pytest -m sensor_amt22    # Only AMT22 tests
pytest -m sensor_hall     # Only Hall sensor tests
```

### 4. End-of-Line (EOL) Tests

**Purpose**: Comprehensive production testing.

**Marker**: `@pytest.mark.eol`

**Running**:
```bash
pytest -m eol
```

**Characteristics**:
- Extensive coverage
- Tests all major features
- Validates calibration accuracy
- Checks error handling
- Long duration (15+ minutes)

### 5. DFU/Bootloader Tests

**Purpose**: Firmware update and bootloader testing.

**Marker**: `@pytest.mark.dfu`

**Running**:
```bash
pytest -m dfu
```

## Test Markers Reference

### Pytest Markers

```python
import pytest

@pytest.mark.hitl_default        # Basic hardware test
@pytest.mark.sensor_amt22        # Requires AMT22 encoder
@pytest.mark.sensor_as5047       # Requires AS5047 encoder
@pytest.mark.sensor_hall         # Requires Hall sensors
@pytest.mark.eol                 # End-of-line comprehensive test
@pytest.mark.dfu                 # Bootloader/firmware update test
```

### Running Multiple Markers

```bash
# Run hitl_default AND sensor_as5047 tests
pytest -m "hitl_default or sensor_as5047"

# Run all except EOL tests
pytest -m "not eol"

# Run only AS5047 EOL tests
pytest -m "eol and sensor_as5047"
```

## Writing Tests

### Test Structure (Base Class)

All HITL tests inherit from [TMTestCase](studio/Python/tests/tm_test_case.py):

```python
import pytest
from tests.tm_test_case import TMTestCase

class TestMyFeature(TMTestCase):
    """Test suite for my feature"""

    @pytest.mark.hitl_default
    def test_feature_basic(self):
        """Test basic functionality"""
        # Verify initial state
        self.check_state(0)  # Should be IDLE

        # Calibrate if needed
        self.try_calibrate()

        # Test your feature
        self.tm.my_endpoint = 123.0
        result = self.tm.my_endpoint

        self.assertAlmostEqual(result, 123.0, delta=0.1)

    @pytest.mark.hitl_default
    def test_feature_edge_case(self):
        """Test edge case handling"""
        self.try_calibrate()

        # Test out-of-range input
        with self.assertRaises(Exception):
            self.tm.my_endpoint = 99999.0
```

### TMTestCase Helper Methods

**Key Methods**:
- `self.tm` - Device instance (automatically created in setUp)
- `self.check_state(expected_state)` - Assert controller state
- `self.try_calibrate()` - Calibrate if not already calibrated
- `self.wait_for_calibration()` - Wait for calibration to complete

**Example Usage**:
```python
# Check state
self.check_state(0)  # Verify IDLE
self.check_state(2)  # Verify CL_CONTROL

# Calibrate
self.try_calibrate()  # Only calibrates if needed

# Standard assertions
self.assertEqual(actual, expected)
self.assertAlmostEqual(actual, expected, delta=0.01)
self.assertGreater(value, threshold)
self.assertLess(value, threshold)
```

### Test Naming Conventions

**Format**: `test_<feature>_<scenario>`

**Examples**:
```python
def test_position_control_basic(self):
    """Test basic position control"""
    pass

def test_position_control_trajectory(self):
    """Test position control with trajectory planner"""
    pass

def test_calibration_resistance_range(self):
    """Test resistance calibration with out-of-range motor"""
    pass

def test_error_handling_overcurrent(self):
    """Test automatic transition to IDLE on overcurrent"""
    pass
```

### Example: Complete Test

```python
import pytest
from tests.tm_test_case import TMTestCase

class TestPositionControl(TMTestCase):
    """Test position control functionality"""

    @pytest.mark.hitl_default
    def test_position_setpoint_tracking(self):
        """Test position setpoint tracking accuracy"""
        # Setup
        self.check_state(0)
        self.try_calibrate()

        # Enter position control mode
        self.tm.controller.position_mode()
        self.check_state(2)  # CL_CONTROL

        # Test multiple setpoints
        for setpoint in [0, 5000, -5000, 10000]:
            self.tm.controller.position.setpoint = setpoint

            # Wait for settling
            time.sleep(0.5)

            # Check tracking accuracy
            position = self.tm.sensors.user_frame.position_estimate
            error = abs(position - setpoint)
            self.assertLess(error, 50, f"Tracking error too high: {error} ticks")

        # Return to idle
        self.tm.controller.idle()
        self.check_state(0)

    @pytest.mark.hitl_default
    def test_position_control_velocity_limit(self):
        """Test position control respects velocity limit"""
        self.try_calibrate()

        # Set low velocity limit
        self.tm.controller.velocity.limit = 10000  # ticks/s

        # Enter position control
        self.tm.controller.position_mode()

        # Command large position change
        self.tm.controller.position.setpoint = 50000

        # Monitor velocity during motion
        time.sleep(0.1)
        velocity = self.tm.sensors.user_frame.velocity_estimate

        # Velocity should not exceed limit
        self.assertLess(abs(velocity), 11000, "Velocity limit exceeded")
```

## Safety Testing Requirements

### For Control Loop Changes

If you modify control loop code, **all** of the following tests are required:

**Timing Tests**:
- [ ] Measure `tm.scheduler.load` < 3000 cycles (at 150 MHz)
- [ ] Test at maximum load (all features enabled)
- [ ] Verify no `SCHEDULER_WARNINGS_CONTROL_BLOCK_REENTERED`

**Multi-Board Tests**:
- [ ] Test on R5.x board (high-current)
- [ ] Test on M5.x board (gimbal)
- [ ] (Optional) Test on R3.x board if available

**Multi-Motor Tests**:
- [ ] Test with high-current motor (>5A rated)
- [ ] Test with gimbal motor (<5A rated)
- [ ] Test at different voltages (12V, 24V, 48V if applicable)

**Stability Tests**:
- [ ] Monitor for oscillations (visual and audible)
- [ ] Monitor for instability during acceleration/deceleration
- [ ] Check for overheating (>10 minute run at moderate load)
- [ ] Verify no erratic behavior at startup or shutdown

**Error Handling Tests**:
- [ ] Force overcurrent → Verify IDLE transition
- [ ] Force undervoltage → Verify IDLE transition
- [ ] Trigger watchdog → Verify IDLE transition
- [ ] Check all error flags set correctly

**Code**:
```python
# Timing test
load = tm.scheduler.load
assert load < 3000, f"Control loop overrun: {load} cycles"

# Error test
tm.controller.position_mode()
# Force error condition (e.g., set unrealistic setpoint)
time.sleep(1.0)
assert tm.controller.state == 0, "Did not transition to IDLE on error"
```

### For Calibration Changes

If you modify calibration code:

**Multi-Motor Tests**:
- [ ] Test on 5+ different motors (varying R, L, pole pairs)
- [ ] Verify calibration succeeds consistently (10+ attempts per motor)
- [ ] Check measured R/L values against known specifications
- [ ] Test with noisy power supply (introduce line noise)

**Fault Detection Tests**:
- [ ] Disconnect one phase → Verify abnormal voltage detection
- [ ] Short two phases → Verify abnormal voltage detection
- [ ] Test with wrong motor type → Verify out-of-range detection
- [ ] Test with obstructed shaft → Verify pole pair detection failure

**Code**:
```python
# Calibration accuracy test
tm.controller.calibrate()
time.sleep(5)  # Wait for completion

# Check results
R = tm.motor.R
L = tm.motor.L
pole_pairs = tm.motor.pole_pairs

# Verify within expected range (example for high-current motor)
assert 0.01 < R < 1.0, f"R out of range: {R} Ω"
assert 5e-6 < L < 1e-3, f"L out of range: {L} H"
assert 1 <= pole_pairs <= 24, f"Pole pairs out of range: {pole_pairs}"
```

### For Current Limit Changes

If you modify current limits:

**Incremental Tests**:
- [ ] Gradually increase current and verify trip at 1.5× limit
- [ ] Test at different bus voltages (12V, 24V, 48V)
- [ ] Monitor for hardware damage (smoke, overheating, burnt MOSFETs)
- [ ] Verify warning flags set correctly

**Hardware Monitoring**:
- [ ] Use thermal camera or temperature probe on MOSFETs
- [ ] Monitor bus voltage for droop under load
- [ ] Check for PCB discoloration or component damage
- [ ] Inspect motor windings for overheating

## Test Environment Setup

### Hardware Setup

**Minimal Setup** (for hitl_default):
1. Tinymovr board (R5.2 or M5.1 recommended)
2. Appropriate motor (high-current or gimbal)
3. Power supply (12-48V, adequate current rating)
4. CAN interface (CANine, PCAN-USB, etc.)
5. Motor mounted securely with free shaft rotation

**Extended Setup** (for sensor-specific tests):
- External encoder (AS5047, AMT22) with SPI connection
- Hall sensor motor with 3-wire Hall connections

### Software Setup

**Python Environment**:
```bash
cd studio/Python
pip install -e .              # Install Tinymovr client
pip install pytest            # Install test framework
```

**CAN Interface Configuration**:

**Linux (socketcan)**:
```bash
sudo ip link set can0 type can bitrate 1000000
sudo ip link set up can0
```

**CANine**:
```python
from tinymovr import init_router
from tinymovr.config import get_bus_config
import can

params = get_bus_config(["canine"], bitrate=1000000)
init_router(can.Bus, params)
```

### Device Configuration

**Before Running Tests**:
```python
from tinymovr import create_device

tm = create_device(node_id=1)

# Check firmware version
print(f"Firmware: {tm.fw_version}")
print(f"Protocol hash: {tm.protocol_hash}")

# Check board info
print(f"Board: {tm.board}")

# Erase config for clean state (optional)
tm.erase_config()
tm.reset()
```

## Debugging Failed Tests

### Common Failures

**1. Device Not Found**

**Symptom**:
```
IncompatibleSpecVersionError: Device found, but incompatible
```

**Causes**:
- Protocol hash mismatch (firmware vs. Python client)
- CAN interface not configured
- Wrong node ID
- Device not powered

**Debug**:
```python
# Check protocol hash
print(f"Device hash: {tm.protocol_hash}")
print(f"Expected: 641680925")  # Current v2.3.x hash

# Check CAN bus
import can
bus = can.Bus(interface='socketcan', channel='can0', bitrate=1000000)
print(bus.recv(timeout=1.0))  # Should see heartbeat messages
```

**2. Calibration Fails**

**Symptom**:
```
AssertionError: Motor errors: 16 (ABNORMAL_CALIBRATION_VOLTAGE)
```

**Causes**:
- Motor phases not connected
- Wrong motor type for board (gimbal on R5 board)
- Power supply unstable
- Motor shaft obstructed

**Debug**:
```python
# Read calibration details
print(f"Vbus: {tm.Vbus} V")
print(f"I_cal: {tm.motor.I_cal} A")
print(f"R: {tm.motor.R} Ω")
print(f"Motor errors: {tm.motor.errors}")

# Check expected ranges (R5 board, high-current motor)
# R should be 0.01-1.0 Ω
# L should be 5-1000 μH
```

**3. Position Control Unstable**

**Symptom**: Motor oscillates around setpoint

**Causes**:
- Gains too high
- Encoder noise
- Mechanical resonance
- Current limit too low

**Debug**:
```python
# Check gains
print(f"Position gain: {tm.controller.position.gain}")
print(f"Velocity gain: {tm.controller.velocity.gain}")

# Check limits
print(f"Current limit: {tm.controller.current.Iq_limit} A")
print(f"Velocity limit: {tm.controller.velocity.limit} ticks/s")

# Monitor loop performance
print(f"Scheduler load: {tm.scheduler.load} cycles")
```

**4. Timing Overruns**

**Symptom**:
```
SCHEDULER_WARNINGS_CONTROL_BLOCK_REENTERED
```

**Causes**:
- Control loop code too slow
- Missing `TM_RAMFUNC` annotation
- Using `double` instead of `float`
- Blocking operations in control loop

**Debug**:
```python
load = tm.scheduler.load
print(f"CPU load: {load} / 7500 cycles ({load/7500*100:.1f}%)")

# Should be <3000 cycles (40%)
# >5000 cycles indicates serious problem
```

## Continuous Integration

### Test Selection for CI

**Fast Tests** (run on every commit):
```bash
pytest -m "not hitl_default and not eol"  # Simulation tests only
```

**Hardware Tests** (run on hardware testbed):
```bash
pytest -m hitl_default  # Basic HITL tests (~5 minutes)
```

**Comprehensive Tests** (run before release):
```bash
pytest -m eol  # End-of-line tests (~20 minutes)
```

### Test Reports

Generate JUnit XML for CI integration:
```bash
pytest --junitxml=test-results.xml -v
```

Generate coverage report:
```bash
pytest --cov=tinymovr --cov-report=html
```

## Best Practices

### Do's

✓ **Always calibrate first** in HITL tests (use `self.try_calibrate()`)
✓ **Use appropriate markers** (@pytest.mark.hitl_default, etc.)
✓ **Test edge cases** (limits, errors, invalid inputs)
✓ **Check for errors** after operations
✓ **Return to IDLE** at end of test
✓ **Use descriptive test names** and docstrings
✓ **Test on multiple board revisions** for firmware changes

### Don'ts

❌ **Don't skip calibration** and expect position control to work
❌ **Don't forget markers** on hardware tests (will break CI)
❌ **Don't leave motor running** after test (always call `tm.controller.idle()`)
❌ **Don't test safety-critical code** without hardware verification
❌ **Don't modify production config** in tests (use test-specific node IDs)
❌ **Don't assume motor type** (check board revision and adjust limits)

## Test Checklist for Pull Requests

Before submitting a PR:

**Code Changes**:
- [ ] Code builds without warnings
- [ ] All existing tests pass
- [ ] New tests added for new functionality
- [ ] Tests cover edge cases and error conditions

**Hardware Changes**:
- [ ] Tested on R5.x board
- [ ] Tested on M5.x board (if applicable)
- [ ] Verified `tm.scheduler.load` < 3000 cycles
- [ ] Monitored for oscillations, instability, overheating
- [ ] No scheduler warnings

**Documentation**:
- [ ] Test docstrings describe what is being tested
- [ ] SAFETY.md updated if adding safety constraints
- [ ] TESTING.md updated if adding new test patterns

## References

- [CONVENTIONS.md](CONVENTIONS.md) - Code style and patterns
- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture
- [SAFETY.md](SAFETY.md) - Safety-critical constraints
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution workflow
- [studio/Python/tests/tm_test_case.py](studio/Python/tests/tm_test_case.py) - Base test class

---

**Document Status**: Living document, updated as testing practices evolve.
