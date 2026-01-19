# Claude Agent Navigation Guide

**Purpose**: Navigation hub for AI agents working with Tinymovr. Points to specialized documentation, does not duplicate content.

**Target Audience**: Claude and other AI coding assistants.

## Quick Start

### 1. Before You Begin

**Essential Reading** (read in order):
1. [ARCHITECTURE.md](ARCHITECTURE.md) - System overview and module map
2. [SAFETY.md](SAFETY.md) - Safety-critical constraints (**required** before touching firmware)
3. [STRUCTURE.md](STRUCTURE.md) - Directory organization

**Supplementary**:
- [AVLOS_GUIDE.md](AVLOS_GUIDE.md) - Protocol modification workflow
- [CONVENTIONS.md](CONVENTIONS.md) - Code style and patterns
- [TESTING.md](TESTING.md) - Testing practices
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution workflow for humans

### 2. Key Entry Points

| Task | Starting Point |
|------|----------------|
| Firmware initialization | [firmware/src/main.c](firmware/src/main.c) |
| Control loop (20 kHz) | [firmware/src/controller/controller.c](firmware/src/controller/controller.c) |
| Motor calibration | [firmware/src/motor/motor.c](firmware/src/motor/motor.c) |
| API specification | [studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml](studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml) |
| Python client | [studio/Python/tinymovr/](studio/Python/tinymovr/) |
| Test base class | [studio/Python/tests/tm_test_case.py](studio/Python/tests/tm_test_case.py) |

### 3. Module Map

See [ARCHITECTURE.md Module Map](ARCHITECTURE.md#module-organization) for complete module responsibilities and dependencies.

## Decision Tree: Which Document Should I Read?

Use this decision tree to find the right documentation:

```
┌─ What are you trying to do? ─┐
│
├─ Understand system design
│  └─→ Read: ARCHITECTURE.md
│
├─ Modify control loop or calibration
│  └─→ Read: SAFETY.md (REQUIRED)
│     └─→ Then: ARCHITECTURE.md (control flow section)
│
├─ Add/modify API endpoint
│  └─→ Read: AVLOS_GUIDE.md (complete workflow with examples)
│
├─ Write tests
│  └─→ Read: TESTING.md (test types, markers, patterns)
│
├─ Find a file or understand directory layout
│  └─→ Read: STRUCTURE.md
│
├─ Learn code style or patterns
│  └─→ Read: CONVENTIONS.md
│
├─ Set up development environment
│  └─→ Read: CONTRIBUTING.md (setup section)
│
└─ Navigate codebase (you are here)
   └─→ Read: This document (CLAUDE.md)
```

## Common Tasks Navigation

### Task: Add a New API Endpoint

**Documentation**: [AVLOS_GUIDE.md § Complete Workflow](AVLOS_GUIDE.md#complete-workflow-adding-a-new-endpoint)

**Summary**:
1. Edit YAML spec
2. Run code generator
3. Implement C functions
4. Build and flash firmware
5. Test with Python client

**No code examples here** - see AVLOS_GUIDE.md for complete walkthrough with examples.

### Task: Modify Control Loop

**Documentation**: [SAFETY.md § Real-Time Constraints](SAFETY.md#real-time-constraints)

**Critical Requirements**:
- Execution time <50 μs (<3000 CPU cycles @ 150 MHz)
- Mark function `TM_RAMFUNC` if called from control loop
- No blocking operations
- Use `float` (not `double`)

**Testing**: [TESTING.md § Safety Testing Requirements](TESTING.md#safety-testing-requirements)

**No code examples here** - see SAFETY.md for constraints and CONVENTIONS.md for patterns.

### Task: Add Board Revision

**Documentation**: [STRUCTURE.md § Firmware Structure](STRUCTURE.md#firmware-structure)

**Files to Modify**:
- [firmware/src/common.h](firmware/src/common.h) - Add revision defines
- [firmware/Makefile](firmware/Makefile) - Update build rules

**No code examples here** - see CONVENTIONS.md for naming patterns.

### Task: Write Tests

**Documentation**: [TESTING.md § Writing Tests](TESTING.md#writing-tests)

**Test Types**:
- **Simulation**: No hardware required ([TESTING.md § Simulation Tests](TESTING.md#1-simulation-tests-no-hardware-required))
- **HITL**: Requires hardware ([TESTING.md § HITL Tests](TESTING.md#2-hardware-in-the-loop-hitl-tests))
- **Sensor-specific**: Requires specific encoder ([TESTING.md § Sensor-Specific Tests](TESTING.md#3-sensor-specific-tests))

**No code examples here** - see TESTING.md for complete test patterns and base class usage.

### Task: Debug Calibration Issues

**Documentation**: [SAFETY.md § Calibration Safety](SAFETY.md#calibration-safety)

**Common Errors**:
- `PHASE_RESISTANCE_OUT_OF_RANGE` → Check motor connections, motor type
- `ABNORMAL_CALIBRATION_VOLTAGE` → Check power supply, shorts/opens
- `POLE_PAIRS_OUT_OF_RANGE` → Check shaft rotation, encoder mounting

**No debug code here** - see SAFETY.md for error details and TESTING.md for debugging strategies.

## Critical Gotchas

### Firmware Gotchas

1. **Board Revision Required**: Must specify `make release REV=R52` (or R53, M51)
2. **PAC55xx SDK Not in Repo**: Download from Qorvo, apply patch ([CONTRIBUTING.md § Firmware Setup](CONTRIBUTING.md#firmware-setup))
3. **Float vs Double**: Always use `float` - Cortex-M4F has single-precision FPU only
4. **Missing TM_RAMFUNC**: Control loop functions must be marked `TM_RAMFUNC`
5. **Calibration Timing**: Do not reduce `CAL_R_LEN` or `CAL_L_LEN` (see [SAFETY.md](SAFETY.md))
6. **Blocking in Control Loop**: Never call `printf()`, `delay()`, or `malloc()` from control loop

**Details**: [SAFETY.md § Common Pitfalls](SAFETY.md#common-pitfalls)

### Python Gotchas

1. **Protocol Hash Mismatch**: Firmware and Python client must have matching protocol hash
2. **BusRouter Not Initialized**: Must call `init_router()` before `create_device()`
3. **Forgot to Calibrate**: Position/velocity control requires calibration first
4. **Unit Mismatch**: Understand native units (ticks, ticks/s) or use Pint units
5. **Test Without Marker**: Hardware tests must have `@pytest.mark.hitl_default`

**Details**: [TESTING.md § Debugging Failed Tests](TESTING.md#debugging-failed-tests)

### Avlos Gotchas

1. **Edited Generated Files**: Never edit `can_endpoints.c/h` or `tm_enums.h` directly
2. **C Function Signature Mismatch**: YAML `dtype` must match C function return type
3. **Forgot to Regenerate**: After YAML change, must run `avlos from file <spec_file>.yaml` (where `<spec_file>` is the appropriate protocol version like `tinymovr_2_3_x` or `tinymovr_2_4_x`)

**Details**: [AVLOS_GUIDE.md § Common Pitfalls](AVLOS_GUIDE.md#common-pitfalls)

## Project Context

### Two Separate Projects

**Firmware** (Embedded C):
- **Target**: Qorvo PAC5527 MCU (ARM Cortex-M4F @ 150 MHz)
- **Purpose**: Real-time motor control (Field-Oriented Control)
- **Architecture**: Bare-metal, no RTOS, 20 kHz control loop
- **Build**: GNU Make + arm-none-eabi-gcc
- **Entry**: [firmware/src/main.c](firmware/src/main.c)

**Studio** (Python):
- **Target**: Host computer (Linux, macOS, Windows)
- **Purpose**: User interface and motor configuration
- **Architecture**: Schema-driven client (Avlos framework)
- **Build**: Python package (pip install)
- **Entry**: [studio/Python/tinymovr/](studio/Python/tinymovr/)

**Communication**: CAN bus via Avlos protocol (schema-driven, auto-generated from YAML)

### Key Design Decisions

1. **No RTOS**: Bare-metal for predictability and low overhead
2. **Fixed 20 kHz PWM**: Simplifies timing analysis, above audible range
3. **RAM Execution**: Critical functions run from RAM for deterministic timing
4. **Schema-Driven API**: Avlos framework eliminates manual wrapper coding
5. **Automatic Safety**: Errors force IDLE state, cannot be disabled

**Details**: [ARCHITECTURE.md § Key Design Decisions](ARCHITECTURE.md#key-design-decisions)

## Safety-Critical Information

**⚠️ READ BEFORE MODIFYING FIRMWARE**

### Control Loop Constraints

- **Hard Deadline**: <50 μs (1 / 20 kHz)
- **CPU Budget**: <3000 cycles @ 150 MHz (40% utilization)
- **Monitor**: `tm.scheduler.load` (should be <3000)

### Current Limits (Board-Specific)

- **R3.x / R5.x**: 5.0 A min trip, 30 A peak max
- **M5.x**: 1.0 A min trip, 10 A peak max
- **Trip Threshold**: 1.5× configured limit

### Calibration Voltage Limits (Board-Specific)

- **R3.x / R5.x**: 0.5 V max (high-current motors)
- **M5.x**: 5.0 V max (gimbal motors)

**Complete Details**: [SAFETY.md](SAFETY.md)

## Testing Checklist

Before submitting changes:

**All Changes**:
- [ ] Code builds without warnings
- [ ] Relevant tests pass

**Firmware Changes**:
- [ ] Read [SAFETY.md](SAFETY.md) completely
- [ ] Verified `tm.scheduler.load` < 3000 cycles
- [ ] Tested on multiple board revisions (R5, M5)
- [ ] No blocking operations in control loop
- [ ] Marked critical functions `TM_RAMFUNC`

**Protocol Changes**:
- [ ] Followed [AVLOS_GUIDE.md](AVLOS_GUIDE.md) workflow
- [ ] Generated code with `avlos.generate`
- [ ] Protocol hash updated in firmware and client
- [ ] HITL tests pass

**Complete Checklist**: [TESTING.md § Test Checklist for Pull Requests](TESTING.md#test-checklist-for-pull-requests)

## Quick Command Reference

### Building Firmware

```bash
cd firmware
make debug REV=R52     # Debug build
make release REV=R52   # Release build
make clean             # Clean build artifacts
```

**Details**: [STRUCTURE.md § Build Artifacts](STRUCTURE.md#build-artifacts)

### Running Tests

```bash
cd studio/Python
python -m unittest tests/test_simulation.py  # Simulation (no hardware)
pytest -m hitl_default                        # Basic HITL (requires hardware)
pytest -m eol                                 # Comprehensive tests
```

**Details**: [TESTING.md § Test Types](TESTING.md#test-types)

### Code Generation

```bash
cd studio/Python/tinymovr/specs
avlos from file <spec_file>.yaml
```

**Note**: Replace `<spec_file>` with the appropriate protocol version (e.g., `tinymovr_2_3_x`, `tinymovr_2_4_x`). Always verify which spec file matches the firmware version being worked on.

**Details**: [AVLOS_GUIDE.md § Code Generation Configuration](AVLOS_GUIDE.md#code-generation-configuration)

## When You're Stuck

### Finding Files

**Use**: [STRUCTURE.md](STRUCTURE.md) - Complete directory reference

### Understanding System Design

**Use**: [ARCHITECTURE.md](ARCHITECTURE.md) - Module map, control flow, design patterns

### Understanding Code Style

**Use**: [CONVENTIONS.md](CONVENTIONS.md) - C/C++ style, Python style, naming conventions

### Debugging Tests

**Use**: [TESTING.md § Debugging Failed Tests](TESTING.md#debugging-failed-tests)

### Understanding Safety Constraints

**Use**: [SAFETY.md](SAFETY.md) - Real-time constraints, current limits, calibration safety

### Modifying Protocol

**Use**: [AVLOS_GUIDE.md](AVLOS_GUIDE.md) - Complete workflow with examples

## Document Quick Reference

| Document | Purpose | Use When |
|----------|---------|----------|
| **ARCHITECTURE.md** | System design patterns | Understanding module organization, control flow |
| **SAFETY.md** | Safety-critical constraints | Modifying control loop, calibration, or limits |
| **STRUCTURE.md** | Directory layout | Finding files, understanding organization |
| **CONVENTIONS.md** | Code style and patterns | Writing code, reviewing code |
| **TESTING.md** | Testing practices | Writing tests, debugging test failures |
| **AVLOS_GUIDE.md** | Protocol modification | Adding/modifying API endpoints |
| **CONTRIBUTING.md** | Contribution workflow | Setting up environment, submitting PRs |
| **CLAUDE.md** (this) | Navigation hub | Finding the right documentation |

## External References

- **Avlos Framework**: https://github.com/tinymovr/avlos
- **User Documentation**: https://tinymovr.readthedocs.io
- **Discord Server**: https://discord.gg/vNvmpfthug
- **GitHub Issues**: https://github.com/tinymovr/Tinymovr/issues

---

**Document Status**: Navigation hub, points to specialized documentation. Does not duplicate content.

**Philosophy**: Each file has ONE clear purpose. No content duplication. (Inspired by [Get Shit Done](https://github.com/glittercowboy/get-shit-done))
