![FW Check Build](https://github.com/yconst/tinymovr/workflows/Tinymovr%20Firmware%20Check%2FBuild/badge.svg)
![Studio Lint Test](https://github.com/yconst/tinymovr/workflows/Tinymovr%20Studio%20Lint%2FTest/badge.svg)
![Docs Build](https://github.com/yconst/tinymovr/workflows/Tinymovr%20Docs%20Build/badge.svg)

> **Warning**
>
> This is the legacy 0.x.x version of the Tinymovr Firmware and Studio app. For the the latest stable version, check out the [main branch](https://github.com/tinymovr/Tinymovr/).

## Tinymovr Motor Controller

This is the main repository for Tinymovr Motor Controller.

Tinymovr is an affordable motor controller for precise control of 3-phase brushless motors. Tinymovr is highly integrated incorporating an advanced MCU (Qorvo PAC5527), compact power stage, integrated absolute angle encoder (MPS MA702) and CAN and UART connectivity. 

### Documentation

For Tinymovr documentation, please visit the [Tinymovr docs at Readthedocs](https://tinymovr.readthedocs.io).

### Repository Structure

The master branch of this repository reflects the latest release. **Note that atm the master branch only accepts hotfixes.**

Development takes place at the [develop](https://github.com/tinymovr/Tinymovr/tree/develop) branch.

We try to follow [A successful Git branching model](https://nvie.com/posts/a-successful-git-branching-model/) as much as possible.

The repository is organized as follows:

[firmware](./firmware/): Firmware for the PAC5527 MCU in Tinymovr

[studio](./studio/): Tinymovr Studio client software and library

[hardware](./hardware): Designs and drawings for related hardware

[documentation](./docs): Documentation source

### External Links

[Tinymovr Website and Store](https://tinymovr.com)

[Blog](https://hackaday.io/project/168650/logs)

[BLDC-Design-Help Repository](https://github.com/ThotAlion/BLDC-design-help), with lots of useful information and Tinymovr benchmarks

