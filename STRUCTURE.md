# Tinymovr Directory Structure

**Purpose**: Quick reference to project organization and file locations.

**Target Audience**: Developers and AI agents navigating the codebase.

## Repository Organization

```
Tinymovr/
├── firmware/              # Embedded C firmware for PAC5527 MCU
├── studio/                # Python client library and tools
├── hardware/              # Hardware design files and datasheets
├── docs/                  # Sphinx documentation (user-facing)
├── avlos_config.yaml      # Code generation configuration
├── ARCHITECTURE.md        # System architecture guide
├── SAFETY.md             # Safety-critical constraints
├── AVLOS_GUIDE.md        # Protocol modification guide
├── CONVENTIONS.md        # Code style and patterns
├── TESTING.md            # Testing practices
├── CLAUDE.md             # AI agent navigation
├── CONTRIBUTING.md       # Contribution workflow
└── README.md             # Project overview
```

## Firmware Structure

### Top Level

```
firmware/
├── src/                   # Source code (17 modules)
├── pac55xx_sdk/          # Qorvo PAC55xx SDK (not in repo)
├── bootloader/           # CAN/DFU bootloader source
├── build/                # Build artifacts (.elf, .bin, .map)
├── release_binaries/     # Pre-built release binaries
├── Makefile              # Build system
├── pac55xx.ld            # Linker script (memory layout)
└── sdk_patch.patch       # SDK modifications
```

### Source Modules

```
firmware/src/
├── main.c                # Entry point, initialization
├── common.h              # Board revisions, global includes
├── config.h              # System constants, timing parameters
├── tm_enums.h            # Generated enums (from Avlos)
│
├── controller/           # FOC control loops
│   ├── controller.c      # Main control loop (20 kHz)
│   └── controller.h      # Control state and configuration
│
├── motor/                # Motor management and calibration
│   ├── motor.c           # Calibration sequences (R, L, poles)
│   └── motor.h           # Motor parameters, limits
│
├── observer/             # State estimation
│   ├── observer.c        # Position/velocity estimation
│   └── observer.h        # Observer state
│
├── sensor/               # Encoder drivers
│   ├── sensors.c         # Sensor abstraction layer
│   ├── ma7xx.c           # MPS MA7xx (onboard)
│   ├── as5047.c          # AMS AS5047 (external SPI)
│   ├── amt22.c           # CUI AMT22 (external SPI)
│   └── hall.c            # Hall effect sensors
│
├── gatedriver/           # PWM generation
│   ├── gatedriver.c      # Space Vector Modulation (SVM)
│   └── gatedriver.h      # PWM configuration
│
├── adc/                  # Current sensing
│   ├── adc.c             # ADC initialization, ISR
│   └── adc.h             # ADC channels, calibration
│
├── can/                  # CAN bus communication
│   ├── can.c             # CAN driver, message handling
│   ├── can.h             # CAN configuration
│   ├── can_endpoints.c   # Generated Avlos endpoints
│   └── can_endpoints.h   # Generated Avlos declarations
│
├── uart/                 # UART communication (alternative)
│   ├── uart.c            # UART driver
│   └── uart.h            # UART configuration
│
├── ssp/                  # SPI/SSP peripheral
│   ├── ssp.c             # SPI driver for external encoders
│   └── ssp.h             # SPI configuration
│
├── nvm/                  # Non-volatile memory (flash)
│   ├── nvm.c             # Configuration persistence
│   └── nvm.h             # NVM structure
│
├── system/               # Platform-level services
│   ├── system.c          # Clock config, reset, voltage monitoring
│   └── system.h          # System state
│
├── scheduler/            # Timing and synchronization
│   ├── scheduler.c       # Control loop synchronization
│   └── scheduler.h       # Scheduler state, load monitoring
│
├── watchdog/             # Communication timeout
│   ├── watchdog.c        # Watchdog timer management
│   └── watchdog.h        # Watchdog configuration
│
├── timer/                # Hardware timers
│   ├── timer.c           # PWM timer setup
│   └── timer.h           # Timer configuration
│
├── utils/                # Math utilities
│   ├── utils.c           # Fast trig, SVM algorithm
│   └── utils.h           # Utility functions
│
└── rtt/                  # SEGGER RTT debug output
    ├── SEGGER_RTT.c      # RTT implementation
    └── SEGGER_RTT.h      # RTT interface
```

### Key Firmware Files

| File | Purpose |
|------|---------|
| [firmware/src/main.c](firmware/src/main.c) | Entry point, startup sequence |
| [firmware/src/common.h](firmware/src/common.h) | Board revision defines, includes |
| [firmware/src/config.h](firmware/src/config.h) | Timing, PWM frequency, limits |
| [firmware/src/controller/controller.c](firmware/src/controller/controller.c) | Main control loop (20 kHz) |
| [firmware/src/motor/motor.c](firmware/src/motor/motor.c) | Calibration implementation |
| [firmware/src/motor/motor.h](firmware/src/motor/motor.h) | Calibration constants, motor limits |
| [firmware/src/can/can_endpoints.c](firmware/src/can/can_endpoints.c) | Generated Avlos endpoints (97 endpoints) |
| [firmware/src/tm_enums.h](firmware/src/tm_enums.h) | Generated enums (states, errors, options) |
| [firmware/Makefile](firmware/Makefile) | Build system (REV=R52, debug/release) |
| [firmware/pac55xx.ld](firmware/pac55xx.ld) | Memory layout (128KB flash, 32KB RAM) |

## Studio Structure

### Top Level

```
studio/Python/
├── tinymovr/             # Main package
├── tests/                # Test suite (HITL and simulation)
├── scripts/              # Utility scripts
├── setup.py              # Package installation
└── package.json          # Node.js dependencies (GUI)
```

### Package Structure

```
studio/Python/tinymovr/
├── __init__.py           # Package initialization
├── __main__.py           # CLI entry point
│
├── specs/                # YAML API specifications (Avlos)
│   ├── tinymovr_1_3_x.yaml
│   ├── tinymovr_2_0_x.yaml
│   ├── tinymovr_2_1_x.yaml
│   ├── tinymovr_2_2_x.yaml
│   └── tinymovr_2_3_x.yaml  # Current version
│
├── config/               # Configuration and spec loading
│   ├── config.py         # Spec loading, hash matching, device discovery
│   └── __init__.py
│
├── codec/                # Binary serialization
│   ├── codec.py          # Multibyte codec for CAN messages
│   └── __init__.py
│
├── bus_router.py         # Global singleton, message distribution
├── bus_manager.py        # CAN bus connection, auto-reconnect
├── channel.py            # Per-device communication channel
├── device_discovery.py   # Automatic device detection via heartbeat
├── constants.py          # Constants (node IDs, timeouts)
│
├── gui/                  # Qt-based graphical interface
│   ├── main.py           # GUI entry point
│   ├── widgets/          # Custom widgets
│   └── resources/        # Icons, stylesheets
│
└── resources/            # Package resources
    └── schema.json       # JSON schema for validation
```

### Test Structure

```
studio/Python/tests/
├── tm_test_case.py       # Base class for HITL tests
├── test_simulation.py    # Simulation tests (no hardware)
├── test_board.py         # Core HITL tests
├── test_sensors.py       # Sensor-specific tests
├── test_homing.py        # Homing feature tests
├── test_trajectory.py    # Trajectory planner tests
└── test_eol.py          # End-of-line comprehensive tests
```

### Key Studio Files

| File | Purpose |
|------|---------|
| [studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml](studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml) | Current API specification |
| [studio/Python/tinymovr/config/config.py](studio/Python/tinymovr/config/config.py) | Spec loading and hash matching |
| [studio/Python/tinymovr/bus_router.py](studio/Python/tinymovr/bus_router.py) | Message routing singleton |
| [studio/Python/tinymovr/channel.py](studio/Python/tinymovr/channel.py) | Per-device CAN channel |
| [studio/Python/tests/tm_test_case.py](studio/Python/tests/tm_test_case.py) | Base class for HITL tests |
| [studio/Python/setup.py](studio/Python/setup.py) | Package installation config |

## Hardware Structure

```
hardware/
├── R5/                   # R5.x series boards (high-current)
│   ├── R50/              # R5.0 design files
│   ├── R51/              # R5.1 design files
│   ├── R52/              # R5.2 design files
│   └── R53/              # R5.3 design files
│
├── M5/                   # M5.x series boards (gimbal)
│   └── M51/              # M5.1 design files
│
├── R3/                   # R3.x series boards (legacy)
│   ├── R32/              # R3.2 design files
│   └── R33/              # R3.3 design files
│
├── Actuator/             # Complete actuator designs
├── ServoKit/             # Development/testing hardware
├── Datasheets/           # MCU and component datasheets
│   ├── PAC5527 Data Sheet.pdf
│   └── UG_PAC5527.pdf
│
└── misc/                 # Miscellaneous hardware files
```

## Documentation Structure

```
docs/
├── index.rst             # Documentation home
├── conf.py               # Sphinx configuration
│
├── hardware/             # Hardware documentation
│   ├── board_versions.rst
│   └── specifications.rst
│
├── control/              # Control theory and tuning
│   ├── position_control.rst
│   ├── velocity_control.rst
│   └── current_control.rst
│
├── features/             # Feature guides
│   ├── calibration.rst
│   ├── homing.rst
│   └── trajectory.rst
│
├── protocol/             # Communication protocol
│   ├── overview.rst
│   └── reference.rst     # Generated from Avlos
│
├── studio/               # Python client usage
│   ├── installation.rst
│   └── quickstart.rst
│
├── develop/              # Developer guides
│   ├── firmware.rst
│   └── python.rst
│
└── interfaces/           # Interface guides
    ├── can.rst
    └── uart.rst
```

## Code Generation Files

| File | Purpose |
|------|---------|
| [avlos_config.yaml](avlos_config.yaml) | Avlos code generation configuration |
| [studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml](studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml) | API specification (input) |
| [firmware/src/can/can_endpoints.c](firmware/src/can/can_endpoints.c) | Generated C implementation |
| [firmware/src/can/can_endpoints.h](firmware/src/can/can_endpoints.h) | Generated C header |
| [firmware/src/tm_enums.h](firmware/src/tm_enums.h) | Generated enums |
| [docs/protocol/reference.rst](docs/protocol/reference.rst) | Generated documentation |

**Workflow**:
```bash
python -m avlos.generate avlos_config.yaml
```

## Build Artifacts

### Firmware Build

```
firmware/build/
├── tinymovr_fw.elf       # Executable with debug symbols
├── tinymovr_fw.bin       # Binary for flashing
├── tinymovr_fw.map       # Memory map
└── *.o                   # Object files
```

### Python Build

```
studio/Python/
├── dist/                 # Wheel and source distribution
│   ├── tinymovr-X.Y.Z-py3-none-any.whl
│   └── tinymovr-X.Y.Z.tar.gz
└── tinymovr.egg-info/    # Package metadata
```

## Ignored Directories

Not tracked in version control (see `.gitignore`):

```
firmware/pac55xx_sdk/     # PAC55xx SDK (copyright restrictions)
firmware/build/           # Build artifacts
studio/Python/dist/       # Python distribution packages
studio/Python/*.egg-info/ # Python package metadata
venv/                     # Python virtual environment
docs/_build/              # Sphinx build output
__pycache__/              # Python bytecode cache
.vscode/                  # Editor settings
.DS_Store                 # macOS metadata
```

## Quick Navigation by Task

### Modifying Control Loop
- Start: [firmware/src/controller/controller.c](firmware/src/controller/controller.c)
- Check: [SAFETY.md](SAFETY.md) for constraints

### Adding API Endpoint
- Edit: [studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml](studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml)
- Generate: `python -m avlos.generate avlos_config.yaml`
- Implement: Appropriate module in `firmware/src/`

### Modifying Calibration
- Edit: [firmware/src/motor/motor.c](firmware/src/motor/motor.c)
- Check: [firmware/src/motor/motor.h](firmware/src/motor/motor.h) for constants

### Writing Tests
- HITL: [studio/Python/tests/test_board.py](studio/Python/tests/test_board.py)
- Simulation: [studio/Python/tests/test_simulation.py](studio/Python/tests/test_simulation.py)
- Base class: [studio/Python/tests/tm_test_case.py](studio/Python/tests/tm_test_case.py)

### Adding Board Revision
- Edit: [firmware/src/common.h](firmware/src/common.h)
- Update: [firmware/Makefile](firmware/Makefile)

## References

- [ARCHITECTURE.md](ARCHITECTURE.md) - System design and module responsibilities
- [CONVENTIONS.md](CONVENTIONS.md) - Code style and patterns
- [AVLOS_GUIDE.md](AVLOS_GUIDE.md) - Protocol modification workflow
- [TESTING.md](TESTING.md) - Testing practices

---

**Document Status**: Living document, updated as project structure evolves.
