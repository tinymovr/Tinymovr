![FW Check Build](https://github.com/yconst/tinymovr/workflows/Tinymovr%20Firmware%20Check%2FBuild/badge.svg)
![Studio Lint Test](https://github.com/yconst/tinymovr/workflows/Tinymovr%20Studio%20Lint%2FTest/badge.svg)
![Docs Build](https://github.com/yconst/tinymovr/workflows/Tinymovr%20Docs%20Build/badge.svg)
[![Discord](https://img.shields.io/discord/742400176664084535)](https://discord.gg/UnmfuVzKuQ)

[Tinymovr is an affordable motor controller](https://tinymovr.com) with integrated encoder and CAN bus for precise control of 3-phase brushless motors (PMSMs). 

This repository holds the open source firmware, client library, hardware designs and documentation source.

> **Note**
>
> This is the latest stable version of the Tinymovr Firmware and Studio app. For the legacy 0.x.x version, check out the [legacy branch](https://github.com/tinymovr/Tinymovr/tree/attic/legacy).

## Getting Started

Please visit the [Quick Start Guide](https://tinymovr.readthedocs.io/en/latest/quickstart.html)

## Getting Help

[Full documentation is available at Readthedocs](https://tinymovr.readthedocs.io).

Please visit the [Discussions Page](https://github.com/tinymovr/Tinymovr/discussions) and the [Discord Server](https://discord.gg/vNvmpfthug) to ask any questions. If you are unable to find an answer to your question, you are welcome to [start a new discussion](https://github.com/tinymovr/Tinymovr/discussions/new/choose) or ask away at [our Discord server](https://discord.gg/vNvmpfthug).

If you would like to propose a feature or think you have found a bug, [open a new issue](https://github.com/tinymovr/Tinymovr/issues/new).

### Repository Structure

> **Note**: The `main` branch reflects the latest release. Development happens on [develop](https://github.com/tinymovr/Tinymovr/tree/develop).

The repository is organized as follows:

[firmware](./firmware/): Firmware for the PAC5527 MCU in Tinymovr

[studio](./studio/): Tinymovr Studio client software and library

[hardware](./hardware): Designs and drawings for related hardware

[docs](./docs): User documentation source (built with Sphinx)

See also: Developer documentation in root (ARCHITECTURE.md, CONTRIBUTING.md, etc.)

### Developer Documentation

For contributors and AI agents working with the codebase:

- **[ARCHITECTURE.md](ARCHITECTURE.md)** - System design and module organization
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Setup, workflow, and PR process
- **[SAFETY.md](SAFETY.md)** - Safety-critical constraints (read before modifying firmware)
- **[CLAUDE.md](CLAUDE.md)** - Navigation guide for AI agents

Additional technical documentation:
- [AVLOS_GUIDE.md](AVLOS_GUIDE.md) - Protocol modification workflow
- [CONVENTIONS.md](CONVENTIONS.md) - Code style and patterns
- [TESTING.md](TESTING.md) - Testing practices
- [STRUCTURE.md](STRUCTURE.md) - Directory organization
- [PAC5527_NOTES.md](PAC5527_NOTES.md) - MCU architecture

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/tinymovr/tinymovr/tags). 

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

**New contributors**: See [CONTRIBUTING.md](CONTRIBUTING.md) for setup instructions and workflow.

**AI agents**: See [CLAUDE.md](CLAUDE.md) for navigation and guidelines.

## License

[GPLv3](https://choosealicense.com/licenses/gpl-3.0/)

### External Links

[Tinymovr Website and Store](https://tinymovr.com)

[Blog](https://tinymovr.com/blogs/news)

[Discord server](https://discord.gg/vNvmpfthug)

[Project page on Hackaday](https://hackaday.io/project/168650-tinymovr-motor-controller)

[BLDC-Design-Help Repository](https://github.com/ThotAlion/BLDC-design-help), with lots of useful information and Tinymovr benchmarks

