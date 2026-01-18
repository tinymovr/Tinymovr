# Contributing to Tinymovr

Thank you for your interest in contributing to Tinymovr! This guide will help you get started with development, testing, and submitting contributions.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Code Style](#code-style)
- [Commit Guidelines](#commit-guidelines)
- [Pull Request Process](#pull-request-process)
- [Testing Guidelines](#testing-guidelines)
- [Documentation](#documentation)
- [Getting Help](#getting-help)

## Code of Conduct

This project adheres to the [Contributor Covenant Code of Conduct](https://www.contributor-covenant.org/version/2/1/code_of_conduct/). By participating, you are expected to uphold this code. Please report unacceptable behavior to the project maintainers.

## Getting Started

### Prerequisites

**For Firmware Development**:
- GNU ARM Embedded Toolchain (arm-none-eabi-gcc)
- Python 3.9+ (for Avlos code generation)
- Make (Linux/macOS) or GNU Make for Windows
- Optional: Segger J-Link or ST-Link debugger

**For Python Development**:
- Python 3.9 or later (3.11 recommended)
- pip and virtualenv
- CAN interface hardware for testing (CANine, slcan, socketcan, etc.)

**For Documentation**:
- Sphinx
- Furo theme
- See [docs/requirements.txt](docs/requirements.txt)

### Repository Setup

1. **Clone the repository**:
   ```bash
   git clone https://github.com/tinymovr/Tinymovr.git
   cd Tinymovr
   ```

2. **Check out the develop branch**:
   ```bash
   git checkout develop
   ```

   **Important**: Active development happens on the `develop` branch. Only hotfixes are merged directly to `main`.

### Firmware Setup

1. **Download PAC55xx SDK**:
   - Visit Qorvo website (https://www.qorvo.com)
   - Register and download PAC55xx SDK
   - Extract SDK files

2. **Copy SDK to firmware directory**:
   ```bash
   cp -r /path/to/pac55xx_sdk firmware/pac55xx_sdk/
   ```

3. **Apply SDK patch**:
   ```bash
   cd firmware
   pip3 install patch
   python3 -m patch sdk_patch.patch
   ```

4. **Verify build**:
   ```bash
   make debug REV=R52
   ```

   If successful, you'll see `build/tinymovr_fw.elf` created.

**Note**: The PAC55xx SDK is not included in the repository due to Qorvo licensing restrictions. You must download it separately.

### Python Setup

1. **Install in editable mode**:
   ```bash
   cd studio/Python
   pip3 install -e .
   ```

2. **For GUI support** (optional):
   ```bash
   pip3 install -e .[gui]
   ```

3. **Verify installation**:
   ```bash
   tinymovr_cli --help
   ```

   You should see the help text for the Tinymovr CLI.

### Documentation Setup

```bash
cd docs
pip3 install -r requirements.txt
make html
```

Open `docs/build/html/index.html` in your browser.

## Development Workflow

### Branching Strategy

Tinymovr follows a Git Flow-inspired branching model:

```
main          ← Latest stable release (tagged)
  ↓
develop       ← Active development (all features merge here)
  ↓
feature/*     ← New features
fix/*         ← Bug fixes
refactor/*    ← Code restructuring
docs/*        ← Documentation changes
```

**Reference**: [README.md](README.md#branching-model)

### Creating a Branch

1. **Ensure develop is up-to-date**:
   ```bash
   git checkout develop
   git pull origin develop
   ```

2. **Create your branch**:
   ```bash
   git checkout -b feature/my-feature-name
   ```

**Branch Naming Convention**:
- `feature/description` - New features (e.g., `feature/motor-temperature-estimation`)
- `fix/description` - Bug fixes (e.g., `fix/calibration-voltage-debouncing`)
- `refactor/description` - Code restructuring (e.g., `refactor/observer-module`)
- `docs/description` - Documentation only (e.g., `docs/add-architecture-guide`)

### Making Changes

**Before you start**:
1. Read [ARCHITECTURE.md](ARCHITECTURE.md) to understand the system
2. Read [SAFETY.md](SAFETY.md) if modifying firmware (especially control code)
3. Read [AVLOS_GUIDE.md](AVLOS_GUIDE.md) if modifying the API protocol

**During development**:
- Make small, focused commits
- Write clear commit messages (see [Commit Guidelines](#commit-guidelines))
- Test frequently (see [Testing Guidelines](#testing-guidelines))
- Document safety implications if modifying control code

### Firmware Changes

**Build**:
```bash
cd firmware
make debug REV=R52    # For debugging
make release REV=R52  # For release
```

**Test on hardware**:
1. Flash firmware (J-Link, DFU, etc.)
2. Run Python HITL tests (see [Testing Guidelines](#testing-guidelines))
3. Monitor for oscillations, overheating, erratic behavior

**Key files** (see [ARCHITECTURE.md](ARCHITECTURE.md)):
- [firmware/src/controller/controller.c](firmware/src/controller/controller.c) - Control loops
- [firmware/src/motor/motor.c](firmware/src/motor/motor.c) - Calibration
- [firmware/src/sensor/](firmware/src/sensor/) - Encoder drivers
- [firmware/src/observer/observer.c](firmware/src/observer/observer.c) - State estimation

### Python Changes

**Run simulation tests** (no hardware required):
```bash
cd studio/Python
python -m unittest tests/test_simulation.py
```

**Run HITL tests** (requires hardware):
```bash
pytest -m hitl_default
```

**Key files**:
- [studio/Python/tinymovr/](studio/Python/tinymovr/) - Client library
- [studio/Python/tests/](studio/Python/tests/) - Test suite

### Avlos Protocol Changes

If modifying the API (adding/removing endpoints):

1. **Edit YAML spec**:
   ```bash
   vim studio/Python/tinymovr/specs/tinymovr_2_3_x.yaml
   ```

2. **Generate code**:
   ```bash
   avlos from file <spec_file>.yaml
   ```

   This generates:
   - [firmware/src/can/can_endpoints.h](firmware/src/can/can_endpoints.h)
   - [firmware/src/can/can_endpoints.c](firmware/src/can/can_endpoints.c)
   - [firmware/src/tm_enums.h](firmware/src/tm_enums.h)
   - [docs/protocol/reference.rst](docs/protocol/reference.rst)

3. **Implement C functions** in appropriate module

4. **Rebuild firmware**:
   ```bash
   cd firmware
   make release REV=R52
   ```

5. **Flash and test**:
   ```bash
   cd studio/Python
   pytest -m hitl_default
   ```

**Important**: Protocol hash will change. See [AVLOS_GUIDE.md](AVLOS_GUIDE.md) for details.

## Code Style

**See**: [CONVENTIONS.md](CONVENTIONS.md) for complete code style guidelines.

**Quick Summary**:
- **C/C++**: K&R-inspired, 4-space indentation, `TM_RAMFUNC` for control loop
- **Python**: PEP 8, Black formatter (127 chars), type hints encouraged
- **YAML**: 2-space indentation, clear summaries

**Critical**:
- Use `float` (not `double`) in firmware
- No blocking operations in control loop
- See [SAFETY.md](SAFETY.md) for safety constraints

## Commit Guidelines

### Commit Message Format

```
<type>: <summary>

<body>

<footer>
```

### Types

- `feat`: New feature
- `fix`: Bug fix
- `refactor`: Code restructure (no behavior change)
- `test`: Add/modify tests
- `docs`: Documentation changes
- `chore`: Build system, dependencies, tooling
- `style`: Formatting, whitespace (no logic change)

### Summary

- **Imperative mood**: "Add feature" (not "Added feature" or "Adds feature")
- **Lowercase** (except proper nouns like "Python")
- **No period** at end
- **Max 72 characters**

### Body (Optional but Recommended)

- Explain **what** and **why**, not how
- Wrap at 72 characters
- Separate from summary with blank line

### Footer (If Applicable)

- **Issue references**: `Fixes #123`, `Relates to #456`, `Closes #789`
- **Breaking changes**: `BREAKING CHANGE: description`
- **Co-authors**: `Co-authored-by: Name <email>`

### Examples

**Good commits**:

```
feat: Add motor temperature estimation endpoint

Estimate motor temperature from phase resistance change during operation.
Useful for thermal management in high-current applications.

Adds new endpoint: tm.motor.temperature (degC)
Formula: T = T_cal + (R - R_cal) / (R_cal * alpha)

Fixes #234
```

```
fix: Add debouncing to resistance calibration voltage check

Power line noise causes false ABNORMAL_CALIBRATION_VOLTAGE errors.
Added 15ms debounce window after 75ms warm-up to allow voltage to
stabilize.

Safety note: Actual faults (shorts, open circuits) still detected
after debounce period.

CAL_R_WARMUP_ITERATIONS = 1500 (~75ms @ 20kHz)
CAL_R_ABNORMAL_DEBOUNCE = 300 (~15ms @ 20kHz)

Fixes #456
```

```
refactor: Extract SVM computation to separate function

Improves code readability and enables unit testing of SVM algorithm
in isolation.

No functional change.
```

```
docs: Add comprehensive architecture documentation

Creates ARCHITECTURE.md with AI-optimized system overview, module map,
and cross-references to related documentation.

Includes:
- Module map with dependencies
- Control flow diagrams
- Calibration sequence details
- Communication protocol structure

Related to #789
```

**Bad commits**:

```
update code
```

```
Fixed bug
```

```
WIP
```

```
Changes
```

## Pull Request Process

### Before Creating PR

**Checklist**:
- [ ] Code builds without warnings
- [ ] Tests pass (unit and HITL if applicable)
- [ ] Code follows style guidelines
- [ ] Documentation updated (if user-facing change)
- [ ] Commit messages follow format
- [ ] Branch is up-to-date with develop

**Update branch**:
```bash
git checkout develop
git pull origin develop
git checkout feature/my-feature
git rebase develop
# Resolve conflicts if any
git push --force-with-lease origin feature/my-feature
```

### Creating PR

1. **Push branch**:
   ```bash
   git push origin feature/my-feature
   ```

2. **Create PR on GitHub**:
   - **Base**: `develop` (NOT `main`)
   - **Title**: Same as commit summary if single commit
   - **Description**: Use template below

### PR Description Template

```markdown
## Summary
Brief description of changes (1-2 sentences)

## Motivation
Why is this change needed? What problem does it solve?

## Changes
- Bullet list of specific changes
- Keep it concise but complete
- Reference file paths for major changes

## Testing
How was this tested?
- [ ] Unit tests pass
- [ ] HITL tests pass (specify board revision and motor type)
- [ ] Manual testing performed (describe setup)

## Safety Considerations
(If firmware change) Any safety implications?
- Timing impact (tm.scheduler.load before/after)?
- Current/voltage limits affected?
- Error handling modified?

## Breaking Changes
(If applicable) Does this break existing APIs or behavior?

## Checklist
- [ ] Code builds without warnings
- [ ] Tests added/updated
- [ ] Documentation updated
- [ ] Commit messages follow format
- [ ] Branch rebased on develop

Fixes #<issue_number>
```

### Review Process

**What reviewers check**:
- **Correctness**: Does it work as intended?
- **Safety**: Could it cause harm (especially firmware changes)?
- **Style**: Follows coding guidelines?
- **Tests**: Adequate coverage?
- **Documentation**: User-visible changes documented?

**Addressing feedback**:
```bash
# Make changes based on review
git add .
git commit -m "refactor: Address review feedback"
git push origin feature/my-feature
```

**DO NOT**:
- Force-push after review starts (unless rebasing on develop)
- Squash commits before review approval
- Merge before CI passes

### After Approval

Maintainer will:
1. Merge to `develop`
2. Update changelog
3. Close related issues

**Celebrating**: Your contribution is now part of Tinymovr! 🎉

## Testing Guidelines

**See**: [TESTING.md](TESTING.md) for complete testing documentation.

**Quick Summary**:
- **Test Types**: Simulation (no hardware), HITL (hardware required), sensor-specific, EOL
- **Markers**: `@pytest.mark.hitl_default`, `@pytest.mark.sensor_as5047`, etc.
- **Base Class**: [TMTestCase](studio/Python/tests/tm_test_case.py) for HITL tests

**All PRs must include**:
- Tests for new functionality
- HITL tests for firmware changes (if hardware-dependent)
- Regression tests if fixing a bug

**Safety Testing** (for control loop changes):
- Verify `tm.scheduler.load` < 3000 cycles
- Test on multiple board revisions (R5, M5)
- Test with different motors (high-current, gimbal)
- Monitor for oscillations, instability, overheating
- Document testing conditions in PR

## Documentation

### When to Update Docs

**Always update documentation when**:
- Adding new endpoint (update [AVLOS_GUIDE.md](AVLOS_GUIDE.md) example)
- Changing API behavior
- Adding configuration option
- Fixing non-obvious bug (add to troubleshooting)
- Modifying safety constraints (update [SAFETY.md](SAFETY.md))

### Documentation Locations

- **User Documentation**: [docs/](docs/) - Sphinx docs (user-facing)
- **System Architecture**: [ARCHITECTURE.md](ARCHITECTURE.md)
- **Safety Constraints**: [SAFETY.md](SAFETY.md)
- **Development Guide**: [CLAUDE.md](CLAUDE.md) (for AI agents)
- **Protocol Guide**: [AVLOS_GUIDE.md](AVLOS_GUIDE.md)
- **MCU Details**: [PAC5527_NOTES.md](PAC5527_NOTES.md)

### Building Docs Locally

```bash
cd docs
pip3 install -r requirements.txt
make html
```

Open `docs/build/html/index.html` in your browser.

### Documentation Style

- **Clear, concise language**
- **Code examples** for complex topics
- **Screenshots** for GUI features
- **Safety warnings** prominently displayed
- **Links** to related sections

**Example**:
```markdown
## Motor Calibration

Tinymovr requires calibration before first use:

```python
tm.controller.calibrate()
```

**Warning**: Ensure motor is free to rotate during calibration.

The calibration sequence measures:
1. Phase resistance (R)
2. Phase inductance (L)
3. Motor pole pairs
4. Encoder eccentricity

See [SAFETY.md](SAFETY.md) for calibration constraints.
```

## Getting Help

### Before Asking

1. **Check documentation**: https://tinymovr.readthedocs.io
2. **Search existing issues**: https://github.com/tinymovr/Tinymovr/issues
3. **Search discussions**: https://github.com/tinymovr/Tinymovr/discussions
4. **Read architecture docs**: [ARCHITECTURE.md](ARCHITECTURE.md), [CLAUDE.md](CLAUDE.md)

### Where to Ask

**General questions**:
- [Discord Server](https://discord.gg/vNvmpfthug)
- [GitHub Discussions](https://github.com/tinymovr/Tinymovr/discussions)

**Bug reports**:
- [GitHub Issues](https://github.com/tinymovr/Tinymovr/issues/new)

**Feature requests**:
- Start a [GitHub Discussion](https://github.com/tinymovr/Tinymovr/discussions/new/choose)
- If consensus is reached, create an issue

**Development questions**:
- [Discord #development channel](https://discord.gg/vNvmpfthug)

### When Asking

**Be specific**:
- Describe your setup (board revision, motor, firmware version)
- Provide code snippets (use markdown code blocks)
- Include error messages (full stack trace)
- Show what you've tried

**Good question**:
```
I'm trying to implement position control on an R5.2 board with firmware
v2.3.0 and a 14-pole-pair motor. After calibration, when I set the
position setpoint:

tm.controller.position_mode()
tm.controller.position.setpoint = 5000

The motor oscillates around the target. Here's the output:

tm.scheduler.load: 2800 cycles
tm.controller.position.gain: 20.0
tm.controller.velocity.gain: 8e-5

I've tried reducing position gain to 10.0, but oscillation persists.
Any suggestions?
```

**Bad question**:
```
Motor doesn't work. Help!
```

## Recognition

Contributors are recognized in:
- **GitHub contributors page**
- **CHANGELOG.md**
- **Co-authored-by** in commits (if pair programming)
- **Discord contributor role** (request on Discord)

Thank you for contributing to Tinymovr!

---

**See Also**:
- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture
- [SAFETY.md](SAFETY.md) - Safety-critical constraints
- [STRUCTURE.md](STRUCTURE.md) - Directory organization
- [CONVENTIONS.md](CONVENTIONS.md) - Code style and patterns
- [TESTING.md](TESTING.md) - Testing practices
- [AVLOS_GUIDE.md](AVLOS_GUIDE.md) - Protocol modification guide
- [PAC5527_NOTES.md](PAC5527_NOTES.md) - MCU architecture
- [CLAUDE.md](CLAUDE.md) - AI agent navigation guide

**External Resources**:
- [Tinymovr Documentation](https://tinymovr.readthedocs.io)
- [Discord Server](https://discord.gg/vNvmpfthug)
- [Tinymovr Website](https://tinymovr.com)
