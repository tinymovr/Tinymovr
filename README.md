![FW Check Build](https://github.com/yconst/tinymovr/workflows/Tinymovr%20Firmware%20Check%2FBuild/badge.svg)
![Studio Lint Test](https://github.com/yconst/tinymovr/workflows/Tinymovr%20Studio%20Lint%2FTest/badge.svg)
![Docs Build](https://github.com/yconst/tinymovr/workflows/Tinymovr%20Docs%20Build/badge.svg)
[![Discord](https://img.shields.io/discord/742400176664084535)](https://discord.gg/CzcCaXbU)

[Tinymovr is an affordable motor controller](https://tinymovr.com) for precise control of 3-phase brushless motors. Tinymovr works with up to 38V input voltage, drives motors up to 40A continuous phase current, integrates an absolute angle encoder (MPS MA702) and features rich connectivity.

This repository holds the open source firmware, client library, hardware designs and documentation source.

> **Note**
>
> This is the latest stable version of the Tinymovr Firmware and Studio app. For the legacy 0.x.x version, check out the [legacy branch](https://github.com/tinymovr/Tinymovr/tree/attic/legacy).

**[Documentation@Readthedocs](https://tinymovr.readthedocs.io)**

## Getting Started

Please visit the [Quick Start Guide](https://tinymovr.readthedocs.io/en/latest/quickstart.html)

## Getting Help

Please visit the [Discussions Page](https://github.com/tinymovr/Tinymovr/discussions) and the [Discord Server](https://discord.gg/vNvmpfthug) to ask any questions. If you are unable to find an answer to your question, you are welcome to [start a new discussion](https://github.com/tinymovr/Tinymovr/discussions/new/choose) or ask away at [our Discord server](https://discord.gg/vNvmpfthug).

If you would like to propose a feature or think you have found a bug, [open a new issue](https://github.com/tinymovr/Tinymovr/issues/new).

### Repository Structure

> **Note**
>
> The `main` branch of this repository reflects the latest release. Development takes place at the [develop branch](https://github.com/tinymovr/Tinymovr/tree/develop).

The repository is organized as follows:

[firmware](./firmware/): Firmware for the PAC5527 MCU in Tinymovr

[studio](./studio/): Tinymovr Studio client software and library

[hardware](./hardware): Designs and drawings for related hardware

[documentation](./docs): Documentation source

### Branching Model

The `main` branch reflects the latest release.

Features are merged into the [develop](https://github.com/tinymovr/Tinymovr/tree/develop) branch.

Hotfixes are merged directly into `main`, resulting in a new release.

We try to follow [A successful Git branching model](https://nvie.com/posts/a-successful-git-branching-model/) as much as possible, but within reason.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/tinymovr/tinymovr/tags). 

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License

[GPLv3](https://choosealicense.com/licenses/gpl-3.0/)

### External Links

[Tinymovr Website and Store](https://tinymovr.com)

[Blog](https://tinymovr.com/blogs/news)

[Discord server](https://discord.gg/vNvmpfthug)

[Project page on Hackaday](https://hackaday.io/project/168650-tinymovr-motor-controller)

[BLDC-Design-Help Repository](https://github.com/ThotAlion/BLDC-design-help), with lots of useful information and Tinymovr benchmarks

